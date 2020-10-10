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
#include "MbcUnlSachenMMC2.h"
#include "../../main.h"

#define MODE_LOCKED_DMG 0
#define MODE_LOCKED_CGB 1
#define MODE_UNLOCKED   2
void MbcUnlSachenMMC2::resetVars(bool preserveMulticartState) {
    AbstractMbc::resetVars(preserveMulticartState);
    outerBank =0;
    outerMask =0;
    rom_bank =1;
    mode =haveBootstrap && options->use_bootstrap? MODE_LOCKED_DMG: MODE_UNLOCKED;
    unlockCount =0;
}

void MbcUnlSachenMMC2::readMbcSpecificVarsFromStateFile(FILE *statefile) {
    fread(&(outerBank), sizeof(byte), 1, statefile);
    fread(&(outerMask), sizeof(byte), 1, statefile);
    fread(&(mode), sizeof(byte), 1, statefile);
    fread(&(unlockCount), sizeof(byte), 1, statefile);
}

void MbcUnlSachenMMC2::writeMbcSpecificVarsToStateFile(FILE *statefile) {
    fwrite(&(outerBank), sizeof(byte), 1, statefile);
    fwrite(&(outerMask), sizeof(byte), 1, statefile);
    fwrite(&(mode), sizeof(byte), 1, statefile);
    fwrite(&(unlockCount), sizeof(byte), 1, statefile);
}

byte MbcUnlSachenMMC2::readMemory(unsigned short address) {
	if (address <0x8000) {
		// The unlock sequence is 0x31 transitions of A15 from low to high. Starting on the last transition, RA7 will follow A7.
		if (unlockCount ==0x30) {
			if (mode ==MODE_LOCKED_DMG) {
				mode =MODE_LOCKED_CGB;
				unlockCount =0;
			} else
			if (mode ==MODE_LOCKED_CGB) mode =MODE_UNLOCKED;
		}
		if (unlockCount !=0x30) unlockCount++;
		
		// While locked, the mapper will keep RA7 set. This is used to display the Sachen logo instead of the Nintendo logo for the DMG bootstrap ROM.
		if (mode ==MODE_LOCKED_CGB) address |=0x80;
		
		// Sachen chose to scramble the header of their games. When A8 is high, while A15..A9 are low, the mapper will perform the following map:
		// RA0 <= A6, RA1 <= A4, RA4 <= A1, RA6 <= A0
		// When A8 is low or A15..A9 are not low, RAn lines will track their respective An line. 
		if ((address &0xFF00) ==0x0100)
			address =address &~0x53 |
				address >>6 &0x01 |
				address >>3 &0x02 |
				address <<3 &0x10 |
				address <<6 &0x40
			;
	}
	// No Sachen MMC2 game seems to have a logo check, so no need to put the logo into video memory when not emulating the bootstrap ROM.
	return BasicMbc::readMemory(address);
}

void MbcUnlSachenMMC2::writeMemory(unsigned short address, register byte data) {
	switch(address >>13) {
		case 0:	// Base ROM bank register
			if ((rom_bank &0x30) ==0x30) outerBank =data;
			break;
		case 1:	// ROM bank register
			rom_bank =data? data: 1;
			break;
		case 2:	// ROM bank mask register
			if ((rom_bank &0x30) ==0x30) {
				outerMask =data;

				// Print outer bank information for extraction script
				char buff[100];
				sprintf(buff,"FE MULTI.GB GAME%02u.GB %06X,%05X", outerBank >>1, (outerBank &outerMask) <<14, ((~outerMask &0x0F) +1) <<14);
				debug_win(buff);				
			}
			break;
		default:break;
	}
	// Update memory map
	for (int bank =0; bank<=3; bank++) gbMemMap[bank] =&(*gbCartRom)[((outerBank &outerMask |                    0) <<14 &rom_size_mask[(*gbCartridge)->ROMsize]) +bank*0x1000];
	for (int bank =4; bank<=7; bank++) gbMemMap[bank] =&(*gbCartRom)[((outerBank &outerMask | rom_bank &~outerMask) <<14 &rom_size_mask[(*gbCartridge)->ROMsize]) +bank*0x1000 -0x4000];
}

void MbcUnlSachenMMC2::signalMemoryWrite(unsigned short address, register byte data) {
	if (mode ==MODE_LOCKED_DMG && address >=0xC000 && address <=0xDFFF) {
		unlockCount =0;
		mode =MODE_LOCKED_CGB;
	}
}
