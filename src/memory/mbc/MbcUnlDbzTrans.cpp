/*
 * Additional mapper support for hhugboy emulator
 * by taizou 2016
 * This file released under Creative Commons CC0 https://creativecommons.org/publicdomain/zero/1.0/legalcode
 *
 * As part of the hhugboy project it is also licensed under the GNU General Public License v2
 * See "license.txt" in the project root
 */

#include "MbcUnlDbzTrans.h"

void MbcUnlDbzTrans::writeMemory(unsigned short address, register byte data) {

    // Half Bank Switch(tm)
    if ( ( address & 0xf0ff ) == 0x70d2 ) {
        int bankStartAddr = data<<14;
        bankStartAddr &= rom_size_mask[(*gbCartridge)->ROMsize];
        gbMemMap[0x6] = &(*gbCartRom)[bankStartAddr+0x2000];
        gbMemMap[0x7] = &(*gbCartRom)[bankStartAddr+0x3000];
        waitingForOtherHalf = true;
        return;
    } else if ( ( ( address & 0xf0ff ) == 0x20d2 ) || ( waitingForOtherHalf && ( address & 0xf000 ) == 0x2000 ) ) {
        int bankStartAddr = data<<14;
        bankStartAddr &= rom_size_mask[(*gbCartridge)->ROMsize];
        gbMemMap[0x4] = &(*gbCartRom)[bankStartAddr];
        gbMemMap[0x5] = &(*gbCartRom)[bankStartAddr+0x1000];
        waitingForOtherHalf = false;
        return;
    }

    MbcNin5::writeMemory(address, data);
}
