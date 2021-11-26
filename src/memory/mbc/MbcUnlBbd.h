/*
 * Additional mapper support for hhugboy emulator
 * by taizou 2016
 * This file released under Creative Commons CC0 https://creativecommons.org/publicdomain/zero/1.0/legalcode
 *
 * As part of the hhugboy project it is also licensed under the GNU General Public License v2
 * See "license.txt" in the project root
 */
#ifndef HHUGBOY_MBCUNLBBD_H
#define HHUGBOY_MBCUNLBBD_H


#include "MbcNin5_LogoSwitch.h"

class MbcUnlBbd : public MbcNin5_LogoSwitch {
    public:
        MbcUnlBbd();
        virtual byte readMemory(register unsigned short address) override;
        virtual void writeMemory(unsigned short address, register byte data) override;

        virtual void resetVars(bool preserveMulticartState) override;

        virtual void readMbcSpecificVarsFromStateFile(FILE *savefile) override;
        virtual void writeMbcSpecificVarsToStateFile(FILE *savefile) override;

    protected:
        MbcUnlBbd( byte bbdBitSwapMode, byte bbdBankSwapMode );

        virtual byte swapDataByte( byte data );
        virtual byte swapBankByte( byte data );

        virtual bool isDataSwapModeSupported();
        virtual bool isBankSwapModeSupported();

        byte bitSwapMode;
        byte bankSwapMode;

        byte bbdDataReordering[8][8] = {
                {0,1,2,3,4,5,6,7}, // 00 - Normal
                {0,1,2,3,4,5,6,7}, // 01 - NOT KNOWN YET
                {0,1,2,3,4,5,6,7}, // 02 - NOT KNOWN YET
                {0,1,2,3,4,5,6,7}, // 03 - NOT KNOWN YET
                {0,1,5,3,4,6,2,7}, // 04 - Garou
                {0,1,2,6,4,5,3,7}, // 05 - Harry
                {0,1,2,3,4,5,6,7}, // 06 - NOT KNOWN YET
                {0,1,5,3,4,2,6,7}, // 07 - Digimon
        };
        byte bbdBankReordering[8][8] = {
                {0,1,2,3,4,5,6,7}, // 00 - Normal
                {0,1,2,3,4,5,6,7}, // 01 - NOT KNOWN YET
                {0,1,2,3,4,5,6,7}, // 02 - NOT KNOWN YET
                {0,1,2,6,7,5,3,4}, // 03 - 0,1 unconfirmed. Digimon/Garou
                {0,1,2,3,4,5,6,7}, // 04 - NOT KNOWN YET
                {0,1,2,7,3,4,5,6}, // 05 - 0,1 unconfirmed. Harry
                {0,1,2,3,4,5,6,7}, // 06 - NOT KNOWN YET
                {0,1,2,3,4,5,6,7}, // 07 - NOT KNOWN YET
        };


};


#endif //HHUGBOY_MBCUNLBBD_H
