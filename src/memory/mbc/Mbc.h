//
// Created by Alex on 20/08/2016.
//

#ifndef HHUGBOY_MBCBASE_H
#define HHUGBOY_MBCBASE_H


#include "../../types.h"
#include "../../rom.h"

enum
{
    HUC3_READ = 0,
    HUC3_WRITE = 1,
    HUC3_NONE = 2
};

class Mbc {
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

    Mbc();

/*** BAD PUBLICS END ***/

    void init(byte** gbMemMap, GBrom** gbRom, byte** gbMemory, byte* gbRomBankXor);
    virtual byte readMemory(register unsigned short address) = 0;
    virtual void resetVars() = 0;
protected:
    byte** gbMemMap;
    byte** gbMemory;
    GBrom** gbRom;
    byte* gbRomBankXor;
};


#endif //HHUGBOY_MBCBASE_H
