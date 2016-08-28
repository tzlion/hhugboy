/*
 * Additional mapper support for hhugboy emulator
 * by taizou 2016
 * This file released under Creative Commons CC0 https://creativecommons.org/publicdomain/zero/1.0/legalcode
 *
 * As part of the hhugboy project it is also licensed under the GNU General Public License v2
 * See "license.txt" in the project root
 */

#include "MbcUnlLbMulti.h"

#include <cstdio>

void MbcUnlLbMulti::writeMemory(unsigned short address, register byte data) {

    if ( !bc_select ) {

        // Initial set of writes to switch into the menu are done to 5080 and 7080
        // Subsequent writes to select game are done to 5000 and 7000

        if ( address >= 0x5000 && address <= 0x5FFF ) {
            multiCommand = data;
            return;
        }
        if ( address >= 0x7000 && address <= 0x7FFF ) {

            bool effectChange = false;

            switch( multiCommand ) {
                case 0xAA: // Set ROM bank
                    if ( multiRomSelect == 0 ) {
                        multiRomSelect = data;
                    } else {
                        // The initial bank switch to activate the menu is followed by another write of 07 to 7080
                        // Which seems to immediately perform the switch without waiting for the rest of the sequence
                        // No idea if this write has to be 7
                        effectChange = true;
                    }
                    break;
                case 0xBB: // Set RAM bank
                    multiRamSelect = data;
                    break;
                case 0x55: // Set other shit and do the switch
                    multiOtherStuff = data;
                    effectChange = true;
                    bc_select = true; // prevent further changes
                    break;
            }

            if ( effectChange ) {
                byte romSize = multiOtherStuff & (byte)0x07;
                byte eightMegBankNo = ( multiOtherStuff & (byte)0x08 ) >> 3; // 0 or 1 - haven't observed carts >16m yet
                bool doReset = ( (multiOtherStuff & 0x80) == 0x80 );

                //  not sure if there are other potential mapper modes..
                mbc1Mode = ( ( multiOtherStuff & 0x60 ) == 0x60 );

                (*gbRom)->ROMsize = 7 - romSize; // Inverse of Nintendo's sizes e.g. 00 = 4m, 07 = 32k

                multicartOffset = (multiRomSelect << 0x0F) + (eightMegBankNo << 0x17);

                // todo: RAM bankswitching should not really be allowed for values 0x30-0x3F
                // Could set rom->RAMsize here but that's also used to determine save file size :(
                if ( multiRamSelect >= 0x20 && multiRamSelect <= 0x3F ) {
                    multicartRamOffset = ( multiRamSelect - 0x20 ) << 0x0D;
                } else {
                    multicartRamOffset = 0;
                }

                resetRomMemoryMap(true);

                if ( doReset ) {
                    deferredReset = true;
                }

                multiCommand=0; multiRomSelect=0; multiRamSelect=0; multiOtherStuff = 0;
            }

            return;

        }
    }

    if ( mbc1Mode ) {
        // Convert MBC1 style ROM switch writes to MBC5 ones
        // Won't work with MBC1 games >512k or using RAM if any carts contain games like that
        if ( address >= 0x2000 && address < 0x4000 )
        {
            if ( address >= 0x3000 )
                address -= 0x1000;
            if ( data == 0 )
                data = 1;
        }
    }

    MbcNin5::writeMemory(address, data);
}

void MbcUnlLbMulti::resetVars(bool preserveMulticartState) {

    if ( !preserveMulticartState ) {
        multiCommand=0;
        multiRomSelect=0;
        multiRamSelect=0;
        multiOtherStuff=0;
        mbc1Mode =false;
    }

    AbstractMbc::resetVars(preserveMulticartState);
}

void MbcUnlLbMulti::writeMbcSpecificVarsToStateFile(FILE *statefile) {
    fwrite(&(bc_select),sizeof(int),1,statefile);
    fwrite(&(multiCommand), sizeof(byte), 1, statefile);
    fwrite(&(multiRomSelect), sizeof(byte), 1, statefile);
    fwrite(&(multiRamSelect), sizeof(byte), 1, statefile);
    fwrite(&(multiOtherStuff), sizeof(byte), 1, statefile);
    fwrite(&(multicartOffset),sizeof(int),1,statefile);
    fwrite(&(multicartRamOffset),sizeof(int),1,statefile);
    fwrite(&(mbc1Mode),sizeof(bool),1,statefile);
    fwrite(&((*gbRom)->ROMsize),sizeof(bool),1,statefile);
}

void MbcUnlLbMulti::readMbcSpecificVarsFromStateFile(FILE *statefile) {
    fread(&(bc_select),sizeof(int),1,statefile);
    fread(&(multiCommand), sizeof(byte), 1, statefile);
    fread(&(multiRomSelect), sizeof(byte), 1, statefile);
    fread(&(multiRamSelect), sizeof(byte), 1, statefile);
    fread(&(multiOtherStuff), sizeof(byte), 1, statefile);
    fread(&(multicartOffset),sizeof(int),1,statefile);
    fread(&(multicartRamOffset),sizeof(int),1,statefile);
    fread(&(mbc1Mode),sizeof(bool),1,statefile);
    fread(&((*gbRom)->ROMsize),sizeof(bool),1,statefile);

    resetRomMemoryMap(true);
}
