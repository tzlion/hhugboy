//
// Created by Alex on 20/08/2016.
//

#include "Mbc.h"

void Mbc::init(byte** gbMemMap, GBrom** gbRom, byte** gbMemory, byte* gbRomBankXor) {
    this->gbMemMap = gbMemMap;
    this->gbRom = gbRom;
    this->gbMemory = gbMemory;
    this->gbRomBankXor = gbRomBankXor;
}

Mbc::Mbc():
        bc_select(0),

        HuC3_RAMvalue(0),
        HuC3_RAMaddress(0),
        HuC3_address(0),
        HuC3_RAMflag(0),
        HuC3_last_time(time(0)),
        HuC3_flag(HUC3_NONE),
        HuC3_time(0),
        HuC3_shift(0),

        MBC7_cs(0),
        MBC7_sk(0),
        MBC7_state(0),
        MBC7_buffer(0),
        MBC7_idle(0),
        MBC7_count(0),
        MBC7_code(0),
        MBC7_address(0),
        MBC7_writeEnable(0),
        MBC7_value(0),

        tama_flag(0),
        tama_time(0),
        tama_val4(0),
        tama_val5(0),
        tama_val6(0),
        tama_val7(0),
        tama_count(0),
        tama_month(0),
        tama_change_clock(0),

        sintax_mode(0),
        sintax_xor2(0),
        sintax_xor3(0),
        sintax_xor4(0),
        sintax_xor5(0),

        MBC1memorymodel(0),

        RTCIO(0),

        RTC_latched(0),
        cameraIO(0)
{

}
