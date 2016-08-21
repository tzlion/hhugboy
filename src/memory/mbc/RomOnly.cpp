//
// Created by Alex on 21/08/2016.
//

#include "RomOnly.h"
byte RomOnly::readMemory(register unsigned short address) {

    /* if(address >= 0xA000 && address < 0xC000)
     {
        if((!rom->RAMsize && !rom->battery))
           return 0xFF;
     }*/

    //  wchar_t wrmessage[50];
    //  wsprintf(wrmessage,L"MM %X %X",superaddroffset,mem_map[0x0]);
    //  renderer.showMessage(wrmessage,60,GB1);

    /* mem_map[0x0] = &cartridge[superaddroffset];
     mem_map[0x1] = &cartridge[superaddroffset+0x1000];
     mem_map[0x2] = &cartridge[superaddroffset+0x2000];
     mem_map[0x3] = &cartridge[superaddroffset+0x3000];

     mem_map[0x4] = &cartridge[superaddroffset+0x4000];
     mem_map[0x5] = &cartridge[superaddroffset+0x5000];
     mem_map[0x6] = &cartridge[superaddroffset+0x6000];
     mem_map[0x7] = &cartridge[superaddroffset+0x7000];    */

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
