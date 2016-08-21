//
// Created by Alex on 20/08/2016.
//

#include "AbstractMbc.h"
#include <cstdio>

void AbstractMbc::init(byte** gbMemMap, GBrom** gbRom, byte** gbMemory, byte* gbRomBankXor, byte** gbCartridge, byte** gbCartRam, int* gbRumbleCounter) {
    this->gbMemMap = gbMemMap;
    this->gbRom = gbRom;
    this->gbMemory = gbMemory;
    this->gbRomBankXor = gbRomBankXor;
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

void AbstractMbc::resetVars() {
    // todo: move to respective mappers
    MBC1memorymodel = 0;
    MBChi = 0;
    MBClo = 1;
    rom_bank = 1;
    ram_bank = 0;
    RTCIO = 0;

    bc_select = 0;

    cameraIO = 0;
    RTC_latched = 0;

    rtc.s = 0;
    rtc.m = 0;
    rtc.h = 0;
    rtc.d = 0;
    rtc.control = 0;
    rtc.last_time = time(0);
    rtc.cur_register = 0x08;

    tama_flag = 0;
    tama_time = 0;
    tama_val6 = 0;
    tama_val7 = 0;
    tama_val4 = 0;
    tama_val5 = 0;
    tama_count = 0;
    tama_month = 0;
    tama_change_clock = 0;

    HuC3_flag = HUC3_NONE;
    HuC3_RAMvalue = 1;

    sintax_mode = 0;
    sintax_xor2 = sintax_xor3 = sintax_xor4 = sintax_xor5 = 0;
}

void AbstractMbc::readNewMbcSpecificVarsFromStateFile(FILE *statefile) {

}

void AbstractMbc::readOldMbcSpecificVarsFromStateFile(FILE *statefile) {

}

void AbstractMbc::writeNewMbcSpecificVarsToStateFile(FILE *statefile) {

}

void AbstractMbc::writeOldMbcSpecificVarsToStateFile(FILE *statefile) {

}

void AbstractMbc::readMbcSpecificVarsFromSaveFile(FILE *savefile) {
    // todo: move to respective mappers
    if((*gbRom)->RTC || (*gbRom)->bankType == TAMA5)
    {
        fread(&(rtc).s, sizeof(int), 1, savefile);
        fread(&(rtc).m, sizeof(int), 1, savefile);
        fread(&(rtc).h, sizeof(int), 1, savefile);
        fread(&(rtc).d, sizeof(int), 1, savefile);
        fread(&(rtc).control, sizeof(int), 1, savefile);
        fread(&(rtc).last_time, sizeof(time_t), 1, savefile);
        rtc_latch = rtc;
    }

    if((*gbRom)->bankType == TAMA5)
        fread(&(tama_month), sizeof(int), 1, savefile);

    if((*gbRom)->bankType == HuC3)
    {
        fread(&(HuC3_time), sizeof(unsigned int), 1, savefile);
        fread(&(HuC3_last_time), sizeof(time_t), 1, savefile);
        fread(&(rtc).s, sizeof(int), 1, savefile);
    }

}

void AbstractMbc::writeMbcSpecificVarsToSaveFile(FILE *savefile) {
    // todo: move to respective mappers
    if((*gbRom)->RTC || (*gbRom)->bankType == TAMA5)
    {
        fwrite(&(rtc).s, sizeof(int), 1, savefile);
        fwrite(&(rtc).m, sizeof(int), 1, savefile);
        fwrite(&(rtc).h, sizeof(int), 1, savefile);
        fwrite(&(rtc).d, sizeof(int), 1, savefile);
        fwrite(&(rtc).control, sizeof(int), 1, savefile);
        fwrite(&(rtc).last_time, sizeof(time_t), 1, savefile);
    }

    if((*gbRom)->bankType == TAMA5)
        fwrite(&(tama_month), sizeof(int), 1, savefile);

    if((*gbRom)->bankType == HuC3)
    {
        fwrite(&(HuC3_time), sizeof(unsigned int), 1, savefile);
        fwrite(&(HuC3_last_time), sizeof(time_t), 1, savefile);
        fwrite(&(rtc).s, sizeof(int), 1, savefile);
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

void AbstractMbc::resetRomMemoryMap(bool resetOffset) {
    if ( resetOffset ) {
        superaddroffset = 0;
    }
    gbMemMap[0x0] = &(*gbCartridge)[superaddroffset+0x0000];
    gbMemMap[0x1] = &(*gbCartridge)[superaddroffset+0x1000];
    gbMemMap[0x2] = &(*gbCartridge)[superaddroffset+0x2000];
    gbMemMap[0x3] = &(*gbCartridge)[superaddroffset+0x3000];
    gbMemMap[0x4] = &(*gbCartridge)[superaddroffset+0x4000];
    gbMemMap[0x5] = &(*gbCartridge)[superaddroffset+0x5000];
    gbMemMap[0x6] = &(*gbCartridge)[superaddroffset+0x6000];
    gbMemMap[0x7] = &(*gbCartridge)[superaddroffset+0x7000];
}
