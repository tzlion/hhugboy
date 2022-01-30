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

#include "GB_MBC.h"

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
#include "mbc/MbcLicMk12.h"
#include "mbc/MbcUnlLbMulti.h"
#include "mbc/MbcUnlBbd.h"
#include "mbc/MbcUnlDbzTrans.h"
#include "mbc/MbcUnlHitek.h"
#include "mbc/MbcUnlNtNew.h"
#include "mbc/MbcUnlNtOld1.h"
#include "mbc/MbcUnlNtOld2.h"
#include "mbc/MbcUnlPokeJadeDia.h"
#include "mbc/MbcUnlWisdomTree.h"
#include "mbc/MbcUnlSachenMMC1.h"
#include "mbc/MbcUnlSachenMMC2.h"
#include "mbc/MbcLicM161.h"
#include "mbc/MbcUnlRocketGames.h"
#include "mbc/MbcUnlNewGbHk.h"
#include "mbc/MbcUnlGgb81.h"
#include "linker/LinkerWrangler.h"
#include "../ui/dialogs/LinkerLog.h"

// So maybe this should be "cart" and a lot of whats in rom.cpp now e.g. autodetection should go in here..

//int RTCIO = 0;
//int RTC_latched = 0;

byte determineSelectedBankNo(int offset) {
    byte lastbyte = LinkerWrangler::readThroughLinker(offset + 0x3fff);
    if (lastbyte <= 0x7f && (lastbyte != 0x00 && lastbyte != 0x01 && lastbyte != 0x7f && lastbyte != 0x20 && lastbyte != 0x10)) {
        // last byte is the bank number
        return lastbyte;
    }

    byte firstbyte;
    byte twentiethbyte;
    byte byteff0;
    byte hundredthbyte;

    switch(lastbyte) {
        // last byte is unique so we can determine the bank number from it
        case 0xf4: return 0x08;
        case 0xda: return 0x15;
        case 0x98: return 0x1b;
        case 0x10: return 0x5d;
        case 0x80: return 0x16;
        case 0x01: return 0x04;
        // last byte is not unique so we gotta find another byte that is unique within that subset
        case 0x83:
            firstbyte = LinkerWrangler::readThroughLinker(offset);
            switch (firstbyte) {
                case 0x00: return 0x01;
                case 0x21: return 0x02;
                default: return 0xff; // unexpected
            }
        case 0x7f:
            firstbyte = LinkerWrangler::readThroughLinker(offset);
            switch (firstbyte) {
                case 0x19: return 0x19; // first byte in rare cases actually is the bank no too
                case 0xcd: return 0x7f;
                default: return 0xff; // unexpected
            }
        case 0x20:
            twentiethbyte = LinkerWrangler::readThroughLinker(offset + 0x20);
            switch (twentiethbyte) {
                case 0x5e: return 0x20;
                case 0x8f: return 0x22;
                case 0x00: return 0x24;
                case 0xfc: return 0x28;
                default: return 0xff; // unexpected
            }
        case 0xff:
            byteff0 = LinkerWrangler::readThroughLinker(offset + 0xff0);
            switch (byteff0) {
                case 0xc1: return 0x00;
                case 0x3f: return 0x2d;
                case 0x20: return 0x2f;
                case 0xff: return 0x30;
                case 0x7f: return 0x31;
                case 0x60: return 0x32;
                case 0xf8: return 0x38;
                case 0xf6: return 0x5e;
                default: return 0xff; // unexpected
            }
        case 0x00:
            // this one is the worst
            byteff0 = LinkerWrangler::readThroughLinker(offset + 0xff0);
            switch (byteff0) {
                case 0x62: return 0x03;
                case 0x43: return 0x0b;
                case 0x81: return 0x0e;
                case 0x01: return 0x0f;
                case 0x38: return 0x10;
                case 0x10: return 0x12;
                case 0x08: return 0x13;
                case 0x80: return 0x1f;
                case 0x3f: return 0x23;
                case 0xe8: return 0x26;
                case 0xfe: return 0x29;
                case 0x09: return 0x2b;
                case 0xfd: return 0x5c;
                case 0xe0: return 0x60;
                case 0x11: return 0x62;
                case 0xff:
                    firstbyte = LinkerWrangler::readThroughLinker(offset);
                    switch (firstbyte) {
                        case 0x1a: return 0x1a;
                        case 0x00: return 0x35;
                        default: return 0xff; // unexpected
                    }
                case 0xd1:
                    hundredthbyte = LinkerWrangler::readThroughLinker(offset + 0x100);
                    switch (hundredthbyte) {
                        case 0x4e: return 0x0c;
                        case 0x00: return 0x0d;
                        default: return 0xff; // unexpected
                    }
                case 0xfc:
                    hundredthbyte = LinkerWrangler::readThroughLinker(offset + 0x100);
                    switch (hundredthbyte) {
                        case 0xcb: return 0x14;
                        case 0x00: return 0x2a;
                        case 0xf0: return 0x2c;
                        default: return 0xff; // unexpected
                    }
                case 0x00:
                    // this one is the double worst
                    firstbyte = LinkerWrangler::readThroughLinker(offset);
                    switch (firstbyte) {
                        case 0x07: return 0x07;
                        case 0x17: return 0x17;
                        case 0x18: return 0x18;
                        case 0x00:
                            hundredthbyte = LinkerWrangler::readThroughLinker(offset + 0x100);
                            switch(hundredthbyte) {
                                case 0x80: return 0x11;
                                case 0x00: return 0x63;
                                default: return 0xff; // unexpected
                            }
                        default: return 0xff; // unexpected
                    }
                default: return 0xff; // unexpected
            }
        default: return 0xff; // unexpected
    }
}

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


bool runningmode = false;
byte runningvalue = 00;
byte cur6000 = 00;
byte cur7001 = 00;
byte cur7002 = 00;
byte cur7003 = 00;
byte cur7004 = 00;
byte cur7005 = 00;
byte cur7006 = 00;
byte cur7007 = 00;
byte cur7009 = 00;
byte cur700a = 00;

int changeaddr = 0;
int changevals = 0;
byte changeval1 = 0;
byte changeval2 = 0;
byte changeval3 = 0;
byte changeval4 = 0;

bool shouldreplace = false;
int replaceaddr = 0;
int replacesourcebank = 0;

byte gb_mbc::readmemory_cart(register unsigned short address) {
    if (LinkerWrangler::shouldReadThroughLinker(address)) {
        return LinkerWrangler::readThroughLinker(address);
    }
    if (address >= changeaddr && address < (changeaddr + changevals)) {
        if (address == changeaddr) return changeval1;
        if (address == changeaddr+1) return changeval2;
        if (address == changeaddr+2) return changeval3;
        if (address == changeaddr+3) return changeval4;
    }
    if (shouldreplace && address >= replaceaddr && address < 0x4000) {
       //int actualaddress = (replacesourcebank << 0xe) + address;
        byte* pointer = &(*this->gbCartRom)[replacesourcebank << 0xe];
        return pointer[address];
    }
    return mbc->readMemory(address);
}

byte last6000 = 00;
byte last7000 = 00;
byte last7001 = 00;
byte last7002 = 00;
byte last7003 = 00;
byte last7004 = 00;
byte last7005 = 00;
byte last7006 = 00;
byte last7007 = 00;
byte last7008 = 00;
byte last7009 = 00;
byte last700a = 00;
byte last700b = 00;
byte last700c = 00;
byte last700d = 00;
byte last700e = 00;
byte last700f = 00;

byte next6000 = 00;
byte next7000 = 00;
byte next7001 = 00;
byte next7002 = 00;
byte next7003 = 00;
byte next7004 = 00;
byte next7005 = 00;
byte next7006 = 00;
byte next7007 = 00;
byte next7008 = 00;
byte next7009 = 00;
byte next700a = 00;
byte next700b = 00;
byte next700c = 00;
byte next700d = 00;
byte next700e = 00;
byte next700f = 00;


void gb_mbc::writememory_cart(unsigned short address, register byte data) {
    if (LinkerWrangler::shouldWriteThroughLinker(address, data)) {
        LinkerWrangler::writeThroughLinker(address,data);
    }
    mbc->writeMemory(address,data);
    char buffer[420];
   /* byte firstbank = determineSelectedBankNo(0x0000);
    byte rombank = determineSelectedBankNo(0x4000);
    sprintf(buffer, "determined bank %02x / %02x", firstbank,rombank);
    LinkerLog::addMessage(buffer);*/
    if (address >= 0x6000) {

        unsigned short funkyaddress = address & 0xf00f;

        sprintf(buffer, "Funky write: %04x %02x", address, data);
        debug_win(buffer);

        if (funkyaddress == 0x7000 && data == 0x96) {
            if (runningmode) {
                debug_print("Running mode enabled when running mode already on");
                debug_win(buffer);
            }
            runningmode = true;
            runningvalue = 00;
        } else if (funkyaddress == 0x700f && data == 0x96) {
            if (!runningmode) {
                debug_print("Running mode disabled when running mode already off");
                debug_win(buffer);
            }
            runningmode = false;
        } else if (!runningmode) {
            sprintf(buffer, "Funky address written when running mode off: %04x %02x", address, data);
            debug_print(buffer);
            debug_win(buffer);
        } else if (funkyaddress >= 0x700b) {
            sprintf(buffer, "Write to unknown funky address: %04x %02x", address, data);
            debug_print(buffer);
            debug_win(buffer);
        } else {
            runningvalue = ((runningvalue & 1) ? 0x80 : 0) + (runningvalue >> 1);
            runningvalue = runningvalue ^ data;
            sprintf(buffer, "Addr %04x Data %02x Running %02x", address, data, runningvalue);
            debug_win(buffer);
            switch(funkyaddress) {
                case 0x7001:
                    cur7001 = runningvalue;
                    break;
                case 0x7002:
                    if (runningvalue >= 0x40) {
                        sprintf(buffer, "Value outside bank 0 at 7002: %02x", runningvalue);
                       // debug_print(buffer);
                        debug_win(buffer);
                    }
                    cur7002 = runningvalue;
                    break;
                case 0x7003:
                    if (runningvalue != 0) {
                        sprintf(buffer, "Nonzero value at 7003: %02x", runningvalue);
                       // debug_print(buffer);
                        debug_win(buffer);
                    }
                    cur7003 = runningvalue;
                    break;
                case 0x7004:
                    cur7004 = runningvalue;
                    break;
                case 0x7005:
                    cur7005 = runningvalue;
                    break;
                case 0x7006:
                    cur7006 = runningvalue;
                    break;
                case 0x7007:
                    cur7007 = runningvalue;
                    break;
                case 0x7000:
                    changeaddr = (cur7003 << 16) + (cur7002 << 8) + cur7001;
                    changeaddr &= 0x3fff; /// hMMMMMM
                    changeval1 = cur7004;
                    changeval2 = cur7005;
                    changeval3 = cur7006;
                    changeval4 = cur7007;
                    switch(runningvalue & 7) {
                        case 4:
                            changevals = 1;
                            break;
                        case 5:
                            changevals = 2;
                            break;
                        case 6:
                            changevals = 3;
                            break;
                        case 7:
                            changevals = 4;
                            break;
                        default:
                            sprintf(buffer, "Unknown command at 7008: %02x", runningvalue);
                            debug_print(buffer);
                            debug_win(buffer);
                            changevals = 0;
                    }
                    sprintf(buffer, "Now changing: addr %06x vals %02x %02x %02x %02x count %01x", changeaddr, changeval1, changeval2, changeval3, changeval4, changevals);
                    debug_win(buffer);
                    break;

                case 0x7009:
                    cur7009 = runningvalue;
                    break;
                case 0x700a:
                    if (runningvalue >= 0x40) {
                        sprintf(buffer, "Value outside bank 0 at 700a: %02x", runningvalue);
                        debug_print(buffer);
                        debug_win(buffer);
                    }
                    cur700a = runningvalue;
                    break;
                case 0x6000:
                    cur6000 = runningvalue;
                    break;
                case 0x7008:
                    replaceaddr = (cur700a << 8) + cur7009;
                    replacesourcebank = cur6000;
                    if ((runningvalue & 0xf) == 0xf) { // F to pay respects
                        shouldreplace = true;
                    } else {
                        shouldreplace = false;
                        sprintf(buffer, "Unknown command at 7008: %02x", runningvalue);
                        debug_print(buffer);
                        debug_win(buffer);
                    }
                    sprintf(buffer, "Now replacing: addr %04x sourcebank %02x shouldreplace %01x", replaceaddr, replacesourcebank, shouldreplace);
                    debug_win(buffer);

                    break;
            }
        }



        if (address==0x6000) next6000= data;
        if (address==0x7000) next7000= data;
        if (address==0x7001) next7001= data;
        if (address==0x7002) next7002= data;
        if (address==0x7003) next7003= data;
        if (address==0x7004) next7004= data;
        if (address==0x7005) next7005= data;
        if (address==0x7006) next7006= data;
        if (address==0x7007) next7007= data;
        if (address==0x7008) next7008= data;
        if (address==0x7009) next7009= data;
        if (address==0x700a) next700a= data;
        if (address==0x700b) next700b= data;
        if (address==0x700c) next700c= data;
        if (address==0x700d) next700d= data;
        if (address==0x700e) next700e= data;
        if (address==0x700f) next700f= data;

    //if (address == 0x700f && data == 0x96) {

        if (address == 0x7008 || address == 0x700f) {

            if (
                    last6000 != next6000 ||
                    last7000 != next7000 ||
                    last7001 != next7001 ||
                    last7002 != next7002 ||
                    last7003 != next7003 ||
                    last7004 != next7004 ||
                    last7005 != next7005 ||
                    last7006 != next7006 ||
                    last7007 != next7007 ||
                    last7008 != next7008 ||
                    last7009 != next7009 ||
                    last700a != next700a ||
                    last700b != next700b ||
                    last700c != next700c ||
                    last700d != next700d ||
                    last700e != next700e ||
                    last700f != next700f
                    ) {

                LinkerWrangler::recacheBank0();
            }


            last6000 = next6000;
            last7000 = next7000;
            last7001 = next7001;
            last7002 = next7002;
            last7003 = next7003;
            last7004 = next7004;
            last7005 = next7005;
            last7006 = next7006;
            last7007 = next7007;
            last7008 = next7008;
            last7009 = next7009;
            last700a = next700a;
            last700b = next700b;
            last700c = next700c;
            last700d = next700d;
            last700e = next700e;
            last700f = next700f;

        }
    }
    /*if (firstbank != 0x00) {
        byte r00 = LinkerWrangler::readThroughLinker(0x0000);
        byte r01 = LinkerWrangler::readThroughLinker(0x0001);
        byte r02 = LinkerWrangler::readThroughLinker(0x0002);
        byte r03 = LinkerWrangler::readThroughLinker(0x0003);
        sprintf(buffer, "rom0 0000 %02x %02x %02x %02x", r00,r01,r02,r03);
        LinkerLog::addMessage(buffer);
        r00 = LinkerWrangler::readThroughLinker(0x2000);
        r01 = LinkerWrangler::readThroughLinker(0x2001);
        r02 = LinkerWrangler::readThroughLinker(0x2002);
        r03 = LinkerWrangler::readThroughLinker(0x2003);
        sprintf(buffer, "rom0 2000 %02x %02x %02x %02x", r00,r01,r02,r03);
        LinkerLog::addMessage(buffer);
    }*/
    /*if (rombank != 0xff) {
        mbc->writeMemory(0x2000, rombank);
    }*/
}

void gb_mbc::signalMemoryWrite(unsigned short address, register byte data) {
    mbc->signalMemoryWrite(address,data);
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
        case MEMORY_MK12:
            mbc = new MbcLicMk12();
            break;
        case MEMORY_LBMULTI:
            mbc = new MbcUnlLbMulti();
            break;
        case MEMORY_NTNEW:
            mbc = new MbcUnlNtNew();
            break;
        case MEMORY_POKEJD:
            mbc = new MbcUnlPokeJadeDia();
            break;
        case MEMORY_WISDOMTREE:
            mbc = new MbcUnlWisdomTree();
            break;
        case MEMORY_SACHENMMC1:
            mbc = new MbcUnlSachenMMC1();
            break;
        case MEMORY_SACHENMMC2:
            mbc = new MbcUnlSachenMMC2();
            break;
        case MEMORY_M161:
            mbc = new MbcLicM161();
            break;
        case MEMORY_ROCKET:
            mbc = new MbcUnlRocketGames();
            break;
        case MEMORY_NEWGBHK:
            mbc = new MbcUnlNewGbHk();
            break;
        case MEMORY_GGB81:
            mbc = new MbcUnlGgb81();
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

