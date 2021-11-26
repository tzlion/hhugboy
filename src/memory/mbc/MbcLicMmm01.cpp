/*
 * Additional mapper support for hhugboy emulator
 * by NewRisingSun 2020
 * This file released under Creative Commons CC0 https://creativecommons.org/publicdomain/zero/1.0/legalcode
 *
 * As part of the hhugboy project it is also licensed under the GNU General Public License v2
 * See "license.txt" in the project root
 */
#include <cstdio>
#include "MbcLicMmm01.h"

void MbcLicMmm01::resetVars(bool preserveMulticartState) {
    outerBank =0;
    locked =false;
    ram_bank =0;
    rom_bank =0;
    RAMenable =0;
    AbstractMbc::resetVars(preserveMulticartState);
    sync();
}

void MbcLicMmm01::readMbcSpecificVarsFromStateFile(FILE *statefile) {
    fread(&(outerBank), sizeof(byte), 1, statefile);
    fread(&(locked), sizeof(bool), 1, statefile);
}

void MbcLicMmm01::writeMbcSpecificVarsToStateFile(FILE *statefile) {
    fwrite(&(outerBank), sizeof(byte), 1, statefile);
    fwrite(&(locked), sizeof(bool), 1, statefile);
}

void MbcLicMmm01::writeMemory(unsigned short address, register byte data) {
	switch(address >>13) {
		case 0:	if (locked)
				RAMenable =(data &0xF) ==0xA;
			else
				locked =true;
			break;
		case 1:	if (locked)
				rom_bank =data;
			else
				outerBank =data;
			break;
		case 2:	if (locked) ram_bank =data;
			break;
		case 3:	break;
		default:
			if (RAMenable) gbMemMap[address>>12][address&0x0FFF] = data;
			return; // No memory map update needed
	}
	sync();	
}

void MbcLicMmm01::sync() {
	// ROM
	if (locked) {
		for (int bank =0; bank<=3; bank++) gbMemMap[bank] =&(*gbCartRom)[((outerBank          ) <<14 &rom_size_mask[(*gbCartridge)->ROMsize]) +bank*0x1000];
		for (int bank =4; bank<=7; bank++) gbMemMap[bank] =&(*gbCartRom)[((outerBank +rom_bank) <<14 &rom_size_mask[(*gbCartridge)->ROMsize]) +bank*0x1000 -0x4000];
	} else
		for (int bank =0; bank<=7; bank++) gbMemMap[bank] =&(*gbCartRom)[(                0xFF  <<15 &rom_size_mask[(*gbCartridge)->ROMsize]) +bank*0x1000];
	// RAM
	for (int bank =0xA; bank<=0xB; bank++) gbMemMap[bank] =&(*gbCartRam)[(ram_bank &maxRAMbank[(*gbCartridge)->ROMsize])<<13  | bank*0x1000];
}
