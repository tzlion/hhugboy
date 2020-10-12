/*
 * Additional mapper support for hhugboy emulator
 * by NewRisingSun 2020
 * This file released under Creative Commons CC0 https://creativecommons.org/publicdomain/zero/1.0/legalcode
 *
 * As part of the hhugboy project it is also licensed under the GNU General Public License v2
 * See "license.txt" in the project root
 */
#include <cstdio>
#include "MbcUnlRocketGames.h"
#include "../../main.h"

#define MODE_LOCKED_DMG 0
#define MODE_LOCKED_CGB 1
#define MODE_UNLOCKED   2
void MbcUnlRocketGames::resetVars(bool preserveMulticartState) {
    AbstractMbc::resetVars(preserveMulticartState);
    outerBank =0;
    rom_bank =1;
    mode =haveBootstrap? MODE_LOCKED_DMG: MODE_UNLOCKED;
    unlockCount =0;
}

const static byte rocketXorNintendoLogo[48] = { // XOR values to get from Rocket Games logo to Nintendo logo
	0xdf, 0xce, 0x97, 0x78, 0xcd, 0x2f, 0xf0, 0x0b, 0x0b, 0xea, 0x78, 
	0x83, 0x08, 0x1d, 0x9a, 0x45, 0x11, 0x2b, 0xe1, 0x11, 0xf8, 0x88, 
	0xf8, 0x8e, 0xfe, 0x88, 0x2a, 0xc4, 0xff, 0xfc, 0xd9, 0x87, 0x22, 
	0xab, 0x67, 0x7d, 0x77, 0x2c, 0xa8, 0xee, 0xff, 0x9b, 0x99, 0x91, 
	0xaa, 0x9b, 0x33, 0x3e
};

void MbcUnlRocketGames::readMbcSpecificVarsFromStateFile(FILE *statefile) {
    fread(&(outerBank), sizeof(byte), 1, statefile);
}

void MbcUnlRocketGames::writeMbcSpecificVarsToStateFile(FILE *statefile) {
    fwrite(&(outerBank), sizeof(byte), 1, statefile);
}

byte MbcUnlRocketGames::readMemory(unsigned short address) {
	if (address <0x8000) {
		// The unlock sequence is 0x31 transitions of A15 from low to high. Starting on the last transition, RA7 will follow A7.
		if (unlockCount ==0x30) {
			if (mode ==MODE_LOCKED_DMG) {
				mode =MODE_LOCKED_CGB;
				unlockCount =0;
			} else
			if (mode ==MODE_LOCKED_CGB)
				mode =MODE_UNLOCKED;
		}
		if (unlockCount !=0x30) unlockCount++;
		
		if (mode ==MODE_LOCKED_CGB && address >=0x0104 && address <0x134)
			return BasicMbc::readMemory(address) ^rocketXorNintendoLogo[address -0x0104];
	}
	
	return BasicMbc::readMemory(address);
}

void MbcUnlRocketGames::writeMemory(unsigned short address, register byte data) {
	switch(address) {
		case 0x3F00:
			rom_bank =data? data: 1;
			break;
		case 0x3FC0:
			outerBank =data <<4;
			break;
		default:// Write to RAM, if present
			gbMemMap[address>>12][address&0x0FFF] = data;
			return; // No memory map update needed
	}
	// Update memory map
	for (int bank =0; bank<=3; bank++) gbMemMap[bank] =&(*gbCartRom)[((outerBank |        0) <<14 &rom_size_mask[(*gbCartridge)->ROMsize]) +bank*0x1000];
	for (int bank =4; bank<=7; bank++) gbMemMap[bank] =&(*gbCartRom)[((outerBank | rom_bank) <<14 &rom_size_mask[(*gbCartridge)->ROMsize]) +bank*0x1000 -0x4000];
}

void MbcUnlRocketGames::signalMemoryWrite(unsigned short address, register byte data) {
	if (mode ==MODE_LOCKED_DMG && address >=0xC000 && address <=0xDFFF) {
		unlockCount =0;
		mode =MODE_LOCKED_CGB;
	}
}
