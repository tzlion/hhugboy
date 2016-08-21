//
// Created by Alex on 20/08/2016.
//

#include <cstdio>
#include "MbcNin3.h"

byte MbcNin3::readMemory(register unsigned short address) {

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

    return BasicMbc::readMemory(address);
}

void MbcNin3::readOldMbcSpecificVarsFromStateFile(FILE *statefile) {
    if((*gbRom)->RTC)
    {
        fread(&(rtc).s, sizeof(int), 1, statefile);
        fread(&(rtc).m, sizeof(int), 1, statefile);
        fread(&(rtc).h, sizeof(int), 1, statefile);
        fread(&(rtc).d, sizeof(int), 1, statefile);
        fread(&(rtc).control, sizeof(int), 1, statefile);
        fread(&(rtc).last_time, sizeof(time_t), 1, statefile);

        fread(&(rtc_latch).s, sizeof(int), 1, statefile);
        fread(&(rtc_latch).m, sizeof(int), 1, statefile);
        fread(&(rtc_latch).h, sizeof(int), 1, statefile);
        fread(&(rtc_latch).d, sizeof(int), 1, statefile);
        fread(&(rtc_latch).control, sizeof(int), 1, statefile);
        fread(&(rtc_latch).last_time, sizeof(time_t), 1, statefile);
    }
}

void MbcNin3::writeOldMbcSpecificVarsToStateFile(FILE *statefile) {
    if((*gbRom)->RTC)
    {
        fwrite(&(rtc).s, sizeof(int), 1, statefile);
        fwrite(&(rtc).m, sizeof(int), 1, statefile);
        fwrite(&(rtc).h, sizeof(int), 1, statefile);
        fwrite(&(rtc).d, sizeof(int), 1, statefile);
        fwrite(&(rtc).control, sizeof(int), 1, statefile);
        fwrite(&(rtc).last_time, sizeof(time_t), 1, statefile);

        fwrite(&(rtc_latch).s, sizeof(int), 1, statefile);
        fwrite(&(rtc_latch).m, sizeof(int), 1, statefile);
        fwrite(&(rtc_latch).h, sizeof(int), 1, statefile);
        fwrite(&(rtc_latch).d, sizeof(int), 1, statefile);
        fwrite(&(rtc_latch).control, sizeof(int), 1, statefile);
        fwrite(&(rtc_latch).last_time, sizeof(time_t), 1, statefile);
    }
}
