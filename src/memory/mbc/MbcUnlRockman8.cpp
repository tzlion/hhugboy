//
// Created by Alex on 21/08/2016.
//

#include "MbcUnlRockman8.h"

void MbcUnlRockman8::writeMemory(unsigned short address, register byte data) {
    if(address < 0x2000)
        return;

    if(address < 0x4000) // Is it a ROM bank switch?
    {
        data &= 0x1F;

        if(data == 0)
            data = 1;

        if(data > maxROMbank[(*gbRom)->ROMsize])
            data -= 8; // <--- MAKE IT WORK!!!

        rom_bank = data;

        cart_address = data<<14;

        cart_address &= rom_size_mask[(*gbRom)->ROMsize];

        gbMemMap[0x4] = &(*gbCartridge)[cart_address];
        gbMemMap[0x5] = &(*gbCartridge)[cart_address+0x1000];
        gbMemMap[0x6] = &(*gbCartridge)[cart_address+0x2000];
        gbMemMap[0x7] = &(*gbCartridge)[cart_address+0x3000];

        return;
    }

    if(address < 0x8000)
        return;

    // Always allow RAM writes.

    gbMemMap[address>>12][address&0x0FFF] = data;
}
