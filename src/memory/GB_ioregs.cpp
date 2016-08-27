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
 
//#include "ioregs.h"
#include <stdio.h>
#include <stdlib.h>
//#include <iostream>

//using namespace std;

#include "../devices.h"
#include "../cpu.h"
#include "../SGB.h"
#include "../directinput.h"
#include "../sound.h"
#include "../GB.h"
#include "../mainloop.h"

// I/O Regs read common for all readmemory functions
byte gb_system::io_reg_read(register unsigned short address)
{         
   if(address >= 0xFF00)
   {
      switch(address&0x00FF)
      {
      case 0x00:
      {
         if(sgb_mode) 
         {
            sgb_readingcontrol |= 4;
            sgb_reset_state();
                     
            if(sgb_multiplayer)
            {
               switch(sgb_nextcontrol)
               {
               case 0x0F:
                  Check_KBInput(0);
               break;
               case 0x0E:
                  Check_KBInput(1);
               break;
               case 0x0D:
                  Check_KBInput(2);
               break;
               case 0x0C:
                  Check_KBInput(3);
               break;
               }
            }
         } 
         
         if((memory[0xFF00]&0x30) == 0x00) //P14 and P15 not set
            memory[0xFF00] = (0xC0|(button_pressed[B_A]|button_pressed[B_RIGHT])|((button_pressed[B_B]|button_pressed[B_LEFT])<<1)|((button_pressed[B_SELECT]|button_pressed[B_UP])<<2)|((button_pressed[B_START]|button_pressed[B_DOWN])<<3));         
         else 
         if((memory[0xFF00]&0x30) == 0x20) //P14 not set
            memory[0xFF00] = (0xE0|button_pressed[B_RIGHT]|(button_pressed[B_LEFT]<<1)|(button_pressed[B_UP]<<2)|(button_pressed[B_DOWN]<<3));
         else 
         if((memory[0xFF00]&0x30) == 0x10) //P15 not set
            memory[0xFF00] = (0xD0|button_pressed[B_A]|(button_pressed[B_B]<<1)|(button_pressed[B_SELECT]<<2)|(button_pressed[B_START]<<3));
         else
            if(sgb_mode && sgb_multiplayer) 
               memory[0xFF00] = 0xF0 | sgb_nextcontrol;
            else 
               memory[0xFF00] = 0xFF;

         return memory[0xFF00];
      }
      break;
      
      case 0x30: case 0x31: case 0x32: case 0x33: case 0x34: case 0x35: case 0x36:
      case 0x37: case 0x38: case 0x39: case 0x3A: case 0x3B: case 0x3C: case 0x3D: 
      case 0x3E: case 0x3F: // waveform data
         if(channel3_on)
            return 0xFF;
         else
            return memory[address];
      break;

      case 0x56: // RP infrared
      if(gbc_mode)
      {  
         if((memory[0xFF56]&0xC0) == 0xC0) // reading data
         {
             if(multiple_gb && (memory_another[0xFF56]&0x01)) 
             {
                 // Another GB is sending signal
                 return (memory[0xFF56]&0xFD);
             }
             if(memory[0xFF56]&0x01)
             {
                // Reading own IR
                return (memory[0xFF56]&0xFD);
             }
         }

         return ((memory[0xFF56]&0xFD)|0x02); // No signal
      }
      break;
      // undocumented GBC registers
      case 0x6C:
         if(gbc_mode)
            return (memory[0xFF6C]&0x01);
      break;
      case 0x75:
         if(gbc_mode)
            return (memory[0xFF75]&0x70);
      break;     
      }
   }
      
   return mem_map[address>>12][address&0x0FFF];
}

// I/O regs and memory echo common for all writememory functions
int gb_system::io_reg_write(register unsigned short address,register byte data)
{
 /*  if(address >= 0x8000 && address < 0xA000)
   {
      if(LCDon && (memory[0xFF41]&0x03) == 0x03)
         return 1;
   }  
   if(address >= 0xFE00 && address < 0xFEA0)
   {
      if(LCDon && (memory[0xFF41]&0x02))
         return 1;   
   }
   if(address >= 0xFEA0 && address < 0xFF00)
      return 1;
   if(gbc_mode && LCDon && (address==0xFF69 || address==0xFF6B) && (memory[0xFF41]&0x03)==3)
     return 1; */
      
   // memory echo
   if(address >= 0xE000 && address < 0xFE00)
      mem_map[(address-0x2000)>>12][(address-0x2000)&0x0FFF] = data;
   if(address >= 0xC000 && address < 0xDE00)
      memory[(address+0x2000)] = data;

   if(address >= 0xFF00)
   {
      switch(address&0x00FF)
      {
      case 0x00: // P1
         if(sgb_mode && sgb_file_transfer)
            sgb_transfer(data);
            
         memory[0xFF00] = ((memory[0xFF00] & 0xCF)|(data&0x30));
      return 1;
      case 0x01: // SB
         memory[0xFF01] = data;
      return 1;
      case 0x02: // SC serial control
      {
         memory[0xFF02] = data;  
  
         if(data&0x80)
         {              
            if(!gbc_mode || !(data&2))
               CYCLES_SERIAL = CYCLES_SERIAL_GB;
            else if(gbc_mode && (data&2))
               CYCLES_SERIAL = CYCLES_SERIAL_GBC;

            serialbits = 0;
            cycles_serial = 8*CYCLES_SERIAL;
         }
      }
      return 1;
      case 0x04: // DIV  
         memory[0xFF04] = 0x00;
      return 1;  
      case 0x05: // TIMA
         memory[0xFF05] = data;        
      return 1;
      case 0x06: // TMA
         memory[0xFF06] = data;    
      return 1;      
      case 0x07: // TAC    
         memory[0xFF07] = 0xF8|data;
         timeron = (data&0x04);
         switch(data&0x03)
         {
            case 0: 
               timer_freq = CYCLES_TIMER_MODE0;
               if(memory[0xFF06] != 0xd8) // fix for Korodice
                  cycles_timer = timer_freq; 
            break;
            case 1: cycles_timer = timer_freq = CYCLES_TIMER_MODE1; break;
            case 2: cycles_timer = timer_freq = CYCLES_TIMER_MODE2; break;
            case 3: cycles_timer = timer_freq = CYCLES_TIMER_MODE3; break;
         }        
      return 1;
      case 0x0F: // IF
         memory[0xFF0F] = 0xE0|(data&0x1F);
      return 1;
      
      case 0x10:
      case 0x11:
      case 0x12:
      case 0x13:
      case 0x14:
      case 0x16:
      case 0x17:
      case 0x18:
      case 0x19:
      case 0x1a:
      case 0x1b:
      case 0x1c:
      case 0x1d:
      case 0x1e:
      case 0x20:
      case 0x21:
      case 0x22:
      case 0x23:
      case 0x24:
      case 0x25:
      case 0x26: 
      {
         sound_register(address,data);
         return 1;
      }
      
      case 0x30: case 0x31: case 0x32: case 0x33: case 0x34: case 0x35: case 0x36:
      case 0x37: case 0x38: case 0x39: case 0x3A: case 0x3B: case 0x3C: case 0x3D: 
      case 0x3E: case 0x3F: // waveform data
            memory[address] = data; 
      return 1;
      
      case 0x40: // LCDC        
         if((memory[0xFF40]&0x80) ^ (data&0x80)) // LCD display is turned on/off
         {                          
            if(data&0x80) // on
            {
               LCDon = 1;

               cycles_LCD = CYCLES_LCD_MODE2+268;
               skip_frame = (gbc_mode?1:4);
               // original GB has slower LCD, so don't draw next 4 frames to
               // prevent wrong graphics display                                           

               set_LCD_mode(2);
               memory[0xFF44] = 0;
            } else // off
            {
               LCDon = 0;
               
               off_counter = 456;
                           
               memory[0xFF41] &= 0xFC;
               memory[0xFF44] = 0;

               LCD_clear_needed = true;
            }
         }  
              
         if(!(memory[0xFF40]&0x20) && (data&0x20) && windowline == -1 && (memory[0xFF44] > memory[0xFF4A] && memory[0xFF44] < 144))     
            windowline = 154; // don't draw window                

         memory[0xFF40] = data;

         tile_map = ((memory[0xFF40] & 0x08)?0x1c00:0x1800);
         win_tile_map = ((memory[0xFF40] & 0x40)?0x1c00:0x1800);
         tile_pattern = ((memory[0xFF40] & 0x10)?0x0000:0x0800);  
      return 1;
      case 0x41: // STAT
         memory[0xFF41] = 0x80|((memory[0xFF41]&0x07)|(data&0x78));
         
         int_line = -1; //??? fix for Ken Griffey Jr's Slugfest
         
         if(!gbc_mode && LCDon && (memory[0xFF41]&0x03)==1 && memory[0xFF44] != 0) // DMG bug
            set_int(2);
      return 1;
      case 0x42: // SCY
         memory[0xFF42] = data;         
      return 1;       
      case 0x43: // SCX
         memory[0xFF43] = data;
      return 1;             
      case 0x44: // LY   
          //Read only
      return 1; 
      case 0x45: // LYC
      {                 
         memory[0xFF45] = data;

         if(LCDon) // can cause LYC interrupt
         { 
            compareLYCtoLY();
            if((memory[0xFF41] & 0x40) && (memory[0xFF41] & 0x04))
               set_int(2);
         }
      }
      return 1;
      case 0x46: // DMA transfer 
         copy_memory(0xFE00,data<<8,0xA0);
         memory[0xFF46] = data;
      return 1; 
      case 0x47: // BG & W palette
         BGP[0] = (data & 0x03);
         BGP[1] = (data & 0x0c)>>2;
         BGP[2] = (data & 0x30)>>4;
         BGP[3] = (data & 0xc0)>>6; 
         memory[0xFF47] = data; 
      return 1;
      case 0x48: // OBJ palette 0
         //OBP0[0] = (data & 0x03); //it's transparent
         OBP0[1] = (data & 0x0c)>>2;
         OBP0[2] = (data & 0x30)>>4;
         OBP0[3] = (data & 0xc0)>>6;
         memory[0xFF48] = data; 
      return 1;
      case 0x49: // OBJ palette 1
         //OBP1[0] = (data & 0x03); //it's transparent
         OBP1[1] = (data & 0x0c)>>2;
         OBP1[2] = (data & 0x30)>>4;
         OBP1[3] = (data & 0xc0)>>6; 
         memory[0xFF49] = data; 
      return 1;
      case 0x4A: // WY                               //?
         if((memory[0xFF40]&0x20) && memory[0xFF44] < 10 && memory[0xFF44] > memory[0xFF4A])// && data > 80 && data!=128)
         {
            wy_set = data;
            return 1;
         }
            //fix for Razor
         if(memory[0xFF44] != 89 && memory[0xFF44] < 144 && windowline == -1 && data < memory[0xFF44] && (memory[0xFF40]&0x20))
            windowline = 154; // fix for Puyo Wars,Urusei Yatsura      
         else if(windowline == 154 && data > memory[0xFF44]) 
            windowline = 0; // Zen Intergalactic Ninja 
 
         memory[0xFF4A] = data; 
      return 1;             
      case 0x4B: // WX          
         memory[0xFF4B] = data;                         
      return 1;             
      case 0x4D: // KEY1 speed switch
      if(gbc_mode)
      {
         memory[0xFF4D] = (memory[0xFF4D] & 0x80) | (data & 1);
         return 1;
      }
      break;
      case 0x4F: // VBK //switch video RAM bank
      if(gbc_mode)
      {
         data = (data&1);
         vram_bank = data;
         int vramadr = data*0x2000;
         mem_map[0x08] = &VRAM[vramadr];
         mem_map[0x09] = &VRAM[vramadr + 0x1000];
         memory[0xFF4F] = 0xFE|data;
         return 1;
      }
      break;
      case 0x51: // HDMA1
      if(gbc_mode)
      {      
         if(data > 0x7f && data < 0xa0)
            data = 0;

         hdma_source = (data << 8) | (memory[0xFF52] & 0xf0);
      
         memory[0xFF51] = data;
         return 1;
      }
      break;
      case 0x52: // HDMA2
      if(gbc_mode)
      {        
         data = data & 0xf0;
      
         hdma_source = (memory[0xFF51] << 8) | (data);
      
         memory[0xFF52] = data;
         return 1;
      }
      break;
      case 0x53: // HDMA3
      if(gbc_mode)
      {        
         data = data & 0x1f;
         hdma_destination = (data << 8) | (memory[0xFF54] & 0xf0);
         hdma_destination += 0x8000;
         
         memory[0xFF53] = data;
         return 1;
      }
      break;
      case 0x54: // HDMA4
      if(gbc_mode)
      {
         data = data & 0xf0;
         hdma_destination = ((memory[0xFF53] & 0x1f) << 8) | data;
         hdma_destination += 0x8000;
         
         memory[0xFF54] = data;
         return 1;
      }
      break;
      case 0x55: // HDMA5
      if(gbc_mode)
      {
         hdma_bytes = ( (data & 0x7F) + 1) << 4;
         if(hdma_on) 
         {
            if(data & 0x80) 
            {
               memory[0xFF55] = (data & 0x7F);
            } else 
            {
               memory[0xFF55] = 0xFF;
               hdma_on = 0;
            }
         } else 
         {
            if(data & 0x80) // HDMA
            {
               hdma_on = 1;
               memory[0xFF55] = data & 0x7F;
               if((memory[0xFF41]&0x03) == 0)
                  do_hdma();
            } else // GDMA
            {  
               gdma_rest = (((data&0x7F)+1)*8); // is this OK?
                                             
               copy_memory(hdma_destination,hdma_source,hdma_bytes);

               hdma_destination += hdma_bytes;
               hdma_source += hdma_bytes;
          
               memory[0xFF53] = ((hdma_destination - 0x8000) >> 8) & 0x1F;
               memory[0xFF54] = hdma_destination & 0xF0;
               memory[0xFF51] = (hdma_source >> 8) & 0xFF;
               memory[0xFF52] = hdma_source & 0xF0;
            }
         }
         return 1;
      }
      break;
      case 0x56: // RP infrared
      if(gbc_mode)
      {
  /*       if(data&0x01)
         {
              if(multiple_gb)
              {
              }
         } else if(memory[0xFF56]&0x01)
         {
              if(multiple_gb)
              {
              }
         }*/
 
         memory[0xFF56] = (data&0xFD);

         return 1;
      }
      break;
      case 0x68: // BCPS
      if(gbc_mode)
      {
         if(!rom->CGB) // this is a GBC game
            rom->CGB = 1;
            
         int index = (data & 0x3f) >> 1;
         int hilo  = (data & 0x01);
      
         memory[0xff68] = data;
         memory[0xff69] = (hilo ? (GBC_BGP[index] >> 8) : (GBC_BGP[index] & 0x00ff));
         return 1;
      }
      break;
      case 0x69: // BCPD
      if(gbc_mode)
      {
         int index = (memory[0xFF68] & 0x3f) >> 1;
         int hilo = (memory[0xFF68] & 1);
         
         memory[0xFF69] = data;
         GBC_BGP[index] = (hilo ? ((data << 8) | (GBC_BGP[index] & 0xff)) : ((GBC_BGP[index] & 0xff00) | data)) & 0x7fff;
         
         if(memory[0xff68] & 0x80) 
         {
            int pindex = ((memory[0xff68] & 0x3f) + 1) & 0x3f;
        
            memory[0xff68] = (memory[0xff68] & 0x80) | pindex;
            memory[0xff69] = (pindex & 1 ? (GBC_BGP[pindex>>1] >> 8) : (GBC_BGP[pindex>>1] & 0x00ff));
         }
         return 1;
      }
      break;
      case 0x6A: // OCPS
      if(gbc_mode)
      {    
         int index = (data & 0x3f) >> 1;
         int hilo  = (data & 0x01);
      
         memory[0xff6A] = data;
         memory[0xff6B] = (hilo ? (GBC_OBP[index] >> 8) : (GBC_OBP[index] & 0x00ff));
         return 1;
      }
      break;
      case 0x6B: // OCPD
      if(gbc_mode)
      {   
         int index = (memory[0xFF6A] & 0x3f) >> 1;
         int hilo = (memory[0xFF6A] & 1);
         
         memory[0xFF6B] = data;
         GBC_OBP[index] = (hilo ? ((data << 8) | (GBC_OBP[index] & 0xff)) : ((GBC_OBP[index] & 0xff00) | data)) & 0x7fff;
         
         if(memory[0xFF6A] & 0x80) 
         {
            int pindex = ((memory[0xFF6A] & 0x3f) + 1) & 0x3f;
        
            memory[0xFF6A] = (memory[0xFF6A] & 0x80) | pindex;
            memory[0xFF6B] = (pindex & 1 ? (GBC_OBP[pindex>>1] >> 8) : (GBC_OBP[pindex>>1] & 0x00ff));
         }
         return 1;
      }
      break;
      case 0x70: // SVBK // switch internal RAM bank
      if(gbc_mode)
      {
         data = (data&7);
         int bank = data;
         if(data == 0)
            bank = 1;
         
         wram_bank = bank;
         
         mem_map[0x0D] = &WRAM[bank*0x1000];

         memory[0xFF70] = 0xF8|data;
         return 1;
      }
      break;
      
      // undocumented GBC registers
      case 0x6C: 
      if(gbc_mode)
      {
         memory[0xFF6C] = data&0x01;
         return 1;
      }
      break;
      case 0x72: 
      case 0x73:
      case 0x74:
      if(gbc_mode)
      {
         memory[address] = data;
         return 1;
      }
      break;               
      case 0x75: 
      if(gbc_mode)
      {
         memory[0xFF75] = data&0x70;
         return 1;
      }
      break;
      case 0x77: 
      case 0x76: 
      if(gbc_mode)
      {
         // read only
         return 1;
      }
      break;
      case 0xFF: // IE
         memory[0xFFFF] = data&0x1F;
      return 1;
      }
   }
   return 0;
}

