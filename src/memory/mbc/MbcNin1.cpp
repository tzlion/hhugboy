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

#include "MbcNin1.h"

void MbcNin1::writeMemory(unsigned short address, register byte data) {
    if(address < 0x2000)// Is it a RAM bank enable/disable?
    {
        RAMenable = ( (data&0x0A) == 0x0A ? 1 : 0);
        return;
    }

    if(address < 0x4000) // Is it a ROM bank switch?
    {
        if(MBC1memorymodel == 0)
        {
            if(data == 0)
                data = 1;

            MBClo = data;

            rom_bank = MBClo|(MBChi<<5);

            cart_address = MBClo<<14;
            cart_address |= (MBChi<<19);

            cart_address &= rom_size_mask[(*gbCartridge)->ROMsize];

            gbMemMap[0x4] = &(*gbCartRom)[cart_address];
            gbMemMap[0x5] = &(*gbCartRom)[cart_address+0x1000];
            gbMemMap[0x6] = &(*gbCartRom)[cart_address+0x2000];
            gbMemMap[0x7] = &(*gbCartRom)[cart_address+0x3000];
        } else
        {
            if(data == 0)
                data = 1;

            rom_bank = data;

            int cadr = rom_bank<<14;

            cadr &= rom_size_mask[(*gbCartridge)->ROMsize];

            gbMemMap[0x4] = &(*gbCartRom)[cadr];
            gbMemMap[0x5] = &(*gbCartRom)[cadr+0x1000];
            gbMemMap[0x6] = &(*gbCartRom)[cadr+0x2000];
            gbMemMap[0x7] = &(*gbCartRom)[cadr+0x3000];
        }
        return;
    }

    if(address < 0x6000) // Is it a RAM bank switch?
    {
        if(MBC1memorymodel == 0)
        {
            if((((data&0x03)<<5)|MBClo) > maxROMbank[(*gbCartridge)->ROMsize])
                return;

            MBChi = (data&0x03);

            rom_bank = MBClo|(MBChi<<5);

            cart_address = MBClo<<14;
            cart_address |= (MBChi<<19);

            cart_address &= rom_size_mask[(*gbCartridge)->ROMsize];

            gbMemMap[0x4] = &(*gbCartRom)[cart_address];
            gbMemMap[0x5] = &(*gbCartRom)[cart_address+0x1000];
            gbMemMap[0x6] = &(*gbCartRom)[cart_address+0x2000];
            gbMemMap[0x7] = &(*gbCartRom)[cart_address+0x3000];
            return;
        }

        if((*gbCartridge)->RAMsize <= 2) // no need to change it if there isn't over 8KB ram
            return;

        data &= 0x03;

        if(data > maxRAMbank[(*gbCartridge)->RAMsize])
            data &= maxRAMbank[(*gbCartridge)->RAMsize];

        ram_bank = data;

        int madr = data<<13;
        gbMemMap[0xA] = &(*gbCartRam)[madr];
        gbMemMap[0xB] = &(*gbCartRam)[madr+0x1000];
        return;
    }

    if(address < 0x8000) // Is it a MBC1 max memory model change?
    {
        MBC1memorymodel = (data&0x01);
        return;
    }

/*   if(address >= 0xA000 && address < 0xC000)
   {
      if((!RAMenable || !rom->RAMsize) && !RAM_always_enable)
         return;
   }*/

    gbMemMap[address>>12][address&0x0FFF] = data;
}
