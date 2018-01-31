/*
   hhugboy Game Boy emulator
   copyright 2013 taizou

   Based on GEST
   Copyright (C) 2003-2010 TM

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
#include <stdlib.h>
#include <time.h>

#include "rendering/render.h"
#include "GB.h"
#include "SGB.h"
#include "cpu.h"
#include "debug.h"
#include "mainloop.h"
#include "GB_gfx.h"
#include "config.h"
#include "sound.h"
#include "devices.h"
#include "main.h"
#include "memory/GB_MBC.h"
#include "ui/window.h"

int gb_system::gfx_bit_count = 16;

gb_system::gb_system():
        mbc(new gb_mbc(mem_map,&cartridge,&rom,&cartRAM,&rumble_counter,&memory)),
        frames(0),
        LCD_clear_needed(false),
        skip_frame(0),
        off_counter(0),
        serialbits(0),
        IWait(0),
        STAT_wait(0),
        draw_wait(0),
        frameskip_counter(0),
        mode3_cyc(0),
        wy_set(-1),
        IME(0),
        CPUHalt(0),
        gb_speed(0),
        int_line(-1),
        timer_freq(CYCLES_TIMER_MODE0),
        cycles_DIV(CYCLES_DIV),
        cycles_timer(CYCLES_TIMER_MODE0),
        cycles_LCD(CYCLES_LCD_MODE2),
        cycles_serial(0),
        cycles_sound(0),
        EI_count(0),
        timeron(0),
        LCDon(1),
        tile_map(0),
        tile_pattern(0),
        win_tile_map(0),
        sprite_number(0),
        windowline(-1),
        memory(NULL),
        memory_another(NULL),
        cartRAM(NULL),
        VRAM(NULL),
        WRAM(NULL),
        cartridge(NULL),
        hdma_source(0x0000),
        hdma_destination(0x8000),
        hdma_bytes(0),
        hdma_on(0),
        gdma_rest(0),
        gfx_buffer1(NULL),
        gfx_buffer2(NULL),
        gfx_buffer3(NULL),
        gfx_buffer4(NULL),
        romloaded(false),
        system_type(SYS_AUTO),
        rom(NULL),
        gbc_mode(0),
        A(1),
        F(0xB0),
        ZFLAG(1),
        HFLAG(1),
        CFLAG(1),
        NFLAG(1),
        //flags(0x5100),
        CYCLES_SERIAL(CYCLES_SERIAL_GB),
        wram_bank(1),
        vram_bank(0),

        sound_on(1),
        sound_index(0),
        sound_buffer_index(0),
        sound_balance(0),
        sound_level1(0),
        sound_level2(0),
        sound_digital(0),

        channel1_on(0),
        channel1_ATL(0),
        channel1_skip(0),
        channel1_continue(0),
        channel1_sweepATL(0),
        channel1_sweepATLreload(0),
        channel1_sweepdir(0),
        channel1_sweepsteps(0),
        channel1_envelopevolume(0),
        channel1_envelopeATL(0),
        channel1_envelopeATLreload(0),
        channel1_envelopedir(0),
        channel1_index(0),
        channel1_wave(sound_wave_pattern[2]),

        channel2_on(0),
        channel2_ATL(0),
        channel2_skip(0),
        channel2_continue(0),
        channel2_envelopevolume(0),
        channel2_envelopeATL(0),
        channel2_envelopeATLreload(0),
        channel2_envelopedir(0),
        channel2_index(0),
        channel2_wave(sound_wave_pattern[2]),

        channel3_on(0),
        channel3_continue(0),
        channel3_ATL(0),
        channel3_skip(0),
        channel3_index(0),
        channel3_last(0),
        channel3_outputlevel(0),

        channel4_on(0),
        channel4_clock(0),
        channel4_ATL(0),
        channel4_skip(0),
        channel4_index(0),
        channel4_shiftright(0x7F),
        channel4_shiftskip(0),
        channel4_shiftindex(0),
        channel4_Nsteps(0),
        channel4_countdown(0),
        channel4_continue(0),
        channel4_envelopevolume(0),
        channel4_envelopeATL(0),
        channel4_envelopedir(0),
        channel4_envelopeATLreload(0),

        rumble_counter(0),

        romFileSize(0)
{
   button_pressed[B_LEFT]=button_pressed[B_RIGHT]=button_pressed[B_DOWN]=button_pressed[B_UP]=1;
   button_pressed[B_START]=button_pressed[B_SELECT]=button_pressed[B_A]=button_pressed[B_B]=1;

   last_button_pressed[B_LEFT]=last_button_pressed[B_RIGHT]=last_button_pressed[B_DOWN]=last_button_pressed[B_UP]=1;
   last_button_pressed[B_START]=last_button_pressed[B_SELECT]=last_button_pressed[B_A]=last_button_pressed[B_B]=1;

   BC.W = 0x0013;
   DE.W = 0x00D8;
   HL.W = 0x014D;
   SP.W = 0xFFFE;
   PC.W = 0x0100;
   
   BGP[0] = 0;
   BGP[1] = 3;
   BGP[2] = 3;
   BGP[3] = 3;         
   
   OBP0[0] = 3;
   OBP0[1] = 3;
   OBP0[2] = 3;
   OBP0[3] = 3;     
   
   OBP1[0] = 3;    
   OBP1[1] = 3;
   OBP1[2] = 3;
   OBP1[3] = 3;         
}

bool gb_system::init()
{
   memory = new byte[65536];
   if(!memory) 
      return false;
      
   cartRAM = new byte[256*1024];
   if(!cartRAM) 
      return false;
   memset(cartRAM,0xFF,128*1024);
   
   VRAM = new byte[16384];
   if(!VRAM)
      return false;
      
   WRAM = new byte[32768];
   if(!WRAM)
      return false;
      
   rom = new GBrom;
   if(!rom)
      return false;
           
   return true;
}

bool gb_system::init_gfx()
{
   if(gfx_bit_count == 16)
   {
      gfx_buffer1 = new WORD[160*144];
      gfx_buffer2 = new WORD[160*144];
      gfx_buffer3 = new WORD[160*144];
      gfx_buffer4 = new WORD[160*144];
   } else
   {
      gfx_buffer1 = new DWORD[160*144];
      gfx_buffer2 = new DWORD[160*144];
      gfx_buffer3 = new DWORD[160*144];
      gfx_buffer4 = new DWORD[160*144];
   }      
   if(gfx_buffer1 == NULL || gfx_buffer2 == NULL || gfx_buffer3 == NULL || gfx_buffer4 == NULL)
      return false;   
      
   gfx_buffer = gfx_buffer1;
   gfx_buffer_old = gfx_buffer2;
   gfx_buffer_older = gfx_buffer3;
   gfx_buffer_oldest = gfx_buffer4;
      
   fill_gfx_buffers(0UL); 
   
   return true;
}

gb_system::~gb_system() 
{
   if(memory != NULL)
   {
      delete [] memory;
      memory = NULL;
   }
   if(cartRAM != NULL)
   {
      delete [] cartRAM;
      cartRAM = NULL;
   }
   if(VRAM != NULL)
   {
      delete [] VRAM;
      VRAM = NULL;
   }
   if(WRAM != NULL)
   {
      delete [] WRAM;
      WRAM = NULL;
   }      
   if(gfx_buffer1 != NULL)
   {
      if(gfx_bit_count == 16)
         delete [] (WORD*)gfx_buffer1;
      else
         delete [] (DWORD*)gfx_buffer1;
      gfx_buffer1 = NULL;
   }      
   if(gfx_buffer2 != NULL)
   {
      if(gfx_bit_count == 16)
         delete [] (WORD*)gfx_buffer2;
      else
         delete [] (DWORD*)gfx_buffer2;
      gfx_buffer2 = NULL;
   }
   if(gfx_buffer3 != NULL)
   {
      if(gfx_bit_count == 16)
         delete [] (WORD*)gfx_buffer3;
      else
         delete [] (DWORD*)gfx_buffer3;
      gfx_buffer3 = NULL;
   }
   if(gfx_buffer4 != NULL)
   {
      if(gfx_bit_count == 16)
         delete [] (WORD*)gfx_buffer4;
      else
         delete [] (DWORD*)gfx_buffer4;
      gfx_buffer4 = NULL;
   }
   if(cartridge != NULL)
   {
      delete [] cartridge;
      cartridge = NULL;
   }
   if(rom != NULL)
   {
      delete rom;
      rom = NULL;
   }
}

void gb_system::reset(bool change_mode, bool preserveMulticartState)
{
   int old_sgb_mode = sgb_mode;
   int old_gbc_mode = gbc_mode;

   emulating = true;

    int cgbState = rom->CGB;
    int sgbState = rom->SGB;

   
   //change mode according to user selection
   if(change_mode == false)
   {
     ; // do nothing
   } else {

       if ( preserveMulticartState ) { // It's a multicart resetting so we may need to change modes..
           byte cgbFlag = mbc->readmemory_cart( 0x0143 );
           if(cgbFlag == 0x80)
               cgbState = 1;
           else if(cgbFlag == 0xC0)
               cgbState = 2; // gbc only
           else
               cgbState = 0;
           byte sgbFlag = mbc->readmemory_cart( 0x0146 );
           if(sgbFlag == 0x03)
               sgbState = 1;
           else
               sgbState = 0;
       }

       if(system_type == SYS_GBP || system_type == SYS_GB)
           sgb_mode = gbc_mode = 0;
       else
       if(system_type == SYS_GBC || system_type == SYS_GBA)
       {
           gbc_mode = 1;
           sgb_mode = 0;
           if(options->GBC_SGB_border == GBC_WITH_SGB_BORDER && sgbState)
           {
               sgb_mode = 1;
               gbc_mode = 1;
           } else
           if(options->GBC_SGB_border == GBC_WITH_INITIAL_SGB_BORDER && sgbState)
           {
               sgb_mode = 1;
               gbc_mode = 0;
           }
       }
       else
       if((system_type == SYS_SGB || system_type == SYS_SGB2) || (options->GBC_SGB_border && cgbState && sgbState))
       {
           sgb_mode = 1;
           gbc_mode = 0;
           if(cgbState && options->GBC_SGB_border == GBC_WITH_SGB_BORDER)
               gbc_mode = 1;
       }
       else
       {
           sgb_mode = gbc_mode = 0;

           if(cgbState)
               gbc_mode = 1;

           if(sgbState && !gbc_mode)
               sgb_mode = 1;
       }

   }
   if(change_mode)
      border_uploaded = 0;
   
   if(multiple_gb && sgb_mode) // don't allow SGB mode
   {
      sgb_mode = 0;
      if(cgbState) gbc_mode = 1;
   }
         
   if(sgb_mode)
   {
      fill_gfx_buffers(0UL);             
   } else   
   if(gbc_mode)
   {
      if(gfx_bit_count==16)
         fill_gfx_buffers(palette.gfxPal16[0x7fff]|(palette.gfxPal16[0x7fff]<<16)); 
      else
         fill_gfx_buffers(palette.gfxPal32[0x7fff]);               
   } else   
   {
      if(gfx_bit_count==16)
         fill_gfx_buffers(palette.gfxPal16[GBC_BGP[0]]|(palette.gfxPal16[GBC_BGP[0]]<<16)); 
      else
         fill_gfx_buffers(palette.gfxPal32[GBC_BGP[0]]);                 
   }
           
   if(options->video_GBCBGA_real_colors && ((old_gbc_mode^gbc_mode) || system_type == SYS_GBC || system_type == SYS_GBA || system_type == SYS_AUTO))
      palette.mixGbcColours();
   
   //resize the window
   if(!old_sgb_mode && sgb_mode)
   {
      setWinSize(256,224);
   } else 
   if(old_sgb_mode && !sgb_mode)
   {
      setWinSize(160,144);                   
   }

   button_pressed[B_LEFT]=button_pressed[B_RIGHT]=button_pressed[B_DOWN]=button_pressed[B_UP]=1;
   button_pressed[B_START]=button_pressed[B_SELECT]=button_pressed[B_A]=button_pressed[B_B]=1;

   sensor_dir[0] = sensor_dir[1] = sensor_dir[2] = sensor_dir[3] = 0;

   IWait = 0;
   STAT_wait = 0;
   draw_wait = 0;
      
   sprite_number = 0;
   off_counter = 0;
   serialbits = 0;
   
   timeron = 0;
   LCDon = 1;
   
   windowline = -1;
   
   hdma_on = 0;
   hdma_source = 0;
   hdma_destination = 0x8000;
   hdma_bytes = 0;
   gdma_rest = 0;

   frames = 0;

   mem_reset(preserveMulticartState);
   
   cpu_reset();

   sound_reset();

    mbc->resetMbcVariables(preserveMulticartState);

   reset_devices();

   sgb_mask = 0;
   speedup = 0;
   sgb_multiplayer = 0;

   if(sgb_mode)
      sgb_reset();

   if(sgb_mode) (renderer.*renderer.drawBorder)();

    renderer.handleWindowResize();
}

void gb_system::cpu_reset()
{   
   A = 0x01;
   F = 0xB0;
   ZFLAG = HFLAG = CFLAG = 1;
   NFLAG = 0;
//   flags = 0x5100;
      
   BC.W = 0x0013;
   DE.W = 0x00D8;
   HL.W = 0x014D;
   SP.W = 0xFFFE;
   PC.W = 0x0100;
   
   CPUHalt = 0;
   IME = 0;
   EI_count = 0; 
   int_line = -1;
   
   cycles_DIV = CYCLES_DIV;
   cycles_timer = CYCLES_TIMER_MODE0;
   cycles_LCD = CYCLES_LCD_MODE2;
   cycles_serial = 0;  
   gb_speed = 0;
   CYCLES_SERIAL = CYCLES_SERIAL_GB;
   
   if(system_type == SYS_GBA)
      BC.W = 0x0113;
   else
   if(system_type == SYS_GBP || system_type == SYS_SGB2)
      A = 0xFF;
   
   if(gbc_mode)
      A = 0x11;
}

void gb_system::do_hdma()
{
  copy_memory(hdma_destination,hdma_source,0x10);

  hdma_destination += 0x10;
  hdma_source += 0x10;
  
  memory[0xFF52] += 0x10;
  if(memory[0xFF52] == 0x00)
    ++memory[0xFF51];
  
  memory[0xFF54] += 0x10;
  if(memory[0xFF54] == 0x00)
    ++memory[0xFF53];
      
  hdma_bytes -= 0x10;
  --memory[0xFF55];

  if(memory[0xFF55] == 0xff)
    hdma_on = 0;
}

void gb_system::handle_interrupt()
{
   CPUHalt = IME = EI_count = 0;
   PUSH(PC.B.h); 
   PUSH(PC.B.l);       

   if(memory[0xFFFF]&memory[0xFF0F]&0x01)  // V-Blank P 1 0x0040
   { 
      memory[0xFF0F] &= 0xFE;
      PC.W = 0x0040;
      return;
   }  
  
   if((memory[0xFFFF]&memory[0xFF0F]&0x02))  // LCDC stat P 2 0x0048
   {
      memory[0xFF0F] &= 0xFD;
      PC.W = 0x0048;
      return;
   } 
               
   if(memory[0xFFFF]&memory[0xFF0F]&0x04)  // Timer Overflow P 3 0x0050
   {
      memory[0xFF0F] &= 0xFB;
      PC.W = 0x0050;
      return;
   } 
      
   if(memory[0xFFFF]&memory[0xFF0F]&0x08)  // Serial Transfer P 4 0x0058
   {
      memory[0xFF0F] &= 0xF7;
      PC.W = 0x0058;
      return;
   }
   
   if(memory[0xFFFF]&memory[0xFF0F]&0x10)  // Hi-Lo Pin(button press) P 5 0x0060
   {
      memory[0xFF0F] &= 0xEF;
      PC.W = 0x0060;
      return;
   }   
}

