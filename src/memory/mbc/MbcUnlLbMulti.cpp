//
// Created by Alex on 23/08/2016.
//

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
                byte romSize = multiOtherStuff & 0x07; // Inverse of Nintendo's sizes
                // 00 = 4m, 01 = 2m, 02 = 1m, 03 = 512k, 04 = 256k, 05 = 128k, 06 = 64k, 07 = 32k
                byte eightMegBankNo = ( multiOtherStuff & 0x08 ) >> 3; // 0 or 1 - haven't observed carts >16m yet
                byte doReset = ( multiOtherStuff & 0x80 ) >> 7;

                multicartOffset = (multiRomSelect << 0x0F) + (eightMegBankNo << 0x17);

                gbMemMap[0x0] = &(*gbCartridge)[multicartOffset];
                gbMemMap[0x1] = &(*gbCartridge)[multicartOffset+0x1000];
                gbMemMap[0x2] = &(*gbCartridge)[multicartOffset+0x2000];
                gbMemMap[0x3] = &(*gbCartridge)[multicartOffset+0x3000];

                gbMemMap[0x4] = &(*gbCartridge)[multicartOffset+0x4000];
                gbMemMap[0x5] = &(*gbCartridge)[multicartOffset+0x5000];
                gbMemMap[0x6] = &(*gbCartridge)[multicartOffset+0x6000];
                gbMemMap[0x7] = &(*gbCartridge)[multicartOffset+0x7000];

                // todo: RAM bankswitching should not really be allowed for values 0x30-0x3F
                if ( multiRamSelect >= 0x20 && multiRamSelect <= 0x3F ) {
                    multicartRamOffset = ( multiRamSelect - 0x20 ) << 0x0D;
                } else {
                    multicartRamOffset = 0;
                }

                gbMemMap[0xA] = &(*gbCartRam)[multicartRamOffset];
                gbMemMap[0xB] = &(*gbCartRam)[multicartRamOffset+0x1000];

                if ( doReset ) {
                    deferredReset = true;
                }

                multiCommand=0; multiRomSelect=0; multiRamSelect=0; multiOtherStuff = 0;
            }

            return;

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
    }

    AbstractMbc::resetVars(preserveMulticartState);
}

void MbcUnlLbMulti::writeMbcSpecificVarsToStateFile(FILE *statefile) {
    fwrite(&(multiCommand), sizeof(byte), 1, statefile);
    fwrite(&(multiRomSelect), sizeof(byte), 1, statefile);
    fwrite(&(multiRamSelect), sizeof(byte), 1, statefile);
    fwrite(&(multiOtherStuff), sizeof(byte), 1, statefile);
    fwrite(&(multicartOffset),sizeof(int),1,statefile);
    fwrite(&(multicartRamOffset),sizeof(int),1,statefile);
    fwrite(&(bc_select),sizeof(int),1,statefile);
}

void MbcUnlLbMulti::readMbcSpecificVarsFromStateFile(FILE *statefile) {
    fread(&(multiCommand), sizeof(byte), 1, statefile);
    fread(&(multiRomSelect), sizeof(byte), 1, statefile);
    fread(&(multiRamSelect), sizeof(byte), 1, statefile);
    fread(&(multiOtherStuff), sizeof(byte), 1, statefile);
    fread(&(multicartOffset),sizeof(int),1,statefile);
    fread(&(multicartRamOffset),sizeof(int),1,statefile);
    fread(&(bc_select),sizeof(int),1,statefile);

    resetRomMemoryMap(true);
}
