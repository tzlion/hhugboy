//
// Created by Alex on 18/08/2016.
//
#ifndef HHUGBOY_GB_MBC_H_H
#define HHUGBOY_GB_MBC_H_H

class gb_system;

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
    gb_mbc(gb_system* inGB);

    int HuC3_register[8];
    int HuC3_RAMvalue;
    int HuC3_RAMaddress;
    int HuC3_address;
    int HuC3_RAMflag;

    time_t HuC3_last_time;
    int HuC3_flag;
    unsigned int HuC3_time;
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
    int tama_month;
    int tama_change_clock;

    memoryaccess memory_read;
    memoryaccess memory_write;

    void rtc_update();
    void update_HuC3time();
    void update_tama_RTC();

    byte readmemory_cart(register unsigned short address);
    void writememory_cart(unsigned short address,register byte data);

//private:
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

private:
    gb_system* aGB;
};

#endif //HHUGBOY_GB_MBC_H_H
