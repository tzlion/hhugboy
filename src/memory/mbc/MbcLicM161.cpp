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
