/*
 * Additional mapper support for hhugboy emulator
 * by taizou 2017
 * This file released under Creative Commons CC0 https://creativecommons.org/publicdomain/zero/1.0/legalcode
 *
 * As part of the hhugboy project it is also licensed under the GNU General Public License v2
 * See "license.txt" in the project root
 */

#include <cstdio>
#include "MbcUnlNtK11.h"
#include "../../debug.h"
#include "MbcNin1.h"

void MbcUnlNtK11::writeMemory(unsigned short address, register byte data) {

    if(address < 0x2000) {
        BasicMbc::writeMemory(address,data);
        return;
    }

    if (address >= 0x2000 && address <= 0x3FFF) {
        data &= 0x1f; // MBC1 stylez
        if (data == 0) data = 1;
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

void MbcUnlNtK11::handleOldMakonCartModeSet(unsigned short address, byte data) {
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

MbcUnlNtK11::MbcUnlNtK11(int originalRomSize) :
        isWeirdMode(false) {
    this->originalRomSize = originalRomSize;
}

void MbcUnlNtK11::resetVars(bool preserveMulticartState) {
    isWeirdMode = false;
    (*gbRom)->ROMsize = originalRomSize;
    AbstractMbc::resetVars(preserveMulticartState);
}

void MbcUnlNtK11::readMbcSpecificVarsFromStateFile(FILE *statefile) {
    fread(&(isWeirdMode), sizeof(bool), 1, statefile);
    fread(&(multicartOffset),sizeof(int),1,statefile);
    fread(&((*gbRom)->ROMsize),sizeof(bool),1,statefile);
    resetRomMemoryMap(true);
}

void MbcUnlNtK11::writeMbcSpecificVarsToStateFile(FILE *statefile) {
    fwrite(&(isWeirdMode), sizeof(bool), 1, statefile);
    fwrite(&(multicartOffset),sizeof(int),1,statefile);
    fwrite(&((*gbRom)->ROMsize),sizeof(bool),1,statefile);
}
