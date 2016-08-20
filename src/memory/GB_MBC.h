//
// Created by Alex on 18/08/2016.
//
#ifndef HHUGBOY_GB_MBC_H_H
#define HHUGBOY_GB_MBC_H_H

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
    MEMORY_SINTAX
};

enum
{
    HUC3_READ = 0,
    HUC3_WRITE = 1,
    HUC3_NONE = 2
};

class gb_mbc {

public:
    gb_mbc(byte** gbMemMap, byte** gbCartridge, GBrom** gbRom, byte** gbCartRam, byte* romBankXor, int* rumbleCounter, byte** gbMemory);

    void setMemoryRead(memoryaccess memory_read);
    void setMemoryWrite(memoryaccess memory_write);

    int getRomBank();
    int getRamBank();

    byte readmemory_cart(register unsigned short address);
    void writememory_cart(unsigned short address,register byte data);
    void resetMbcVariables();
    void resetRomMemoryMap(bool resetOffset);

    void writeMbcSpecificStuffToSaveFile(FILE *savefile);
    void readMbcSpecificStuffFromSaveFile(FILE *savefile);

    void readMbcBanksFromStateFile(FILE *statefile);
    void readMbcMoreCrapFromStateFile(FILE *statefile);
    void writeMbcBanksToStateFile(FILE *statefile);
    void writeMbcOtherStuffToStateFile(FILE *statefile);

    void writeNewerCartSpecificVarsToStateFile(FILE *statefile);
    void writeCartSpecificVarsToStateFile(FILE *statefile);
    void readCartSpecificVarsFromStateFile(FILE *statefile);
    void readNewerCartSpecificVarsFromStateFile(FILE *statefile);

private:

    byte** gbMemMap;
    GBrom** gbRom;
    byte** gbCartRam;
    byte** gbCartridge;
    byte* gbRomBankXor;
    int* gbRumbleCounter;
    byte** gbMemory;

    memoryaccess memory_read;
    memoryaccess memory_write;

    int rom_bank;
    int ram_bank;

    unsigned short MBChi;
    unsigned short MBClo;

    int MBC1memorymodel;
    int RAMenable;

    int RTCIO;
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

    int superaddroffset;

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

    int bc_select; // for collection carts

    int tama_flag;
    byte tama_time;
    int tama_val4;
    int tama_val5;
    int tama_val6;
    int tama_val7;
    int tama_count;
    int tama_change_clock;

    void rtc_update();
    void update_HuC3time();
    void update_tama_RTC();

    // MBC R/W methods
    byte readmemory_default(register unsigned short address);
    byte readmemory_MBC3(register unsigned short address);
    byte readmemory_Camera(register unsigned short address);
    void writememory_default(unsigned short address,register byte data);
    void writememory_MBC1(unsigned short address,register byte data);
    void writememory_MBC2(unsigned short address,register byte data);
    void writememory_MBC3(unsigned short address,register byte data);
    void writememory_MBC5(unsigned short address,register byte data, bool isNiutoude, bool isSintax);
    void writememory_Camera(unsigned short address,register byte data);
    byte readmemory_HuC3(register unsigned short address);
    void writememory_HuC3(register unsigned short address,register byte data);
    void writememory_MBC7(unsigned short address,register byte data);
    byte readmemory_MBC7(register unsigned short address);
    byte readmemory_TAMA5(register unsigned short address);
    byte readmemory_sintax(register unsigned short address);
    void writememory_TAMA5(register unsigned short address,register byte data);
    void writememory_Rockman8(register unsigned short address,register byte data);
    void writememory_BC(register unsigned short address,register byte data);
    void writememory_8in1(register unsigned short address,register byte data);
    void writememory_MMM01(register unsigned short address,register byte data);
    void writememory_MK12(register unsigned short address,register byte data);
    void writememory_poke(register unsigned short address,register byte data);
    void setXorForBank(byte bankNo);

    void readRtcVarsFromStateFile(FILE *statefile);
    void readHuc3VarsFromStateFile(FILE *statefile);
    void readMbc7VarsFromStateFile(FILE *statefile);
    void readTama5VarsFromStateFile(FILE *statefile);
    void readMoreTama5VarsFromStateFile(FILE *statefile);
    void writeRtcVarsToStateFile(FILE *statefile);
    void writeHuc3VarsToStateFile(FILE *statefile);
    void writeMbc7VarsToStateFile(FILE *statefile);
    void writeTama5VarsToStateFile(FILE *statefile);
    void writeMoreTama5VarsToStateFile(FILE *statefile);

};

#endif //HHUGBOY_GB_MBC_H_H
