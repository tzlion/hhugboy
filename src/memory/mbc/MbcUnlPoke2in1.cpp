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

#include "MbcUnlPoke2in1.h"

int bank0_change = 0; // hmmm

void MbcUnlPoke2in1::writeMemory(unsigned short address, register byte data) {
    if(address < 0x2000)// Is it a RAM bank enable/disable?
    {
        RAMenable = ( (data&0x0A) == 0x0A ? 1 : 0);
        bank0_change = ( (data&0xC0) == 0xC0 ? 1 : 0);
        return;
    }

    if(address < 0x4000) // Is it a ROM bank switch?
    {
        data &= 0x7F;
        if(data==0)
            data=1;
        data += MBChi;

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
        return;
    }

    if(address >= 0xA000 && address < 0xC000)
    {
        if(bank0_change && address==0xA100 && !bc_select)
        {
            MBClo = 0;
            if(data==1)
                MBChi = 2;
            else
            if(data!=0xc0)
                MBChi = 66;
            else
                bc_select = 1;

            cart_address = MBChi<<14;

            gbMemMap[0x0] = &(*gbCartRom)[cart_address];
            gbMemMap[0x1] = &(*gbCartRom)[cart_address+0x1000];
            gbMemMap[0x2] = &(*gbCartRom)[cart_address+0x2000];
            gbMemMap[0x3] = &(*gbCartRom)[cart_address+0x3000];

            gbMemMap[0x4] = &(*gbCartRom)[cart_address+0x4000];
            gbMemMap[0x5] = &(*gbCartRom)[cart_address+0x5000];
            gbMemMap[0x6] = &(*gbCartRom)[cart_address+0x6000];
            gbMemMap[0x7] = &(*gbCartRom)[cart_address+0x7000];
            return;
        }
        //if(!RAMenable || !rom->RAMsize)
        //   return;
    }

    gbMemMap[address>>12][address&0x0FFF] = data;
}
