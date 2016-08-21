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
    virtual void resetVars() = 0;
    virtual void writeOldMbcSpecificVarsToStateFile(FILE *statefile) = 0;
    virtual void writeNewMbcSpecificVarsToStateFile(FILE *statefile) = 0;
    virtual void readOldMbcSpecificVarsFromStateFile(FILE *statefile) = 0;
    virtual void readNewMbcSpecificVarsFromStateFile(FILE *statefile) = 0;
protected:
    byte** gbMemMap;
    byte** gbMemory;
    GBrom** gbRom;
    byte* gbRomBankXor;
    byte** gbCartridge;
    byte** gbCartRam;
};


#endif //HHUGBOY_ABSTRACTMBC_H
