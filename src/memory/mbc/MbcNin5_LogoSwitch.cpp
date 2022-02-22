/*
 * Additional mapper support for hhugboy emulator
 * by NewRisingSun 2020
 * This file released under Creative Commons CC0 https://creativecommons.org/publicdomain/zero/1.0/legalcode
 *
 * As part of the hhugboy project it is also licensed under the GNU General Public License v2
 * See "license.txt" in the project root
 */

#include "MbcNin5_LogoSwitch.h"
#include "../../main.h"

#define MODE_LOCKED_DMG 0
#define MODE_LOCKED_CGB 1
#define MODE_UNLOCKED   2
#define MODE_DONE       3

MbcNin5_LogoSwitch::MbcNin5_LogoSwitch() {
    initLogoState();
}

void MbcNin5_LogoSwitch::initLogoState() {
    logoMode = haveBootstrap && options->use_bootstrap ? MODE_LOCKED_DMG : MODE_DONE;
    logoCount = 0;
}

void MbcNin5_LogoSwitch::resetVars(bool preserveMulticartState) {
    MbcNin5::resetVars(preserveMulticartState);
    if (!preserveMulticartState) {
        initLogoState();
    }	
}

void MbcNin5_LogoSwitch::readMbcSpecificVarsFromStateFile(FILE *statefile) {
    fread(&(logoMode), sizeof(byte), 1, statefile);
    fread(&(logoCount), sizeof(byte), 1, statefile);
}

void MbcNin5_LogoSwitch::writeMbcSpecificVarsToStateFile(FILE *statefile) {
    fwrite(&(logoMode), sizeof(byte), 1, statefile);
    fwrite(&(logoCount), sizeof(byte), 1, statefile);
}

byte MbcNin5_LogoSwitch::readMemory(unsigned short address) {
	if (address <0x8000) {
		if (logoCount ==0x30) {
			if (logoMode ==MODE_LOCKED_DMG) {
				logoMode =MODE_LOCKED_CGB;
				logoCount =0;
			} else
			if (logoMode ==MODE_LOCKED_CGB) logoMode =MODE_UNLOCKED;
		}
		if (logoCount !=0x30) logoCount++;
		
		if (address ==0x100) logoMode =MODE_DONE;
		if (logoMode ==MODE_LOCKED_DMG || logoMode ==MODE_UNLOCKED && address >=0x104 && address <0x134) address |=0x80;
	}
	return MbcNin5::readMemory(address);
}

void MbcNin5_LogoSwitch::signalMemoryWrite(unsigned short address, register byte data) {
	if (logoMode ==MODE_LOCKED_DMG && address >=0xC000 && address <=0xDFFF) {
		logoCount =0;
		logoMode =MODE_LOCKED_CGB;
	}
}
