#include <cstdio>
#include "MbcUnlMakonOld.h"
#include "../../debug.h"

void MbcUnlMakonOld::writeMemory(unsigned short address, register byte data) {

    // Maybe this should extend MBC1 idk

    if (address >= 0x2000 && address <= 0x3FFF) {

        if (data == 0) data = 1; // MBC1 stylez

        if (isWeirdMode) {
            byte flippo[8] = {0,1,2,4,3,6,5,7};

            data = switchOrder(data,flippo);
        }

        rom_bank = data;

        int bankAddress = rom_bank<<14;

        bankAddress &= rom_size_mask[(*gbRom)->ROMsize];

        gbMemMap[0x4] = &(*gbCartridge)[bankAddress];
        gbMemMap[0x5] = &(*gbCartridge)[bankAddress+0x1000];
        gbMemMap[0x6] = &(*gbCartridge)[bankAddress+0x2000];
        gbMemMap[0x7] = &(*gbCartridge)[bankAddress+0x3000];

        return;
    }

    if (address >= 0x5000 && address <= 0x5FFF) {
        if (data == 0x10 && address == 0x50EF) {
            isWeirdMode = true;
        } else {
            debug_print("Unknown 5xxx write");
        }
        return;
    }

    if (address < 0x8000) {
        // ignore anything else in the cart area for now
        return;
    }

    gbMemMap[address>>12][address&0x0FFF] = data;
}

MbcUnlMakonOld::MbcUnlMakonOld() :
        isWeirdMode(false) {
}

void MbcUnlMakonOld::resetVars(bool preserveMulticartState) {
    isWeirdMode = false;
    AbstractMbc::resetVars(preserveMulticartState);
}

void MbcUnlMakonOld::readMbcSpecificVarsFromStateFile(FILE *statefile) {
    fread(&(isWeirdMode), sizeof(bool), 1, statefile);
}

void MbcUnlMakonOld::writeMbcSpecificVarsToStateFile(FILE *statefile) {
    fwrite(&(isWeirdMode), sizeof(bool), 1, statefile);
}

