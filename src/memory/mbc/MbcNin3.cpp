/*
   hhugboy Game Boy emulator
   copyright 2013-2016 taizou
   Based on GEST
   Copyright (C) 2003-2010 TM
   Incorporating code from VisualBoyAdvance
   Copyright (C) 1999-2004 by Forgotten

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <cstdio>
#include "MbcNin3.h"

byte MbcNin3::readMemory(register unsigned short address) {

    if(address >= 0xA000 && address < 0xC000)
    {
        if((*gbCartridge)->RTC && RTCIO)
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
    if((*gbCartridge)->RTC)
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
    if((*gbCartridge)->RTC)
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
        cadr &= rom_size_mask[(*gbCartridge)->ROMsize];
        gbMemMap[0x4] = &(*gbCartRom)[cadr];
        gbMemMap[0x5] = &(*gbCartRom)[cadr+0x1000];
        gbMemMap[0x6] = &(*gbCartRom)[cadr+0x2000];
        gbMemMap[0x7] = &(*gbCartRom)[cadr+0x3000];
        return;
    }

    if(address < 0x6000) // Is it a RAM bank switch?
    {
        if((*gbCartridge)->RTC && data>8)
        {
            RTCIO = 1;
            rtc.cur_register = data;

            return;
        } else RTCIO = 0;

        if((*gbCartridge)->RAMsize <= 2) // no need to change it if there isn't over 8KB ram
            return;

        data &= 0x03;

        if(data > maxRAMbank[(*gbCartridge)->RAMsize])
            data = maxRAMbank[(*gbCartridge)->RAMsize];

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
        if(RAMenable && (*gbCartridge)->RTC && RTCIO)
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
