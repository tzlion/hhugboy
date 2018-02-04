/*
   hhugboy Game Boy emulator
   copyright 2013-2016 taizou
   Based on GEST
   Copyright (C) 2003-2010 TM
   Incorporating code from VisualBoyAdvance
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

#include "AbstractMbc.h"
#include <cstdio>

void AbstractMbc::init(byte** gbMemMap, Cartridge** gbRom, byte** gbMemory, byte** gbCartridge, byte** gbCartRam, int* gbRumbleCounter) {
    this->gbMemMap = gbMemMap;
    this->gbRom = gbRom;
    this->gbMemory = gbMemory;
    this->gbCartridge = gbCartridge;
    this->gbCartRam = gbCartRam;
    this->gbRumbleCounter = gbRumbleCounter;
}

AbstractMbc::AbstractMbc():
        rom_bank(1),
        ram_bank(0),

        RAMenable(0),
        MBChi(0),
        MBClo(0),

        bc_select(0),

        MBC1memorymodel(0),

        RTCIO(0),

        RTC_latched(0),

        multicartOffset(0),
        multicartRamOffset(0),

        romBankXor(0)
{

}

void AbstractMbc::resetVars(bool preserveMulticartState = false) {

    MBC1memorymodel = 0;
    MBChi = 0;
    MBClo = 1;
    rom_bank = 1;
    ram_bank = 0;
    RTCIO = 0;

    if ( !preserveMulticartState ) {
        bc_select = 0;
    }

    RTC_latched = 0;

    rtc.s = 0;
    rtc.m = 0;
    rtc.h = 0;
    rtc.d = 0;
    rtc.control = 0;
    rtc.last_time = time(0);
    rtc.cur_register = 0x08;

}

void AbstractMbc::readSgbMbcSpecificVarsFromStateFile(FILE *statefile) {

}

void AbstractMbc::readMbcSpecificVarsFromStateFile(FILE *statefile) {

}

void AbstractMbc::writeSgbMbcSpecificVarsToStateFile(FILE *statefile) {

}

void AbstractMbc::writeMbcSpecificVarsToStateFile(FILE *statefile) {

}

void AbstractMbc::readMbcSpecificVarsFromSaveFile(FILE *savefile) {
    // todo: move to respective mappers
    if((*gbRom)->RTC || (*gbRom)->mbcType == MEMORY_TAMA5)
    {
        fread(&(rtc).s, sizeof(int), 1, savefile);
        fread(&(rtc).m, sizeof(int), 1, savefile);
        fread(&(rtc).h, sizeof(int), 1, savefile);
        fread(&(rtc).d, sizeof(int), 1, savefile);
        fread(&(rtc).control, sizeof(int), 1, savefile);
        fread(&(rtc).last_time, sizeof(time_t), 1, savefile);
        rtc_latch = rtc;
    }
}

void AbstractMbc::writeMbcSpecificVarsToSaveFile(FILE *savefile) {
    // todo: move to respective mappers
    if((*gbRom)->RTC || (*gbRom)->mbcType == MEMORY_TAMA5)
    {
        fwrite(&(rtc).s, sizeof(int), 1, savefile);
        fwrite(&(rtc).m, sizeof(int), 1, savefile);
        fwrite(&(rtc).h, sizeof(int), 1, savefile);
        fwrite(&(rtc).d, sizeof(int), 1, savefile);
        fwrite(&(rtc).control, sizeof(int), 1, savefile);
        fwrite(&(rtc).last_time, sizeof(time_t), 1, savefile);
    }
}

void AbstractMbc::writeMbcOtherStuffToStateFile(FILE *statefile) {
    fwrite(&( MBC1memorymodel), sizeof(int), 1, statefile);
    fwrite(&(RAMenable), sizeof(int), 1, statefile);
    fwrite(&(MBChi), sizeof(unsigned int), 1, statefile);
    fwrite(&(MBClo), sizeof(unsigned int), 1, statefile);
}

void AbstractMbc::writeMbcBanksToStateFile(FILE *statefile) {
    fwrite(&(rom_bank), sizeof(int), 1, statefile);
    fwrite(&(ram_bank), sizeof(int), 1, statefile);
}

void AbstractMbc::readMbcOtherStuffFromStateFile(FILE *statefile) {
    fread(&(MBC1memorymodel), sizeof(int), 1, statefile);
    fread(&(RAMenable), sizeof(int), 1, statefile);
    fread(&(MBChi), sizeof(unsigned int), 1, statefile);
    fread(&(MBClo), sizeof(unsigned int), 1, statefile);
}

void AbstractMbc::readMbcBanksFromStateFile(FILE *statefile) {
    fread(&(rom_bank), sizeof(int), 1, statefile);
    fread(&(ram_bank), sizeof(int), 1, statefile);
}

void AbstractMbc::resetRomMemoryMap(bool preserveMulticartState) {
    if ( !preserveMulticartState ) {
        multicartOffset = 0;
        multicartRamOffset = 0;
    }
    setRom0Offset(multicartOffset);
    setRom1Offset(multicartOffset + 0x4000);

    //todo: do this on savestates too
    if((*gbRom)->RAMsize>2) {
        gbMemMap[0xA] = &(*gbCartRam)[multicartRamOffset];
        gbMemMap[0xB] = &(*gbCartRam)[multicartRamOffset + 0x1000];
    }
}

void AbstractMbc::setRom0Offset(int offset) {
    gbMemMap[0x0] = &(*gbCartridge)[offset];
    gbMemMap[0x1] = &(*gbCartridge)[offset+0x1000];
    gbMemMap[0x2] = &(*gbCartridge)[offset+0x2000];
    gbMemMap[0x3] = &(*gbCartridge)[offset+0x3000];
}

void AbstractMbc::setRom1Offset(int offset) {
    gbMemMap[0x4] = &(*gbCartridge)[offset];
    gbMemMap[0x5] = &(*gbCartridge)[offset+0x1000];
    gbMemMap[0x6] = &(*gbCartridge)[offset+0x2000];
    gbMemMap[0x7] = &(*gbCartridge)[offset+0x3000];
}

void AbstractMbc::setRom1Bank(int bankNo) {
    rom_bank = bankNo;
    int bankAddress = rom_bank<<14;
    bankAddress &= rom_size_mask[(*gbRom)->ROMsize];
    bankAddress += multicartOffset;
    setRom1Offset(bankAddress);
}
