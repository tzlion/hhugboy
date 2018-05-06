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
#include "mbc/MbcNin1.h"
#include "mbc/MbcNin2.h"
#include "mbc/MbcUnlRockman8.h"
#include "mbc/MbcNin1Multi.h"
#include "mbc/MbcLicMmm01.h"
#include "mbc/MbcUnlPoke2in1.h"
#include "mbc/MbcUnlSachen8in1.h"
#include "mbc/MbcLicMk12.h"
#include "mbc/MbcUnlLbMulti.h"
#include "mbc/MbcUnlBbd.h"
#include "mbc/MbcUnlDbzTrans.h"
#include "mbc/MbcUnlHitek.h"
#include "mbc/MbcUnlNtNew.h"
#include "mbc/MbcUnlNtOld1.h"
#include "mbc/MbcUnlNtOld2.h"
#include "../GbLinker.h"
#include "../ui/dialogs/LinkerLog.h"

// So maybe this should be "cart" and a lot of whats in rom.cpp now e.g. autodetection should go in here..

//int RTCIO = 0;
//int RTC_latched = 0;

// Eventually GB should contain cart and cart should contain MBC
gb_mbc::gb_mbc(byte** gbMemMap, byte** gbCartRom, Cartridge** gbCartridge, byte** gbCartRam, int* gbRumbleCounter, byte** gbMemory):

        mbcType(MEMORY_DEFAULT)

{
    this->gbCartRom = gbCartRom;
    this->gbMemMap = gbMemMap;
    this->gbCartridge = gbCartridge;
    this->gbCartRam = gbCartRam;
    this->gbRumbleCounter = gbRumbleCounter;
    this->gbMemory = gbMemory;

    setMemoryReadWrite(mbcType);
}

void gb_mbc::resetMbcVariables(bool preserveMulticartState = false)
{
    mbc->resetVars(preserveMulticartState);
}

bool readBank0 = false;
U8 bank0[0x4000];
bool readBank1 = false;
U8 bank1[0x4000];

byte last4000 = 00;

byte gb_mbc::readmemory_cart(register unsigned short address) {

    if (!GbLinker::linkerActive) {
        return mbc->readMemory(address);
    }

    if (address < 0x8000 || last4000 < 0x08) {
        return mbc->readMemory(address);
    }
    // super fights still doesnt work like this.. does that mean bank 0 changes maybe?
    if (address < 0x4000) {
        if (!readBank0) {
            LinkerLog::addMessage("DUMPING BANK 0");
            GbLinker::readBlock(bank0,0,0x4000);
            readBank0=true;
        }
        return bank0[address];
    }
    if (address < 0x8000) {
        // for PB Dreams this is slower.. you could probably cache the banks for an official game
        if (!readBank1) {
            LinkerLog::addMessage("DUMPING BANK 1");
            GbLinker::readBlock(bank1,0x4000,0x4000);
            readBank1=true;
        }
        return bank1[address-0x4000];
    }
    //if (address<0x4000) return linker::bank0[address]; // quicker maybe // tho not sure if working
    // super fighter 2001 doesnt work this way but ping ball dreams works pretty well
    byte datum = GbLinker::readByte(address);
    if (address >= 0x8000) {
        char fart[420];
        sprintf(fart, "read %02x from %04x", datum, address);
        LinkerLog::addMessage(fart);
    }
    return datum;

    byte data = mbc->readMemory(address);
    if (address >= 0x8000) {
        char fart[420];
        sprintf(fart, "read %02x from %04x", data, address);
        debug_win(fart);
    }
    return data;
}

void gb_mbc::writememory_cart(unsigned short address, register byte data) {

    if (!GbLinker::linkerActive) {
        mbc->writeMemory(address,data);
        return;
    }

    readBank1 = false;
    if (address == 0x4000) last4000 = data;
    if (address == 0x4000 /*&& data >= 0x08*/)
        GbLinker::writeByte(address,data);
    mbc->writeMemory(address,data);
    // then read the current banks into mem and use them
    return;

  //  if (!((address == 0x2000 && data == 0x01)||(address == 0x3000 && data == 0x00))) {
        char fart[420];
        sprintf(fart, "write %02x to %04x", data, address);
        debug_win(fart);
  //  }

    mbc->writeMemory(address,data);
}

void gb_mbc::writeMbcSpecificStuffToSaveFile(FILE *savefile) {
    mbc->writeMbcSpecificVarsToSaveFile(savefile);
}

void gb_mbc::readMbcSpecificStuffFromSaveFile(FILE *savefile){
    mbc->readMbcSpecificVarsFromSaveFile(savefile);
}

void gb_mbc::readNewerCartSpecificVarsFromStateFile(FILE *statefile) {
    mbc->readSgbMbcSpecificVarsFromStateFile(statefile);
}

void gb_mbc::readCartSpecificVarsFromStateFile(FILE *statefile){
    mbc->readMbcSpecificVarsFromStateFile(statefile);
}

void gb_mbc::writeCartSpecificVarsToStateFile(FILE *statefile) {
    mbc->writeMbcSpecificVarsToStateFile(statefile);
}

void gb_mbc::writeNewerCartSpecificVarsToStateFile(FILE *statefile) {
    mbc->writeSgbMbcSpecificVarsToStateFile(statefile);
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

void gb_mbc::resetRomMemoryMap(bool preserveMulticartState = false) {
    mbc->resetRomMemoryMap(preserveMulticartState);
}

int gb_mbc::getRomBank() {
    return mbc->rom_bank;
}

int gb_mbc::getRamBank() {
    return mbc->ram_bank;
}

void gb_mbc::setMemoryReadWrite(MbcType memory_type) {
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
        case MEMORY_BBD:
            mbc = new MbcUnlBbd();
            break;
        case MEMORY_HITEK:
            mbc = new MbcUnlHitek();
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
            mbc = new MbcNin1();
            break;
        case MEMORY_MBC2:
            mbc = new MbcNin2();
            break;
        case MEMORY_ROCKMAN8:
            mbc = new MbcUnlRockman8();
            break;
        case MEMORY_NTOLD1:
            mbc = new MbcUnlNtOld1((*gbCartridge)->ROMsize);
            break;
        case MEMORY_NTOLD2:
            mbc = new MbcUnlNtOld2((*gbCartridge)->ROMsize);
            break;
        case MEMORY_MBC1MULTI:
            mbc = new MbcNin1Multi();
            break;
        case MEMORY_MMM01:
            mbc = new MbcLicMmm01();
            break;
        case MEMORY_POKE:
            mbc = new MbcUnlPoke2in1();
            break;
        case MEMORY_DBZTRANS:
            mbc = new MbcUnlDbzTrans();
            break;
        case MEMORY_8IN1:
            mbc = new MbcUnlSachen8in1();
            break;
        case MEMORY_MK12:
            mbc = new MbcLicMk12();
            break;
        case MEMORY_LBMULTI:
            mbc = new MbcUnlLbMulti();
            break;
        case MEMORY_NTNEW:
            mbc = new MbcUnlNtNew();
            break;
        case MEMORY_DEFAULT:
        default:
            mbc = new BasicMbc();
            break;
    }

    mbc->init( gbMemMap, gbCartridge, gbMemory, gbCartRom, gbCartRam, gbRumbleCounter );
}

bool gb_mbc::shouldReset() {
    if ( mbc->deferredReset ) {
        mbc->deferredReset = false;
        return true;
    }
    return false;
}

int gb_mbc::getOffset() {
    return mbc->multicartOffset;
}

int gb_mbc::getRamOffset() {
    return mbc->multicartOffset;
}

