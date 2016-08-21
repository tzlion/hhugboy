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
 * Something to not forget
 * For things that used to vary based on BANK TYPE e.g. Save states did this
 * The Bank type may be something more standard even if the MBC type is set to something weird
 * See rom.cpp
 * Savestates should be fine since the only MBCs that save their own shit are mostly weird ones
 * The most "standard" one is MBC3 as used by Pokemon G/S.. And only saves the RTC shit
 * HOPEFULLY no one is overriding an MBC3 RTC ROM with their own business
 * There are also some banktypes like MBC4 which is set w/o any MBC selected (so it just falls back to Default)
 * The "Unknown Cart" case should fall back to MBC5 tbh, would prob fix some pirates or at least make them boot
 * Oh actually the "Default" case is basically like an MBC lol. It varies only when BankType == ROM then it does shit all on bankswitches
 * Hey and I don't want MBC to start knowing too much that actually should be under cart I guess
 * E.g. stuff that is currently in the "rom" object like ramsize,romsize,...
 * Maybe our current GB_MBC could become Cartridge or CartWrangler or sth
 * ROM should then belong to IT.
 */

class AbstractMbc {
public:

    /*** BAD PUBLICS START - SHOULD BE MAPPER SPECIFIC AND PRIVATE (OR PROTECTED SOMETIMES) ***/

    int MBC1memorymodel;

    int bc_select; // for collection carts

    int HuC3_register[8];
    int HuC3_RAMvalue;
    int HuC3_RAMaddress;
    int HuC3_address;
    int HuC3_RAMflag;

    int HuC3_flag;
    int HuC3_shift;

    int MBC7_cs; // chip select
    int MBC7_sk; // ?
    int MBC7_state; // mapper state
    int MBC7_buffer; // buffer for receiving serial data
    int MBC7_idle; // idle state
    int MBC7_count; // count of bits received
    int MBC7_code; // command received
    int MBC7_address; // address received
    int MBC7_writeEnable; // write enable
    int MBC7_value; // value to return on ram

    int tama_flag;
    byte tama_time;
    int tama_val4;
    int tama_val5;
    int tama_val6;
    int tama_val7;
    int tama_count;
    int tama_change_clock;

    int RTCIO; // RTC applies to multiple mappers so all can extend some RTC class I guess
    int RTC_latched;

    int cameraIO;

    int tama_month;
    time_t HuC3_last_time;
    unsigned int HuC3_time;
    rtc_clock rtc;
    rtc_clock rtc_latch;

    byte sintax_mode;
    byte sintax_xor2;
    byte sintax_xor3;
    byte sintax_xor4;
    byte sintax_xor5;

/*** BAD PUBLICS END ***/

/*** THESE SHOULD BE PROTECTED IN THIS CLASS BUT NOT PUBLIC ***/
    int rom_bank;
    int ram_bank;

    unsigned short MBChi;
    unsigned short MBClo;

    int RAMenable;
/** MORE BAD PUBLICS END **/
/*** THESE SHOULD GO SOMEWHERE ELSE ***/
    unsigned int cart_address = 0; // this is dodgy just get rid of it eventually kthx
    int maxROMbank[9] = { 1, 3, 7, 15, 31, 63, 127, 255, 511 };
    int maxRAMbank[6] = { 0, 0, 0, 4, 15, 7 };
    int rom_size_mask[9] = { 0x00007fff, 0x0000ffff, 0x0001ffff, 0x0003ffff, 0x0007ffff, 0x000fffff, 0x001fffff, 0x003fffff, 0x007fffff };
/** MORE BAD PUBLICS END **/

    AbstractMbc();

    void init(byte** gbMemMap, GBrom** gbRom, byte** gbMemory, byte* gbRomBankXor, byte** gbCartridge, byte** gbCartRam);
    virtual byte readMemory(register unsigned short address) = 0;
    virtual void writeMemory(unsigned short address, register byte data) = 0;
    virtual void resetVars();
    virtual void writeOldMbcSpecificVarsToStateFile(FILE *statefile);
    virtual void writeNewMbcSpecificVarsToStateFile(FILE *statefile);
    virtual void readOldMbcSpecificVarsFromStateFile(FILE *statefile);
    virtual void readNewMbcSpecificVarsFromStateFile(FILE *statefile);
protected:
    byte** gbMemMap;
    byte** gbMemory;
    GBrom** gbRom;
    byte* gbRomBankXor;
    byte** gbCartridge;
    byte** gbCartRam;
};


#endif //HHUGBOY_ABSTRACTMBC_H
