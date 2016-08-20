//
// Created by Alex on 20/08/2016.
//

#include "Mbc3.h"

byte Mbc3::readMemory(register unsigned short address) {

    if(address >= 0xA000 && address < 0xC000)
    {
        if((*gbRom)->RTC && RTCIO)
        {
            switch(rtc.cur_register)
            {
                case 0x08:
                    return rtc_latch.s;
                case 0x09:
                    return rtc_latch.m;
                case 0x0A:
                    return rtc_latch.h;
                case 0x0B:
                    return (rtc_latch.d&0xFF);
                case 0x0C:
                    return ((rtc_latch.d&0x100)>>8)|rtc_latch.control;
            }
        }
        //if(!rom->RAMsize)
        //   return 0xFF;
    }

    return Default::readMemory(address);
}
