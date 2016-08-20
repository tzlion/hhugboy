/*
   hhugboy Game Boy emulator
   copyright 2013-2016 taizou

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
#include "../config.h"
#include "../GB_gfx.h"
#include "../debug.h"
#include "../mainloop.h"
#include <time.h>

#include "../GB.h"
#include "GB_MBC.h"

#include "../debug.h"
#include "zlib/zconf.h"
#include <stdio.h>

#include "../main.h"
#include "mbc/Mbc3.h"
#include "mbc/MbcCamera.h"
#include "mbc/Mbc7.h"
#include "mbc/Huc3.h"
#include "mbc/Tama5.h"
#include "mbc/Sintax.h"

byte vfmultimode=0;
byte vfmultibank=0;
byte vfmultimem=0;
byte vfmultifinal=0;

int maxROMbank[9] =
{
 1, 3, 7, 15, 31, 63, 127, 255, 511
}; 

int maxRAMbank[6] =
{
 0, 0, 0, 4, 15, 7
};

int rom_size_mask[9] = 
{ 
   0x00007fff,
   0x0000ffff,
   0x0001ffff,
   0x0003ffff,
   0x0007ffff,
   0x000fffff,
   0x001fffff,
   0x003fffff,
   0x007fffff
};

unsigned int cart_address = 0;

//int RTCIO = 0;
//int RTC_latched = 0;

// Eventually GB should contain cart and cart should contain MBC
gb_mbc::gb_mbc(byte** gbMemMap, byte** gbCartridge, GBrom** gbRom, byte** gbCartRam, byte* gbRomBankXor, int* gbRumbleCounter, byte** gbMemory):

        mbcType(MEMORY_DEFAULT),

        RAMenable(0),
        rom_bank(1),
        ram_bank(0),
        MBChi(0),
        MBClo(0)

{
    this->gbCartridge = gbCartridge;
    this->gbMemMap = gbMemMap;
    this->gbRom = gbRom;
    this->gbCartRam = gbCartRam;
    this->gbRomBankXor = gbRomBankXor;
    this->gbRumbleCounter = gbRumbleCounter;
    this->gbMemory = gbMemory;

    setMemoryReadWrite(mbcType);
}

void gb_mbc::resetMbcVariables()
{
    mbc->MBC1memorymodel = 0;
    MBChi = 0;
    MBClo = 1;
    rom_bank = 1;
    ram_bank = 0;
    mbc->RTCIO = 0;

    mbc->bc_select = 0;

    mbc->cameraIO = 0;
    mbc->RTC_latched = 0;

    mbc->rtc.s = 0;
    mbc->rtc.m = 0;
    mbc->rtc.h = 0;
    mbc->rtc.d = 0;
    mbc->rtc.control = 0;
    mbc->rtc.last_time = time(0);
    mbc->rtc.cur_register = 0x08;

    mbc->tama_flag = 0;
    mbc->tama_time = 0;
    mbc->tama_val6 = 0;
    mbc->tama_val7 = 0;
    mbc->tama_val4 = 0;
    mbc->tama_val5 = 0;
    mbc->tama_count = 0;
    mbc->tama_month = 0;
    mbc->tama_change_clock = 0;

    mbc->HuC3_flag = HUC3_NONE;
    mbc->HuC3_RAMvalue = 1;

    mbc->sintax_mode = 0;
    mbc->sintax_xor2 = mbc->sintax_xor3 = mbc->sintax_xor4 = mbc->sintax_xor5 = 0;
}

byte gb_mbc::readmemory_cart(register unsigned short address) {
    return mbc->readMemory(address);
}

void gb_mbc::writememory_cart(unsigned short address, register byte data) {
    switch(mbcType)
    {
        case MEMORY_MBC1:
            writememory_MBC1(address,data);
            break;

        case MEMORY_MBC2:
            writememory_MBC2(address,data);
            break;

        case MEMORY_MBC3:
            writememory_MBC3(address,data);
            break;

        case MEMORY_MBC5:
            writememory_MBC5(address,data,false,false);
            break;

        case MEMORY_MBC7:
            writememory_MBC7(address,data);
            break;

        case MEMORY_CAMERA:
            writememory_Camera(address,data);
            break;

        case MEMORY_HUC3:
            writememory_HuC3(address,data);
            break;

        case MEMORY_TAMA5:
            writememory_TAMA5(address,data);
            break;

        case MEMORY_ROCKMAN8:
            writememory_Rockman8(address,data);
            break;

        case MEMORY_BC:
            writememory_BC(address,data);
            break;

        case MEMORY_MMM01:
            writememory_MMM01(address,data);
            break;

        case MEMORY_POKE:
            writememory_poke(address,data);
            break;

        case MEMORY_8IN1:
            writememory_8in1(address,data);
            break;

        case MEMORY_MK12:
            writememory_MK12(address,data);
            break;

        case MEMORY_NIUTOUDE:
            writememory_MBC5(address,data,true,false);
            break;

        case MEMORY_SINTAX:
            writememory_MBC5(address,data,false,true);
            break;

        default:
        case MEMORY_DEFAULT:
            writememory_default(address,data);
            break;
    }
}

void gb_mbc::setXorForBank(byte bankNo)
{
  	switch(bankNo & 0x0F) {
	case 0x00: case 0x04: case 0x08: case 0x0C:
		*gbRomBankXor = mbc->sintax_xor2;
		break;
	case 0x01: case 0x05: case 0x09: case 0x0D:
		*gbRomBankXor = mbc->sintax_xor3;
		break;
	case 0x02: case 0x06: case 0x0A: case 0x0E:
		*gbRomBankXor = mbc->sintax_xor4;
		break;
	case 0x03: case 0x07: case 0x0B: case 0x0F:
		*gbRomBankXor = mbc->sintax_xor5;
		break;
  	}
  	
  	//char buff[200];
  //	sprintf(buff,"bank no %x abbr %x xor %x",bankNo,bankNo&0x0F,rom_bank_xor);
  //	debug_print(buff);
}

//-------------------------------------------------------------------------
// writememory_default:
// for ROM only and for undocumented/unknown MBCs
//-------------------------------------------------------------------------
void gb_mbc::writememory_default(register unsigned short address,register byte data)
{
   if(address < 0x2000)// Is it a RAM bank enable/disable?
   {
      RAMenable = ( (data&0x0A) == 0x0A ? 1 : 0);
      return;  
   } 
   
   if(address < 0x4000) // Is it a ROM bank switch?
   {  
      if((*gbRom)->bankType == ROM)
         return;
      else
      {
         if(data == 0)
            data = 1;
         
         rom_bank = data;
         
         int cadr = (data<<14)+((MBChi<<1)<<14);
         gbMemMap[0x4] = &(*gbCartridge)[cadr];
         gbMemMap[0x5] = &(*gbCartridge)[cadr+0x1000];
         gbMemMap[0x6] = &(*gbCartridge)[cadr+0x2000];
         gbMemMap[0x7] = &(*gbCartridge)[cadr+0x3000];
         return; 
      }
   }

   if(address < 0x6000) // Is it a RAM bank switch?
   {          
      data &= 0x03;
              
      ram_bank = data;
      
      int madr = data<<13;
      gbMemMap[0xA] = &(*gbCartRam)[madr];
      gbMemMap[0xB] = &(*gbCartRam)[madr+0x1000];
      return;
   }

   if(address < 0x8000) // BHGOS multicart
   {
      if(++mbc->bc_select == 2 && (*gbRom)->ROMsize>1)
      {
         MBChi = (data&0xFF);

         cart_address = (MBChi<<1)<<14;

         gbMemMap[0x0] = &(*gbCartridge)[cart_address];
         gbMemMap[0x1] = &(*gbCartridge)[cart_address+0x1000];
         gbMemMap[0x2] = &(*gbCartridge)[cart_address+0x2000];
         gbMemMap[0x3] = &(*gbCartridge)[cart_address+0x3000];

         gbMemMap[0x4] = &(*gbCartridge)[cart_address+0x4000];
         gbMemMap[0x5] = &(*gbCartridge)[cart_address+0x5000];
         gbMemMap[0x6] = &(*gbCartridge)[cart_address+0x6000];
         gbMemMap[0x7] = &(*gbCartridge)[cart_address+0x7000];
      }
      return;
   }

   // Always allow RAM writes.

   gbMemMap[address>>12][address&0x0FFF] = data;
}

//-------------------------------------------------------------------------
// writememory_8in1:
// for Sachen 8in1
//-------------------------------------------------------------------------
void gb_mbc::writememory_8in1(register unsigned short address,register byte data)
{
   if(address < 0x2000)
      return;

   if(address < 0x4000) // Is it a ROM bank switch?
   {
      data &= 0x1f;
      if(data == 0)
         data = 1;

      rom_bank = data;

      int cadr = (data<<14)+(MBChi<<14);
      cadr &= rom_size_mask[(*gbRom)->ROMsize];
      gbMemMap[0x4] = &(*gbCartridge)[cadr];
      gbMemMap[0x5] = &(*gbCartridge)[cadr+0x1000];
      gbMemMap[0x6] = &(*gbCartridge)[cadr+0x2000];
      gbMemMap[0x7] = &(*gbCartridge)[cadr+0x3000];
      return;
   }

   if(address < 0x6000) // Is it a RAM bank switch?
   {
      if(address == 0x4000 && mbc->bc_select < 3) // game select
      {
         ++mbc->bc_select;

         MBClo = 0;

         MBChi = (data&0x1f);

         cart_address = MBChi<<14;
         gbMemMap[0x0] = &(*gbCartridge)[cart_address];
         gbMemMap[0x1] = &(*gbCartridge)[cart_address+0x1000];
         gbMemMap[0x2] = &(*gbCartridge)[cart_address+0x2000];
         gbMemMap[0x3] = &(*gbCartridge)[cart_address+0x3000];

         gbMemMap[0x4] = &(*gbCartridge)[cart_address+0x4000];
         gbMemMap[0x5] = &(*gbCartridge)[cart_address+0x5000];
         gbMemMap[0x6] = &(*gbCartridge)[cart_address+0x6000];
         gbMemMap[0x7] = &(*gbCartridge)[cart_address+0x7000];
         return;
      }

      data &= 0x03;

      ram_bank = data;

      int madr = data<<13;
      gbMemMap[0xA] = &(*gbCartRam)[madr];
      gbMemMap[0xB] = &(*gbCartRam)[madr+0x1000];
      return;
   }

   if(address < 0x8000)
      return;

   // Always allow RAM writes.

   gbMemMap[address>>12][address&0x0FFF] = data;
}

//-------------------------------------------------------------------------
// writememory_MBC1:
// for MBC1 and HuC1
//-------------------------------------------------------------------------
void gb_mbc::writememory_MBC1(register unsigned short address,register byte data)
{
   if(address < 0x2000)// Is it a RAM bank enable/disable?
   {
       RAMenable = ( (data&0x0A) == 0x0A ? 1 : 0);
      return;
   }

   if(address < 0x4000) // Is it a ROM bank switch?
   {
      if(mbc->MBC1memorymodel == 0)
      {
         if(data == 0)
            data = 1;

         MBClo = data;

         rom_bank = MBClo|(MBChi<<5);

         cart_address = MBClo<<14;
         cart_address |= (MBChi<<19);

         cart_address &= rom_size_mask[(*gbRom)->ROMsize];

         gbMemMap[0x4] = &(*gbCartridge)[cart_address];
         gbMemMap[0x5] = &(*gbCartridge)[cart_address+0x1000];
         gbMemMap[0x6] = &(*gbCartridge)[cart_address+0x2000];
         gbMemMap[0x7] = &(*gbCartridge)[cart_address+0x3000];
      } else
      {
         if(data == 0)
            data = 1;

         rom_bank = data;

         int cadr = rom_bank<<14;

         cadr &= rom_size_mask[(*gbRom)->ROMsize];

         gbMemMap[0x4] = &(*gbCartridge)[cadr];
         gbMemMap[0x5] = &(*gbCartridge)[cadr+0x1000];
         gbMemMap[0x6] = &(*gbCartridge)[cadr+0x2000];
         gbMemMap[0x7] = &(*gbCartridge)[cadr+0x3000];
      }
      return;
   }

   if(address < 0x6000) // Is it a RAM bank switch?
   {
      if(mbc->MBC1memorymodel == 0)
      {
         if((((data&0x03)<<5)|MBClo) > maxROMbank[(*gbRom)->ROMsize])
            return;

         MBChi = (data&0x03);

         rom_bank = MBClo|(MBChi<<5);

         cart_address = MBClo<<14;
         cart_address |= (MBChi<<19);

         cart_address &= rom_size_mask[(*gbRom)->ROMsize];

         gbMemMap[0x4] = &(*gbCartridge)[cart_address];
         gbMemMap[0x5] = &(*gbCartridge)[cart_address+0x1000];
         gbMemMap[0x6] = &(*gbCartridge)[cart_address+0x2000];
         gbMemMap[0x7] = &(*gbCartridge)[cart_address+0x3000];
         return;
      }

      if((*gbRom)->RAMsize <= 2) // no need to change it if there isn't over 8KB ram
         return;

      data &= 0x03;

      if(data > maxRAMbank[(*gbRom)->RAMsize])
         data = maxRAMbank[(*gbRom)->RAMsize];

      ram_bank = data;

      int madr = data<<13;
      gbMemMap[0xA] = &(*gbCartRam)[madr];
      gbMemMap[0xB] = &(*gbCartRam)[madr+0x1000];
      return;
   }

   if(address < 0x8000) // Is it a MBC1 max memory model change?
   {
       mbc->MBC1memorymodel = (data&0x01);
      return;
   }

/*   if(address >= 0xA000 && address < 0xC000)
   {
      if((!RAMenable || !rom->RAMsize) && !RAM_always_enable)
         return;
   }*/

   gbMemMap[address>>12][address&0x0FFF] = data;
}

//-------------------------------------------------------------------------
// writememory_MMM01:
// for MMM01
// Only game with MBC set as MMM01 in header I've seen is
// Momotarou Collection 2, which is propably a bad dump?
//-------------------------------------------------------------------------
void gb_mbc::writememory_MMM01(register unsigned short address,register byte data)
{
   if(address < 0x2000)
      return;

   if(address < 0x4000) // Is it a ROM bank switch?
   {
      if(data == 0)
         data = 1;

      rom_bank = data;

      int cadr = (rom_bank<<14)+(MBChi<<14);

      cadr &= rom_size_mask[(*gbRom)->ROMsize];

      gbMemMap[0x4] = &(*gbCartridge)[cadr];
      gbMemMap[0x5] = &(*gbCartridge)[cadr+0x1000];
      gbMemMap[0x6] = &(*gbCartridge)[cadr+0x2000];
      gbMemMap[0x7] = &(*gbCartridge)[cadr+0x3000];
      return;
   }

   if(address < 0x6000) // Is it a RAM bank switch?
   {
      if(address == 0x5fff && !mbc->bc_select)
      {
         mbc->bc_select = 1;

         data &= 0x2f;
         MBClo = 0;

         MBChi = data;
         cart_address = MBChi<<14;

         gbMemMap[0x0] = &(*gbCartridge)[cart_address];
         gbMemMap[0x1] = &(*gbCartridge)[cart_address+0x1000];
         gbMemMap[0x2] = &(*gbCartridge)[cart_address+0x2000];
         gbMemMap[0x3] = &(*gbCartridge)[cart_address+0x3000];

         gbMemMap[0x4] = &(*gbCartridge)[cart_address+0x4000];
         gbMemMap[0x5] = &(*gbCartridge)[cart_address+0x5000];
         gbMemMap[0x6] = &(*gbCartridge)[cart_address+0x6000];
         gbMemMap[0x7] = &(*gbCartridge)[cart_address+0x7000];
         return;
      }

      if((*gbRom)->RAMsize <= 2) // no need to change it if there isn't over 8KB ram
         return;

      data &= 0x03;

      if(data > maxRAMbank[(*gbRom)->RAMsize])
         data = maxRAMbank[(*gbRom)->RAMsize];

      ram_bank = data;

      int madr = data<<13;
      gbMemMap[0xA] = &(*gbCartRam)[madr];
      gbMemMap[0xB] = &(*gbCartRam)[madr+0x1000];
      return;
   }

   if(address < 0x8000)
      return;

   // Always allow RAM writes.

   gbMemMap[address>>12][address&0x0FFF] = data;
}

//-------------------------------------------------------------------------
// writememory_BC:
// for Collection Carts
//-------------------------------------------------------------------------
void gb_mbc::writememory_BC(register unsigned short address,register byte data)
{
   if(address < 0x2000)// Is it a RAM bank enable/disable?
   {
      RAMenable = ( (data&0x0A) == 0x0A ? 1 : 0);
      return;
   }

   if(address < 0x4000) // Is it a ROM bank switch?
   {
      data &= 0x0F;

      if(data == 0)
         data = 1;

      rom_bank = data|(MBChi<<4);

      int cadr = rom_bank<<14;

      cadr &= rom_size_mask[(*gbRom)->ROMsize];

      gbMemMap[0x4] = &(*gbCartridge)[cadr];
      gbMemMap[0x5] = &(*gbCartridge)[cadr+0x1000];
      gbMemMap[0x6] = &(*gbCartridge)[cadr+0x2000];
      gbMemMap[0x7] = &(*gbCartridge)[cadr+0x3000];
      return;
   }

   if(address < 0x6000)
   {
      if(address == 0x4000 || address == 0x5fff) // game select
      {
         MBClo = 0;
         MBChi = (data&0x03);

         cart_address = (MBChi<<4)<<14;

         gbMemMap[0x0] = &(*gbCartridge)[cart_address];
         gbMemMap[0x1] = &(*gbCartridge)[cart_address+0x1000];
         gbMemMap[0x2] = &(*gbCartridge)[cart_address+0x2000];
         gbMemMap[0x3] = &(*gbCartridge)[cart_address+0x3000];

         gbMemMap[0x4] = &(*gbCartridge)[cart_address+0x4000];
         gbMemMap[0x5] = &(*gbCartridge)[cart_address+0x5000];
         gbMemMap[0x6] = &(*gbCartridge)[cart_address+0x6000];
         gbMemMap[0x7] = &(*gbCartridge)[cart_address+0x7000];
         return;
      }
      return;
   }

   if(address < 0x8000)
   {
      mbc->MBC1memorymodel = (data&0x01);
      return;
   }

  /* if(address >= 0xA000 && address < 0xC000)
   {
      if((!RAMenable || !rom->RAMsize) && !RAM_always_enable)
         return;
   }*/

   gbMemMap[address>>12][address&0x0FFF] = data;
}

//-------------------------------------------------------------------------
// writememory_MK12:
// for Mortal Kombat 1&2
//-------------------------------------------------------------------------
void gb_mbc::writememory_MK12(register unsigned short address,register byte data)
{
   if(address < 0x2000)// Is it a RAM bank enable/disable?
   {
      RAMenable = ( (data&0x0A) == 0x0A ? 1 : 0);
      return;
   }

   if(address < 0x4000) // Is it a ROM bank switch?
   {
      data &= 0x0F;

      if(data == 0)
         data = 1;

      rom_bank = data+MBChi;

      int cadr = rom_bank<<14;

      cadr &= rom_size_mask[(*gbRom)->ROMsize];

      gbMemMap[0x4] = &(*gbCartridge)[cadr];
      gbMemMap[0x5] = &(*gbCartridge)[cadr+0x1000];
      gbMemMap[0x6] = &(*gbCartridge)[cadr+0x2000];
      gbMemMap[0x7] = &(*gbCartridge)[cadr+0x3000];
      return;
   }

   if(address < 0x6000)
   {
      if(address == 0x5000) // game select
      {
         MBClo = 0;
         MBChi = (data&0x03);
         if(MBChi==2) MBChi = 17;

         cart_address = MBChi<<14;

         gbMemMap[0x0] = &(*gbCartridge)[cart_address];
         gbMemMap[0x1] = &(*gbCartridge)[cart_address+0x1000];
         gbMemMap[0x2] = &(*gbCartridge)[cart_address+0x2000];
         gbMemMap[0x3] = &(*gbCartridge)[cart_address+0x3000];

         gbMemMap[0x4] = &(*gbCartridge)[cart_address+0x4000];
         gbMemMap[0x5] = &(*gbCartridge)[cart_address+0x5000];
         gbMemMap[0x6] = &(*gbCartridge)[cart_address+0x6000];
         gbMemMap[0x7] = &(*gbCartridge)[cart_address+0x7000];
         return;
      }
      return;
   }

   if(address < 0x8000)
   {
       mbc->MBC1memorymodel = (data&0x01);
      return;
   }

  /* if(address >= 0xA000 && address < 0xC000)
   {
      if((!RAMenable || !rom->RAMsize) && !RAM_always_enable)
         return;
   }*/

   gbMemMap[address>>12][address&0x0FFF] = data;
}
//-------------------------------------------------------------------------
// writememory_Rockman8:
// for Rockman8
// only game that requires memory echo emulation ?
//-------------------------------------------------------------------------
void gb_mbc::writememory_Rockman8(register unsigned short address,register byte data)
{
   if(address < 0x2000)
      return;

   if(address < 0x4000) // Is it a ROM bank switch?
   {
      data &= 0x1F;

      if(data == 0)
         data = 1;

      if(data > maxROMbank[(*gbRom)->ROMsize])
         data -= 8; // <--- MAKE IT WORK!!!

      rom_bank = data;

      cart_address = data<<14;

      cart_address &= rom_size_mask[(*gbRom)->ROMsize];

      gbMemMap[0x4] = &(*gbCartridge)[cart_address];
      gbMemMap[0x5] = &(*gbCartridge)[cart_address+0x1000];
      gbMemMap[0x6] = &(*gbCartridge)[cart_address+0x2000];
      gbMemMap[0x7] = &(*gbCartridge)[cart_address+0x3000];

      return;
   }

   if(address < 0x8000)
      return;

   // Always allow RAM writes.

   gbMemMap[address>>12][address&0x0FFF] = data;
}

//-------------------------------------------------------------------------
// writememory_MBC2:
// for MBC2
//-------------------------------------------------------------------------
void gb_mbc::writememory_MBC2(register unsigned short address,register byte data)
{
   if(address < 0x2000)// Is it a RAM bank enable/disable?
   {
      if(!(address&0x0100))
         RAMenable =  (data&0x0F) == 0x0A;
      return;
   }

   if(address < 0x4000) // Is it a ROM bank switch?
   {
      //if(address&0x0100)
      {
         data &= 0x0F;
         if(data==0)
            data=1;
         if(data > maxROMbank[(*gbRom)->ROMsize])
            data = maxROMbank[(*gbRom)->ROMsize];

         rom_bank = data;

         int cadr = data<<14;
         gbMemMap[0x4] = &(*gbCartridge)[cadr];
         gbMemMap[0x5] = &(*gbCartridge)[cadr+0x1000];
         gbMemMap[0x6] = &(*gbCartridge)[cadr+0x2000];
         gbMemMap[0x7] = &(*gbCartridge)[cadr+0x3000];
      }
      return;
   }

   if(address < 0x6000) // Is it a RAM bank switch?
      return;

   if(address < 0x8000)
      return;

 /*  if(address >= 0xA000 && address < 0xC000)
   {
      if(!RAMenable || !rom->battery)
         return;
   }*/

   gbMemMap[address>>12][address&0x0FFF] = data;
}

void gb_mbc::rtc_update()
{
   if(mbc->rtc.control&0x40)
   {
       mbc->rtc.last_time = time(0);
      return;
   }

   time_t now = time(0);
   time_t diff = now-mbc->rtc.last_time;
   if(diff > 0)
   {
    mbc->rtc.s += diff % 60;
    if(mbc->rtc.s > 59)
    {
      mbc->rtc.s -= 60;
      mbc->rtc.m++;
    }

    diff /= 60;

    mbc->rtc.m += diff % 60;
    if(mbc->rtc.m > 59)
    {
      mbc->rtc.m -= 60;
      mbc->rtc.h++;
    }

    diff /= 60;

    mbc->rtc.h += diff % 24;
    if(mbc->rtc.h > 24)
    {
      mbc->rtc.h -= 24;
      mbc->rtc.d++;
    }
    diff /= 24;

    mbc->rtc.d += diff;
    if(mbc->rtc.d > 255)
    {
      if(mbc->rtc.d > 511)
      {
         mbc->rtc.d %= 512;
         mbc->rtc.control |= 0x80;
      }
      mbc->rtc.control = (mbc->rtc.control & 0xfe) | (mbc->rtc.d>255 ? 1 : 0);
    }
  }
  mbc->rtc.last_time = now;
}

//-------------------------------------------------------------------------
// writememory_poke:
// for Pokemon Red & Blue 2-in-1
//-------------------------------------------------------------------------
int bank0_change = 0;

void gb_mbc::writememory_poke(register unsigned short address,register byte data)
{
   if(address < 0x2000)// Is it a RAM bank enable/disable?
   {
      RAMenable = ( (data&0x0A) == 0x0A ? 1 : 0);
      bank0_change = ( (data&0xC0) == 0xC0 ? 1 : 0);
      return;
   }

   if(address < 0x4000) // Is it a ROM bank switch?
   {
      data &= 0x7F;
      if(data==0)
         data=1;
      data += MBChi;

      rom_bank = data;

      int cadr = data<<14;
      cadr &= rom_size_mask[(*gbRom)->ROMsize];
      gbMemMap[0x4] = &(*gbCartridge)[cadr];
      gbMemMap[0x5] = &(*gbCartridge)[cadr+0x1000];
      gbMemMap[0x6] = &(*gbCartridge)[cadr+0x2000];
      gbMemMap[0x7] = &(*gbCartridge)[cadr+0x3000];
      return;
   }

   if(address < 0x6000) // Is it a RAM bank switch?
   {
      if((*gbRom)->RAMsize <= 2) // no need to change it if there isn't over 8KB ram
         return;

      data &= 0x03;

      if(data > maxRAMbank[(*gbRom)->RAMsize])
         data = maxRAMbank[(*gbRom)->RAMsize];

      ram_bank = data;

      int madr = data<<13;
      gbMemMap[0xA] = &(*gbCartRam)[madr];
      gbMemMap[0xB] = &(*gbCartRam)[madr+0x1000];
      return;
   }

   if(address < 0x8000)
   {
      return;
   }

   if(address >= 0xA000 && address < 0xC000)
   {
      if(bank0_change && address==0xA100 && !mbc->bc_select)
      {
         MBClo = 0;
         if(data==1)
            MBChi = 2;
         else
         if(data!=0xc0)
            MBChi = 66;
         else
            mbc->bc_select = 1;

         cart_address = MBChi<<14;

         gbMemMap[0x0] = &(*gbCartridge)[cart_address];
         gbMemMap[0x1] = &(*gbCartridge)[cart_address+0x1000];
         gbMemMap[0x2] = &(*gbCartridge)[cart_address+0x2000];
         gbMemMap[0x3] = &(*gbCartridge)[cart_address+0x3000];

         gbMemMap[0x4] = &(*gbCartridge)[cart_address+0x4000];
         gbMemMap[0x5] = &(*gbCartridge)[cart_address+0x5000];
         gbMemMap[0x6] = &(*gbCartridge)[cart_address+0x6000];
         gbMemMap[0x7] = &(*gbCartridge)[cart_address+0x7000];
         return;
      }
      //if(!RAMenable || !rom->RAMsize)
      //   return;
   }

   gbMemMap[address>>12][address&0x0FFF] = data;
}

//-------------------------------------------------------------------------
// writememory_MBC3:
// for MBC3
//-------------------------------------------------------------------------
void gb_mbc::writememory_MBC3(register unsigned short address,register byte data)
{

   if(address < 0x2000)// Is it a RAM bank enable/disable?
   {
      RAMenable = ( (data&0x0A) == 0x0A ? 1 : 0);
      return;
   }

   if(address < 0x4000) // Is it a ROM bank switch?
   {
      data &= 0x7F;
      if(data==0)
         data=1;

      rom_bank = data;

      int cadr = data<<14;
      cadr &= rom_size_mask[(*gbRom)->ROMsize];
      gbMemMap[0x4] = &(*gbCartridge)[cadr];
      gbMemMap[0x5] = &(*gbCartridge)[cadr+0x1000];
      gbMemMap[0x6] = &(*gbCartridge)[cadr+0x2000];
      gbMemMap[0x7] = &(*gbCartridge)[cadr+0x3000];
      return;
   }

   if(address < 0x6000) // Is it a RAM bank switch?
   {
      if((*gbRom)->RTC && data>8)
      {
         mbc->RTCIO = 1;
         mbc->rtc.cur_register = data;

         return;
      } else mbc->RTCIO = 0;

      if((*gbRom)->RAMsize <= 2) // no need to change it if there isn't over 8KB ram
         return;

      data &= 0x03;

      if(data > maxRAMbank[(*gbRom)->RAMsize])
         data = maxRAMbank[(*gbRom)->RAMsize];

      ram_bank = data;

      int madr = data<<13;
      gbMemMap[0xA] = &(*gbCartRam)[madr];
      gbMemMap[0xB] = &(*gbCartRam)[madr+0x1000];
      return;
   }

   if(address < 0x8000)
   {
      if(data == 1)
      {
         rtc_update();
         mbc->RTC_latched = !mbc->RTC_latched;
         mbc->rtc_latch = mbc->rtc;
      }
      return;
   }

   if(address >= 0xA000 && address < 0xC000)
   {
      if(RAMenable && (*gbRom)->RTC && mbc->RTCIO)
      {
         time(&(mbc->rtc).last_time);
         switch(mbc->rtc.cur_register)
         {
            case 0x08:
               mbc->rtc.s = data;
            break;
            case 0x09:
               mbc->rtc.m = data;
            break;
            case 0x0A:
               mbc->rtc.h = data;
            break;
            case 0x0B:
               mbc->rtc.d = data;
            break;
            case 0x0C:
               mbc->rtc.control = data;
               mbc->rtc.d |= (data&1)<<8;
            break;
         }
      }
      //if(!RAMenable || !rom->RAMsize)
      //   return;
   }

   gbMemMap[address>>12][address&0x0FFF] = data;
}

//-------------------------------------------------------------------------
// writememory_MBC5:
// for MBC5 and MBC5 rumble
//-------------------------------------------------------------------------
void gb_mbc::writememory_MBC5(register unsigned short address,register byte data,bool isNiutoude,bool isSintax)
{
    bool vfmulti = true;
    if ( vfmulti && !mbc->bc_select ) {

        if ( /*address & 0xF000 == 0x5000*/ address >= 0x5000 && address <= 0x5FFF ) {
            vfmultimode = data;
        }
        if ( /*address & 0xF000 == 0x7000*/ address >= 0x7000 && address <= 0x7FFF ) {
            bool effectChange = false;
            switch( vfmultimode ) {
                case 0xAA:
                    if ( vfmultibank == 0 ) {
                        vfmultibank = data;
                    } else {
                        effectChange = true;
                    }
                    break;
                case 0xBB:
                    vfmultimem = data;
                    break;
                case 0x55:
                    vfmultifinal = data;
                    effectChange = true;
                    break;
            }
            if ( effectChange ) {

                byte size = vfmultifinal & 0x07;
                byte eightMegBankNo = ( vfmultifinal & 0x08 ) >> 3; // 0 or 1
                byte doReset = ( vfmultifinal & 0x80 ) >> 7; // 0 or 1 again

                int addroffset = vfmultibank << 15;
                addroffset += (eightMegBankNo << 0x17);
                superaddroffset = addroffset;

                wchar_t wrmessage[50];
                wsprintf(wrmessage,L"MM %X %X",superaddroffset,vfmultibank);
                renderer.showMessage(wrmessage,60,GB1);

                gbMemMap[0x0] = &(*gbCartridge)[addroffset];
                gbMemMap[0x1] = &(*gbCartridge)[addroffset+0x1000];
                gbMemMap[0x2] = &(*gbCartridge)[addroffset+0x2000];
                gbMemMap[0x3] = &(*gbCartridge)[addroffset+0x3000];

                gbMemMap[0x4] = &(*gbCartridge)[addroffset+0x4000];
                gbMemMap[0x5] = &(*gbCartridge)[addroffset+0x5000];
                gbMemMap[0x6] = &(*gbCartridge)[addroffset+0x6000];
                gbMemMap[0x7] = &(*gbCartridge)[addroffset+0x7000];

                // todo: Do the bank switch, changes the effective ROM..
                // todo: Do the memory switch
                if ( doReset ) {
                      GB1->reset(true,true);
                  /*  GB1->load_rom(L"dodgy-hardcoded-path",superaddroffset); // reload the ROM from a new offset
                      superaddroffset = 0;
                      GB1->reset();
                    */
                  // IT SEEMS that if we do a reset here it just doesnt fuck work properly anyway?
                  // Need to re-reset thru the menu (assuming superaddroffset not changed) & then it works
                }

               if ( vfmultifinal>0) mbc->bc_select = 1;

                vfmultimode=0; vfmultibank=0; vfmultimem=0; vfmultifinal = 0;

                return;
            }
        }
    }

   if(address < 0x2000)// Is it a RAM bank enable/disable?
   {
      RAMenable = ( (data&0x0A) == 0x0A ? 1 : 0);
      return;
   }

   if(address < 0x3000)
   {
	   	// 2100 needs to be not-ignored (for Cannon Fodder's sound)
	   	// but 2180 DOES need to be ignored (for FF DX3)
	   	// Determined to find the right number here
      if (isNiutoude && address > 0x2100) {
      	return;
      }
      byte origData = data;

      if (isSintax) {
      	switch(mbc->sintax_mode & 0x0f) {
      		// Maybe these could go in a config file, so new ones can be added easily?
      		case 0x0D: {
      			byte flips[] = {6,7,0,1,2,3,4,5};
      			data = switchOrder( data, flips );
      			//data = ((data & 0x03) << 6 ) + ( data >> 2 );
      			break;
      		}
      		case 0x09: {
      		//	byte flips[] = {4,5,2,3,7,6,1,0}; // Monkey..no
      			byte flips[] = {3,2,5,4,7,6,1,0};
      			data = switchOrder( data, flips );
      			break;
      		}

      		case 0x00: { // 0x10=lion 0x00 hmmmmm // 1 and 0 unconfirmed
      			byte flips[] = {0,7,2,1,4,3,6,5};
      			data = switchOrder( data, flips );
      			break;
      		}

      		case 0x01: {
      			byte flips[] = {7,6,1,0,3,2,5,4};
      			data = switchOrder( data, flips );
      			break;
      		}

      		case 0x05: {
      			byte flips[] = {0,1,6,7,4,5,2,3}; // Not 100% on this one
      			data = switchOrder( data, flips );
      			break;
      		}

      		case 0x07: {
      			byte flips[] = {5,7,4,6,2,3,0,1}; // 5 and 7 unconfirmed
      			data = switchOrder( data, flips );
      			break;
      		}

      		case 0x0B: {
      			byte flips[] = {5,4,7,6,1,0,3,2}; // 5 and 6 unconfirmed
      			data = switchOrder( data, flips );
      			break;
      		}

      	}

      	setXorForBank(origData);

      }


      // Set current xor so we dont have to figure it out on every read

      rom_bank = data|(MBChi<<8);
      cart_address = rom_bank<<14;

      cart_address &= rom_size_mask[(*gbRom)->ROMsize];

      cart_address += superaddroffset;

      MBClo = data;

      gbMemMap[0x4] = &(*gbCartridge)[cart_address];
      gbMemMap[0x5] = &(*gbCartridge)[cart_address+0x1000];
      gbMemMap[0x6] = &(*gbCartridge)[cart_address+0x2000];
      gbMemMap[0x7] = &(*gbCartridge)[cart_address+0x3000];

    //  if(origData == 0x69) {
	//    	char buff[100];
	//		sprintf(buff,"%X %X %X",origData,data,cart_address);
	//		debug_print(buff);
    //  }



      return;
   }

   if(address < 0x4000) // Is it a ROM bank switch?
   {
      data = data&1;

      rom_bank = MBClo|(data<<8);

      cart_address = rom_bank<<14;

      cart_address &= rom_size_mask[(*gbRom)->ROMsize];

      cart_address += superaddroffset;

      MBChi = data;

      gbMemMap[0x4] = &(*gbCartridge)[cart_address];
      gbMemMap[0x5] = &(*gbCartridge)[cart_address+0x1000];
      gbMemMap[0x6] = &(*gbCartridge)[cart_address+0x2000];
      gbMemMap[0x7] = &(*gbCartridge)[cart_address+0x3000];

      return;
   }

   if(address < 0x6000) // Is it a RAM bank switch?
   {

   		// sintaxs not entirely understood addressing thing hi

   		// check sintax_mode was not already set; if it was, ignore it (otherwise Metal Max breaks)
   		if (isSintax && mbc->sintax_mode ==0 && address >= 0x5000 ) {

   		 switch(0x0F & data) {
   		 	case 0x0D: // old
   		 	case 0x09: // ???
   		 	case 0x00:// case 0x10: // LiON, GoldenSun
   		 	case 0x01: // LANGRISSER
   		 	case 0x05: // Maple, PK Platinum
   		 	case 0x07: // Bynasty5
   		 	case 0x0B: // Shaolin
   		 		// These are all supported
   		 	break;

   		 	default:
	 			char buff[100];
   		 		sprintf(buff,"Unknown Sintax Mode %X Addr %X - probably won't work!",data,address);
   		 		debug_print(buff);
   			break;
   		 }
   		 mbc->sintax_mode=data;

   		 writememory_MBC5(0x2000,01,false,true); // force a fake bank switch

   		 return;

   		}

      if((*gbRom)->rumble)
      {
         if(data&0x08)
            *gbRumbleCounter = 4;
         data &= 0x07;
      }

      if((*gbRom)->RAMsize <= 2) // no need to change it if there isn't over 8KB ram
         return;

      data &= 0x0F;

      if(data > maxRAMbank[(*gbRom)->RAMsize])
         data = maxRAMbank[(*gbRom)->RAMsize];

      ram_bank = data;

      int madr = data<<13;
      gbMemMap[0xA] = &(*gbCartRam)[madr];
      gbMemMap[0xB] = &(*gbCartRam)[madr+0x1000];
      return;
   }

   if(address<0x8000)
   {
   		if ( isSintax && address >= 0x7000 ) {

   			int xorNo = ( address & 0x00F0 ) >> 4;
   			switch (xorNo) {
   				case 2:
   					mbc->sintax_xor2 = data;
   				break;
   				case 3:
   					mbc->sintax_xor3 = data;
   				break;
   				case 4:
   					mbc->sintax_xor4 = data;
   				break;
   				case 5:
					mbc->sintax_xor5 = data;
   				break;
   			}

   			if (*gbRomBankXor == 0 ) {
   				setXorForBank(4);
   			}


   			return;

   		}


    /*  if(++mbc->bc_select == 2 && rom->ROMsize>1)
      {
         MBChi = (data&0xFF);

         cart_address = (MBChi<<1)<<14;

         mem_map[0x0] = &cartridge[cart_address];
         mem_map[0x1] = &cartridge[cart_address+0x1000];
         mem_map[0x2] = &cartridge[cart_address+0x2000];
         mem_map[0x3] = &cartridge[cart_address+0x3000];

         mem_map[0x4] = &cartridge[cart_address+0x4000];
         mem_map[0x5] = &cartridge[cart_address+0x5000];
         mem_map[0x6] = &cartridge[cart_address+0x6000];
         mem_map[0x7] = &cartridge[cart_address+0x7000];
      }*/
      return;
   }

 /*  if(address >= 0xA000 && address < 0xC000)
   {
      if(!RAMenable || !rom->RAMsize)
         return;
   }*/

   gbMemMap[address>>12][address&0x0FFF] = data;
}

//-------------------------------------------------------------------------
// writememory_Camera:
// for the Gameboy Pocket Camera
//-------------------------------------------------------------------------
void gb_mbc::writememory_Camera(register unsigned short address,register byte data)
{
   if(address < 0x2000)// Is it a RAM bank enable/disable?
   {
      RAMenable = ( (data&0x0A) == 0x0A ? 1 : 0);
      return;
   }

   if(address < 0x4000) // Is it a ROM bank switch?
   {
      if(data == 0)
         data = 1;
      if(data > maxROMbank[(*gbRom)->ROMsize])
         data = maxROMbank[(*gbRom)->ROMsize];

      rom_bank = data;

      int cadr = data<<14;
      gbMemMap[0x4] = &(*gbCartridge)[cadr];
      gbMemMap[0x5] = &(*gbCartridge)[cadr+0x1000];
      gbMemMap[0x6] = &(*gbCartridge)[cadr+0x2000];
      gbMemMap[0x7] = &(*gbCartridge)[cadr+0x3000];
      return;
   }

   if(address < 0x6000) // Is it a RAM bank switch?
   {
      if(data == 0x10)
      {
         mbc->cameraIO = 1;
         return;
      }
      else
         mbc->cameraIO = 0;

      data &= 0x0F;

      if(data > maxRAMbank[(*gbRom)->RAMsize])
         data = maxRAMbank[(*gbRom)->RAMsize];

      ram_bank = data;

      int madr = data<<13;
      gbMemMap[0xA] = &(*gbCartRam)[madr];
      gbMemMap[0xB] = &(*gbCartRam)[madr+0x1000];
      return;

   }

   if(address<0x8000)
      return;

 /*  if(address >= 0xA000 && address < 0xC000)
   {
      if(!RAMenable)
         return;
   }*/

   gbMemMap[address>>12][address&0x0FFF] = data;
}

void gb_mbc::update_HuC3time()
{
   time_t now = time(0);
   time_t diff = now-mbc->HuC3_last_time;
   if(diff > 0)
   {
      mbc->rtc.s += diff % 60; // use mbc->rtc.s to store seconds
      if(mbc->rtc.s > 59)
      {
          mbc->rtc.s -= 60;
         mbc->HuC3_time++;
      }

      diff /= 60;

      mbc->HuC3_time += diff % 60;
      if((mbc->HuC3_time&0xFFF) > 1439)
      {
         mbc->HuC3_time = (mbc->HuC3_time&0xFFFF000)|((mbc->HuC3_time&0xFFF)-1440);
         mbc->HuC3_time += 0x1000; // day counter ?
      }

      diff /= 60;

      mbc->HuC3_time += (diff % 24)*60;
      if((mbc->HuC3_time&0xFFF) > 1439)
      {
         mbc->HuC3_time = (mbc->HuC3_time&0xFFFF000)|((mbc->HuC3_time&0xFFF)-1440);
         mbc->HuC3_time += 0x1000; // day counter ?
      }

      diff /= 24;

      mbc->HuC3_time += (diff<<12);
      if(((mbc->HuC3_time&0xFFF000)>>12) > 356)
      {
         mbc->HuC3_time = mbc->HuC3_time&0xF000FFF;
         mbc->HuC3_time += 0x1000000; // year counter ????
      }
   }
   mbc->HuC3_last_time = now;
}

//-------------------------------------------------------------------------
// writememory_HuC3:
// for HuC-3
//-------------------------------------------------------------------------
void gb_mbc::writememory_HuC3(register unsigned short address,register byte data)
{
   if(address < 0x2000)// Is it a RAM bank enable/disable?
   {
      RAMenable = ( (data&0x0A) == 0x0A ? 1 : 0);
      mbc->HuC3_RAMflag = data;
      return;
   }

   if(address < 0x4000) // Is it a ROM bank switch?
   {
      if(data == 0)
         data = 1;
      if(data > maxROMbank[(*gbRom)->ROMsize])
         data = maxROMbank[(*gbRom)->ROMsize];

      rom_bank = data;

      int cadr = data<<14;
      gbMemMap[0x4] = &(*gbCartridge)[cadr];
      gbMemMap[0x5] = &(*gbCartridge)[cadr+0x1000];
      gbMemMap[0x6] = &(*gbCartridge)[cadr+0x2000];
      gbMemMap[0x7] = &(*gbCartridge)[cadr+0x3000];
      return;
   }

   if(address < 0x6000) // Is it a RAM bank switch?
   {
      data &= 0x0F;

      if(data > maxRAMbank[(*gbRom)->RAMsize])
         data = maxRAMbank[(*gbRom)->RAMsize];

      ram_bank = data;

      int madr = data<<13;
      gbMemMap[0xA] = &(*gbCartRam)[madr];
      gbMemMap[0xB] = &(*gbCartRam)[madr+0x1000];
      return;
   }

   if(address < 0x8000) // programs will write 1 here
      return;

   if(address >= 0xA000 && address < 0xC000)
   {
      if(mbc->HuC3_RAMflag < 0x0b || mbc->HuC3_RAMflag > 0x0e) // write to RAM
      {
         if(!RAMenable || !(*gbRom)->RAMsize)
            return;
      } else
      {
         if(mbc->HuC3_RAMflag == 0x0B) // send command ?
         {
            switch(data & 0xf0)
            {
            case 0x10: // read time
               update_HuC3time();
               if(mbc->HuC3_flag == HUC3_READ)
               {
                  mbc->HuC3_RAMvalue = ((mbc->HuC3_time>>mbc->HuC3_shift)&0x0F);
                  mbc->HuC3_shift += 4;
                  if(mbc->HuC3_shift > 24)
                     mbc->HuC3_shift = 0;
               }
            break;
            case 0x30: // write to registers (minute,day and year(?) counters)
               // to write time 23:59 program will send commands
               // 3F 39 35 30 30 30 31
               // mbc->HuC3_time will then be 59F = 1439 = 23*60+59 minutes
               if(mbc->HuC3_flag == HUC3_WRITE)
               {
                  if(mbc->HuC3_shift == 0)
                     mbc->HuC3_time = 0;
                  if(mbc->HuC3_shift < 24)
                  {
                     mbc->HuC3_time |= ((data&0x0F)<<mbc->HuC3_shift);
                     mbc->HuC3_shift += 4;
                     if(mbc->HuC3_shift == 24)
                        mbc->HuC3_flag = HUC3_READ;
                  }
               }
            break;
            case 0x40: // special command ?
               switch(data&0x0F)
               {
               case 0x00: //  ?
                  //HuC3_flag = HUC3_READ;
                  mbc->HuC3_shift = 0;
               break;
               case 0x03: // write time mode ?
                  mbc->HuC3_flag = HUC3_WRITE;
                  mbc->HuC3_shift = 0;
               break;
               case 0x07: // read time mode ?
                  mbc->HuC3_flag = HUC3_READ;
                  mbc->HuC3_shift = 0;
               break;
               case 0x06: // alarm clock sound test
               break;
               case 0x08: // set alarm clock time ?
                  //HuC3_flag = HUC3_NONE;
               break;
               case 0x0F: // yobidashi (call) ?
               break;
               default:
                  //char buffer[100];
                  //sprintf(buffer,"HuC3-command:%x",data);
                  //debug_print(buffer);
               break;
               }
            break;
            case 0x50: // ?
            {
               //HuC3_register[0] = (HuC3_register[0] & 0x0f) | ((data << 4)&0x0f);
            }
            break;
            case 0x60: // ?
            {
               //mbc->HuC3_RAMvalue = 1;
               mbc->HuC3_flag = HUC3_READ;
            }
            break;
            }
         } else if(mbc->HuC3_RAMflag == 0x0C) // not used ?
         {
            // ?
         } else if(mbc->HuC3_RAMflag == 0x0D) // programs will write 0xFE here
         {
            // maybe a execute command function ?
         }
      }
   }

   gbMemMap[address>>12][address&0x0FFF] = data;
}

//-------------------------------------------------------------------------
// writememory_MBC7:
// for MBC7
//-------------------------------------------------------------------------
void gb_mbc::writememory_MBC7(register unsigned short address,register byte data)
{
   if(address < 0x2000)
      return;

   if(address < 0x4000) // Is it a ROM bank switch?
   {
      data = data&0x7F;

      if(data==0)
         data=1;


       rom_bank = data;

      cart_address = data<<14;

      cart_address &= rom_size_mask[(*gbRom)->ROMsize];

       gbMemMap[0x4] = &(*gbCartridge)[cart_address];
       gbMemMap[0x5] = &(*gbCartridge)[cart_address+0x1000];
       gbMemMap[0x6] = &(*gbCartridge)[cart_address+0x2000];
       gbMemMap[0x7] = &(*gbCartridge)[cart_address+0x3000];
      return;
   }

   if(address < 0x6000) // Is it a RAM bank switch?
   {
      if(data<8)
      {
          RAMenable = 0;

         if((*gbRom)->RAMsize <= 2) // no need to change it if there isn't over 8KB ram
            return;

         data &= 0x03;

         if(data > maxRAMbank[(*gbRom)->RAMsize])
            data = maxRAMbank[(*gbRom)->RAMsize];

          ram_bank = data;

         int madr = data<<13;
          gbMemMap[0xA] = &(*gbCartRam)[madr];
          gbMemMap[0xB] = &(*gbCartRam)[madr+0x1000];
         return;  
      } else
      {
          RAMenable=0;
         return;
      }

   }
   
   if(address < 0x8000)
      return;
   
   if(address >= 0xA000 && address < 0xC000)
   {
      if(address == 0xa080) 
      {
         // special processing needed
         int oldCs = mbc->MBC7_cs,oldSk=mbc->MBC7_sk;

          mbc->MBC7_cs=data>>7;
          mbc->MBC7_sk=(data>>6)&1;
    
         if(!oldCs && mbc->MBC7_cs)
         {
            if(mbc->MBC7_state == 5)
            {
               if(mbc->MBC7_writeEnable)
               {
                   (*gbMemory)[0xa000+mbc->MBC7_address*2] = mbc->MBC7_buffer>>8;
                   (*gbMemory)[0xa000+mbc->MBC7_address*2+1] = mbc->MBC7_buffer&0xff;
               }
                mbc->MBC7_state = 0;
                mbc->MBC7_value = 1;
            } else 
            {
                mbc->MBC7_idle = true;
                mbc->MBC7_state = 0;
            }
        }
    
        if(!oldSk && mbc->MBC7_sk)
        {
           if(mbc->MBC7_idle)
           {
              if(data & 0x02) 
              {
                  mbc->MBC7_idle = false;
                  mbc->MBC7_count = 0;
                  mbc->MBC7_state = 1;
              }
           } else 
           {
              switch(mbc->MBC7_state)
              {
              case 1:
                 // receiving command
                  mbc->MBC7_buffer <<= 1;
                  mbc->MBC7_buffer |= (data & 0x02)?1:0;
                  mbc->MBC7_count++;
                 if(mbc->MBC7_count == 2)
                 {
                    // finished receiving command
                     mbc->MBC7_state = 2;
                     mbc->MBC7_count = 0;
                     mbc->MBC7_code = mbc->MBC7_buffer & 3;
                 }
              break;
              case 2:
                 // receive address
                  mbc->MBC7_buffer <<= 1;
                  mbc->MBC7_buffer |= (data&0x02)?1:0;
                  mbc->MBC7_count++;
                 if(mbc->MBC7_count==8)
                 {
                    // finish receiving
                    mbc->MBC7_state = 3;
                    mbc->MBC7_count = 0;
                    mbc->MBC7_address = mbc->MBC7_buffer&0xff;
                    if(mbc->MBC7_code == 0)
                    {
                       if((mbc->MBC7_address>>6) == 0)
                       {
                          mbc->MBC7_writeEnable = 0;
                          mbc->MBC7_state = 0;
                       } else if((mbc->MBC7_address>>6) == 3)
                       {
                          mbc->MBC7_writeEnable = 1;
                          mbc->MBC7_state = 0;
                       }
                    }
                 }
              break;
              case 3:
                 mbc->MBC7_buffer <<= 1;
                 mbc->MBC7_buffer |= (data&0x02)?1:0;
                 mbc->MBC7_count++;
          
                 switch(mbc->MBC7_code)
                 {
                 case 0:
                    if(mbc->MBC7_count==16)
                    {
                       if((mbc->MBC7_address>>6)==0)
                       {
                          mbc->MBC7_writeEnable = 0;
                          mbc->MBC7_state = 0;
                       } else if((mbc->MBC7_address>>6) == 1)
                       {
                          if(mbc->MBC7_writeEnable)
                          {
                             for(int i=0;i<256;i++) 
                             {
                                 (*gbMemory)[0xa000+i*2] = mbc->MBC7_buffer >> 8;
                                 (*gbMemory)[0xa000+i*2+1] = mbc->MBC7_buffer & 0xff;
                             }
                          }
                          mbc->MBC7_state = 5;
                       } else if((mbc->MBC7_address>>6) == 2)
                       {
                          if(mbc->MBC7_writeEnable)
                          {
                             for(int i=0;i<256;i++)
                                *((unsigned short *)&((*gbMemory))[0xa000+i*2]) = 0xffff;
                          }
                          mbc->MBC7_state = 5;
                       } else if((mbc->MBC7_address>>6) == 3)
                       {
                          mbc->MBC7_writeEnable = 1;
                          mbc->MBC7_state = 0;
                       }
                       mbc->MBC7_count = 0;
                    }
                 break;
                 case 1:
                    if(mbc->MBC7_count == 16)
                    {
                       mbc->MBC7_count = 0;
                       mbc->MBC7_state = 5;
                       mbc->MBC7_value = 0;
                    }
                 break;
                 case 2:
                    if(mbc->MBC7_count == 1)
                    {
                       mbc->MBC7_state = 4;
                       mbc->MBC7_count = 0;
                       mbc->MBC7_buffer = ((*gbMemory)[0xa000+mbc->MBC7_address*2]<<8)|((*gbMemory)[0xa000+mbc->MBC7_address*2+1]);
                    }
                 break;
                 case 3:
                    if(mbc->MBC7_count == 16)
                    {
                       mbc->MBC7_count = 0;
                       mbc->MBC7_state = 5;
                       mbc->MBC7_value = 0;
                       mbc->MBC7_buffer = 0xffff;
                    }
                 break;
                 }
              break;
              }
          }
       }
    
       if(oldSk && !mbc->MBC7_sk)
       {
          if(mbc->MBC7_state == 4)
          { 
             mbc->MBC7_value = (mbc->MBC7_buffer & 0x8000)?1:0;
             mbc->MBC7_buffer <<= 1;
             mbc->MBC7_count++;
             if(mbc->MBC7_count == 16)
             {
                mbc->MBC7_count = 0;
                mbc->MBC7_state = 0;
             }
          }
       }
      }
      return;
   }
   
    gbMemMap[address>>12][address&0x0FFF] = data;
}

//-------------------------------------------------------------------------
// writememory_TAMA5:
// for Bandai TAMA5 (Tamagotchi3)
//-------------------------------------------------------------------------
void gb_mbc::writememory_TAMA5(register unsigned short address,register byte data)
{   
   if(address < 0x8000) // ?
   {  
      return;   
   }
   
   if(address >= 0xA000 && address < 0xC000)
   {       
      if(address == 0xA000)
      {                 
         switch(mbc->tama_flag)
         {
         case 0: // rom bank lower nibble
         {
            data &= 0x0F;

             MBClo = data;
             rom_bank = MBClo|(MBChi<<4);
       
            int cadr = rom_bank<<14;
             gbMemMap[0x4] = &(*gbCartridge)[cadr];
             gbMemMap[0x5] = &(*gbCartridge)[cadr+0x1000];
             gbMemMap[0x6] = &(*gbCartridge)[cadr+0x2000];
             gbMemMap[0x7] = &(*gbCartridge)[cadr+0x3000];
         }
         return;
         case 1: // rom bank high bit
         {
            data &= 0x01;
            
            MBChi = data;
            
            rom_bank = MBClo|(MBChi<<4);
       
            int cadr = rom_bank<<14;
            gbMemMap[0x4] = &(*gbCartridge)[cadr];
            gbMemMap[0x5] = &(*gbCartridge)[cadr+0x1000];
            gbMemMap[0x6] = &(*gbCartridge)[cadr+0x2000];
            gbMemMap[0x7] = &(*gbCartridge)[cadr+0x3000];
         }
         return;
         case 4: // RTC controls
            mbc->tama_val4 = data;
         return;
         case 5: // write time (and write memory????)
            mbc->tama_val5 = data;
            ++mbc->tama_count;
            if(mbc->tama_count==1 && data == 0) mbc->tama_change_clock |= 2;
            if(mbc->tama_count==2 && data == 1) mbc->tama_change_clock |= 1;
            if(mbc->tama_change_clock == 3) mbc->rtc.last_time = time(0);

             (*gbMemory)[0xA000+(mbc->tama_val6<<4)+mbc->tama_val7] = mbc->tama_val4|(data<<4);
            
            //which time counter is changed?
            if(mbc->tama_count==6 && mbc->tama_change_clock==3)
            {
               mbc->tama_month = data;
            } else
            if(mbc->tama_count==7 && mbc->tama_change_clock==3)
            {
               mbc->tama_month += data*10;
            } else
            if(mbc->tama_count==8 && mbc->tama_change_clock==3)
            {
               mbc->rtc.d = data;
            } else
            if(mbc->tama_count==9 && mbc->tama_change_clock==3)
            {
                mbc->rtc.d += data*10;
            } else
            if(mbc->tama_count==10 && mbc->tama_change_clock==3)
            {
                mbc->rtc.m = data*10;
            } else
            if(mbc->tama_count==11 && mbc->tama_change_clock==3)
            {
                mbc->rtc.h = data*10;
            } else
            if(mbc->tama_change_clock==3 && mbc->tama_count >= 13)
            {
                mbc->tama_count = 0;
                mbc->tama_change_clock = 0;
            } else
            if(mbc->tama_change_clock!=3 && mbc->tama_count >= 2)
            {
                mbc->tama_count=0;
                mbc->tama_change_clock = 0;
            }
         return;         
         case 6: // RTC controls
             mbc->tama_val6 = data;
         return;         
         case 7: // RTC controls
             mbc->tama_val7 = data;
         return;
         }
      } else
      if(address == 0xA001)
      {
          mbc->tama_flag = data;
                  
         return;
      } 
   } 

    gbMemMap[address>>12][address&0x0FFF] = data;
}

void gb_mbc::readMbcSpecificStuffFromSaveFile(FILE *savefile) {
    if((*gbRom)->RTC || (*gbRom)->bankType == TAMA5)
    {
        fwrite(&(mbc->rtc).s, sizeof(int), 1, savefile);
        fwrite(&(mbc->rtc).m, sizeof(int), 1, savefile);
        fwrite(&(mbc->rtc).h, sizeof(int), 1, savefile);
        fwrite(&(mbc->rtc).d, sizeof(int), 1, savefile);
        fwrite(&(mbc->rtc).control, sizeof(int), 1, savefile);
        fwrite(&(mbc->rtc).last_time, sizeof(time_t), 1, savefile);
    }

    if((*gbRom)->bankType == TAMA5)
        fwrite(&(mbc->tama_month), sizeof(int), 1, savefile);

    if((*gbRom)->bankType == HuC3)
    {
        fwrite(&(mbc->HuC3_time), sizeof(unsigned int), 1, savefile);
        fwrite(&(mbc->HuC3_last_time), sizeof(time_t), 1, savefile);
        fwrite(&(mbc->rtc).s, sizeof(int), 1, savefile);
    }
}

void gb_mbc::writeMbcSpecificStuffToSaveFile(FILE *savefile){
    if((*gbRom)->RTC || (*gbRom)->bankType == TAMA5)
    {
        fread(&(mbc->rtc).s, sizeof(int), 1, savefile);
        fread(&(mbc->rtc).m, sizeof(int), 1, savefile);
        fread(&(mbc->rtc).h, sizeof(int), 1, savefile);
        fread(&(mbc->rtc).d, sizeof(int), 1, savefile);
        fread(&(mbc->rtc).control, sizeof(int), 1, savefile);
        fread(&(mbc->rtc).last_time, sizeof(time_t), 1, savefile);
        mbc->rtc_latch = mbc->rtc;
    }

    if((*gbRom)->bankType == TAMA5)
        fread(&(mbc->tama_month), sizeof(int), 1, savefile);

    if((*gbRom)->bankType == HuC3)
    {
        fread(&(mbc->HuC3_time), sizeof(unsigned int), 1, savefile);
        fread(&(mbc->HuC3_last_time), sizeof(time_t), 1, savefile);
        fread(&(mbc->rtc).s, sizeof(int), 1, savefile);
    }
}

void gb_mbc::readNewerCartSpecificVarsFromStateFile(FILE *statefile) {
    if((*gbRom)->bankType == TAMA5)
    {
        readMoreTama5VarsFromStateFile(statefile);
    }
}

void gb_mbc::readCartSpecificVarsFromStateFile(FILE *statefile){
    if((*gbRom)->RTC)
    {
        readRtcVarsFromStateFile(statefile);
    }

    if((*gbRom)->bankType == HuC3)
    {
        readHuc3VarsFromStateFile(statefile);

    }

    if((*gbRom)->bankType == MBC7)
    {
        readMbc7VarsFromStateFile(statefile);
    }

    if((*gbRom)->bankType == TAMA5)
    {
        readTama5VarsFromStateFile(statefile);
    }
}



void gb_mbc::writeCartSpecificVarsToStateFile(FILE *statefile) {
    if((*gbRom)->RTC)
    {
        writeRtcVarsToStateFile(statefile);
    }

    if((*gbRom)->bankType == HuC3)
    {
        writeHuc3VarsToStateFile(statefile);
    }

    if((*gbRom)->bankType == MBC7)
    {
        writeMbc7VarsToStateFile(statefile);
    }

    if((*gbRom)->bankType == TAMA5)
    {
        writeTama5VarsToStateFile(statefile);
    }
}

void gb_mbc::writeNewerCartSpecificVarsToStateFile(FILE *statefile) {
    if((*gbRom)->bankType == TAMA5)
    {
        writeMoreTama5VarsToStateFile(statefile);
    }
}

void gb_mbc::writeMoreTama5VarsToStateFile(FILE *statefile) {
    fwrite(&(mbc->rtc).s, sizeof(int), 1, statefile);
    fwrite(&(mbc->rtc).m, sizeof(int), 1, statefile);
    fwrite(&(mbc->rtc).h, sizeof(int), 1, statefile);
    fwrite(&(mbc->rtc).d, sizeof(int), 1, statefile);
    fwrite(&(mbc->rtc).control, sizeof(int), 1, statefile);
    fwrite(&(mbc->rtc).last_time, sizeof(time_t), 1, statefile);

    fwrite(&(mbc->tama_time), sizeof(byte), 1, statefile);
    fwrite(&(mbc->tama_val6), sizeof(int), 1, statefile);
    fwrite(&(mbc->tama_val7), sizeof(int), 1, statefile);
    fwrite(&(mbc->tama_val4), sizeof(int), 1, statefile);
    fwrite(&(mbc->tama_val5), sizeof(int), 1, statefile);
    fwrite(&(mbc->tama_count), sizeof(int), 1, statefile);
    fwrite(&(mbc->tama_month), sizeof(int), 1, statefile);
    fwrite(&(mbc->tama_change_clock), sizeof(int), 1, statefile);
}

void gb_mbc::writeTama5VarsToStateFile(FILE *statefile) { fwrite(&(mbc->tama_flag), sizeof(int), 1, statefile); }

void gb_mbc::writeMbc7VarsToStateFile(FILE *statefile) {
    fwrite(&(mbc->MBC7_cs), sizeof(int), 1, statefile);
    fwrite(&(mbc->MBC7_sk), sizeof(int), 1, statefile);
    fwrite(&(mbc->MBC7_state), sizeof(int), 1, statefile);
    fwrite(&(mbc->MBC7_buffer), sizeof(int), 1, statefile);
    fwrite(&(mbc->MBC7_idle), sizeof(int), 1, statefile);
    fwrite(&(mbc->MBC7_count), sizeof(int), 1, statefile);
    fwrite(&(mbc->MBC7_code), sizeof(int), 1, statefile);
    fwrite(&(mbc->MBC7_address), sizeof(int), 1, statefile);
    fwrite(&(mbc->MBC7_writeEnable), sizeof(int), 1, statefile);
    fwrite(&(mbc->MBC7_value), sizeof(int), 1, statefile);
    fwrite(&sensorX,sizeof(int),1,statefile);
    fwrite(&sensorY,sizeof(int),1,statefile);
}

void gb_mbc::writeHuc3VarsToStateFile(FILE *statefile) {
    fwrite(&(mbc->HuC3_time), sizeof(unsigned int), 1, statefile);
    fwrite(&(mbc->HuC3_last_time), sizeof(time_t), 1, statefile);
    fwrite(&(mbc->rtc).s, sizeof(int), 1, statefile);

    //fwrite(mbc->HuC3_register,sizeof(int),8,statefile);
    fwrite(&(mbc->HuC3_RAMvalue), sizeof(int), 1, statefile);
    //fwrite(&mbc->HuC3_address,sizeof(int),1,statefile);
    fwrite(&(mbc->HuC3_RAMflag), sizeof(int), 1, statefile);
}

void gb_mbc::writeRtcVarsToStateFile(FILE *statefile) {
    fwrite(&(mbc->rtc).s, sizeof(int), 1, statefile);
    fwrite(&(mbc->rtc).m, sizeof(int), 1, statefile);
    fwrite(&(mbc->rtc).h, sizeof(int), 1, statefile);
    fwrite(&(mbc->rtc).d, sizeof(int), 1, statefile);
    fwrite(&(mbc->rtc).control, sizeof(int), 1, statefile);
    fwrite(&(mbc->rtc).last_time, sizeof(time_t), 1, statefile);

    fwrite(&(mbc->rtc_latch).s, sizeof(int), 1, statefile);
    fwrite(&(mbc->rtc_latch).m, sizeof(int), 1, statefile);
    fwrite(&(mbc->rtc_latch).h, sizeof(int), 1, statefile);
    fwrite(&(mbc->rtc_latch).d, sizeof(int), 1, statefile);
    fwrite(&(mbc->rtc_latch).control, sizeof(int), 1, statefile);
    fwrite(&(mbc->rtc_latch).last_time, sizeof(time_t), 1, statefile);
}

void gb_mbc::writeMbcOtherStuffToStateFile(FILE *statefile) {
    fwrite(&( mbc->MBC1memorymodel), sizeof(int), 1, statefile);
    fwrite(&(RAMenable), sizeof(int), 1, statefile);
    fwrite(&(MBChi), sizeof(unsigned int), 1, statefile);
    fwrite(&(MBClo), sizeof(unsigned int), 1, statefile);
}

void gb_mbc::writeMbcBanksToStateFile(FILE *statefile) {
    fwrite(&(rom_bank), sizeof(int), 1, statefile);
    fwrite(&(ram_bank), sizeof(int), 1, statefile);
}

void gb_mbc::readMbcMoreCrapFromStateFile(FILE *statefile) {
    fread(&( mbc->MBC1memorymodel), sizeof(int), 1, statefile);
    fread(&(RAMenable), sizeof(int), 1, statefile);
    fread(&(MBChi), sizeof(unsigned int), 1, statefile);
    fread(&(MBClo), sizeof(unsigned int), 1, statefile);
}

void gb_mbc::readMbcBanksFromStateFile(FILE *statefile) {
    fread(&(rom_bank), sizeof(int), 1, statefile);
    fread(&(ram_bank), sizeof(int), 1, statefile);
}

void gb_mbc::readMoreTama5VarsFromStateFile(FILE *statefile) {
    fread(&(mbc->rtc).s, sizeof(int), 1, statefile);
    fread(&(mbc->rtc).m, sizeof(int), 1, statefile);
    fread(&(mbc->rtc).h, sizeof(int), 1, statefile);
    fread(&(mbc->rtc).d, sizeof(int), 1, statefile);
    fread(&(mbc->rtc).control, sizeof(int), 1, statefile);
    fread(&(mbc->rtc).last_time, sizeof(time_t), 1, statefile);

    fread(&(mbc->tama_time), sizeof(byte), 1, statefile);
    fread(&(mbc->tama_val6), sizeof(int), 1, statefile);
    fread(&(mbc->tama_val7), sizeof(int), 1, statefile);
    fread(&(mbc->tama_val4), sizeof(int), 1, statefile);
    fread(&(mbc->tama_val5), sizeof(int), 1, statefile);
    fread(&(mbc->tama_count), sizeof(int), 1, statefile);
    fread(&(mbc->tama_month), sizeof(int), 1, statefile);
    fread(&(mbc->tama_change_clock), sizeof(int), 1, statefile);
}

void gb_mbc::readTama5VarsFromStateFile(FILE *statefile) { fread(&(mbc->tama_flag), sizeof(int), 1, statefile); }

void gb_mbc::readMbc7VarsFromStateFile(FILE *statefile) {
    fread(&(mbc->MBC7_cs), sizeof(int), 1, statefile);
    fread(&(mbc->MBC7_sk), sizeof(int), 1, statefile);
    fread(&(mbc->MBC7_state), sizeof(int), 1, statefile);
    fread(&(mbc->MBC7_buffer), sizeof(int), 1, statefile);
    fread(&(mbc->MBC7_idle), sizeof(int), 1, statefile);
    fread(&(mbc->MBC7_count), sizeof(int), 1, statefile);
    fread(&(mbc->MBC7_code), sizeof(int), 1, statefile);
    fread(&(mbc->MBC7_address), sizeof(int), 1, statefile);
    fread(&(mbc->MBC7_writeEnable), sizeof(int), 1, statefile);
    fread(&(mbc->MBC7_value), sizeof(int), 1, statefile);
    fread(&sensorX,sizeof(int),1,statefile);
    fread(&sensorY,sizeof(int),1,statefile);
}

void gb_mbc::readHuc3VarsFromStateFile(FILE *statefile) {
    fread(&(mbc->HuC3_time), sizeof(unsigned int), 1, statefile);
    fread(&(mbc->HuC3_last_time), sizeof(time_t), 1, statefile);
    fread(&(mbc->rtc).s, sizeof(int), 1, statefile);

    //fread(mbc->HuC3_register,sizeof(int),8,statefile);
    fread(&(mbc->HuC3_RAMvalue), sizeof(int), 1, statefile);
    //fread(&mbc->HuC3_address,sizeof(int),1,statefile);
    fread(&(mbc->HuC3_RAMflag), sizeof(int), 1, statefile);
}

void gb_mbc::readRtcVarsFromStateFile(FILE *statefile) {
    fread(&(mbc->rtc).s, sizeof(int), 1, statefile);
    fread(&(mbc->rtc).m, sizeof(int), 1, statefile);
    fread(&(mbc->rtc).h, sizeof(int), 1, statefile);
    fread(&(mbc->rtc).d, sizeof(int), 1, statefile);
    fread(&(mbc->rtc).control, sizeof(int), 1, statefile);
    fread(&(mbc->rtc).last_time, sizeof(time_t), 1, statefile);

    fread(&(mbc->rtc_latch).s, sizeof(int), 1, statefile);
    fread(&(mbc->rtc_latch).m, sizeof(int), 1, statefile);
    fread(&(mbc->rtc_latch).h, sizeof(int), 1, statefile);
    fread(&(mbc->rtc_latch).d, sizeof(int), 1, statefile);
    fread(&(mbc->rtc_latch).control, sizeof(int), 1, statefile);
    fread(&(mbc->rtc_latch).last_time, sizeof(time_t), 1, statefile);
}

void gb_mbc::resetRomMemoryMap(bool resetOffset=false) {
    if ( resetOffset ) {
        superaddroffset = 0;
    }
    gbMemMap[0x0] = &(*gbCartridge)[superaddroffset+0x0000];
    gbMemMap[0x1] = &(*gbCartridge)[superaddroffset+0x1000];
    gbMemMap[0x2] = &(*gbCartridge)[superaddroffset+0x2000];
    gbMemMap[0x3] = &(*gbCartridge)[superaddroffset+0x3000];
    gbMemMap[0x4] = &(*gbCartridge)[superaddroffset+0x4000];
    gbMemMap[0x5] = &(*gbCartridge)[superaddroffset+0x5000];
    gbMemMap[0x6] = &(*gbCartridge)[superaddroffset+0x6000];
    gbMemMap[0x7] = &(*gbCartridge)[superaddroffset+0x7000];
}

int gb_mbc::getRomBank() {
    return rom_bank;
}

int gb_mbc::getRamBank() {
    return ram_bank;
}

void gb_mbc::setMemoryReadWrite(memoryaccess memory_type) {
    gb_mbc::mbcType = memory_type;

    switch(mbcType)
    {
        case MEMORY_MBC3:
            mbc = new Mbc3();
            break;
        case MEMORY_CAMERA:
            mbc = new MbcCamera();
            break;
        case MEMORY_MBC7:
            mbc = new Mbc7();
            break;
        case MEMORY_HUC3:
            mbc = new Huc3();
            break;
        case MEMORY_TAMA5:
            mbc = new Tama5();
            break;
        case MEMORY_SINTAX:
            mbc = new Sintax();
            break;
        default:
        case MEMORY_DEFAULT:
            mbc = new Default();
            break;
    }

    mbc->init( gbMemMap, gbRom, gbMemory, gbRomBankXor );
}
