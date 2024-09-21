/*
 * Additional mapper support for hhugboy emulator
 * by taizou 2024
 * This file released under Creative Commons CC0 https://creativecommons.org/publicdomain/zero/1.0/legalcode
 *
 * As part of the hhugboy project it is also licensed under the GNU General Public License v2
 * See "license.txt" in the project root
 */

#ifndef HHUGBOY_MBCUNLSKOBLEE8_H
#define HHUGBOY_MBCUNLSKOBLEE8_H

#include "BasicMbc.h"
#include "MbcNin5_LogoSwitch.h"

//-------------------------------------------------------------------------
// For SKOB Space Fighter X/Final Fantasy x/Fantasy War (possibly others?)
// Note unlike most GBC protected carts this does not boot up in standard MBC5 mode
// To dump one of these carts "normally" you will need to write 00 to 5001, 7000, 7001, 7002, 7003 before dumping
//-------------------------------------------------------------------------
class MbcUnlSkobLee8 : public MbcNin5_LogoSwitch {
public:
    MbcUnlSkobLee8();
    virtual byte readMemory(register unsigned short address) override;
    virtual void writeMemory(unsigned short address, register byte data) override;

    virtual void resetVars(bool preserveMulticartState) override;

    virtual void readMbcSpecificVarsFromStateFile(FILE *savefile) override;
    virtual void writeMbcSpecificVarsToStateFile(FILE *savefile) override;

private:

    void setXorForBank(byte bankNo);

    byte bankReorderMode;
    byte xor00;
    byte xor01;
    byte xor02;
    byte xor03;
    byte requestedBankNo;
    byte romBankXor;

    byte reordering0f[8] = {1,3,2,0,5,4,7,6}; // 1 and 3 unconfirmed
    byte noReordering[8] = {0,1,2,3,4,5,6,7};
};


#endif //HHUGBOY_MBCUNLSKOBLEE8_H
