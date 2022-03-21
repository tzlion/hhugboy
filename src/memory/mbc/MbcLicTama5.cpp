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
#include "MbcLicTama5.h"

byte MbcLicTama5::readMemory(register unsigned short address) {

    if(address >= 0xA000 && address < 0xC000)
    {
        if(address == 0xA000)
        {
            switch(tama_flag)
            {
                case 0x0A: // test register ?
                    return 1;
                case 0x0D: // RTC registers
                {
                    byte read = 0;
                    if(tama_val4 == 1)
                    {
                        if(tama_val6 == 0x04)
                        {
                            if(tama_val7 == 0x06) // get minutes higher nibble
                            {
                                read = rtc.m;
                                if(read > 0x0f)
                                    read += 6;
                                if(read > 0x1f)
                                    read += 6;
                                if(read > 0x2f)
                                    read += 6;
                                if(read > 0x3f)
                                    read += 6;
                                if(read > 0x4f)
                                    read += 6;
                                if(read == 0x5A)
                                    read = 0;
                                read = (read&0xf0)>>4;
                            }
                            else
                            if(tama_val7 == 0x07) // get hours higher nibble
                            {
                                read = rtc.h;
                                if(read > 0x0f)
                                    read += 6;
                                if(read > 0x1f)
                                    read += 6;
                                if(read > 0x2f)
                                    read += 6;
                                if(read > 0x3f)
                                    read += 6;

                                read = (read&0xf0)>>4;
                            }
                        }
                    } else
                    { // read memory ?
                        read = (*gbCartRam)[(tama_val6<<4)|tama_val7];
                    }
                    return read;
                }
                case 0x0C: // RTC registers
                {
                    updateTamaRtc();
                    byte read = 0;
                    switch(tama_val4)
                    {
                        case 0:
                            if(rtc.s == 0)
                                read = 0;
                            else
                                read = rtc.s - (tama_time+1); // return 0 if second has passed
                            tama_time = rtc.s;
                            break;
                        case 1:
                            if(tama_val6 == 0x04)
                            {
                                if(tama_val7 == 0x06) // get minutes lower nibble
                                {
                                    read = rtc.m;

                                    //change into correct format
                                    if(read > 0x0f)
                                        read += 6;
                                    if(read > 0x1f)
                                        read += 6;
                                    if(read > 0x2f)
                                        read += 6;
                                    if(read > 0x3f)
                                        read += 6;
                                    if(read > 0x4f)
                                        read += 6;
                                    if(read == 0x5A)
                                        read = 0;

                                    read &= 0x0f;
                                }
                                else
                                if(tama_val7 == 0x07) // get hours lower nibble
                                {
                                    read = rtc.h;

                                    //change into correct format
                                    if(read > 0x0f)
                                        read += 6;
                                    if(read > 0x1f)
                                        read += 6;
                                    if(read > 0x2f)
                                        read += 6;
                                    if(read > 0x3f)
                                        read += 6;

                                    read &= 0x0f;
                                }
                            }
                            break;
                        case 7: // day low
                            read = rtc.d&0xF;
                            break;
                        case 8: // day high
                            read = ((rtc.d&0xf0)>>4);
                            break;
                        case 9: // month
                            read = tama_month;
                            break;
                        case 0xA: // ? (year maybe?)
                            read = 0;
                            break;
                        case 0xF: // ? (year maybe?)
                            read = 0;
                            break;
                    }
                    return read;
                }
            }
        }
    }

    return BasicMbc::readMemory(address);
}

void MbcLicTama5::updateTamaRtc() {
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
        if(rtc.d > 31)
        {
            rtc.d -= 31;
            tama_month++;
            if(tama_month > 12)
                tama_month -= 12;
        }
    }
    rtc.last_time = now;
}

void MbcLicTama5::readMbcSpecificVarsFromStateFile(FILE *statefile) {
    fread(&(tama_flag), sizeof(int), 1, statefile);

    fread(&(rtc).s, sizeof(int), 1, statefile);
    fread(&(rtc).m, sizeof(int), 1, statefile);
    fread(&(rtc).h, sizeof(int), 1, statefile);
    fread(&(rtc).d, sizeof(int), 1, statefile);
    fread(&(rtc).control, sizeof(int), 1, statefile);
    fread(&(rtc).last_time, sizeof(time_t), 1, statefile);

    fread(&(tama_time), sizeof(byte), 1, statefile);
    fread(&(tama_val6), sizeof(int), 1, statefile);
    fread(&(tama_val7), sizeof(int), 1, statefile);
    fread(&(tama_val4), sizeof(int), 1, statefile);
    fread(&(tama_val5), sizeof(int), 1, statefile);
    fread(&(tama_count), sizeof(int), 1, statefile);
    fread(&(tama_month), sizeof(int), 1, statefile);
    fread(&(tama_change_clock), sizeof(int), 1, statefile);
}

void MbcLicTama5::writeMbcSpecificVarsToStateFile(FILE *statefile) {
    fwrite(&(tama_flag), sizeof(int), 1, statefile);

    fwrite(&(rtc).s, sizeof(int), 1, statefile);
    fwrite(&(rtc).m, sizeof(int), 1, statefile);
    fwrite(&(rtc).h, sizeof(int), 1, statefile);
    fwrite(&(rtc).d, sizeof(int), 1, statefile);
    fwrite(&(rtc).control, sizeof(int), 1, statefile);
    fwrite(&(rtc).last_time, sizeof(time_t), 1, statefile);

    fwrite(&(tama_time), sizeof(byte), 1, statefile);
    fwrite(&(tama_val6), sizeof(int), 1, statefile);
    fwrite(&(tama_val7), sizeof(int), 1, statefile);
    fwrite(&(tama_val4), sizeof(int), 1, statefile);
    fwrite(&(tama_val5), sizeof(int), 1, statefile);
    fwrite(&(tama_count), sizeof(int), 1, statefile);
    fwrite(&(tama_month), sizeof(int), 1, statefile);
    fwrite(&(tama_change_clock), sizeof(int), 1, statefile);
}

void MbcLicTama5::writeMemory(unsigned short address, register byte data) {
    if(address < 0x8000) // ?
    {
        return;
    }

    if(address >= 0xA000 && address < 0xC000)
    {
        if(address == 0xA000)
        {
            switch(tama_flag)
            {
                case 0: // rom bank lower nibble
                {
                    data &= 0x0F;

                    MBClo = data;
                    rom_bank = MBClo|(MBChi<<4);

                    int cadr = rom_bank<<14;
                    gbMemMap[0x4] = &(*gbCartRom)[cadr];
                    gbMemMap[0x5] = &(*gbCartRom)[cadr+0x1000];
                    gbMemMap[0x6] = &(*gbCartRom)[cadr+0x2000];
                    gbMemMap[0x7] = &(*gbCartRom)[cadr+0x3000];
                }
                    return;
                case 1: // rom bank high bit
                {
                    data &= 0x01;

                    MBChi = data;

                    rom_bank = MBClo|(MBChi<<4);

                    int cadr = rom_bank<<14;
                    gbMemMap[0x4] = &(*gbCartRom)[cadr];
                    gbMemMap[0x5] = &(*gbCartRom)[cadr+0x1000];
                    gbMemMap[0x6] = &(*gbCartRom)[cadr+0x2000];
                    gbMemMap[0x7] = &(*gbCartRom)[cadr+0x3000];
                }
                    return;
                case 4: // RTC controls
                    tama_val4 = data;
                    return;
                case 5: // write time (and write memory????)
                    tama_val5 = data;
                    ++tama_count;
                    if(tama_count==1 && data == 0) tama_change_clock |= 2;
                    if(tama_count==2 && data == 1) tama_change_clock |= 1;
                    if(tama_change_clock == 3) rtc.last_time = time(0);

                    (*gbCartRam)[(tama_val6<<4)+tama_val7] = tama_val4|(data<<4);

                    //which time counter is changed?
                    if(tama_count==6 && tama_change_clock==3)
                    {
                        tama_month = data;
                    } else
                    if(tama_count==7 && tama_change_clock==3)
                    {
                        tama_month += data*10;
                    } else
                    if(tama_count==8 && tama_change_clock==3)
                    {
                        rtc.d = data;
                    } else
                    if(tama_count==9 && tama_change_clock==3)
                    {
                        rtc.d += data*10;
                    } else
                    if(tama_count==10 && tama_change_clock==3)
                    {
                        rtc.m = data*10;
                    } else
                    if(tama_count==11 && tama_change_clock==3)
                    {
                        rtc.h = data*10;
                    } else
                    if(tama_change_clock==3 && tama_count >= 13)
                    {
                        tama_count = 0;
                        tama_change_clock = 0;
                    } else
                    if(tama_change_clock!=3 && tama_count >= 2)
                    {
                        tama_count=0;
                        tama_change_clock = 0;
                    }
                    return;
                case 6: // RTC controls
                    tama_val6 = data;
                    return;
                case 7: // RTC controls
                    tama_val7 = data;
                    return;
            }
        } else
        if(address == 0xA001)
        {
            tama_flag = data;

            return;
        }
    }

    gbMemMap[address>>12][address&0x0FFF] = data;
}

MbcLicTama5::MbcLicTama5():
        tama_flag(0),
        tama_time(0),
        tama_val4(0),
        tama_val5(0),
        tama_val6(0),
        tama_val7(0),
        tama_count(0),
        tama_month(0),
        tama_change_clock(0)
{}

void MbcLicTama5::resetVars(bool preserveMulticartState) {
    tama_flag = 0;
    tama_time = 0;
    tama_val6 = 0;
    tama_val7 = 0;
    tama_val4 = 0;
    tama_val5 = 0;
    tama_count = 0;
    tama_month = 0;
    tama_change_clock = 0;
    AbstractMbc::resetVars(preserveMulticartState);
}

void MbcLicTama5::readMbcSpecificVarsFromSaveFile(FILE *savefile) {
    AbstractMbc::readMbcSpecificVarsFromSaveFile(savefile); // gotta call this first for now because it does some RTC stuff
    fread(&(tama_month), sizeof(int), 1, savefile);
}

void MbcLicTama5::writeMbcSpecificVarsToSaveFile(FILE *savefile) {
    AbstractMbc::writeMbcSpecificVarsToSaveFile(savefile); // gotta call this first for now because it does some RTC stuff
    fwrite(&(tama_month), sizeof(int), 1, savefile);
}
