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
#include "MbcNin7.h"
#include "../../mainloop.h" // Todo: can we not pls. The sensor values actually belong to the cart!

byte MbcNin7::readMemory(register unsigned short address) {

    if(address >= 0xA000 && address < 0xC000)
    {
        switch(address & 0xa0f0)
        {
            case 0xa000:
            case 0xa010:
            case 0xa060:
            case 0xa070:
                return 0;
            case 0xa020:
                // sensor X low byte
                return sensorX & 255;
            case 0xa030:
                // sensor X high byte
                return sensorX >> 8;
            case 0xa040:
                // sensor Y low byte
                return sensorY & 255;
            case 0xa050:
                // sensor Y high byte
                return sensorY >> 8;
            case 0xa080:
                return MBC7_value;
        }
        return 0xff;
    }

    return BasicMbc::readMemory(address);
}

void MbcNin7::readMbcSpecificVarsFromStateFile(FILE *statefile) {
    fread(&(MBC7_cs), sizeof(int), 1, statefile);
    fread(&(MBC7_sk), sizeof(int), 1, statefile);
    fread(&(MBC7_state), sizeof(int), 1, statefile);
    fread(&(MBC7_buffer), sizeof(int), 1, statefile);
    fread(&(MBC7_idle), sizeof(int), 1, statefile);
    fread(&(MBC7_count), sizeof(int), 1, statefile);
    fread(&(MBC7_code), sizeof(int), 1, statefile);
    fread(&(MBC7_address), sizeof(int), 1, statefile);
    fread(&(MBC7_writeEnable), sizeof(int), 1, statefile);
    fread(&(MBC7_value), sizeof(int), 1, statefile);
    fread(&sensorX,sizeof(int),1,statefile);
    fread(&sensorY,sizeof(int),1,statefile);
}

void MbcNin7::writeMbcSpecificVarsToStateFile(FILE *statefile) {
    fwrite(&(MBC7_cs), sizeof(int), 1, statefile);
    fwrite(&(MBC7_sk), sizeof(int), 1, statefile);
    fwrite(&(MBC7_state), sizeof(int), 1, statefile);
    fwrite(&(MBC7_buffer), sizeof(int), 1, statefile);
    fwrite(&(MBC7_idle), sizeof(int), 1, statefile);
    fwrite(&(MBC7_count), sizeof(int), 1, statefile);
    fwrite(&(MBC7_code), sizeof(int), 1, statefile);
    fwrite(&(MBC7_address), sizeof(int), 1, statefile);
    fwrite(&(MBC7_writeEnable), sizeof(int), 1, statefile);
    fwrite(&(MBC7_value), sizeof(int), 1, statefile);
    fwrite(&sensorX,sizeof(int),1,statefile);
    fwrite(&sensorY,sizeof(int),1,statefile);
}

void MbcNin7::writeMemory(unsigned short address, register byte data) {
    if(address < 0x2000)
        return;

    if(address < 0x4000) // Is it a ROM bank switch?
    {
        data = data&0x7F;

        if(data==0)
            data=1;


        rom_bank = data;

        cart_address = data<<14;

        cart_address &= rom_size_mask[(*gbRom)->ROMsize];

        gbMemMap[0x4] = &(*gbCartridge)[cart_address];
        gbMemMap[0x5] = &(*gbCartridge)[cart_address+0x1000];
        gbMemMap[0x6] = &(*gbCartridge)[cart_address+0x2000];
        gbMemMap[0x7] = &(*gbCartridge)[cart_address+0x3000];
        return;
    }

    if(address < 0x6000) // Is it a RAM bank switch?
    {
        if(data<8)
        {
            RAMenable = 0;

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
        } else
        {
            RAMenable=0;
            return;
        }

    }

    if(address < 0x8000)
        return;

    if(address >= 0xA000 && address < 0xC000)
    {
        if(address == 0xa080)
        {
            // special processing needed
            int oldCs = MBC7_cs,oldSk=MBC7_sk;

            MBC7_cs=data>>7;
            MBC7_sk=(data>>6)&1;

            if(!oldCs && MBC7_cs)
            {
                if(MBC7_state == 5)
                {
                    if(MBC7_writeEnable)
                    {
                        (*gbMemory)[0xa000+MBC7_address*2] = MBC7_buffer>>8;
                        (*gbMemory)[0xa000+MBC7_address*2+1] = MBC7_buffer&0xff;
                    }
                    MBC7_state = 0;
                    MBC7_value = 1;
                } else
                {
                    MBC7_idle = true;
                    MBC7_state = 0;
                }
            }

            if(!oldSk && MBC7_sk)
            {
                if(MBC7_idle)
                {
                    if(data & 0x02)
                    {
                        MBC7_idle = false;
                        MBC7_count = 0;
                        MBC7_state = 1;
                    }
                } else
                {
                    switch(MBC7_state)
                    {
                        case 1:
                            // receiving command
                            MBC7_buffer <<= 1;
                            MBC7_buffer |= (data & 0x02)?1:0;
                            MBC7_count++;
                            if(MBC7_count == 2)
                            {
                                // finished receiving command
                                MBC7_state = 2;
                                MBC7_count = 0;
                                MBC7_code = MBC7_buffer & 3;
                            }
                            break;
                        case 2:
                            // receive address
                            MBC7_buffer <<= 1;
                            MBC7_buffer |= (data&0x02)?1:0;
                            MBC7_count++;
                            if(MBC7_count==8)
                            {
                                // finish receiving
                                MBC7_state = 3;
                                MBC7_count = 0;
                                MBC7_address = MBC7_buffer&0xff;
                                if(MBC7_code == 0)
                                {
                                    if((MBC7_address>>6) == 0)
                                    {
                                        MBC7_writeEnable = 0;
                                        MBC7_state = 0;
                                    } else if((MBC7_address>>6) == 3)
                                    {
                                        MBC7_writeEnable = 1;
                                        MBC7_state = 0;
                                    }
                                }
                            }
                            break;
                        case 3:
                            MBC7_buffer <<= 1;
                            MBC7_buffer |= (data&0x02)?1:0;
                            MBC7_count++;

                            switch(MBC7_code)
                            {
                                case 0:
                                    if(MBC7_count==16)
                                    {
                                        if((MBC7_address>>6)==0)
                                        {
                                            MBC7_writeEnable = 0;
                                            MBC7_state = 0;
                                        } else if((MBC7_address>>6) == 1)
                                        {
                                            if(MBC7_writeEnable)
                                            {
                                                for(int i=0;i<256;i++)
                                                {
                                                    (*gbMemory)[0xa000+i*2] = MBC7_buffer >> 8;
                                                    (*gbMemory)[0xa000+i*2+1] = MBC7_buffer & 0xff;
                                                }
                                            }
                                            MBC7_state = 5;
                                        } else if((MBC7_address>>6) == 2)
                                        {
                                            if(MBC7_writeEnable)
                                            {
                                                for(int i=0;i<256;i++)
                                                    *((unsigned short *)&((*gbMemory))[0xa000+i*2]) = 0xffff;
                                            }
                                            MBC7_state = 5;
                                        } else if((MBC7_address>>6) == 3)
                                        {
                                            MBC7_writeEnable = 1;
                                            MBC7_state = 0;
                                        }
                                        MBC7_count = 0;
                                    }
                                    break;
                                case 1:
                                    if(MBC7_count == 16)
                                    {
                                        MBC7_count = 0;
                                        MBC7_state = 5;
                                        MBC7_value = 0;
                                    }
                                    break;
                                case 2:
                                    if(MBC7_count == 1)
                                    {
                                        MBC7_state = 4;
                                        MBC7_count = 0;
                                        MBC7_buffer = ((*gbMemory)[0xa000+MBC7_address*2]<<8)|((*gbMemory)[0xa000+MBC7_address*2+1]);
                                    }
                                    break;
                                case 3:
                                    if(MBC7_count == 16)
                                    {
                                        MBC7_count = 0;
                                        MBC7_state = 5;
                                        MBC7_value = 0;
                                        MBC7_buffer = 0xffff;
                                    }
                                    break;
                            }
                            break;
                    }
                }
            }

            if(oldSk && !MBC7_sk)
            {
                if(MBC7_state == 4)
                {
                    MBC7_value = (MBC7_buffer & 0x8000)?1:0;
                    MBC7_buffer <<= 1;
                    MBC7_count++;
                    if(MBC7_count == 16)
                    {
                        MBC7_count = 0;
                        MBC7_state = 0;
                    }
                }
            }
        }
        return;
    }

    gbMemMap[address>>12][address&0x0FFF] = data;
}

MbcNin7::MbcNin7():
        MBC7_cs(0),
        MBC7_sk(0),
        MBC7_state(0),
        MBC7_buffer(0),
        MBC7_idle(0),
        MBC7_count(0),
        MBC7_code(0),
        MBC7_address(0),
        MBC7_writeEnable(0),
        MBC7_value(0)
{}
