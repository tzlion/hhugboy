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
#include "mbc/MbcUnlLiCheng.h"

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
    mbc->resetVars();
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

        case MEMORY_MBC3:
        case MEMORY_NIUTOUDE:
        case MEMORY_SINTAX:
        case MEMORY_MBC5:
        case MEMORY_CAMERA:
        case MEMORY_TAMA5:
        case MEMORY_HUC3:
        case MEMORY_ROMONLY:
        case MEMORY_MBC7:
        case MEMORY_DEFAULT:
        default:
            mbc->writeMemory(address,data);
            break;
    }
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

void gb_mbc::writeMbcSpecificStuffToSaveFile(FILE *savefile) {
    mbc->writeMbcSpecificVarsToSaveFile(savefile);
}

void gb_mbc::readMbcSpecificStuffFromSaveFile(FILE *savefile){
    mbc->readMbcSpecificVarsFromSaveFile(savefile);
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
    mbc->writeMbcOtherStuffToStateFile(statefile);
}

void gb_mbc::writeMbcBanksToStateFile(FILE *statefile) {
    mbc->writeMbcBanksToStateFile(statefile);
}

void gb_mbc::readMbcMoreCrapFromStateFile(FILE *statefile) {
    mbc->readMbcOtherStuffFromStateFile(statefile);
}

void gb_mbc::readMbcBanksFromStateFile(FILE *statefile) {
    mbc->readMbcBanksFromStateFile(statefile);
}

void gb_mbc::resetRomMemoryMap(bool resetOffset=false) {
    mbc->resetRomMemoryMap(resetOffset);
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
        case MEMORY_NIUTOUDE:
            mbc = new MbcUnlLiCheng();
            break;
        case MEMORY_MBC5:
            mbc = new MbcNin5();
            break;
        case MEMORY_ROMONLY:
            mbc = new RomOnly();
            break;
        case MEMORY_MBC1:
        case MEMORY_MBC2:
        case MEMORY_ROCKMAN8:
        case MEMORY_BC:
        case MEMORY_MMM01:
        case MEMORY_POKE:
        case MEMORY_8IN1:
        case MEMORY_MK12:
        case MEMORY_DEFAULT:
        default:
            mbc = new BasicMbc();
            break;
    }

    mbc->init( gbMemMap, gbRom, gbMemory, gbRomBankXor, gbCartridge, gbCartRam, gbRumbleCounter );
}
