#include <cstdio>
#include "MbcUnlMakonOld.h"
#include "../../debug.h"
#include "MbcNin1.h"


void MbcUnlMakonOld::writeMemory(unsigned short address, register byte data) {

    if(address < 0x2000) {
        MbcNin1::writeMemory(address,data);
        return;
    }

    if (address >= 0x2000 && address <= 0x3FFF) {

        if (data == 0) data = 1; // MBC1 stylez

        if (isWeirdMode) {
            // okay so this detection is probably definitely hacky and this theory hasn't been tested on real carts but
            // - Mario 3 and Rockman 8 each rely on different bit scrambling behaviour
            // - But the actual cart mapper HW seems to be identical and the write used to enable the mode is the same
            // So how does it know which mode to apply?
            // The only difference in the multi initialisation for each game AFAIK is the ROM size
            // So let's use that for now
            if ((*gbRom)->ROMsize == 4) {
                byte flippo[8] = {0,1,2,3,4,7,5,6}; // Mario
                data = switchOrder(data,flippo);
            } else {
                byte flippo[8] = {0,1,2,4,3,6,5,7}; // Rockman
                data = switchOrder(data,flippo);
            }
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
                if (multicartOffset > 0) {
                    resetRomMemoryMap(true);
                }
                // unemulated behaviour: a further switch should not be possible after the 1st one
                // also unemulated: the actual change should not be effected until you do the write to 5002
                break;
            case 0x02:
                if ((data & 0xF0) == 0xE0) {
                    // indicates the game uses 8kb ram but we leave it always enabled for now
                }
                switch(data & 0x0F) {
                    case 0x00: // 512k
                        (*gbRom)->ROMsize = 0x04;
                        break;
                    case 0x08: // 256k
                        (*gbRom)->ROMsize = 0x03;
                        break;
                    case 0x0f: // 32k
                        (*gbRom)->ROMsize = 0x00;
                        break;
                    default:
                        // rom size seems to be 0x10 minus this value and and multiplied by 32kb
                        // but the mappings defined above cover all observed cases so just fall back to 512k otherwise
                        (*gbRom)->ROMsize = 0x04;
                        break;
                }
                break;
            case 0x03:
                isWeirdMode = (data & 0x10) == 0x10;
                // active rom bank should change at this point, no explicit switch required (observed on real cart)
                writeMemory(0x2000, rom_bank);
                break;
        }
        return;
    }

    if (address < 0x8000) {
        // ignore anything else in the cart area for now
        return;
    }

    MbcNin1::writeMemory(address,data);
    return;
}

MbcUnlMakonOld::MbcUnlMakonOld() :
        isWeirdMode(false) {
}

void MbcUnlMakonOld::resetVars(bool preserveMulticartState) {
    isWeirdMode = false;
    (*gbRom)->ROMsize = 0x04;
    AbstractMbc::resetVars(preserveMulticartState);
}

void MbcUnlMakonOld::readMbcSpecificVarsFromStateFile(FILE *statefile) {
    fread(&(isWeirdMode), sizeof(bool), 1, statefile);
    fread(&(multicartOffset),sizeof(int),1,statefile);
    fread(&((*gbRom)->ROMsize),sizeof(bool),1,statefile);
    resetRomMemoryMap(true);
}

void MbcUnlMakonOld::writeMbcSpecificVarsToStateFile(FILE *statefile) {
    fwrite(&(isWeirdMode), sizeof(bool), 1, statefile);
    fwrite(&(multicartOffset),sizeof(int),1,statefile);
    fwrite(&((*gbRom)->ROMsize),sizeof(bool),1,statefile);
}

