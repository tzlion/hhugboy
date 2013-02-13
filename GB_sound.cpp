/*
   hhugboy Game Boy emulator
   copyright 2013 taizou

   This file incorporates code from VisualBoyAdvance
   Copyright (C) 1999-2004 by Forgotten

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#define WIN32_LEAN_AND_MEAN

#include <memory.h>
#include <windows.h>
#include <stdio.h>

#include "sound.h"
#include "cpu.h"
#include "GB.h"
#include "config.h"

#define SOUND_MAGIC   0x60000000
#define SOUND_MAGIC_2 0x30000000
#define NOISE_MAGIC   5

extern int speedup;

int sound_enable = SND_EN_CH1|SND_EN_CH2|SND_EN_CH3|SND_EN_CH4;

byte sound_wave_pattern[4][32] = 
{
  {0x01,0x01,0x01,0x01,
   0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff},  
  {0x01,0x01,0x01,0x01,
   0x01,0x01,0x01,0x01,
   0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff},
  {0x01,0x01,0x01,0x01,
   0x01,0x01,0x01,0x01,
   0x01,0x01,0x01,0x01,
   0x01,0x01,0x01,0x01,
   0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff},
  {0x01,0x01,0x01,0x01,
   0x01,0x01,0x01,0x01,
   0x01,0x01,0x01,0x01,
   0x01,0x01,0x01,0x01,
   0x01,0x01,0x01,0x01,
   0x01,0x01,0x01,0x01,
   0xff,0xff,0xff,0xff,
   0xff,0xff,0xff,0xff}
};

int sound_freq_ratio[8] = 
{
  1048576, // 0
  524288,  // 1
  262144,  // 2
  174763,  // 3
  131072,  // 4
  104858,  // 5
  87381,   // 6
  74898    // 7
};

int sound_shift_clock[16]= 
{
      2, // 0
      4, // 1
      8, // 2
     16, // 3
     32, // 4
     64, // 5
    128, // 6
    256, // 7
    512, // 8
   1024, // 9
   2048, // 10
   4096, // 11
   8192, // 12
  16384, // 13
  1,     // 14
  1      // 15
};

int CYCLES_SOUND = 96;
//int cycles_sound = CYCLES_SOUND;
int sound_quality = 1;

int channel_n = 0;

FSOUND_SAMPLE* FSbuffer;

const int sound_buffer_len = 1470*2; 
int sound_buffer_total_len = 14700*2;
int sound_next_position = 0;

void gb_system::sound_register(register unsigned short address,register byte data)
{
   int freq = 0;

   switch(address)
   {
   // CHANNEL 1 CONTROLS
   //------------------------------------------------------------------------
   case NR10: // CH1 Sweep register
      channel1_sweepATLreload = channel1_sweepATL = 344 * ((data >> 4) & 7);
      channel1_sweepdir = data & 0x08;
      channel1_sweepsteps = data & 7;
   break;
   case NR11: // CH1 Sound Length/Wave Pattern duty
      channel1_wave = sound_wave_pattern[data >> 6];
      channel1_ATL = 172 * (64 - (data & 0x3F));
   break;
   case NR12: // CH1 Volume envelope
      if((data>>4)==0 && channel1_on && channel1_ATL <= 0) // in "zombie mode" and volume set to 0
      {
         memory[NR52] &= 0xFE;
         channel1_on = 0;
      } else
      {   
         if((channel1_envelopevolume==0 || channel1_envelopevolume==15) && (data >> 4)==0)
            channel1_on=0;
         channel1_envelopevolume = data >> 4;
         channel1_envelopedir = data & 0x08;
         channel1_envelopeATLreload = channel1_envelopeATL = 689 * (data & 7);
      }
   break;
   case NR13: // CH1 Frequency lo
      freq = (((int)(memory[NR14] & 7)) << 8) | data;
      channel1_ATL = 172 * (64 - (memory[NR11] & 0x3F));
      freq = 2048 - freq-1;     
      if(freq)
         channel1_skip = SOUND_MAGIC / freq;
      else
         channel1_skip = 0;
   break;
   case NR14: // CH1 Frequency hi
      freq = (memory[NR13]|((int)(data & 7) << 8));
      freq = 2048 - freq-1;
      channel1_ATL = 172 * (64 - (memory[NR11] & 0x3F));
      channel1_continue = data & 0x40;    
      if(freq)
         channel1_skip = SOUND_MAGIC / freq;
      else
         channel1_skip = 0;

      if(data&0x80)
      {
         memory[NR52] |= 1;
         channel1_envelopevolume = memory[NR12] >> 4;
         channel1_envelopedir = memory[NR12] & 0x08;
         channel1_ATL = 172 * (64 - (memory[NR11] & 0x3f));
         channel1_envelopeATLreload = channel1_envelopeATL = 689 * (memory[NR12] & 7);
         channel1_sweepATLreload = channel1_sweepATL = 344 * ((memory[NR10] >> 4) & 7);
         channel1_sweepsteps = memory[NR10] & 7;
         channel1_sweepdir = memory[NR10] & 0x08;

         channel1_index = 0;
         channel1_on = 1;
      }
   break;
   
   // CHANNEL 2 CONTROLS
   //---------------------------------------------------------------------
   case NR21: // CH2 Sound Length/Wave Pattern duty
      channel2_wave = sound_wave_pattern[data>>6];
      channel2_ATL = 172 * (64 - (data & 0x3f));  
   break;
   case NR22: // CH2 Volume envelope
      if((data>>4)==0 && channel2_on && channel2_ATL <= 0) // in "zombie mode" and volume set to 0
      {
         memory[NR52] &= 0xFD;
         channel2_on = 0;
      } else
      {
         channel2_envelopevolume = data>>4;
         channel2_envelopedir = data&0x08;
         channel2_envelopeATLreload = channel2_envelopeATL = 689 * (data&0x07);   
      }
   break;
   case NR23: // CH2 Frequency lo
      freq = (((int)(memory[NR24]&0x07)) << 8)|data;
      channel2_ATL = 172 * (64 - (memory[NR21] & 0x3F));
      freq = 2048 - freq-1;
      if(freq)
         channel2_skip = SOUND_MAGIC / freq;
      else
         channel2_skip = 0;
   break;
   case NR24: // CH2 Frequency hi
      freq = memory[NR23]|(((int)(data&0x07))<<8);
      channel2_ATL = 172 * (64 - (memory[NR21] & 0x3F));
      freq = 2048 - freq-1;  
      if(freq)
         channel2_skip = SOUND_MAGIC / freq;
      else
         channel2_skip = 0;
      channel2_continue = data & 0x40;
      if(data&0x80)
      {
         memory[NR52] |= 2;
         channel2_envelopevolume = memory[NR22] >> 4;
         channel2_envelopedir = memory[NR22] & 0x08;
         channel2_ATL = 172 * (64 - (memory[NR21] & 0x3f));
         channel2_envelopeATLreload = channel2_envelopeATL = 689 * (memory[NR22] & 7);
         
         channel2_index = 0;
         channel2_on = 1;
      }
   break;
   // CHANNEL 3 CONTROLS
   //------------------------------------------------------------------
   case NR30: // CH3 sound on/off
      if(!(data & 0x80)) 
      {
         memory[NR52] &= 0xFB;
         channel3_on = 0;
      } 
   break;
   case NR31: // CH3 sound length
      channel3_ATL = 172 * (256-data);
   break;
   case NR32: // CH3 select output level
      channel3_outputlevel = (data >> 5) & 3;
   break;
   case NR33: // CH3 Frequency lo
      freq = 2048-(((int)(memory[NR34]&0x07)<<8)|data)-1;
      if(freq)
         channel3_skip = SOUND_MAGIC_2 / freq;
      else
         channel3_skip = 0;
   break;
   case NR34: // CH3 Frequency hi
      freq = 2048-(memory[NR33]|(((int)(data&0x07))<<8))-1;
      if(freq)
         channel3_skip = SOUND_MAGIC_2 / freq;
      else
         channel3_skip = 0;
      channel3_continue = data & 0x40;
      if((data&0x80) && (memory[NR30]&0x80))
      {
         memory[NR52] |= 4;
         channel3_ATL = 172 * (256 - memory[NR31]);

         channel3_index = 0;
         channel3_on = 1;
      }
   break;
   // CHANNEL 4 CONTROLS
   //--------------------------------------------------------------
   case NR41: // CH4 sound length
      channel4_ATL  = 172 * (64 - (data & 0x3f));
   break;
   case NR42: // CH4 volume envelope
      channel4_envelopevolume = data >> 4;
      channel4_envelopedir = data & 0x08;
      channel4_envelopeATLreload = channel4_envelopeATL = 689 * (data & 7);   
   break;
   case NR43: // CH4 Polynomial counter
      freq = sound_freq_ratio[data & 7];
      channel4_Nsteps = data & 0x08;

      channel4_skip = (freq << 8) / NOISE_MAGIC;
    
      channel4_clock = data >> 4;

      freq = freq / sound_shift_clock[channel4_clock];

      channel4_shiftskip = (freq << 8) / NOISE_MAGIC;
      
   break;
   case NR44: // CH4 counter/consecutive; initial
      channel4_continue = data & 0x40;
      if(data & 0x80) 
      {
         memory[NR52] |= 8;
         channel4_envelopevolume = memory[NR42] >> 4;
         channel4_envelopedir = memory[NR42] & 0x08;
         channel4_ATL = 172 * (64 - (memory[NR41] & 0x3f));
         channel4_envelopeATLreload = channel4_envelopeATL = 689 * (memory[NR42] & 7);

         channel4_on = 1;
      
         channel4_index = 0;
         channel4_shiftindex = 0;
      
         freq = sound_freq_ratio[memory[NR43] & 7];

         channel4_skip = (freq << 8) / NOISE_MAGIC;
      
         channel4_Nsteps = memory[NR43] & 0x08;
      
         freq = freq / sound_shift_clock[memory[NR43] >> 4];

         channel4_shiftskip = (freq << 8) / NOISE_MAGIC;
         if(channel4_Nsteps)
            channel4_shiftright = 0x7fff;
         else
            channel4_shiftright = 0x7f;
      }   
   break;
   case NR50: // Channel Control / Volume
      sound_level1 = data&7;
      sound_level2 = (data >> 4) & 7;
   break;
   case NR51: // Selection of sound output terminal
      sound_balance = data;
   break;
   case NR52: // sound on/off
      sound_on = data&0x80;
      memory[NR52] = data&0x80;
      if(!sound_on)
      {
         channel1_on = 0;
         channel2_on = 0;
         channel3_on = 0;
         channel4_on = 0;
      }
   return;
   }
   memory[address] = data;
   
   sound_digital = 1;

   if(channel1_on && channel1_envelopevolume != 0)
      sound_digital = 0;
   if(channel2_on && channel2_envelopevolume != 0)
      sound_digital = 0;
   if(channel3_on && channel3_outputlevel != 0)
      sound_digital = 0;
   if(channel4_on && channel4_envelopevolume != 0)
      sound_digital = 0;   
}

void gb_system::channel1()
{
  int vol = channel1_envelopevolume;

  int freq = 0;

  int value = 0;
  
  if(channel1_on && (channel1_ATL || !channel1_continue)) 
  {
     channel1_index += sound_quality*channel1_skip;
     channel1_index &= 0x1fffffff;

     value = ((signed char)channel1_wave[channel1_index>>24]) * vol;
  }

  sound_buffer[0][sound_index] = value;

  
  if(channel1_on) 
  {
    if(channel1_ATL) 
    {
      channel1_ATL -= sound_quality;
      
      if(channel1_ATL <= 0 && channel1_continue) 
      {
        memory[NR52] &= 0xFE;
        channel1_on = 0;      
      }
    }
    
    if(channel1_envelopeATL) 
    {
      channel1_envelopeATL -= sound_quality;
      
      if(channel1_envelopeATL <= 0) 
      {
        if(channel1_envelopedir) 
        {
          if(channel1_envelopevolume < 15)
             channel1_envelopevolume++;
        } else 
        {
          if(channel1_envelopevolume)
             channel1_envelopevolume--;
        }
        
        channel1_envelopeATL += channel1_envelopeATLreload;
      }
    }
    
    if(channel1_sweepATL) 
    {
      channel1_sweepATL -= sound_quality;
      
      if(channel1_sweepATL <= 0) 
      {
        freq = (((int)(memory[NR14]&7) << 8) | memory[NR13]);
          
        int updown = 1;
        
        if(channel1_sweepdir)
          updown = -1;
        
        int newfreq = 0;
        if(channel1_sweepsteps) 
        {
          newfreq = freq + updown * freq / (1 << channel1_sweepsteps);
          if(newfreq == freq)
            newfreq = 0;
        } else
        {
          newfreq = freq;
          channel1_on = 0; //?    
        }
        
        if(newfreq < 0) 
        {
           channel1_sweepATL += channel1_sweepATLreload;
        } else if(newfreq > 2047) 
        {
           channel1_sweepATL = 0;
           channel1_on = 0;
           memory[NR52] &= 0xFE;
        } else 
        {
           channel1_sweepATL += channel1_sweepATLreload;
           channel1_skip = SOUND_MAGIC/(2048 - newfreq);
          
           memory[NR13] = newfreq & 0xff;
           memory[NR14] = (memory[NR14] & 0xf8) |((newfreq >> 8) & 7);
        }
      }
    }
  }
}

void gb_system::channel2()
{
   int value = 0;
   int vol = channel2_envelopevolume;
   
   if(channel2_on && (channel2_ATL || !channel2_continue))
   {
      channel2_index += sound_quality*channel2_skip;
      channel2_index &= 0x1fffffff;

      value = ((signed char)channel2_wave[channel2_index>>24]) * vol;
   }
   
   sound_buffer[1][sound_index] = value;
   
   if(channel2_on)
   {
      if(channel2_ATL)
      {
         channel2_ATL -= sound_quality;
      
         if(channel2_ATL <= 0 && channel2_continue)
         {
            memory[NR52] &= 0xFD;
            channel2_on = 0;
         }
      } 
      
      if(channel2_envelopeATL)
      {
         channel2_envelopeATL -= sound_quality;
         
         if(channel2_envelopeATL<=0)
         {
            if(channel2_envelopedir)
            {
               if(channel2_envelopevolume<15)
                  ++channel2_envelopevolume;
            } else
            {
               if(channel2_envelopevolume)
                  --channel2_envelopevolume;
            }
            channel2_envelopeATL += channel2_envelopeATLreload;      
         }
      }
   }
}

void gb_system::channel3()
{
  int value = channel3_last;
  
  if(channel3_on && (channel3_ATL || !channel3_continue)) 
  {
     channel3_index += sound_quality*channel3_skip;
     channel3_index &= 0x1fffffff;

     value = memory[0xFF30 + (channel3_index>>25)];
                
     if((channel3_index & 0x01000000))
        value &= 0x0F;
     else 
        value >>= 4;
        
     value -= 8;
    
     switch(channel3_outputlevel) 
     {
     case 0:
        value = 0;
     break;
     case 1:
        value <<= 1;
     break;
     case 2:
        //value <<= 1;//value = (value >> 1);
     break;
     case 3:
        value >>= 1;//value = (value >> 2);
     break;
     }
    
     channel3_last = value;
  }
  
  sound_buffer[2][sound_index] = value;
  
  if(channel3_on) 
  {
    if(channel3_ATL) 
    {
      channel3_ATL -= sound_quality;
      
      if(channel3_ATL <= 0 && channel3_continue) 
      {
        memory[NR52] &= 0xFB;
        channel3_on = 0;
      }
    }
  }
}

void gb_system::channel4()
{
  int vol = channel4_envelopevolume;

  int value = 0;

  if(channel4_clock <= 0x0C) 
  {
    if(channel4_on && (channel4_ATL || !channel4_continue)) 
    {
      channel4_index += sound_quality*channel4_skip;
      channel4_shiftindex += sound_quality*channel4_shiftskip;

      if(channel4_Nsteps) 
      {
        while(channel4_shiftindex > 0x1fffff) 
        {
          channel4_shiftright = (((channel4_shiftright << 6) ^
                                (channel4_shiftright << 5)) & 0x40) |
                                (channel4_shiftright >> 1);
          channel4_shiftindex -= 0x200000;
        }
      } else 
      {
        while(channel4_shiftindex > 0x1fffff) 
        {
          channel4_shiftright = (((channel4_shiftright << 14) ^
                               (channel4_shiftright << 13)) & 0x4000) |
                               (channel4_shiftright >> 1);
          channel4_shiftindex -= 0x200000;   
        }
      }

      channel4_index &= 0x1fffff;    
      channel4_shiftindex &= 0x1fffff;        
    
      value = ((channel4_shiftright & 1)*2-1) * vol;
    } else 
    {
      value = 0;
    }
  }
  
  sound_buffer[3][sound_index] = value;

  if(channel4_on) 
  {
    if(channel4_ATL) 
    {
      channel4_ATL -= sound_quality;
      
      if(channel4_ATL <= 0 && channel4_continue) 
      {
        memory[NR52] &= 0xF7;
        channel4_on = 0;
      }
    }
    
    if(channel4_envelopeATL) 
    {
      channel4_envelopeATL -= sound_quality;
      
      if(channel4_envelopeATL <= 0) 
      {
        if(channel4_envelopedir) 
        {
          if(channel4_envelopevolume < 15)
            channel4_envelopevolume++;
        } else 
        {
          if(channel4_envelopevolume)
            channel4_envelopevolume--;
        }
        channel4_envelopeATL += channel4_envelopeATLreload;
      }
    }
  }
}

void gb_system::sound_mix()
{
   int res = 0;

   if((sound_balance & 0x10) && (sound_enable&SND_EN_CH1)) 
      res += (signed char)sound_buffer[0][sound_index];
   
   if((sound_balance & 0x20) && (sound_enable&SND_EN_CH2)) 
      res += (signed char)sound_buffer[1][sound_index];

   if((sound_balance & 0x40) && (sound_enable&SND_EN_CH3)) 
      res += (signed char)sound_buffer[2][sound_index];

   if((sound_balance & 0x80) && (sound_enable&SND_EN_CH4)) 
      res += (signed char)sound_buffer[3][sound_index];
   
   if(sound_digital)
      res *= sound_level1*60;//res = sound_level1*256;
   else
      res *= sound_level1*60; 
   
   if(options->sound_lowpass_filter == LOWPASS_LEVEL2)
   {
      sound_left[4] = sound_left[3];
      sound_left[3] = sound_left[2];
      sound_left[2] = sound_left[1];
      sound_left[1] = sound_left[0];
      sound_left[0] = res;    
      res = (sound_left[4] + sound_left[3] + 2*sound_left[2] + sound_left[1] + sound_left[0])/6;
   } else
   if(options->sound_lowpass_filter == LOWPASS_LEVEL1)
   {
      sound_left[4] = sound_left[3];
      sound_left[3] = sound_left[2];
      sound_left[2] = sound_left[1];
      sound_left[1] = sound_left[0];
      sound_left[0] = res;    
      res = (sound_left[4] + 2*sound_left[3] + 8*sound_left[2] + 2*sound_left[1] + sound_left[0])/14;
   }
   
   res *= options->sound_volume;
   
   if(res > 32767)
      res = 32767;
   if(res < -32768)
      res = -32768;
      
   if(options->sound_reverse_stereo)
      final_wave[++sound_buffer_index] = res;   
   else
      final_wave[sound_buffer_index++] = res;
   
   res = 0;

   if((sound_balance & 0x01) && (sound_enable&SND_EN_CH1)) 
      res += (signed char)sound_buffer[0][sound_index];

   if((sound_balance & 0x02) && (sound_enable&SND_EN_CH2)) 
      res += (signed char)sound_buffer[1][sound_index];

   if((sound_balance & 0x04) && (sound_enable&SND_EN_CH3)) 
      res += (signed char)sound_buffer[2][sound_index];

   if((sound_balance & 0x08) && (sound_enable&SND_EN_CH4)) 
      res += (signed char)sound_buffer[3][sound_index];
   
   if(sound_digital)
      res *= sound_level2*60;//res = sound_level2*256;
   else
      res *= sound_level2*60; 
   
   if(options->sound_lowpass_filter == LOWPASS_LEVEL2)
   {
      sound_right[4] = sound_right[3];
      sound_right[3] = sound_right[2];
      sound_right[2] = sound_right[1];
      sound_right[1] = sound_right[0];
      sound_right[0] = res;    
      res = (sound_right[4] + sound_right[3] + 2*sound_right[2] + sound_right[1] + sound_right[0])/6;
   } else
   if(options->sound_lowpass_filter == LOWPASS_LEVEL1)
   {
      sound_right[4] = sound_right[3];
      sound_right[3] = sound_right[2];
      sound_right[2] = sound_right[1];
      sound_right[1] = sound_right[0];
      sound_right[0] = res;    
      res = (sound_right[4] + 2*sound_right[3] + 8*sound_right[2] + 2*sound_right[1] + sound_right[0])/14;
   }
   
   res *= options->sound_volume;
      
   if(res > 32767)
      res = 32767;
   if(res < -32768)
      res = -32768;
      
   if(options->sound_reverse_stereo)
      final_wave[-1+sound_buffer_index++] = res;   
   else
      final_wave[sound_buffer_index++] = res;
}

void gb_system::sound_event()
{
if(options->sound_on > 0 && this == GB1)
{
   if(sound_on)
   {
      channel1();
      channel2();
      channel3();
      channel4();

      sound_mix();
   } else
   {
      final_wave[sound_buffer_index++] = 0;
      final_wave[sound_buffer_index++] = 0;      
   }
   
   ++sound_index;
   
   if((2*sound_buffer_index) >= sound_buffer_len)
   {
      sound_index = 0;
      sound_buffer_index = 0;
      
      void* ptr1 = NULL; 
      unsigned int bytes1 = 0; 
      void* ptr2 = NULL; 
      unsigned int bytes2 = 0; 
               
      if(!speedup && FSOUND_IsPlaying(channel_n) == TRUE)
      {
         unsigned int play = 0;
                  
         for(;;)
         {
            play = FSOUND_GetCurrentPosition(channel_n);
            if(!play) 
               break;
            play <<= 2;
            if((play < sound_next_position) || (play > sound_next_position+sound_buffer_len))
               break;
            
           /* if(options->reduce_cpu_usage)
            {
               Sleep(1);
            }*/
         } 
      }
         
      if(FSOUND_Sample_Lock(FSbuffer,sound_next_position,sound_buffer_len,&ptr1,&ptr2,&bytes1,&bytes2) == FALSE)
         return;

      sound_next_position += sound_buffer_len;
      sound_next_position = sound_next_position % sound_buffer_total_len;

      memcpy(ptr1,final_wave,bytes1);
      if(ptr2 != NULL)
         memcpy(ptr2,final_wave+bytes1,bytes2);
      
      FSOUND_Sample_Unlock(FSbuffer,ptr1,ptr2,bytes1,bytes2);
   }
}
}

void gb_system::sound_reset()
{
   CYCLES_SOUND = sound_quality*94;
   cycles_sound = CYCLES_SOUND;
   sound_on = 1;
   sound_index = 0;
   sound_buffer_index = 0;
   sound_level1 = 7;
   sound_level2 = 7;
   sound_next_position = 0;
   
   channel1_on = 0;
   channel1_ATL = 0;
   channel1_skip = 0;
   channel1_continue = 0;
   channel1_sweepATL = 0;
   channel1_sweepATLreload = 0;
   channel1_sweepdir = 0;
   channel1_sweepsteps = 0;
   channel1_envelopevolume = 0;
   channel1_envelopeATL = 0;
   channel1_envelopeATLreload = 0;
   channel1_envelopedir = 0;
   channel1_index = 0;
   channel1_wave = sound_wave_pattern[2];

   channel2_on = 0;
   channel2_ATL = 0;
   channel2_skip = 0;
   channel2_continue = 0;
   channel2_envelopevolume = 0;
   channel2_envelopeATL = 0;
   channel2_envelopeATLreload = 0;
   channel2_envelopedir = 0;
   channel2_index = 0;
   channel2_wave = sound_wave_pattern[2];

   channel3_on = 0;
   channel3_ATL = 0;
   channel3_skip = 0;
   channel3_continue = 0;
   channel3_index = 0;
   channel3_outputlevel = 0;
   
   channel4_on = 0;
   channel4_clock = 0;
   channel4_ATL = 0;
   channel4_skip = 0;
   channel4_index = 0;
   channel4_shiftright = 0x7F;
   channel4_Nsteps = 0;
   channel4_countdown = 0;
   channel4_continue = 0;
   channel4_envelopevolume =  0;
   channel4_envelopeATL = 0;
   channel4_envelopedir = 0;
   channel4_envelopeATLreload = 0;
  
   sound_register(0xff10, 0x80);
   sound_register(0xff11, 0xbf);
   sound_register(0xff12, 0xf3);
   sound_register(0xff14, 0xbf);
   sound_register(0xff16, 0x3f);
   sound_register(0xff17, 0x00);
   sound_register(0xff19, 0xbf);

   sound_register(0xff1a, 0x7f);  
   sound_register(0xff1b, 0xff);
   sound_register(0xff1c, 0x9f);
   sound_register(0xff1e, 0xbf);    

   sound_register(0xff20, 0xff);
   sound_register(0xff21, 0x00);
   sound_register(0xff22, 0x00);
   sound_register(0xff23, 0xbf);
   sound_register(0xff24, 0x77);
   sound_register(0xff25, 0xf3);

   sound_register(0xff26, 0xf0);
   
   channel1_on = 0;
   channel2_on = 0;
   channel3_on = 0;
   channel4_on = 0;

   byte gb_init_wav[]={0x06,0xFE,0x0E,0x7F,0x00,0xFF,0x58,0xDF,0x00,0xEC,0x00,0xBF,0x0C,0xED,0x03,0xF7};
 /*  {
   	0xac, 0xdd, 0xda, 0x48,
	0x36, 0x02, 0xcf, 0x16,
	0x2c, 0x04, 0xe5, 0x2c,
	0xac, 0xdd, 0xda, 0x48
   };*/
   int addr = 0xff30;
   if(!gbc_mode)
   {
      for(int i=0;addr < 0xff40; ++i,++addr)
         memory[addr] = gb_init_wav[i];
   }else
   {
      while(addr < 0xff40) 
      {
         memory[addr++] = 0x00;
         memory[addr++] = 0xff;
      }
   }
  
   memset(final_wave,0x00,sound_buffer_len);
}
