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

#include "RomOnly.h"
byte RomOnly::readMemory(register unsigned short address) {

    /* if(address >= 0xA000 && address < 0xC000)
     {
        if((!rom->RAMsize && !rom->battery))
           return 0xFF;
     }*/

    //  wchar_t wrmessage[50];
    //  wsprintf(wrmessage,L"MM %X %X",multicartOffset,mem_map[0x0]);
    //  renderer.showMessage(wrmessage,60,GB1);

    /* mem_map[0x0] = &cartridge[multicartOffset];
     mem_map[0x1] = &cartridge[multicartOffset+0x1000];
     mem_map[0x2] = &cartridge[multicartOffset+0x2000];
     mem_map[0x3] = &cartridge[multicartOffset+0x3000];

     mem_map[0x4] = &cartridge[multicartOffset+0x4000];
     mem_map[0x5] = &cartridge[multicartOffset+0x5000];
     mem_map[0x6] = &cartridge[multicartOffset+0x6000];
     mem_map[0x7] = &cartridge[multicartOffset+0x7000];    */

    return gbMemMap[address>>12][address&0x0FFF];
}

void RomOnly::writeMemory(unsigned short address, register byte data) {
    if(address < 0x2000)// Is it a RAM bank enable/disable? (is this even possible for ROM only?)
    {
        RAMenable = ( (data&0x0A) == 0x0A ? 1 : 0);
        return;
    }

    if(address < 0x4000) // Is it a ROM bank switch? // bye
    {
        return;
    }

    if(address < 0x6000) // Is it a RAM bank switch? // is THIS even possible for ROM only? Cmon now
    {
        data &= 0x03;

        ram_bank = data;

        int madr = data<<13;
        gbMemMap[0xA] = &(*gbCartRam)[madr];
        gbMemMap[0xB] = &(*gbCartRam)[madr+0x1000];
        return;
    }

    gbMemMap[address>>12][address&0x0FFF] = data;
}
