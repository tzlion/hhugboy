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

#ifndef HHUGBOY_ABSTRACTMBC_H
#define HHUGBOY_ABSTRACTMBC_H


#include <wchar.h>
#include "../../types.h"
#include "../../rom.h"

enum
{
    HUC3_READ = 0,
    HUC3_WRITE = 1,
    HUC3_NONE = 2
};

/**
 * MBC shouldn't really start knowing too much that actually should be under cart though
 * E.g. anything currently in the "rom" object like ramsize,romsize,...
 * Maybe our current GB_MBC could become Cartridge or CartWrangler or something and ROM should then belong to that.
 */

class AbstractMbc {

public:

/*** THESE SHOULD PROBABLY BE PROTECTED BUT ARE ACCESSED FROM OUTSIDE AT THE MOMENT ***/

    int multicartOffset;
    int multicartRamOffset;

    bool deferredReset = false;

    int rom_bank;
    int ram_bank;
    int RAMenable;

/*** SHOULD BE PROTECTED END ***/

/*** THESE SHOULD GO SOMEWHERE ELSE ***/

    unsigned int cart_address = 0; // this is dodgy just get rid of it eventually kthx

    int maxROMbank[9] = { 1, 3, 7, 15, 31, 63, 127, 255, 511 };
    int maxRAMbank[10] = { 0, 0, 0, 3, 15, 7, 0, 0, 0, 4 };
    int rom_size_mask[9] = { 0x00007fff, 0x0000ffff, 0x0001ffff, 0x0003ffff, 0x0007ffff, 0x000fffff, 0x001fffff, 0x003fffff, 0x007fffff };

/*** STUFF THAT SHOULD GO SOMEWHERE ELSE END ***/

    AbstractMbc();

    void init(byte** gbMemMap, Cartridge** gbCartridge, byte** gbMemory, byte** gbCartRom, byte** gbCartRam, int* gbRumbleCounter);
    virtual byte readMemory(register unsigned short address) = 0;
    virtual void writeMemory(unsigned short address, register byte data) = 0;
    virtual void resetVars(bool preserveMulticartState);
    virtual void writeMbcSpecificVarsToStateFile(FILE *statefile);
    virtual void writeSgbMbcSpecificVarsToStateFile(FILE *statefile);
    virtual void readMbcSpecificVarsFromStateFile(FILE *statefile);
    virtual void readSgbMbcSpecificVarsFromStateFile(FILE *statefile);
    virtual void readMbcSpecificVarsFromSaveFile(FILE *savefile);
    virtual void writeMbcSpecificVarsToSaveFile(FILE *savefile);

    void readMbcBanksFromStateFile(FILE *statefile);
    void writeMbcBanksToStateFile(FILE *statefile);
    void readMbcOtherStuffFromStateFile(FILE *statefile);
    void writeMbcOtherStuffToStateFile(FILE *statefile);
    void resetRomMemoryMap(bool preserveMulticartState=false);

protected:
    byte** gbMemMap;
    byte** gbMemory;
    Cartridge** gbCartridge;
    byte** gbCartRom;
    byte** gbCartRam;
    int* gbRumbleCounter;

    unsigned short MBChi;
    unsigned short MBClo;

    void setRom0Offset(int offset);
    void setRom1Offset(int offset);
    void setRom1Bank(int bankNo);

/*** THESE SHOULD BE IN SUB CLASSES ***/

    byte romBankXor;

    int MBC1memorymodel;

    int bc_select; // for collection carts

    int RTCIO; // RTC applies to multiple mappers so all can extend some RTC class I guess
    int RTC_latched;
    rtc_clock rtc;
    rtc_clock rtc_latch;

    inline byte switchOrder( byte input, byte* reorder )
    {
        byte newbyte=0;
        for( byte x=0;x<8;x++ ) {
            newbyte += ( ( input >> ( 7 - reorder[x] ) ) & 1 ) << ( 7 - x );
        }

        return newbyte;
    }

/*** SHOULD BE IN SUBCLASSES END ***/

};


#endif //HHUGBOY_ABSTRACTMBC_H
