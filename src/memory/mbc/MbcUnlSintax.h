/*
 * Additional mapper support for hhugboy emulator
 * by taizou 2013-2016
 * This file released under Creative Commons CC0 https://creativecommons.org/publicdomain/zero/1.0/legalcode
 *
 * As part of the hhugboy project it is also licensed under the GNU General Public License v2
 * See "license.txt" in the project root
 */

#ifndef HHUGBOY_MBCUNLSINTAX_H
#define HHUGBOY_MBCUNLSINTAX_H

#include "BasicMbc.h"
#include "MbcNin5.h"

//-------------------------------------------------------------------------
// for Sintax standard carts
//-------------------------------------------------------------------------
class MbcUnlSintax : public MbcNin5 {
public:
    MbcUnlSintax();
    virtual byte readMemory(register unsigned short address) override;
    virtual void writeMemory(unsigned short address, register byte data) override;

    virtual void resetVars(bool preserveMulticartState) override;

    virtual void readMbcSpecificVarsFromStateFile(FILE *savefile) override;
    virtual void writeMbcSpecificVarsToStateFile(FILE *savefile) override;

private:

    void setXorForBank(byte bankNo);

    byte sintaxMode;
    byte sintaxXor00;
    byte sintaxXor01;
    byte sintaxXor02;
    byte sintaxXor03;
    byte sintaxBankNo;
    byte romBankXor;

    byte reordering[16][8] = {
        {6,1,0,3,2,5,4,7},
        {7,6,1,0,3,2,5,4},
        {6,1,0,3,2,5,4,7},
        {1,0,3,2,5,4,7,6},
        {6,1,0,3,2,5,4,7},
        {4,5,2,3,0,1,6,7},
        {6,1,0,3,2,5,4,7},
        {6,7,4,5,2,3,0,1},
        {6,1,0,3,2,5,4,7},
        {3,2,5,4,7,6,1,0},
        {6,1,0,3,2,5,4,7},
        {5,4,7,6,1,0,3,2},
        {6,1,0,3,2,5,4,7},
        {6,7,0,1,2,3,4,5},
        {6,1,0,3,2,5,4,7},
        {0,1,2,3,4,5,6,7}
    };
};


#endif //HHUGBOY_MBCUNLSINTAX_H
