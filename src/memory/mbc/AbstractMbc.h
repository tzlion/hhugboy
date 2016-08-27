//
// Created by Alex on 20/08/2016.
//

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
 * Note: ROM has a "bankType" which doesn't necessarily correspond with the MBC
 * Generally this will be something more standard even if the MBC type is set to something weird
 * There are also some banktypes like "MBC4" which is set w/o any MBC selected (so it just falls back to Default)
 * (Maybe the "Unknown" case should fall back to MBC5 with 4m size, might fix some pirates or at least make them boot)
 * (Although BasicMBC is pretty similar as is)
 *
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

/*** SHOULD BE PROTECTED END ***/

/*** THESE SHOULD GO SOMEWHERE ELSE ***/

    unsigned int cart_address = 0; // this is dodgy just get rid of it eventually kthx

    int maxROMbank[9] = { 1, 3, 7, 15, 31, 63, 127, 255, 511 };
    int maxRAMbank[10] = { 0, 0, 0, 4, 15, 7, 0, 0, 0, 4 };
    int rom_size_mask[9] = { 0x00007fff, 0x0000ffff, 0x0001ffff, 0x0003ffff, 0x0007ffff, 0x000fffff, 0x001fffff, 0x003fffff, 0x007fffff };

/*** STUFF THAT SHOULD GO SOMEWHERE ELSE END ***/

    AbstractMbc();

    void init(byte** gbMemMap, GBrom** gbRom, byte** gbMemory, byte* gbRomBankXor, byte** gbCartridge, byte** gbCartRam, int* gbRumbleCounter);
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
    GBrom** gbRom;
    byte* gbRomBankXor;
    byte** gbCartridge;
    byte** gbCartRam;
    int* gbRumbleCounter;

    unsigned short MBChi;
    unsigned short MBClo;
    int RAMenable;

/*** THESE SHOULD BE IN SUB CLASSES ***/

    int MBC1memorymodel;

    int bc_select; // for collection carts

    int RTCIO; // RTC applies to multiple mappers so all can extend some RTC class I guess
    int RTC_latched;
    rtc_clock rtc;
    rtc_clock rtc_latch;

/*** SHOULD BE IN SUBCLASSES END ***/

};


#endif //HHUGBOY_ABSTRACTMBC_H
