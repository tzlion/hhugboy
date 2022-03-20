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

#include "../debug.h"
#include "../config.h"
#include "../rendering/render.h"

#include "../GB.h"
#include "Cartridge.h"

unsigned char bootstrapDMG[256], bootstrapCGB[2304], *bootstrap;
bool haveBootstrap_DMG =false;
bool haveBootstrap_CGB =false;
bool haveBootstrap =false;

extern int ramsize[9];
bool mapBootstrap =true;

byte gb_system::readmemory(unsigned short address)
{
    if(number_of_cheats)
        for(int i=0;i<number_of_cheats;++i)
            if(address == cheat[i].address && (!(cheat[i].long_code) || (cheat[i].old_value == mem_map[address>>12][address&0x0fff])))
                return cheat[i].new_value;

    if (mapBootstrap && (address <0x0100 || gbc_mode && address >=0x0200 && address <0x0900)) {
	return bootstrap[address];
    } else
    if ( address <= 0x7FFF || ( address >= 0xA000 && address <= 0xBFFF ) ) {
        return cart->readMemory(address);
    } else {
        return io_reg_read(address);
    }
}

void gb_system::writememory(unsigned short address,byte data)
{
    if ( address ==0xFF50)
	mapBootstrap =false;

    if ( address <= 0x7FFF || ( address >= 0xA000 && address <= 0xBFFF ) ) {
        cart->writeMemory(address, data);
    } else {
        if(io_reg_write(address,data)) return;
        mem_map[address>>12][address&0x0FFF] = data;
    }
    
    // The cartridge needs to see any writes as well, even without the chip enable signal
    cart->mbc->signalMemoryWrite(address, data);
}

void gb_system::set_bootstrap()
{
    bootstrap = gbc_mode ? bootstrapCGB: bootstrapDMG;
    haveBootstrap = gbc_mode ? haveBootstrap_CGB: haveBootstrap_DMG;
    mapBootstrap = haveBootstrap && options->use_bootstrap;
}

void gb_system::mem_reset(bool preserveMulticartState)
{
    set_bootstrap();

   memset(memory+0x8000,0x00,0x1FFF);
   memset(memory+0xFE00,0x00,0xA0);


   // Beat Mania 2 (bad dump)
   memory[0xbccc] = 0xC9; // set RET opcode where Beat Mania 2 jumps

   // Joust & Defender (U) [C][t1]  &  Fire Gear (Bung)
   for(int a=0xc000;a<0xcfff;++a)
      memory[a] = 0x00;
   
   memory[0xc100] = 0xff;// fix for Minesweeper for 'Windows'

   if(gbc_mode)
   {
      mem_map[0x8] = &VRAM[0x0000];
      mem_map[0x9] = &VRAM[0x1000];
      mem_map[0xC] = &memory[0xC000];
      mem_map[0xD] = &WRAM[0x1000];
      mem_map[0xE] = &memory[0xE000];
      mem_map[0xF] = &memory[0xF000];
   } else
   {
      mem_map[0x8] = &memory[0x8000];
      mem_map[0x9] = &memory[0x9000];
      mem_map[0xC] = &memory[0xC000];
      mem_map[0xD] = &memory[0xD000];
      mem_map[0xE] = &memory[0xE000];
      mem_map[0xF] = &memory[0xF000];
   }

   cart->mbc->resetMemoryMap(preserveMulticartState);

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

   wram_bank = 1;
   vram_bank = 0;

   if(gbc_mode)
   {
      if(cartridge->header.CGB)
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

         if(!strcmp(cartridge->header.name,"BUGS CC3 CRACK") || !strcmp(cartridge->header.name,"OS"))
            memcpy(GBC_BGP,GBC_DMGBG_palettes[0],sizeof(unsigned short)*4);

         memcpy(GBC_OBP,GBC_DMGBG_palettes[0],sizeof(unsigned short)*4);
      }
   }

   tile_map = ((memory[0xFF40] & 0x08)?0x1c00:0x1800);
   win_tile_map = ((memory[0xFF40] & 0x40)?0x1c00:0x1800);
   tile_pattern = ((memory[0xFF40] & 0x10)?0x0000:0x0800);

   if(gbc_mode && !cartridge->header.CGB)
   {
      memcpy(GBC_BGP,GBC_DMGBG_palettes[1],sizeof(unsigned short)*4);
      memcpy(GBC_OBP,GBC_DMGOBJ0_palettes[1],sizeof(unsigned short)*4);
      memcpy(GBC_OBP+4,GBC_DMGOBJ1_palettes[1],sizeof(unsigned short)*4);
   } else
   if(!cartridge->header.CGB || !gbc_mode)
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

///////////////////////////////////////////////////////
// Saving                                            //
///////////////////////////////////////////////////////

bool gb_system::write_save()
{
   if(cartridge->RAMsize == 0 && cartridge->mbcType != MEMORY_MBC2) return true;

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
   
   if(cartridge->mbcType == MEMORY_MBC7 || cartridge->mbcType == MEMORY_TAMA5) // Should be done on the MBC
   {
      if(fwrite(cartRAM,sizeof(byte),256,savefile) < 256)
      {
         fclose(savefile);
         SetCurrentDirectory(old_directory);
         return false;
      }      
   } else
   if(cartridge->mbcType == MEMORY_MBC2 && cartridge->battery) // MBC2 + battery
   {
      if(fwrite(cartRAM,sizeof(byte),512,savefile) < 512)
      {
         fclose(savefile);
         SetCurrentDirectory(old_directory);
         return false;
      }   
   } else
   if(cartridge->RAMsize > 2)
   {
      if((int)fwrite(cartRAM,sizeof(byte),ramsize[cartridge->RAMsize]*1024,savefile) < ramsize[cartridge->RAMsize]*1024)
      {
         fclose(savefile);
         SetCurrentDirectory(old_directory);
         return false;
      }
   } else
   {
      if((int)fwrite(cartRAM,sizeof(byte),ramsize[cartridge->RAMsize]*1024,savefile) < ramsize[cartridge->RAMsize]*1024)
      {
         fclose(savefile);
         SetCurrentDirectory(old_directory);
         return false;
      }
   }

   cart->mbc->writeMbcSpecificVarsToSaveFile(savefile);

    fclose(savefile);

   SetCurrentDirectory(old_directory);
   
   return true;
}
bool gb_system::load_save(bool loading_GB1_save_to_GB2)
{
   if(cartridge->RAMsize == 0 && cartridge->mbcType != MEMORY_MBC2) return true;

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

   fseek(savefile, 0L, SEEK_END);
   long saveFileSize = ftell(savefile);
   rewind(savefile);

   byte* dest;
   int ramSizeBytes;

    if (cartridge->mbcType == MEMORY_MBC7 || cartridge->mbcType == MEMORY_TAMA5) {
        dest = cartRAM;
        ramSizeBytes = 256;
    } else if (cartridge->mbcType == MEMORY_MBC2 && cartridge->battery) {
        dest = cartRAM;
        ramSizeBytes = 512;
    } else if (cartridge->RAMsize > 2) {
        dest = cartRAM;
        ramSizeBytes = ramsize[cartridge->RAMsize] * 1024;
    } else {
        dest = cartRAM;
        ramSizeBytes = ramsize[cartridge->RAMsize] * 1024;
    }

    // allow for save file being smaller than ram size in case a config change resulted in a ram size increase
    int bytesToRead = saveFileSize < ramSizeBytes ? saveFileSize : ramSizeBytes;

    ZeroMemory(dest, sizeof(byte) * ramSizeBytes);

    int readBytes = (int)fread(dest, sizeof(byte), bytesToRead, savefile);
    if (readBytes < bytesToRead) {
        fclose(savefile);
        SetCurrentDirectory(old_directory);
        return false;
    }

    if (bytesToRead == ramSizeBytes) { // don't try to read subsequent data if this was an underread
        cart->mbc->readMbcSpecificVarsFromSaveFile(savefile);
    }

    fclose(savefile);
    SetCurrentDirectory(old_directory);

    return true;
}

