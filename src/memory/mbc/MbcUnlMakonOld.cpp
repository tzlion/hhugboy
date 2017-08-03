#include "MbcUnlMakonOld.h"

void MbcUnlMakonOld::writeMemory(unsigned short address, register byte data) {

    // Maybe this should extend MBC1 idk

    if (address >= 0x2000 && address <= 0x3FFF) {

        if (data == 0) data = 1; // MBC1 stylez

        byte flippo[8] = {0,1,2,4,3,6,5,7};

        data = switchOrder(data,flippo);
        rom_bank = data;

        int bankAddress = rom_bank<<14;

        bankAddress &= rom_size_mask[(*gbRom)->ROMsize];

        gbMemMap[0x4] = &(*gbCartridge)[bankAddress];
        gbMemMap[0x5] = &(*gbCartridge)[bankAddress+0x1000];
        gbMemMap[0x6] = &(*gbCartridge)[bankAddress+0x2000];
        gbMemMap[0x7] = &(*gbCartridge)[bankAddress+0x3000];

        return;
    }

    if (address == 0x50ef) {
        if (data == 10) {
            // enable weird mode
            // something like that
            // maybe its gotta be 50ef
        }
        return;
    }

    if (address < 0x8000) {
        // ignore anything else in the cart area for now
        return;
    }

    gbMemMap[address>>12][address&0x0FFF] = data;
}
