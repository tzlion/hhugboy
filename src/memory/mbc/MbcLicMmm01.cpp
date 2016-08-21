//
// Created by Alex on 21/08/2016.
//

#include "MbcLicMmm01.h"

void MbcLicMmm01::writeMemory(unsigned short address, register byte data) {
    if(address < 0x2000)
        return;

    if(address < 0x4000) // Is it a ROM bank switch?
    {
        if(data == 0)
            data = 1;

        rom_bank = data;

        int cadr = (rom_bank<<14)+(MBChi<<14);

        cadr &= rom_size_mask[(*gbRom)->ROMsize];

        gbMemMap[0x4] = &(*gbCartridge)[cadr];
        gbMemMap[0x5] = &(*gbCartridge)[cadr+0x1000];
        gbMemMap[0x6] = &(*gbCartridge)[cadr+0x2000];
        gbMemMap[0x7] = &(*gbCartridge)[cadr+0x3000];
        return;
    }

    if(address < 0x6000) // Is it a RAM bank switch?
    {
        if(address == 0x5fff && !bc_select)
        {
            bc_select = 1;

            data &= 0x2f;
            MBClo = 0;

            MBChi = data;
            cart_address = MBChi<<14;

            gbMemMap[0x0] = &(*gbCartridge)[cart_address];
            gbMemMap[0x1] = &(*gbCartridge)[cart_address+0x1000];
            gbMemMap[0x2] = &(*gbCartridge)[cart_address+0x2000];
            gbMemMap[0x3] = &(*gbCartridge)[cart_address+0x3000];

            gbMemMap[0x4] = &(*gbCartridge)[cart_address+0x4000];
            gbMemMap[0x5] = &(*gbCartridge)[cart_address+0x5000];
            gbMemMap[0x6] = &(*gbCartridge)[cart_address+0x6000];
            gbMemMap[0x7] = &(*gbCartridge)[cart_address+0x7000];
            return;
        }

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
    }

    if(address < 0x8000)
        return;

    // Always allow RAM writes.

    gbMemMap[address>>12][address&0x0FFF] = data;
}
