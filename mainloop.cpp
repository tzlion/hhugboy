/*
   hhugboy Game Boy emulator
   copyright 2013 taizou

   Based on GEST
   Copyright (C) 2003-2010 TM
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
#include <string.h>
#include <stdio.h>

#include <iostream>
using namespace std;

#include "mainloop.h"
#include "render.h"
#include "types.h"
#include "debug.h"
#include "cpu.h"
#include "rom.h"
#include "GB_gfx.h"
#include "SGB.h"
#include "sound.h"
#include "devices.h"
#include "GB.h"
#include "directinput.h"
//#include "directdraw.h"
#include "config.h"

#include "strings.h"

int sensor_dir[4];
int sensorX = 2047;
int sensorY = 2047;

//int frames = 0;

int cycles_to_complete_frame = 70224;

int real_cycle = 0; // for gbc 2xspeed mode

void check_sensor()
{
      if(sensor_dir[SENSOR_LEFT])
      {
         sensorX += 3;
         if(sensorX > 2197)
            sensorX = 2197;
         if(sensorX < 2047)
            sensorX = 2057;
      } else if(sensor_dir[SENSOR_RIGHT])
      {
         sensorX -= 3;
         if(sensorX < 1897)
            sensorX = 1897;
         if(sensorX > 2047)
            sensorX = 2037;
      } else if(sensorX > 2047)
      {
         sensorX -= 2;
         if(sensorX < 2047)
            sensorX = 2047;
      } else
      {
         sensorX += 2;
         if(sensorX > 2047)
            sensorX = 2047;
      }

      if(sensor_dir[SENSOR_UP])
      {
         sensorY += 3;
         if(sensorY > 2197)
            sensorY = 2197;
         if(sensorY < 2047)
            sensorY = 2057;
      } else if(sensor_dir[SENSOR_DOWN])
      {
         sensorY -= 3;
         if(sensorY < 1897)
            sensorY = 1897;
         if(sensorY > 2047)
            sensorY = 2037;
      } else if(sensorY > 2047)
      {
         sensorY -= 2;
         if(sensorY < 2047)
            sensorY = 2047;
      } else
      {
         sensorY += 2;
         if(sensorY > 2047)
            sensorY = 2047;
      }
}

////////////////////////////////////////
//int mainloop()
//-the emulator main loop
////////////////////////////////////////
int data_trans = 0;
byte data_sent = 0;

void gb_system::mainloop()
{
   if(multiple_gb && data_trans && (memory[0xFF02]&0x80) && !(memory[0xFF02]&1))
   {
      memory[0xFF01] = data_sent;
      cycles_serial = 0;
      memory[0xFF02] &= 0x7F;
      set_int(8);
      serialbits = 0;
      data_trans = 0;
   }

   if(CPUHalt == HALT || gdma_rest)
   { 
      cur_cycle = (4>>gb_speed);
      real_cycle = 4;
      if(gdma_rest)
      {
         gdma_rest -= cur_cycle;
         if(gdma_rest < 0)
            gdma_rest = 0;
      }
   } else 
   { 
      opcode = readopcode(PC.W++);
      


      cur_cycle = cycles[opcode];    

      if(CPUHalt == HALT2)
      {
         CPUHalt = 0;
         --PC.W;
      }
      
      #include "cpucore.h"

      real_cycle = cur_cycle;
      cur_cycle >>= gb_speed;
   }

   if(sgb_mode && cycles_SGB) // SGB command timeout
   {
      cycles_SGB -= cur_cycle;
      if(cycles_SGB <= 0)
      {
         cycles_SGB = 0;
         sgb_reset_state();

         cycles_SGB += CYCLES_SGB_TIMEOUT;
      }
   }
   
   cycles_DIV -= real_cycle;
   if(cycles_DIV <= 0)
   {         
      ++memory[0xFF04];
      cycles_DIV += CYCLES_DIV; 
   }               
         
   if(timeron) // Timer emulation
   {
      cycles_timer -= real_cycle;
      if(cycles_timer <= 0)
      {
         ++memory[0xFF05];
         if(memory[0xFF05] == 0x00)
         {
            memory[0xFF05] = memory[0xFF06]; // Load timer modulo
            set_int(4); // Timer interrupt
         }
         cycles_timer += timer_freq; 
      }
   }

   if(LCDon) // LCD emulation
   {
      cycles_LCD -= cur_cycle;
      cycles_to_complete_frame -= cur_cycle;

      if(cycles_LCD <= 0)
      {         
         switch(memory[0xFF41]&0x03) // Which mode has ENDED?
         { 
         case 0: // H-Blank         
            ++memory[0xFF44];
            compareLYCtoLY();

            if(memory[0xFF44] >= 144) // time for V-Blank?
            {                                                
               set_LCD_mode(1); // next: MODE 1
                                                  
               int r_frame_skip = (speedup && options->speedup_skip_9frames ? 9 : options->video_frameskip);
               if(!skip_frame && !sgb_mask && !(frameskip_counter%(r_frame_skip+1)))              
                  draw_screen();
                  
               if(wy_set != -1)
               {
                  memory[0xFF4A] = wy_set;
                  wy_set = -1;
               }
                  
               if(skip_frame)
                  --skip_frame;
   
               ++frameskip_counter;
               
               ++frames;

               cycles_to_complete_frame += 70224;

               windowline = -1;               
               
               IWait = 1;          

               if(gbc_mode)
                  cycles_LCD += 22;
               else
                  cycles_LCD += 28;      
               break;
            }  
                          
            set_LCD_mode(2); // next: MODE 2
            cycles_LCD += CYCLES_LCD_MODE2;

            if((memory[0xFF41] & 0x40) && (memory[0xFF41] & 0x04))
               STAT_wait = 36; 
   
            if(!(memory[0xFF41] & 0x40) && (memory[0xFF41]&0x20))
               set_int(2);                                 
         break;
         case 1: // V-Blank                                                            			
            if(IWait == 1)
            {
               int_line = -1;
               if(gbc_mode)
		  cycles_LCD += 434;	  
               else
                  cycles_LCD += 428;
                           
               set_int(1);
                              
               if(memory[0xFF41]&0x10)
                  set_int(2);
        
               if((memory[0xFF41] & 0x40) && (memory[0xFF41] & 0x04))
                  set_int(2);
               
               if(!sgb_multiplayer)
                  Check_KBInput(0);
               else
                  check_system_keys();
                  
                // joypad interrupt
               if((memory[0xFF00]&0x10) || ((memory[0xFF00]&0x30) == 0x00))
               {
                  if(!button_pressed[B_SELECT] || !button_pressed[B_START] || !button_pressed[B_B] || !button_pressed[B_A])
                     set_int(0x10); 
                  else 
                  if(!last_button_pressed[B_SELECT] || !last_button_pressed[B_START] || !last_button_pressed[B_B] || !last_button_pressed[B_A])
                     set_int(0x10);                            
               }
               if((memory[0xFF00]&0x20) || ((memory[0xFF00]&0x30) == 0x00))
               {
                  if(!button_pressed[B_UP] || !button_pressed[B_DOWN] || !button_pressed[B_LEFT] || !button_pressed[B_RIGHT])
                     set_int(0x10);               
                  else 
                  if(!last_button_pressed[B_UP] || !last_button_pressed[B_DOWN] || !last_button_pressed[B_LEFT] || !last_button_pressed[B_RIGHT])
                     set_int(0x10);                                    
               }
               memcpy(last_button_pressed,button_pressed,8*sizeof(int));    
        
               IWait = 0;
			   
               if(rom->bankType == MBC7)
                  check_sensor();
			   
               break;
            }

            if(memory[0xFF44] == 0)
            {
               //compareLYCtoLY();

               set_LCD_mode(2); // next: MODE 2
               cycles_LCD += CYCLES_LCD_MODE2;
 
               //if(memory[0xFF41]&0x20)
               //   set_int(2);

               //if((memory[0xFF41] & 0x40) && (memory[0xFF41] & 0x04))
               //   set_int(2);
               break;
            } 
            
            if(memory[0xFF44] < 153)
            {
               ++memory[0xFF44];
               if(memory[0xFF44] >= 153)
                  cycles_LCD += 24;
               else
                  cycles_LCD += CYCLES_LCD_MODE1;
            } else
            { 
               memory[0xFF44] = 0;         
               cycles_LCD += 432+18;
            }
            
            if(gbc_mode && memory[0xFF44] >= 153 && !(memory[0xFF41] & 0x40) && (memory[0xFF41]&0x08))
               STAT_wait = CYCLES_LCD_MODE0-58;

            compareLYCtoLY();

            if((memory[0xFF41] & 0x04) && (memory[0xFF41] & 0x40))
               set_int(2);
         break;
         case 2: // searching OAM-RAM       
            draw_wait = 65; //48 //54  

            if(memory[0xFF45] == memory[0xFF44] && memory[0xFF4A] > 130 && memory[0xFF4A] < 144)
               draw_wait = 180-cycles_LCD; // ???? // fix for Parodius

            set_LCD_mode(3); // next: MODE 3
            mode3_cyc = (CYCLES_LCD_MODE3);//+12*sprite_number);
            cycles_LCD += mode3_cyc;               
         break;
         case 3: // transferring data to LCD driver      
            set_LCD_mode(0); // next: MODE0 
            cycles_LCD += (CYCLES_LCD_MODE0-mode3_cyc);

            if(memory[0xFF41]&0x08)
               set_int(2);

            if(hdma_on)
               do_hdma();                                           
         break;     
         }     
      }
   } else // LCD is off
   {
      off_counter -= cur_cycle;
      cycles_to_complete_frame -= cur_cycle;
      
      if(off_counter <= 0)
      {
        /* if(++memory[0xFF44] >= 153)
            memory[0xFF44] = 0;
        */
         off_counter += 456;
      }

      memory[0xFF41] &= 0xFC;

      if(off_counter >= 353) // Hack: FREEART Intro V3
      {
         memory[0xFF41] |= 0x02;
      }

      // Clear LCD ---------------
      if(LCD_clear_needed)
      {
         LCD_clear_needed = false;
         if(!sgb_mask && options->video_LCDoff_clear_screen)
         {
            if(sgb_mode)
            {
               LCDoff_fill_gfx_buffer(0UL);
            } else
            if(gbc_mode)
            {
               if(gfx_bit_count == 16)
                  LCDoff_fill_gfx_buffer(gfx_pal16[0x7fff]|(gfx_pal16[0x7fff]<<16));
               else
                  LCDoff_fill_gfx_buffer(gfx_pal32[0x7fff]);
            }
            else
            {
               if(gfx_bit_count == 16)
                  LCDoff_fill_gfx_buffer(gfx_pal16[GBC_BGP[0]]|(gfx_pal16[GBC_BGP[0]]<<16));
               else
                  LCDoff_fill_gfx_buffer(gfx_pal32[GBC_BGP[0]]);
            }
         }

         if(!sgb_mask)
         {
            draw_screen();
         }
      }

      if(cycles_to_complete_frame <= 0)
      {
         ++frames;
         cycles_to_complete_frame += 70224;
         
         if(!sgb_multiplayer)
            Check_KBInput(0);
         else
            check_system_keys();
            
         if(!sgb_mask && options->video_LCDoff_clear_screen)
         {      
            if(sgb_mode)
            {
               LCDoff_fill_gfx_buffer(0UL);
            } else   
            if(gbc_mode)
            {
               if(gfx_bit_count == 16)
                  LCDoff_fill_gfx_buffer(gfx_pal16[0x7fff]|(gfx_pal16[0x7fff]<<16));
               else
                  LCDoff_fill_gfx_buffer(gfx_pal32[0x7fff]);
            }
            else
            {
               if(gfx_bit_count == 16)
                  LCDoff_fill_gfx_buffer(gfx_pal16[GBC_BGP[0]]|(gfx_pal16[GBC_BGP[0]]<<16));
               else
                  LCDoff_fill_gfx_buffer(gfx_pal32[GBC_BGP[0]]);
            }
         }

         if(!sgb_mask)
         {
             draw_screen();
         }
      }
   }
   
   if(memory[0xFF02]&0x80) // is serial on?
   {
      if(memory[0xFF02]&1) // internal clock
      {     
         cycles_serial -= real_cycle;
         if(cycles_serial <= 0) // end of transmission
         {
            unsigned short data = 0;
               
            if(multiple_gb && (memory_another[0xFF02]&0x80) && !(memory_another[0xFF02]&0x01))
            {
               data_sent = memory[0xFF01];
               data = memory_another[0xFF01];
               
               data_trans = 1;
            }
            else
               data = serial_function(0); 

            memory[0xFF01] = data;
            cycles_serial = 0;
            memory[0xFF02] &= 0x7F;
            set_int(8);
            serialbits = 0;
         }
      } else if((connected_device != DEVICE_NONE && external_clock))
      {
         cycles_serial -= real_cycle;
         if(cycles_serial <= 0)
         {
            //++serialbits;
            //if(serialbits == 8) // end of transmission
            //{
               unsigned short data = 0xffff;
               data = serial_function(1); 
                  
               memory[0xFF01] = data;  
               cycles_serial = 0;
               memory[0xFF02] &= 0x7F;
               set_int(8);
               serialbits = 0;
               //data_sent = 0;     
            //} else
            //   cycles_serial += CYCLES_SERIAL;
         }    
      }
   }
   
   if(draw_wait && !sgb_mask)
   {
      draw_wait -= cur_cycle;
      if(draw_wait <= 0)
      {
         draw_wait = 0;
         int r_frame_skip = (speedup && options->speedup_skip_9frames ? 9 : options->video_frameskip);
         if(memory[0xFF44] < 144 && !(frameskip_counter%(r_frame_skip+1)) && !skip_frame)
         {
            if(gbc_mode)
                draw_line_tile_GBC();
            else
                draw_line_tile_DMG();

            sprite_number = 0;
            if((memory[0xFF40] & 0x02) && (video_enable&VID_EN_SPRITE) && !skip_frame)
               draw_sprites();    
         }
      }
   }
   
   if(STAT_wait)
   {
      STAT_wait -= cur_cycle;
      if(STAT_wait <= 0)
      {            
         STAT_wait = 0;
         set_int(2);
      } 
   }

   cycles_sound -= cur_cycle;
   if(cycles_sound <= 0)
   {
      cycles_sound += CYCLES_SOUND;
      
      sound_event();
   }
 
   if(IME && (memory[0xFF0F]&memory[0xFFFF]))
      handle_interrupt();

   if(EI_count)
   {
      IME = 1;
      EI_count = 0;
   }     
}
