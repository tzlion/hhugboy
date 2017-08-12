#include <cstdio>
#include "MbcUnlMakonOld1.h"
#include "../../debug.h"
#include "MbcNin1.h"


void MbcUnlMakonOld1::writeMemory(unsigned short address, register byte data) {

    if(address < 0x2000) {
        BasicMbc::writeMemory(address,data);
        return;
    }

    if (address >= 0x2000 && address <= 0x3FFF) {
        if (data == 0) data = 1; // MBC1 stylez
        if (isWeirdMode) {
            data = switchOrder(data,flippo1);
        }
        setRom1Bank(data);
        return;
    }

    if ((address & 0xF000) == 0x5000) {
        handleOldMakonCartModeSet(address, data);
        return;
    }

    if (address < 0x8000) {
        // ignore anything else in the cart area for now
        return;
    }

    BasicMbc::writeMemory(address,data);
}

void MbcUnlMakonOld1::handleOldMakonCartModeSet(unsigned short address, byte data) {
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
                case 0x0c: // 128k
                    (*gbRom)->ROMsize = 0x02;
                    break;
                case 0x0e: // 64k
                    (*gbRom)->ROMsize = 0x01;
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
}

MbcUnlMakonOld1::MbcUnlMakonOld1(int originalRomSize) :
        isWeirdMode(false) {
    this->originalRomSize = originalRomSize;
}

void MbcUnlMakonOld1::resetVars(bool preserveMulticartState) {
    isWeirdMode = false;
    (*gbRom)->ROMsize = originalRomSize;
    AbstractMbc::resetVars(preserveMulticartState);
}

void MbcUnlMakonOld1::readMbcSpecificVarsFromStateFile(FILE *statefile) {
    fread(&(isWeirdMode), sizeof(bool), 1, statefile);
    fread(&(multicartOffset),sizeof(int),1,statefile);
    fread(&((*gbRom)->ROMsize),sizeof(bool),1,statefile);
    resetRomMemoryMap(true);
}

void MbcUnlMakonOld1::writeMbcSpecificVarsToStateFile(FILE *statefile) {
    fwrite(&(isWeirdMode), sizeof(bool), 1, statefile);
    fwrite(&(multicartOffset),sizeof(int),1,statefile);
    fwrite(&((*gbRom)->ROMsize),sizeof(bool),1,statefile);
}
