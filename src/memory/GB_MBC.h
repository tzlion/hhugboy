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
#ifndef HHUGBOY_GB_MBC_H_H
#define HHUGBOY_GB_MBC_H_H

#include "mbc/AbstractMbc.h"
#include "mbc/MbcLicHuc3.h"

enum memoryaccess
{
    MEMORY_DEFAULT = 0,
    MEMORY_MBC1,
    MEMORY_MBC2,
    MEMORY_MBC3,
    MEMORY_MBC5,
    MEMORY_CAMERA,
    MEMORY_HUC3,
    MEMORY_MBC7,
    MEMORY_TAMA5,
    MEMORY_ROCKMAN8,
    MEMORY_BC,
    MEMORY_8IN1,
    MEMORY_MMM01,
    MEMORY_MK12,
    MEMORY_POKE,
    MEMORY_NIUTOUDE,
    MEMORY_SINTAX,
    MEMORY_BBD,
    MEMORY_ROMONLY,
    MEMORY_LBMULTI,
    MEMORY_DBZTRANS,
};

class gb_mbc {

public:
    gb_mbc(byte** gbMemMap, byte** gbCartridge, GBrom** gbRom, byte** gbCartRam, byte* romBankXor, int* rumbleCounter, byte** gbMemory);

    void setMemoryReadWrite(memoryaccess memory_type);

    int getRomBank();
    int getRamBank();

    byte readmemory_cart(register unsigned short address);
    void writememory_cart(unsigned short address,register byte data);
    void resetMbcVariables(bool preserveMulticartState);
    void resetRomMemoryMap(bool preserveMulticartState);

    void readMbcSpecificStuffFromSaveFile(FILE *savefile);
    void writeMbcSpecificStuffToSaveFile(FILE *savefile);

    void readMbcBanksFromStateFile(FILE *statefile);
    void readMbcMoreCrapFromStateFile(FILE *statefile);
    void writeMbcBanksToStateFile(FILE *statefile);
    void writeMbcOtherStuffToStateFile(FILE *statefile);

    void writeNewerCartSpecificVarsToStateFile(FILE *statefile);
    void writeCartSpecificVarsToStateFile(FILE *statefile);
    void readCartSpecificVarsFromStateFile(FILE *statefile);
    void readNewerCartSpecificVarsFromStateFile(FILE *statefile);

    bool shouldReset();

    int getOffset();
    int getRamOffset();

private:

    AbstractMbc *mbc;

    byte** gbMemMap;
    GBrom** gbRom;
    byte** gbCartRam;
    byte** gbCartridge;
    byte* gbRomBankXor;
    int* gbRumbleCounter;
    byte** gbMemory;

    memoryaccess mbcType;

};

#endif //HHUGBOY_GB_MBC_H_H
