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
#include "MbcUnlSachenMMC1.h"
#include "../../main.h"

void MbcUnlSachenMMC1::resetVars(bool preserveMulticartState) {
    outerBank =0;
    outerMask =0;
    locked =true;
    unlockCount =0;
    rom_bank =1;
    AbstractMbc::resetVars(preserveMulticartState);
}

void MbcUnlSachenMMC1::readMbcSpecificVarsFromStateFile(FILE *statefile) {
    fread(&(outerBank), sizeof(byte), 1, statefile);
    fread(&(outerMask), sizeof(byte), 1, statefile);
    fread(&(locked), sizeof(bool), 1, statefile);
    fread(&(unlockCount), sizeof(byte), 1, statefile);
}

void MbcUnlSachenMMC1::writeMbcSpecificVarsToStateFile(FILE *statefile) {
    fwrite(&(outerBank), sizeof(byte), 1, statefile);
    fwrite(&(outerMask), sizeof(byte), 1, statefile);
    fwrite(&(locked), sizeof(bool), 1, statefile);
    fwrite(&(unlockCount), sizeof(byte), 1, statefile);
}

byte MbcUnlSachenMMC1::readMemory(unsigned short address) {
	if (haveBootstrap && options->use_bootstrap) {
		if (address <0x8000) {
			// The unlock sequence is 0x31 transitions of A15 from high to low. Starting on the last transition, RA7 will follow A7.
			if (unlockCount ==0x30)
				locked =false;
			else
				unlockCount++;
		}
		
		// While locked, the mapper will keep RA7 set. This is used to display the Sachen logo instead of the Nintendo logo for the DMG bootstrap ROM.
		if (locked) address |=0x80;
	}
	
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
	
	if (locked && address ==0x100 && !(haveBootstrap && options->use_bootstrap)) {
		locked =false;
		// When the bootstrap ROM is not emulated, put the custom logo into video memory, as most Sachen MMC1 games check for its presence as a copy-protection measure.
		for (int i =0; i <0x30; i++) {
			unsigned short int logoAddr =i +0x104;
			logoAddr =logoAddr &~0x53 |
				logoAddr >>6 &0x01 |
				logoAddr >>3 &0x02 |
				logoAddr <<3 &0x10 |
				logoAddr <<6 &0x40 |
				0x80;
			;
			byte logoByte =BasicMbc::readMemory(logoAddr);
			byte byte1 =logoByte >>0 &0x80 | logoByte >>1 &0x40 |
			            logoByte >>1 &0x20 | logoByte >>2 &0x10 |
				    logoByte >>2 &0x08 | logoByte >>3 &0x04 |
				    logoByte >>3 &0x02 | logoByte >>4 &0x01;
			byte byte2 =logoByte <<4 &0x80 | logoByte <<3 &0x40 |
			            logoByte <<3 &0x20 | logoByte <<2 &0x10 |
				    logoByte <<2 &0x08 | logoByte <<1 &0x04 |
				    logoByte <<1 &0x02 | logoByte <<0 &0x01;
			gbMemMap[0x8][0x0010 +i*8 +0] =byte1;
			gbMemMap[0x8][0x0010 +i*8 +2] =byte1;
			gbMemMap[0x8][0x0010 +i*8 +4] =byte2;
			gbMemMap[0x8][0x0010 +i*8 +6] =byte2;
		}
	}
	return BasicMbc::readMemory(address);
}

void MbcUnlSachenMMC1::writeMemory(unsigned short address, register byte data) {
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
		case 3:	break;
		default:// Write to RAM, if present
			gbMemMap[address>>12][address&0x0FFF] = data;
			return; // No memory map update needed
	}
	// Update memory map
	for (int bank =0; bank<=3; bank++) gbMemMap[bank] =&(*gbCartRom)[((outerBank &outerMask |                    0) <<14 &rom_size_mask[(*gbCartridge)->ROMsize]) +bank*0x1000];
	for (int bank =4; bank<=7; bank++) gbMemMap[bank] =&(*gbCartRom)[((outerBank &outerMask | rom_bank &~outerMask) <<14 &rom_size_mask[(*gbCartridge)->ROMsize]) +bank*0x1000 -0x4000];
}
