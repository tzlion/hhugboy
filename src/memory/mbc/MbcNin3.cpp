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

void MbcNin3::readMbcSpecificVarsFromStateFile(FILE *statefile) {
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

void MbcNin3::writeMbcSpecificVarsToStateFile(FILE *statefile) {
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

void MbcNin3::writeMemory(unsigned short address, register byte data) {

    if(address < 0x2000)// Is it a RAM bank enable/disable?
    {
        RAMenable = ( (data&0x0A) == 0x0A ? 1 : 0);
        return;
    }

    if(address < 0x4000) // Is it a ROM bank switch?
    {
        data &= 0x7F;
        if(data==0)
            data=1;

        rom_bank = data;

        int cadr = data<<14;
        cadr &= rom_size_mask[(*gbRom)->ROMsize];
        gbMemMap[0x4] = &(*gbCartridge)[cadr];
        gbMemMap[0x5] = &(*gbCartridge)[cadr+0x1000];
        gbMemMap[0x6] = &(*gbCartridge)[cadr+0x2000];
        gbMemMap[0x7] = &(*gbCartridge)[cadr+0x3000];
        return;
    }

    if(address < 0x6000) // Is it a RAM bank switch?
    {
        if((*gbRom)->RTC && data>8)
        {
            RTCIO = 1;
            rtc.cur_register = data;

            return;
        } else RTCIO = 0;

        if((*gbRom)->RAMsize <= 2) // no need to change it if there isn't over 8KB ram
            return;

        data &= 0x03;

        if(data > maxRAMbank[(*gbRom)->RAMsize])
            data = maxRAMbank[(*gbRom)->RAMsize];

        ram_bank = data;

        int madr = data<<13;
        gbMemMap[0xA] = &(*gbCartRam)[madr];
        gbMemMap[0xB] = &(*gbCartRam)[madr+0x1000];
        return;
    }

    if(address < 0x8000)
    {
        if(data == 1)
        {
            rtcUpdate();
            RTC_latched = !RTC_latched;
            rtc_latch = rtc;
        }
        return;
    }

    if(address >= 0xA000 && address < 0xC000)
    {
        if(RAMenable && (*gbRom)->RTC && RTCIO)
        {
            time(&(rtc).last_time);
            switch(rtc.cur_register)
            {
                case 0x08:
                    rtc.s = data;
                    break;
                case 0x09:
                    rtc.m = data;
                    break;
                case 0x0A:
                    rtc.h = data;
                    break;
                case 0x0B:
                    rtc.d = data;
                    break;
                case 0x0C:
                    rtc.control = data;
                    rtc.d |= (data&1)<<8;
                    break;
            }
        }
        //if(!RAMenable || !rom->RAMsize)
        //   return;
    }

    gbMemMap[address>>12][address&0x0FFF] = data;
}

void MbcNin3::rtcUpdate()
{
    if(rtc.control&0x40)
    {
        rtc.last_time = time(0);
        return;
    }

    time_t now = time(0);
    time_t diff = now-rtc.last_time;
    if(diff > 0)
    {
        rtc.s += diff % 60;
        if(rtc.s > 59)
        {
            rtc.s -= 60;
            rtc.m++;
        }

        diff /= 60;

        rtc.m += diff % 60;
        if(rtc.m > 59)
        {
            rtc.m -= 60;
            rtc.h++;
        }

        diff /= 60;

        rtc.h += diff % 24;
        if(rtc.h > 24)
        {
            rtc.h -= 24;
            rtc.d++;
        }
        diff /= 24;

        rtc.d += diff;
        if(rtc.d > 255)
        {
            if(rtc.d > 511)
            {
                rtc.d %= 512;
                rtc.control |= 0x80;
            }
            rtc.control = (rtc.control & 0xfe) | (rtc.d>255 ? 1 : 0);
        }
    }
    rtc.last_time = now;
}
