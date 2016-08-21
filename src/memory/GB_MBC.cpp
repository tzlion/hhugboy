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
#include "mbc/MbcNin3.h"
#include "mbc/MbcNinCamera.h"
#include "mbc/MbcNin7.h"
#include "mbc/MbcLicHuc3.h"
#include "mbc/MbcLicTama5.h"
#include "mbc/MbcUnlSintax.h"

byte vfmultimode=0;
byte vfmultibank=0;
byte vfmultimem=0;
byte vfmultifinal=0;

//int RTCIO = 0;
//int RTC_latched = 0;

// Eventually GB should contain cart and cart should contain MBC
gb_mbc::gb_mbc(byte** gbMemMap, byte** gbCartridge, GBrom** gbRom, byte** gbCartRam, byte* gbRomBankXor, int* gbRumbleCounter, byte** gbMemory):

        mbcType(MEMORY_DEFAULT)

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
    mbc->MBChi = 0;
    mbc->MBClo = 1;
    mbc->rom_bank = 1;
    mbc->ram_bank = 0;
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

        case MEMORY_CAMERA:
            writememory_Camera(address,data);
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

        case MEMORY_HUC3:
        case MEMORY_ROMONLY:
        case MEMORY_MBC7:
        case MEMORY_DEFAULT:
        default:
            mbc->writeMemory(address,data);
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
  //	sprintf(buff,"bank no %x abbr %x xor %x",bankNo,bankNo&0x0F,mbc->rom_bank_xor);
  //	debug_print(buff);
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

      mbc->rom_bank = data;

      int cadr = (data<<14)+(mbc->MBChi<<14);
      cadr &= mbc->rom_size_mask[(*gbRom)->ROMsize];
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

         mbc->MBClo = 0;

         mbc->MBChi = (data&0x1f);

         mbc->cart_address = mbc->MBChi<<14;
         gbMemMap[0x0] = &(*gbCartridge)[mbc->cart_address];
         gbMemMap[0x1] = &(*gbCartridge)[mbc->cart_address+0x1000];
         gbMemMap[0x2] = &(*gbCartridge)[mbc->cart_address+0x2000];
         gbMemMap[0x3] = &(*gbCartridge)[mbc->cart_address+0x3000];

         gbMemMap[0x4] = &(*gbCartridge)[mbc->cart_address+0x4000];
         gbMemMap[0x5] = &(*gbCartridge)[mbc->cart_address+0x5000];
         gbMemMap[0x6] = &(*gbCartridge)[mbc->cart_address+0x6000];
         gbMemMap[0x7] = &(*gbCartridge)[mbc->cart_address+0x7000];
         return;
      }

      data &= 0x03;

      mbc->ram_bank = data;

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
       mbc->RAMenable = ( (data&0x0A) == 0x0A ? 1 : 0);
      return;
   }

   if(address < 0x4000) // Is it a ROM bank switch?
   {
      if(mbc->MBC1memorymodel == 0)
      {
         if(data == 0)
            data = 1;

         mbc->MBClo = data;

         mbc->rom_bank = mbc->MBClo|(mbc->MBChi<<5);

         mbc->cart_address = mbc->MBClo<<14;
         mbc->cart_address |= (mbc->MBChi<<19);

         mbc->cart_address &= mbc->rom_size_mask[(*gbRom)->ROMsize];

         gbMemMap[0x4] = &(*gbCartridge)[mbc->cart_address];
         gbMemMap[0x5] = &(*gbCartridge)[mbc->cart_address+0x1000];
         gbMemMap[0x6] = &(*gbCartridge)[mbc->cart_address+0x2000];
         gbMemMap[0x7] = &(*gbCartridge)[mbc->cart_address+0x3000];
      } else
      {
         if(data == 0)
            data = 1;

         mbc->rom_bank = data;

         int cadr = mbc->rom_bank<<14;

         cadr &= mbc->rom_size_mask[(*gbRom)->ROMsize];

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
         if((((data&0x03)<<5)|mbc->MBClo) > mbc->maxROMbank[(*gbRom)->ROMsize])
            return;

         mbc->MBChi = (data&0x03);

         mbc->rom_bank = mbc->MBClo|(mbc->MBChi<<5);

         mbc->cart_address = mbc->MBClo<<14;
         mbc->cart_address |= (mbc->MBChi<<19);

         mbc->cart_address &= mbc->rom_size_mask[(*gbRom)->ROMsize];

         gbMemMap[0x4] = &(*gbCartridge)[mbc->cart_address];
         gbMemMap[0x5] = &(*gbCartridge)[mbc->cart_address+0x1000];
         gbMemMap[0x6] = &(*gbCartridge)[mbc->cart_address+0x2000];
         gbMemMap[0x7] = &(*gbCartridge)[mbc->cart_address+0x3000];
         return;
      }

      if((*gbRom)->RAMsize <= 2) // no need to change it if there isn't over 8KB ram
         return;

      data &= 0x03;

      if(data > mbc->maxRAMbank[(*gbRom)->RAMsize])
         data = mbc->maxRAMbank[(*gbRom)->RAMsize];

      mbc->ram_bank = data;

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
      if((!mbc->RAMenable || !rom->RAMsize) && !RAM_always_enable)
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

      mbc->rom_bank = data;

      int cadr = (mbc->rom_bank<<14)+(mbc->MBChi<<14);

      cadr &= mbc->rom_size_mask[(*gbRom)->ROMsize];

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
         mbc->MBClo = 0;

         mbc->MBChi = data;
         mbc->cart_address = mbc->MBChi<<14;

         gbMemMap[0x0] = &(*gbCartridge)[mbc->cart_address];
         gbMemMap[0x1] = &(*gbCartridge)[mbc->cart_address+0x1000];
         gbMemMap[0x2] = &(*gbCartridge)[mbc->cart_address+0x2000];
         gbMemMap[0x3] = &(*gbCartridge)[mbc->cart_address+0x3000];

         gbMemMap[0x4] = &(*gbCartridge)[mbc->cart_address+0x4000];
         gbMemMap[0x5] = &(*gbCartridge)[mbc->cart_address+0x5000];
         gbMemMap[0x6] = &(*gbCartridge)[mbc->cart_address+0x6000];
         gbMemMap[0x7] = &(*gbCartridge)[mbc->cart_address+0x7000];
         return;
      }

      if((*gbRom)->RAMsize <= 2) // no need to change it if there isn't over 8KB ram
         return;

      data &= 0x03;

      if(data > mbc->maxRAMbank[(*gbRom)->RAMsize])
         data = mbc->maxRAMbank[(*gbRom)->RAMsize];

      mbc->ram_bank = data;

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
      mbc->RAMenable = ( (data&0x0A) == 0x0A ? 1 : 0);
      return;
   }

   if(address < 0x4000) // Is it a ROM bank switch?
   {
      data &= 0x0F;

      if(data == 0)
         data = 1;

      mbc->rom_bank = data|(mbc->MBChi<<4);

      int cadr = mbc->rom_bank<<14;

      cadr &= mbc->rom_size_mask[(*gbRom)->ROMsize];

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
         mbc->MBClo = 0;
         mbc->MBChi = (data&0x03);

         mbc->cart_address = (mbc->MBChi<<4)<<14;

         gbMemMap[0x0] = &(*gbCartridge)[mbc->cart_address];
         gbMemMap[0x1] = &(*gbCartridge)[mbc->cart_address+0x1000];
         gbMemMap[0x2] = &(*gbCartridge)[mbc->cart_address+0x2000];
         gbMemMap[0x3] = &(*gbCartridge)[mbc->cart_address+0x3000];

         gbMemMap[0x4] = &(*gbCartridge)[mbc->cart_address+0x4000];
         gbMemMap[0x5] = &(*gbCartridge)[mbc->cart_address+0x5000];
         gbMemMap[0x6] = &(*gbCartridge)[mbc->cart_address+0x6000];
         gbMemMap[0x7] = &(*gbCartridge)[mbc->cart_address+0x7000];
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
      if((!mbc->RAMenable || !rom->RAMsize) && !RAM_always_enable)
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
      mbc->RAMenable = ( (data&0x0A) == 0x0A ? 1 : 0);
      return;
   }

   if(address < 0x4000) // Is it a ROM bank switch?
   {
      data &= 0x0F;

      if(data == 0)
         data = 1;

      mbc->rom_bank = data+mbc->MBChi;

      int cadr = mbc->rom_bank<<14;

      cadr &= mbc->rom_size_mask[(*gbRom)->ROMsize];

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
         mbc->MBClo = 0;
         mbc->MBChi = (data&0x03);
         if(mbc->MBChi==2) mbc->MBChi = 17;

         mbc->cart_address = mbc->MBChi<<14;

         gbMemMap[0x0] = &(*gbCartridge)[mbc->cart_address];
         gbMemMap[0x1] = &(*gbCartridge)[mbc->cart_address+0x1000];
         gbMemMap[0x2] = &(*gbCartridge)[mbc->cart_address+0x2000];
         gbMemMap[0x3] = &(*gbCartridge)[mbc->cart_address+0x3000];

         gbMemMap[0x4] = &(*gbCartridge)[mbc->cart_address+0x4000];
         gbMemMap[0x5] = &(*gbCartridge)[mbc->cart_address+0x5000];
         gbMemMap[0x6] = &(*gbCartridge)[mbc->cart_address+0x6000];
         gbMemMap[0x7] = &(*gbCartridge)[mbc->cart_address+0x7000];
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
      if((!mbc->RAMenable || !rom->RAMsize) && !RAM_always_enable)
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

      if(data > mbc->maxROMbank[(*gbRom)->ROMsize])
         data -= 8; // <--- MAKE IT WORK!!!

      mbc->rom_bank = data;

      mbc->cart_address = data<<14;

      mbc->cart_address &= mbc->rom_size_mask[(*gbRom)->ROMsize];

      gbMemMap[0x4] = &(*gbCartridge)[mbc->cart_address];
      gbMemMap[0x5] = &(*gbCartridge)[mbc->cart_address+0x1000];
      gbMemMap[0x6] = &(*gbCartridge)[mbc->cart_address+0x2000];
      gbMemMap[0x7] = &(*gbCartridge)[mbc->cart_address+0x3000];

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
         mbc->RAMenable =  (data&0x0F) == 0x0A;
      return;
   }

   if(address < 0x4000) // Is it a ROM bank switch?
   {
      //if(address&0x0100)
      {
         data &= 0x0F;
         if(data==0)
            data=1;
         if(data > mbc->maxROMbank[(*gbRom)->ROMsize])
            data = mbc->maxROMbank[(*gbRom)->ROMsize];

         mbc->rom_bank = data;

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
      if(!mbc->RAMenable || !rom->battery)
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
      mbc->RAMenable = ( (data&0x0A) == 0x0A ? 1 : 0);
      bank0_change = ( (data&0xC0) == 0xC0 ? 1 : 0);
      return;
   }

   if(address < 0x4000) // Is it a ROM bank switch?
   {
      data &= 0x7F;
      if(data==0)
         data=1;
      data += mbc->MBChi;

      mbc->rom_bank = data;

      int cadr = data<<14;
      cadr &= mbc->rom_size_mask[(*gbRom)->ROMsize];
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

      if(data > mbc->maxRAMbank[(*gbRom)->RAMsize])
         data = mbc->maxRAMbank[(*gbRom)->RAMsize];

      mbc->ram_bank = data;

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
         mbc->MBClo = 0;
         if(data==1)
            mbc->MBChi = 2;
         else
         if(data!=0xc0)
            mbc->MBChi = 66;
         else
            mbc->bc_select = 1;

         mbc->cart_address = mbc->MBChi<<14;

         gbMemMap[0x0] = &(*gbCartridge)[mbc->cart_address];
         gbMemMap[0x1] = &(*gbCartridge)[mbc->cart_address+0x1000];
         gbMemMap[0x2] = &(*gbCartridge)[mbc->cart_address+0x2000];
         gbMemMap[0x3] = &(*gbCartridge)[mbc->cart_address+0x3000];

         gbMemMap[0x4] = &(*gbCartridge)[mbc->cart_address+0x4000];
         gbMemMap[0x5] = &(*gbCartridge)[mbc->cart_address+0x5000];
         gbMemMap[0x6] = &(*gbCartridge)[mbc->cart_address+0x6000];
         gbMemMap[0x7] = &(*gbCartridge)[mbc->cart_address+0x7000];
         return;
      }
      //if(!mbc->RAMenable || !rom->RAMsize)
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
      mbc->RAMenable = ( (data&0x0A) == 0x0A ? 1 : 0);
      return;
   }

   if(address < 0x4000) // Is it a ROM bank switch?
   {
      data &= 0x7F;
      if(data==0)
         data=1;

      mbc->rom_bank = data;

      int cadr = data<<14;
      cadr &= mbc->rom_size_mask[(*gbRom)->ROMsize];
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

      if(data > mbc->maxRAMbank[(*gbRom)->RAMsize])
         data = mbc->maxRAMbank[(*gbRom)->RAMsize];

      mbc->ram_bank = data;

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
      if(mbc->RAMenable && (*gbRom)->RTC && mbc->RTCIO)
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
      //if(!mbc->RAMenable || !rom->RAMsize)
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
      mbc->RAMenable = ( (data&0x0A) == 0x0A ? 1 : 0);
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

      mbc->rom_bank = data|(mbc->MBChi<<8);
      mbc->cart_address = mbc->rom_bank<<14;

      mbc->cart_address &= mbc->rom_size_mask[(*gbRom)->ROMsize];

      mbc->cart_address += superaddroffset;

      mbc->MBClo = data;

      gbMemMap[0x4] = &(*gbCartridge)[mbc->cart_address];
      gbMemMap[0x5] = &(*gbCartridge)[mbc->cart_address+0x1000];
      gbMemMap[0x6] = &(*gbCartridge)[mbc->cart_address+0x2000];
      gbMemMap[0x7] = &(*gbCartridge)[mbc->cart_address+0x3000];

    //  if(origData == 0x69) {
	//    	char buff[100];
	//		sprintf(buff,"%X %X %X",origData,data,mbc->cart_address);
	//		debug_print(buff);
    //  }



      return;
   }

   if(address < 0x4000) // Is it a ROM bank switch?
   {
      data = data&1;

      mbc->rom_bank = mbc->MBClo|(data<<8);

      mbc->cart_address = mbc->rom_bank<<14;

      mbc->cart_address &= mbc->rom_size_mask[(*gbRom)->ROMsize];

      mbc->cart_address += superaddroffset;

      mbc->MBChi = data;

      gbMemMap[0x4] = &(*gbCartridge)[mbc->cart_address];
      gbMemMap[0x5] = &(*gbCartridge)[mbc->cart_address+0x1000];
      gbMemMap[0x6] = &(*gbCartridge)[mbc->cart_address+0x2000];
      gbMemMap[0x7] = &(*gbCartridge)[mbc->cart_address+0x3000];

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

      if(data > mbc->maxRAMbank[(*gbRom)->RAMsize])
         data = mbc->maxRAMbank[(*gbRom)->RAMsize];

      mbc->ram_bank = data;

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
         mbc->MBChi = (data&0xFF);

         mbc->cart_address = (mbc->MBChi<<1)<<14;

         mem_map[0x0] = &cartridge[mbc->cart_address];
         mem_map[0x1] = &cartridge[mbc->cart_address+0x1000];
         mem_map[0x2] = &cartridge[mbc->cart_address+0x2000];
         mem_map[0x3] = &cartridge[mbc->cart_address+0x3000];

         mem_map[0x4] = &cartridge[mbc->cart_address+0x4000];
         mem_map[0x5] = &cartridge[mbc->cart_address+0x5000];
         mem_map[0x6] = &cartridge[mbc->cart_address+0x6000];
         mem_map[0x7] = &cartridge[mbc->cart_address+0x7000];
      }*/
      return;
   }

 /*  if(address >= 0xA000 && address < 0xC000)
   {
      if(!mbc->RAMenable || !rom->RAMsize)
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
      mbc->RAMenable = ( (data&0x0A) == 0x0A ? 1 : 0);
      return;
   }

   if(address < 0x4000) // Is it a ROM bank switch?
   {
      if(data == 0)
         data = 1;
      if(data > mbc->maxROMbank[(*gbRom)->ROMsize])
         data = mbc->maxROMbank[(*gbRom)->ROMsize];

      mbc->rom_bank = data;

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

      if(data > mbc->maxRAMbank[(*gbRom)->RAMsize])
         data = mbc->maxRAMbank[(*gbRom)->RAMsize];

      mbc->ram_bank = data;

      int madr = data<<13;
      gbMemMap[0xA] = &(*gbCartRam)[madr];
      gbMemMap[0xB] = &(*gbCartRam)[madr+0x1000];
      return;

   }

   if(address<0x8000)
      return;

 /*  if(address >= 0xA000 && address < 0xC000)
   {
      if(!mbc->RAMenable)
         return;
   }*/

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

             mbc->MBClo = data;
             mbc->rom_bank = mbc->MBClo|(mbc->MBChi<<4);
       
            int cadr = mbc->rom_bank<<14;
             gbMemMap[0x4] = &(*gbCartridge)[cadr];
             gbMemMap[0x5] = &(*gbCartridge)[cadr+0x1000];
             gbMemMap[0x6] = &(*gbCartridge)[cadr+0x2000];
             gbMemMap[0x7] = &(*gbCartridge)[cadr+0x3000];
         }
         return;
         case 1: // rom bank high bit
         {
            data &= 0x01;
            
            mbc->MBChi = data;
            
            mbc->rom_bank = mbc->MBClo|(mbc->MBChi<<4);
       
            int cadr = mbc->rom_bank<<14;
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
    mbc->readNewMbcSpecificVarsFromStateFile(statefile);
}

void gb_mbc::readCartSpecificVarsFromStateFile(FILE *statefile){
    mbc->readOldMbcSpecificVarsFromStateFile(statefile);
}

void gb_mbc::writeCartSpecificVarsToStateFile(FILE *statefile) {
    mbc->writeOldMbcSpecificVarsToStateFile(statefile);
}

void gb_mbc::writeNewerCartSpecificVarsToStateFile(FILE *statefile) {
    mbc->writeNewMbcSpecificVarsToStateFile(statefile);
}


void gb_mbc::writeMbcOtherStuffToStateFile(FILE *statefile) {
    fwrite(&( mbc->MBC1memorymodel), sizeof(int), 1, statefile);
    fwrite(&(mbc->RAMenable), sizeof(int), 1, statefile);
    fwrite(&(mbc->MBChi), sizeof(unsigned int), 1, statefile);
    fwrite(&(mbc->MBClo), sizeof(unsigned int), 1, statefile);
}

void gb_mbc::writeMbcBanksToStateFile(FILE *statefile) {
    fwrite(&(mbc->rom_bank), sizeof(int), 1, statefile);
    fwrite(&(mbc->ram_bank), sizeof(int), 1, statefile);
}

void gb_mbc::readMbcMoreCrapFromStateFile(FILE *statefile) {
    fread(&( mbc->MBC1memorymodel), sizeof(int), 1, statefile);
    fread(&(mbc->RAMenable), sizeof(int), 1, statefile);
    fread(&(mbc->MBChi), sizeof(unsigned int), 1, statefile);
    fread(&(mbc->MBClo), sizeof(unsigned int), 1, statefile);
}

void gb_mbc::readMbcBanksFromStateFile(FILE *statefile) {
    fread(&(mbc->rom_bank), sizeof(int), 1, statefile);
    fread(&(mbc->ram_bank), sizeof(int), 1, statefile);
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
    return mbc->rom_bank;
}

int gb_mbc::getRamBank() {
    return mbc->ram_bank;
}

void gb_mbc::setMemoryReadWrite(memoryaccess memory_type) {
    gb_mbc::mbcType = memory_type;

    switch(mbcType)
    {
        case MEMORY_MBC3:
            mbc = new MbcNin3();
            break;
        case MEMORY_CAMERA:
            mbc = new MbcNinCamera();
            break;
        case MEMORY_MBC7:
            mbc = new MbcNin7();
            break;
        case MEMORY_HUC3:
            mbc = new MbcLicHuc3();
            break;
        case MEMORY_TAMA5:
            mbc = new MbcLicTama5();
            break;
        case MEMORY_SINTAX:
            mbc = new MbcUnlSintax();
            break;
        case MEMORY_ROMONLY:
            mbc = new RomOnly();
            break;
        default:
        case MEMORY_DEFAULT:
            mbc = new BasicMbc();
            break;
    }

    mbc->init( gbMemMap, gbRom, gbMemory, gbRomBankXor, gbCartridge, gbCartRam );
}
