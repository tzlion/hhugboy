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

#include "MbcUnlRockman8.h"

void MbcUnlRockman8::writeMemory(unsigned short address, register byte data) {
    if(address < 0x2000)
        return;

    if(address < 0x4000) // Is it a ROM bank switch?
    {
        data &= 0x1F;

        if(data == 0)
            data = 1;

        if(data > maxROMbank[(*gbCartridge)->ROMsize])
            data -= 8; // <--- MAKE IT WORK!!!

        rom_bank = data;

        cart_address = data<<14;

        cart_address &= rom_size_mask[(*gbCartridge)->ROMsize];

        gbMemMap[0x4] = &(*gbCartRom)[cart_address];
        gbMemMap[0x5] = &(*gbCartRom)[cart_address+0x1000];
        gbMemMap[0x6] = &(*gbCartRom)[cart_address+0x2000];
        gbMemMap[0x7] = &(*gbCartRom)[cart_address+0x3000];

        return;
    }

    if(address < 0x8000)
        return;

    // Always allow RAM writes.

    gbMemMap[address>>12][address&0x0FFF] = data;
}
