/*
 * Additional mapper support for hhugboy emulator
 * by taizou 2017
 * This file released under Creative Commons CC0 https://creativecommons.org/publicdomain/zero/1.0/legalcode
 *
 * As part of the hhugboy project it is also licensed under the GNU General Public License v2
 * See "license.txt" in the project root
 */

#include <cstdio>
#include "MbcUnlMakonNew.h"

void MbcUnlMakonNew::writeMemory(unsigned short address, register byte data) {
    if ( (address&0xFF00)== 0x1400 && data == 0x55 ) {
        splitMode = true;
        return;
    }
    if ( splitMode ) {
        if ( (address&0xFF00)== 0x2000 ) {
            int bankStartAddr = data<<0x0D;
            bankStartAddr &= rom_size_mask[(*gbRom)->ROMsize];
            if ( bankStartAddr < 0x4000 ) bankStartAddr += 0x4000;
            gbMemMap[0x4] = &(*gbCartridge)[bankStartAddr];
            gbMemMap[0x5] = &(*gbCartridge)[bankStartAddr+0x1000];
            return;
        }
        if ( (address&0xFF00)== 0x2400 ) {
            int bankStartAddr = data<<0x0D;
            bankStartAddr &= rom_size_mask[(*gbRom)->ROMsize];
            if ( bankStartAddr < 0x4000 ) bankStartAddr += 0x4000;
            gbMemMap[0x6] = &(*gbCartridge)[bankStartAddr+0x0000];
            gbMemMap[0x7] = &(*gbCartridge)[bankStartAddr+0x1000];
            return;
        }
    }

    MbcNin5::writeMemory(address, data);
}

MbcUnlMakonNew::MbcUnlMakonNew() :
        splitMode(false) {
}

void MbcUnlMakonNew::resetVars(bool preserveMulticartState) {
    splitMode = false;
    AbstractMbc::resetVars(preserveMulticartState);
}

void MbcUnlMakonNew::readMbcSpecificVarsFromStateFile(FILE *statefile) {
    fread(&(splitMode), sizeof(bool), 1, statefile);
}

void MbcUnlMakonNew::writeMbcSpecificVarsToStateFile(FILE *statefile) {
    fwrite(&(splitMode), sizeof(bool), 1, statefile);
}
