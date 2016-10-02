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


#include "MbcNin5.h"

class MbcUnlBbd : public MbcNin5 {
    public:
        MbcUnlBbd();
        virtual byte readMemory(register unsigned short address) override;
        virtual void writeMemory(unsigned short address, register byte data) override;

        virtual void resetVars(bool preserveMulticartState) override;

        virtual void readMbcSpecificVarsFromStateFile(FILE *savefile) override;
        virtual void writeMbcSpecificVarsToStateFile(FILE *savefile) override;
    private:
        byte bbdBitSwapMode;
        byte bbdBankSwapMode;

        byte bankReordering03[8] = {0,1,2,6,7,5,3,4}; // 0,1 unconfirmed. Digimon/Garou
        byte bankReordering05[8] = {0,1,2,7,3,4,5,6}; // 0,1 unconfirmed. Harry
        byte dataReordering07[8] = {0,1,5,3,4,2,6,7}; // Digimon
        byte dataReordering05[8] = {0,1,2,6,4,5,3,7}; // Harry
        byte dataReordering04[8] = {0,1,5,3,4,6,2,7}; // Garou
};


#endif //HHUGBOY_MBCUNLBBD_H
