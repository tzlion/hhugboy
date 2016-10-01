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

    byte sintax_mode;
    byte sintax_xor2;
    byte sintax_xor3;
    byte sintax_xor4;
    byte sintax_xor5;

    byte reordering00[8] = {0,7,2,1,4,3,6,5};
    byte reordering01[8] = {7,6,1,0,3,2,5,4};
    byte reordering05[8] = {0,1,6,7,4,5,2,3}; // Not 100% on this one
    byte reordering07[8] = {5,7,4,6,2,3,0,1}; // 5 and 7 unconfirmed
    byte reordering09[8] = {3,2,5,4,7,6,1,0};
    byte reordering0b[8] = {5,4,7,6,1,0,3,2}; // 5 and 6 unconfirmed
    byte reordering0d[8] = {6,7,0,1,2,3,4,5};
    byte noReordering[8] = {0,1,2,3,4,5,6,7};
};


#endif //HHUGBOY_MBCUNLSINTAX_H
