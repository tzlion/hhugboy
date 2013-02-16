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

#define UNICODE
 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <windows.h>

#include <iostream>

using namespace std;

#include "debug.h"
#include "config.h"
#include "render.h"

#include "GB.h"

extern int ramsize[9];

void gb_system::mem_reset()
{
   memset(memory+0x8000,0x00,0x1FFF);
   memset(memory+0xFE00,0x00,0xA0);

   if(rom->bankType==MBC2)
      for(int a=0xa000;a<0xc000;++a)
         memory[a] = 0x0F;
   else
      for(int a=0xa000;a<0xc000;++a)
         memory[a] = 0xFF;

   // Beat Mania 2 (bad dump)
   memory[0xbccc] = 0xC9; // set RET opcode where Beat Mania 2 jumps

   // Joust & Defender (U) [C][t1]  &  Fire Gear (Bung)
   for(int a=0xc000;a<0xcfff;++a)
      memory[a] = 0x00;
   
   memory[0xc100] = 0xff;// fix for Minesweeper for 'Windows'

   mem_map[0x0] = &cartridge[0x0000];
   mem_map[0x1] = &cartridge[0x1000];
   mem_map[0x2] = &cartridge[0x2000];
   mem_map[0x3] = &cartridge[0x3000];
   mem_map[0x4] = &cartridge[0x4000];
   mem_map[0x5] = &cartridge[0x5000];
   mem_map[0x6] = &cartridge[0x6000];
   mem_map[0x7] = &cartridge[0x7000];

   if(gbc_mode)
   {
      mem_map[0x8] = &VRAM[0x0000];
      mem_map[0x9] = &VRAM[0x1000];
      mem_map[0xA] = &memory[0xA000];
      mem_map[0xB] = &memory[0xB000];
      mem_map[0xC] = &memory[0xC000];
      mem_map[0xD] = &WRAM[0x1000];
      mem_map[0xE] = &memory[0xE000];
      mem_map[0xF] = &memory[0xF000];
   } else
   {
      mem_map[0x8] = &memory[0x8000];
      mem_map[0x9] = &memory[0x9000];
      mem_map[0xA] = &memory[0xA000];
      mem_map[0xB] = &memory[0xB000];
      mem_map[0xC] = &memory[0xC000];
      mem_map[0xD] = &memory[0xD000];
      mem_map[0xE] = &memory[0xE000];
      mem_map[0xF] = &memory[0xF000];
   }

   if(rom->RAMsize>2)
   {
      mem_map[0xA] = &cartRAM[0x0000];
      mem_map[0xB] = &cartRAM[0x1000];
   }

   memset(memory+0xFEA0,0xFF,0x60);

   memory[0xFF00] = 0xFF; // P1
   memory[0xFF01] = 0x00; // SB
   memory[0xFF02] = 0x00; // SC
   memory[0xFF04] = 0x00; // DIV
   memory[0xFF05] = 0x00; // TIMA
   memory[0xFF06] = 0x00; // TMA
   memory[0xFF07] = 0x00; // TAC
   memory[0xFF0F] = 0xE1; // IF
   memory[0xFF10] = 0x80; // NR10
   memory[0xFF11] = 0xBF; // NR11
   memory[0xFF12] = 0xF3; // NR12
   memory[0xFF13] = 0xFF; // NR13
   memory[0xFF14] = 0xBF; // NR14
   memory[0xFF15] = 0xFF; // NR20
   memory[0xFF16] = 0x3F; // NR21
   memory[0xFF17] = 0x00; // NR22
   memory[0xFF18] = 0xFF; // NR23
   memory[0xFF19] = 0xBF; // NR24
   memory[0xFF1A] = 0x7F; // NR30
   memory[0xFF1B] = 0xFF; // NR31
   memory[0xFF1C] = 0x9F; // NR32
   memory[0xFF1D] = 0xFF; // NR33
   memory[0xFF1E] = 0xBF; // NR33
   memory[0xFF1F] = 0xFF; // NR40
   memory[0xFF20] = 0xFF; // NR41
   memory[0xFF21] = 0x00; // NR42
   memory[0xFF22] = 0x00; // NR43
   memory[0xFF23] = 0xBF; // NR44
   memory[0xFF24] = 0x77; // NR50
   memory[0xFF25] = 0xF3; // NR51
   memory[0xFF26] = 0xF1; // NR52
   memory[0xFF40] = 0x91; // LCDC
   memory[0xFF41] = 0x81; // STAT
   memory[0xFF42] = 0x00; // SCY
   memory[0xFF43] = 0x00; // SCX
   memory[0xFF44] = 0x00; // LY
   memory[0xFF45] = 0x00; // LYC
   memory[0xFF46] = 0x00; // DMA
   memory[0xFF47] = 0xFC; // BGP
   memory[0xFF48] = 0xFF; // OBP0
   memory[0xFF49] = 0xFF; // OBP1
   memory[0xFF4A] = 0x00; // WY
   memory[0xFF4B] = 0x00; // WX
   memory[0xFF4F] = 0xFE; // VBK
   memory[0xFF4D] = 0x00; // KEY1
   memory[0xFF51] = 0x00; // HDMA1
   memory[0xFF52] = 0x00; // HDMA2
   memory[0xFF53] = 0x00; // HDMA3
   memory[0xFF54] = 0x00; // HDMA4
   memory[0xFF55] = 0x00; // HDMA5
   memory[0xFF68] = 0x00; // BCPS
   memory[0xFF6A] = 0x00; // OCPS
   memory[0xFF69] = 0x00; // BCPD
   memory[0xFF6B] = 0x00; // OCPD
   memory[0xFF70] = 0x00; // SVBK
   memory[0xFFFF] = 0x00; // IE

   BGP[0] = 0;
   BGP[1] = 3;
   BGP[2] = 3;
   BGP[3] = 3;

   OBP0[0]=OBP0[1]=OBP0[2]=OBP0[3]=3;
   OBP1[0]=OBP1[1]=OBP1[2]=OBP1[3]=3;

   MBC1memorymodel = 0;
   MBChi = 0;
   MBClo = 1;
   rom_bank = 1;
   ram_bank = 0;
   wram_bank = 1;
   vram_bank = 0;
   RTCIO = 0;

   if(gbc_mode)
   {
      if(rom->CGB)
      {
         hdma_source = 0x0000;
         hdma_destination = 0x8000;
         memory[0xFF44] = 0x90; // LY
         memory[0xFF45] = 0x00; // LYC
         memory[0xFF41] = 0x81; // STAT
         memory[0xFF55] = 0xFF; // HDMA5
         memory[0xFF56] = 0x02; // RP
         memory[0xFF68] = 0xC0; // BCPS
         memory[0xFF6A] = 0xC0; // OCPS

         memory[0xFF6C] = 0xFE; // undocumented
         memory[0xFF72] = 0x00; // undocumented
         memory[0xFF73] = 0x00; // undocumented
         memory[0xFF74] = 0x00; // undocumented
         memory[0xFF75] = 0x8F; // undocumented
         memory[0xFF76] = 0x00; // undocumented
         memory[0xFF77] = 0x00; // undocumented
         for(int i=0;i<32;)
         {
            GBC_BGP[i++] = 0x7FFF;
            GBC_BGP[i++] = 0x7FFF;
            GBC_BGP[i++] = 0x7FFF;
            GBC_BGP[i++] = 0x7FFF;
         }

         if(!strcmp(rom->name,"BUGS CC3 CRACK") || !strcmp(rom->name,"OS"))
            memcpy(GBC_BGP,GBC_DMGBG_palettes[0],sizeof(unsigned short)*4);

         memcpy(GBC_OBP,GBC_DMGBG_palettes[0],sizeof(unsigned short)*4);
      }
   }

   tile_map = ((memory[0xFF40] & 0x08)?0x1c00:0x1800);
   win_tile_map = ((memory[0xFF40] & 0x40)?0x1c00:0x1800);
   tile_pattern = ((memory[0xFF40] & 0x10)?0x0000:0x0800);

   if(gbc_mode && !rom->CGB)
   {
      memcpy(GBC_BGP,GBC_DMGBG_palettes[1],sizeof(unsigned short)*4);
      memcpy(GBC_OBP,GBC_DMGOBJ0_palettes[1],sizeof(unsigned short)*4);
      memcpy(GBC_OBP+4,GBC_DMGOBJ1_palettes[1],sizeof(unsigned short)*4);
   } else
   if(!rom->CGB || !gbc_mode)
   {
      if(options->video_GB_color == BLACK_WHITE)
      {
         memcpy(GBC_BGP,GBC_DMGBG_palettes[0],sizeof(unsigned short)*4);
         memcpy(GBC_OBP,GBC_DMGOBJ0_palettes[0],sizeof(unsigned short)*4);
         memcpy(GBC_OBP+4,GBC_DMGOBJ1_palettes[0],sizeof(unsigned short)*4);
      } else
      if(options->video_GB_color == LCD_BROWN)
      {
         memcpy(GBC_BGP,LCD_palette_brown,sizeof(unsigned short)*4);
         memcpy(GBC_OBP,LCD_palette_brown,sizeof(unsigned short)*4);
         memcpy(GBC_OBP+4,LCD_palette_brown,sizeof(unsigned short)*4);
      } else
      if(options->video_GB_color == LCD_GREEN)
      {
         memcpy(GBC_BGP,LCD_palette_green,sizeof(unsigned short)*4);
         memcpy(GBC_OBP,LCD_palette_green,sizeof(unsigned short)*4);
         memcpy(GBC_OBP+4,LCD_palette_green,sizeof(unsigned short)*4);
      } else
      if(options->video_GB_color == GRAY)
      {
         memcpy(GBC_BGP,GB_palette_gray,sizeof(unsigned short)*4);
         memcpy(GBC_OBP,GB_palette_gray,sizeof(unsigned short)*4);
         memcpy(GBC_OBP+4,GB_palette_gray,sizeof(unsigned short)*4);
      }
   }

   memset(VRAM,0,16384);
}

void gb_system::memory_variables_reset()
{                                          
   bc_select = 0;       
     
   cameraIO = 0;
   RTC_latched = 0;   
   
   rtc.s = 0;
   rtc.m = 0;
   rtc.h = 0;
   rtc.d = 0;
   rtc.control = 0;
   rtc.last_time = time(0);
   rtc.cur_register = 0x08;

   tama_flag = 0;
   tama_time = 0;
   tama_val6 = 0;
   tama_val7 = 0;
   tama_val4 = 0;
   tama_val5 = 0;
   tama_count = 0;
   tama_month = 0;
   tama_change_clock = 0;
   
   HuC3_flag = HUC3_NONE;
   HuC3_RAMvalue = 1;
}

///////////////////////////////////////////////////////
// Saving                                            //
///////////////////////////////////////////////////////

bool gb_system::write_save()
{
   if(rom->RAMsize == 0) return true;

   wchar_t old_directory[PROGRAM_PATH_SIZE];


   GetCurrentDirectory(PROGRAM_PATH_SIZE,old_directory);
   SetCurrentDirectory(options->save_directory.c_str());
   
   wchar_t save_filename[275]; 
   wcscpy(save_filename,rom_filename);
   

   if(this == GB1)
   {
       wcscat(save_filename,L".sav");
   }
   else
   {
       wcscat(save_filename,L".sv2");
   }
   
   
   //char saveFileA[PROGRAM_PATH_SIZE];
   //wcstombs(saveFileA,save_filename,PROGRAM_PATH_SIZE);
   //debug_print(saveFileA);

   FILE* savefile = _wfopen(save_filename,L"wb");
   if(!savefile)
   {
      SetCurrentDirectory(old_directory);
      return false;
   }
   
   if(rom->bankType == MBC7 || rom->bankType == TAMA5)
   {
      if(fwrite(&memory[0xA000],sizeof(byte),256,savefile) < 256)
      {
         fclose(savefile);
         SetCurrentDirectory(old_directory);
         return false;
      }      
   } else
   if(rom->carttype == 6) // MBC2 + battery
   {
      if(fwrite(&memory[0xA000],sizeof(byte),512,savefile) < 512)
      {
         fclose(savefile);
         SetCurrentDirectory(old_directory);
         return false;
      }   
   } else
   if(rom->RAMsize > 2)
   {
      if((int)fwrite(cartRAM,sizeof(byte),ramsize[rom->RAMsize]*1024,savefile) < ramsize[rom->RAMsize]*1024)
      {
         fclose(savefile);
         SetCurrentDirectory(old_directory);
         return false;
      }
   } else
   {
      if((int)fwrite(&memory[0xA000],sizeof(byte),ramsize[rom->RAMsize]*1024,savefile) < ramsize[rom->RAMsize]*1024)
      {
         fclose(savefile);
         SetCurrentDirectory(old_directory);
         return false;
      }
   }

   if(rom->RTC || rom->bankType==TAMA5)
   {
      fwrite(&rtc.s, sizeof(int),1,savefile);
      fwrite(&rtc.m, sizeof(int),1,savefile);
      fwrite(&rtc.h, sizeof(int),1,savefile);     
      fwrite(&rtc.d, sizeof(int),1,savefile);   
      fwrite(&rtc.control, sizeof(int),1,savefile); 
      fwrite(&rtc.last_time, sizeof(time_t),1,savefile);               
   }   
   
   if(rom->bankType==TAMA5)
      fwrite(&tama_month, sizeof(int),1,savefile);    
      
   if(rom->bankType == HuC3)
   {
      fwrite(&HuC3_time, sizeof(unsigned int),1,savefile);
      fwrite(&HuC3_last_time, sizeof(time_t),1,savefile); 
      fwrite(&rtc.s, sizeof(int),1,savefile);      
   }
   
   fclose(savefile);

   SetCurrentDirectory(old_directory);
   
   return true;
}

bool gb_system::load_save(bool loading_GB1_save_to_GB2)
{
   if(rom->RAMsize == 0) return true;

   wchar_t old_directory[PROGRAM_PATH_SIZE];

   GetCurrentDirectory(PROGRAM_PATH_SIZE,old_directory);

	
   if (!SetCurrentDirectory(options->save_directory.c_str())) {
   		CreateDirectory(options->save_directory.c_str(),NULL);
   		SetCurrentDirectory(options->save_directory.c_str());
   }

   SetCurrentDirectory(options->save_directory.c_str());
     
   wchar_t save_filename[275];
   wcscpy(save_filename,rom_filename);

   if(this == GB1 || loading_GB1_save_to_GB2)
   {
      wcscat(save_filename,L".sav");
   }
   else
   {
      wcscat(save_filename,L".sv2");
   }

   FILE* savefile = _wfopen(save_filename,L"rb");
   if(!savefile) 
   {
      SetCurrentDirectory(old_directory);
      return true;
   }
   
   if(rom->bankType == MBC7 || rom->bankType == TAMA5)
   {
      if(fread(&memory[0xA000],sizeof(byte),256,savefile) < 256)
      {
         fclose(savefile);
         SetCurrentDirectory(old_directory);
         return false;
      }      
   } else
   if(rom->carttype == 6) // MBC2 + battery
   {
      if(fread(&memory[0xA000],sizeof(byte),512,savefile) < 512)
      {
         fclose(savefile);
         SetCurrentDirectory(old_directory);
         return false;
      }   
   } else
   if(rom->RAMsize > 2)
   {
      if((int)fread(cartRAM,sizeof(byte),ramsize[rom->RAMsize]*1024,savefile) < ramsize[rom->RAMsize]*1024)
      {
         fclose(savefile);
         SetCurrentDirectory(old_directory);
         return false;
      }
   } else
   {
      if((int)fread(&memory[0xA000],sizeof(byte),ramsize[rom->RAMsize]*1024,savefile) < ramsize[rom->RAMsize]*1024)
      {
         fclose(savefile);
         SetCurrentDirectory(old_directory);
         return false;
      }
   }
   
   if(rom->RTC || rom->bankType==TAMA5)
   {
      fread(&rtc.s, sizeof(int),1,savefile);
      fread(&rtc.m, sizeof(int),1,savefile);
      fread(&rtc.h, sizeof(int),1,savefile);     
      fread(&rtc.d, sizeof(int),1,savefile);   
      fread(&rtc.control, sizeof(int),1,savefile); 
      fread(&rtc.last_time, sizeof(time_t),1,savefile);    
      rtc_latch = rtc;              
   }
   
   if(rom->bankType==TAMA5)
      fread(&tama_month, sizeof(int),1,savefile);       

   if(rom->bankType == HuC3)
   {
      fread(&HuC3_time, sizeof(unsigned int),1,savefile);
      fread(&HuC3_last_time, sizeof(time_t),1,savefile);
      fread(&rtc.s, sizeof(int),1,savefile);            
   }

   fclose(savefile);
   SetCurrentDirectory(old_directory);
   
   return true;
}

