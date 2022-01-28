/*
 * Additional mapper support for hhugboy emulator
 * by NewRisingSun 2020
 * This file released under Creative Commons CC0 https://creativecommons.org/publicdomain/zero/1.0/legalcode
 *
 * As part of the hhugboy project it is also licensed under the GNU General Public License v2
 * See "license.txt" in the project root
 */

#include "MbcUnlWisdomTree.h"

byte MbcUnlWisdomTree::readMemory(register unsigned short address) {
    return gbMemMap[address>>12][address&0x0FFF];
}

void MbcUnlWisdomTree::writeMemory(unsigned short address, register byte data) {
    if(address < 0x4000)
    {
	rom_bank = address &0xF;
	int bank_start =rom_bank <<15 &rom_size_mask[(*gbCartridge)->ROMsize];
	gbMemMap[0x0] = &(*gbCartRom)[bank_start +0x0000];
	gbMemMap[0x1] = &(*gbCartRom)[bank_start +0x1000];
	gbMemMap[0x2] = &(*gbCartRom)[bank_start +0x2000];
	gbMemMap[0x3] = &(*gbCartRom)[bank_start +0x3000];
	gbMemMap[0x4] = &(*gbCartRom)[bank_start +0x4000];
	gbMemMap[0x5] = &(*gbCartRom)[bank_start +0x5000];
	gbMemMap[0x6] = &(*gbCartRom)[bank_start +0x6000];
	gbMemMap[0x7] = &(*gbCartRom)[bank_start +0x7000];
        return;
    }
}
