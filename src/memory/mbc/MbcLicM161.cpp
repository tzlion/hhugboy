/*
 * Additional mapper support for hhugboy emulator
 * by NewRisingSun 2020
 * This file released under Creative Commons CC0 https://creativecommons.org/publicdomain/zero/1.0/legalcode
 *
 * As part of the hhugboy project it is also licensed under the GNU General Public License v2
 * See "license.txt" in the project root
 */
#include <cstdio>
#include "MbcLicM161.h"

void MbcLicM161::resetVars(bool preserveMulticartState) {
    AbstractMbc::resetVars(preserveMulticartState);
    locked =false;
    rom_bank =0;
    sync();
}

void MbcLicM161::readMbcSpecificVarsFromStateFile(FILE *statefile) {
    fread(&(locked), sizeof(bool), 1, statefile);
}

void MbcLicM161::writeMbcSpecificVarsToStateFile(FILE *statefile) {
    fwrite(&(locked), sizeof(bool), 1, statefile);
}

void MbcLicM161::writeMemory(unsigned short address, register byte data) {
	if (address <0x8000) {
		rom_bank =data;
		locked =true;
		sync();
	} else
		gbMemMap[address>>12][address&0x0FFF] = data;
}

void MbcLicM161::sync() {
	for (int bank =0; bank<=7; bank++) gbMemMap[bank] =&(*gbCartRom)[(rom_bank <<15 &rom_size_mask[(*gbCartridge)->ROMsize]) +bank*0x1000];
}
