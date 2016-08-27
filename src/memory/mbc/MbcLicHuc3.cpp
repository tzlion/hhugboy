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
#include "MbcLicHuc3.h"

byte MbcLicHuc3::readMemory(register unsigned short address) {
    if(address >= 0xA000 && address < 0xC000)
    {
        if(HuC3_RAMflag >= 0x0b && HuC3_RAMflag < 0x0e)
        {
            if(HuC3_RAMflag == 0x0D)
                return 1;
            return HuC3_RAMvalue;
        }
        if(!(*gbRom)->RAMsize)
            return 0xFF;
    }

    return BasicMbc::readMemory(address);
}

void MbcLicHuc3::readMbcSpecificVarsFromStateFile(FILE *statefile) {
    fread(&(HuC3_time), sizeof(unsigned int), 1, statefile);
    fread(&(HuC3_last_time), sizeof(time_t), 1, statefile);
    fread(&(rtc).s, sizeof(int), 1, statefile);

    //fread(HuC3_register,sizeof(int),8,statefile);
    fread(&(HuC3_RAMvalue), sizeof(int), 1, statefile);
    //fread(&HuC3_address,sizeof(int),1,statefile);
    fread(&(HuC3_RAMflag), sizeof(int), 1, statefile);
}

void MbcLicHuc3::writeMbcSpecificVarsToStateFile(FILE *statefile) {
    fwrite(&(HuC3_time), sizeof(unsigned int), 1, statefile);
    fwrite(&(HuC3_last_time), sizeof(time_t), 1, statefile);
    fwrite(&(rtc).s, sizeof(int), 1, statefile);

    //fwrite(HuC3_register,sizeof(int),8,statefile);
    fwrite(&(HuC3_RAMvalue), sizeof(int), 1, statefile);
    //fwrite(&HuC3_address,sizeof(int),1,statefile);
    fwrite(&(HuC3_RAMflag), sizeof(int), 1, statefile);
}

void MbcLicHuc3::writeMemory(unsigned short address, register byte data) {
    if(address < 0x2000)// Is it a RAM bank enable/disable?
    {
        RAMenable = ( (data&0x0A) == 0x0A ? 1 : 0);
        HuC3_RAMflag = data;
        return;
    }

    if(address < 0x4000) // Is it a ROM bank switch?
    {
        if(data == 0)
            data = 1;
        if(data > maxROMbank[(*gbRom)->ROMsize])
            data = maxROMbank[(*gbRom)->ROMsize];

        rom_bank = data;

        int cadr = data<<14;
        gbMemMap[0x4] = &(*gbCartridge)[cadr];
        gbMemMap[0x5] = &(*gbCartridge)[cadr+0x1000];
        gbMemMap[0x6] = &(*gbCartridge)[cadr+0x2000];
        gbMemMap[0x7] = &(*gbCartridge)[cadr+0x3000];
        return;
    }

    if(address < 0x6000) // Is it a RAM bank switch?
    {
        data &= 0x0F;

        if(data > maxRAMbank[(*gbRom)->RAMsize])
            data = maxRAMbank[(*gbRom)->RAMsize];

        ram_bank = data;

        int madr = data<<13;
        gbMemMap[0xA] = &(*gbCartRam)[madr];
        gbMemMap[0xB] = &(*gbCartRam)[madr+0x1000];
        return;
    }

    if(address < 0x8000) // programs will write 1 here
        return;

    if(address >= 0xA000 && address < 0xC000)
    {
        if(HuC3_RAMflag < 0x0b || HuC3_RAMflag > 0x0e) // write to RAM
        {
            if(!RAMenable || !(*gbRom)->RAMsize)
                return;
        } else
        {
            if(HuC3_RAMflag == 0x0B) // send command ?
            {
                switch(data & 0xf0)
                {
                    case 0x10: // read time
                        updateHuc3Time();
                        if(HuC3_flag == HUC3_READ)
                        {
                            HuC3_RAMvalue = ((HuC3_time>>HuC3_shift)&0x0F);
                            HuC3_shift += 4;
                            if(HuC3_shift > 24)
                                HuC3_shift = 0;
                        }
                        break;
                    case 0x30: // write to registers (minute,day and year(?) counters)
                        // to write time 23:59 program will send commands
                        // 3F 39 35 30 30 30 31
                        // HuC3_time will then be 59F = 1439 = 23*60+59 minutes
                        if(HuC3_flag == HUC3_WRITE)
                        {
                            if(HuC3_shift == 0)
                                HuC3_time = 0;
                            if(HuC3_shift < 24)
                            {
                                HuC3_time |= ((data&0x0F)<<HuC3_shift);
                                HuC3_shift += 4;
                                if(HuC3_shift == 24)
                                    HuC3_flag = HUC3_READ;
                            }
                        }
                        break;
                    case 0x40: // special command ?
                        switch(data&0x0F)
                        {
                            case 0x00: //  ?
                                //HuC3_flag = HUC3_READ;
                                HuC3_shift = 0;
                                break;
                            case 0x03: // write time mode ?
                                HuC3_flag = HUC3_WRITE;
                                HuC3_shift = 0;
                                break;
                            case 0x07: // read time mode ?
                                HuC3_flag = HUC3_READ;
                                HuC3_shift = 0;
                                break;
                            case 0x06: // alarm clock sound test
                                break;
                            case 0x08: // set alarm clock time ?
                                //HuC3_flag = HUC3_NONE;
                                break;
                            case 0x0F: // yobidashi (call) ?
                                break;
                            default:
                                //char buffer[100];
                                //sprintf(buffer,"HuC3-command:%x",data);
                                //debug_print(buffer);
                                break;
                        }
                        break;
                    case 0x50: // ?
                    {
                        //HuC3_register[0] = (HuC3_register[0] & 0x0f) | ((data << 4)&0x0f);
                    }
                        break;
                    case 0x60: // ?
                    {
                        //HuC3_RAMvalue = 1;
                        HuC3_flag = HUC3_READ;
                    }
                        break;
                }
            } else if(HuC3_RAMflag == 0x0C) // not used ?
            {
                // ?
            } else if(HuC3_RAMflag == 0x0D) // programs will write 0xFE here
            {
                // maybe a execute command function ?
            }
        }
    }

    gbMemMap[address>>12][address&0x0FFF] = data;
}

void MbcLicHuc3::updateHuc3Time()
{
    time_t now = time(0);
    time_t diff = now-HuC3_last_time;
    if(diff > 0)
    {
        rtc.s += diff % 60; // use rtc.s to store seconds
        if(rtc.s > 59)
        {
            rtc.s -= 60;
            HuC3_time++;
        }

        diff /= 60;

        HuC3_time += diff % 60;
        if((HuC3_time&0xFFF) > 1439)
        {
            HuC3_time = (HuC3_time&0xFFFF000)|((HuC3_time&0xFFF)-1440);
            HuC3_time += 0x1000; // day counter ?
        }

        diff /= 60;

        HuC3_time += (diff % 24)*60;
        if((HuC3_time&0xFFF) > 1439)
        {
            HuC3_time = (HuC3_time&0xFFFF000)|((HuC3_time&0xFFF)-1440);
            HuC3_time += 0x1000; // day counter ?
        }

        diff /= 24;

        HuC3_time += (diff<<12);
        if(((HuC3_time&0xFFF000)>>12) > 356)
        {
            HuC3_time = HuC3_time&0xF000FFF;
            HuC3_time += 0x1000000; // year counter ????
        }
    }
    HuC3_last_time = now;
}

MbcLicHuc3::MbcLicHuc3():
        HuC3_RAMvalue(0),
        HuC3_RAMaddress(0),
        HuC3_address(0),
        HuC3_RAMflag(0),
        HuC3_last_time(time(0)),
        HuC3_flag(HUC3_NONE),
        HuC3_time(0),
        HuC3_shift(0)
{}

void MbcLicHuc3::resetVars(bool preserveMulticartState) {
    HuC3_flag = HUC3_NONE;
    HuC3_RAMvalue = 1;
    AbstractMbc::resetVars(preserveMulticartState);
}

void MbcLicHuc3::readMbcSpecificVarsFromSaveFile(FILE *savefile) {
    fread(&(HuC3_time), sizeof(unsigned int), 1, savefile);
    fread(&(HuC3_last_time), sizeof(time_t), 1, savefile);
    fread(&(rtc).s, sizeof(int), 1, savefile);
}

void MbcLicHuc3::writeMbcSpecificVarsToSaveFile(FILE *savefile) {
    fwrite(&(HuC3_time), sizeof(unsigned int), 1, savefile);
    fwrite(&(HuC3_last_time), sizeof(time_t), 1, savefile);
    fwrite(&(rtc).s, sizeof(int), 1, savefile);
}

