/*
   hhugboy Game Boy emulator
   copyright 2013-2016 taizou
   Based on GEST
   Copyright (C) 2003-2010 TM
   Incorporating code from VisualBoyAdvance
   Copyright (C) 1999-2004 by Forgotten

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
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

const static byte nintendoLogo[48] = {
	0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
	0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
	0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E
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
			return nintendoLogo[address -0x0104];
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
