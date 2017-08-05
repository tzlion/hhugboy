#include <cstdio>
#include "MbcUnlMakonOld.h"
#include "../../debug.h"



void MbcUnlMakonOld::writeMemory(unsigned short address, register byte data) {

    // Maybe this should extend MBC1 idk

    if (address >= 0x2000 && address <= 0x3FFF) {

        if (data == 0) data = 1; // MBC1 stylez

        if (isWeirdMode) {
           //byte flippo[8] = {0,1,2,4,3,6,5,7}; // rocco
            byte flippo[8] = {0,1,2,3,4,7,5,6}; // mario??????? doesnt work
            data = switchOrder(data,flippo);
        }

        rom_bank = data;

        int bankAddress = rom_bank<<14;

        bankAddress &= rom_size_mask[(*gbRom)->ROMsize];

        bankAddress += multicartOffset;

        gbMemMap[0x4] = &(*gbCartridge)[bankAddress];
        gbMemMap[0x5] = &(*gbCartridge)[bankAddress+0x1000];
        gbMemMap[0x6] = &(*gbCartridge)[bankAddress+0x2000];
        gbMemMap[0x7] = &(*gbCartridge)[bankAddress+0x3000];

        return;
    }

    if ((address & 0xF000) == 0x5000) {

        switch(address & 0x03) {
            // no clue if 0x00 does anything
            case 0x01:
                data &= 0x3f; // should be limited to the actual rom size
                multicartOffset = (data << 0x0f);
                resetRomMemoryMap(true);
                // unemulated behaviour: a further switch should not be possible after the 1st one
                // also unemulated: the actual change should not be effected until you do the write to 5002
                break;
            case 0x02:
                (*gbRom)->ROMsize = 0x04; // dubious but works 5now- we really need to map the actual size values here
                // also this write seems to specify if it needs save data or not (??)
                break;
            case 0x03:
                isWeirdMode = (data & 0x10) == 0x10;
                // unemulated behaviour: the change is actually effected immediately on the real cart
                // e.g. the active rom bank changes w/o a further bankswitch required
                break;
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

