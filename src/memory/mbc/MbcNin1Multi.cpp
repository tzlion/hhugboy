/*
   hhugboy Game Boy emulator
   copyright 2013-2018 taizou
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

#include "MbcNin1Multi.h"

void MbcNin1Multi::updateMemoryMap() {

    rom_bank = MBClo|(MBChi<<4);

    int bank0addr = (MBChi<<4)<<14;

    gbMemMap[0x0] = &(*gbCartRom)[bank0addr];
    gbMemMap[0x1] = &(*gbCartRom)[bank0addr+0x1000];
    gbMemMap[0x2] = &(*gbCartRom)[bank0addr+0x2000];
    gbMemMap[0x3] = &(*gbCartRom)[bank0addr+0x3000];

    int cadr = rom_bank<<14;
    cadr &= rom_size_mask[(*gbCartridge)->ROMsize];

    gbMemMap[0x4] = &(*gbCartRom)[cadr];
    gbMemMap[0x5] = &(*gbCartRom)[cadr+0x1000];
    gbMemMap[0x6] = &(*gbCartRom)[cadr+0x2000];
    gbMemMap[0x7] = &(*gbCartRom)[cadr+0x3000];
}

void MbcNin1Multi::writeMemory(unsigned short address, register byte data) {
    if(address < 0x2000)// Is it a RAM bank enable/disable?
    {
        RAMenable = ( (data&0x0A) == 0x0A ? 1 : 0);
        return;
    }

    if(address < 0x4000) // Is it a ROM bank switch?
    {
        MBClo = (data & 0x0F) ?: 1;
        updateMemoryMap();
        return;
    }

    if(address < 0x6000)
    {
        MBChi = (data&0x03);
        updateMemoryMap();
        return;
    }

    if(address < 0x8000)
    {
        MBC1memorymodel = (data&0x01);
        return;
    }

    /* if(address >= 0xA000 && address < 0xC000)
     {
        if((!RAMenable || !rom->RAMsize) && !RAM_always_enable)
           return;
     }*/

    gbMemMap[address>>12][address&0x0FFF] = data;
}
