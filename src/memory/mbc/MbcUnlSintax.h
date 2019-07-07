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
        {0,7,2,1,4,3,6,5}, // 0 - Lion King, Golden Sun
        {7,6,1,0,3,2,5,4}, // 1 - Langrisser
        {0,7,2,1,4,3,6,5}, // 2 - not seen in games but seems to be same as 0
        {1,0,3,2,5,4,7,6}, // 3 - not seen in games but tested on cart
        {0,7,2,1,4,3,6,5}, // 4 - not seen in games but seems to be same as 0
        {0,1,6,7,4,5,2,3}, // 5 - Maple Story, Pokemon Platinum
        {0,7,2,1,4,3,6,5}, // 6 - not seen in games but seems to be same as 0
        {6,7,4,5,2,3,0,1}, // 7 - Bynasty Warriors 5, may have had this slightly wrong before
        {0,7,2,1,4,3,6,5}, // 8 - not seen in games but seems to be same as 0
        {3,2,5,4,7,6,1,0}, // 9 - ???
        {0,7,2,1,4,3,6,5}, // a - not seen in games but seems to be same as 0
        {5,4,7,6,1,0,3,2}, // b - Shaolin Legend
        {0,7,2,1,4,3,6,5}, // c - not seen in games but seems to be same as 0
        {6,7,0,1,2,3,4,5}, // d - Older games
        {0,7,2,1,4,3,6,5}, // e - not seen in games but seems to be same as 0
        {0,1,2,3,4,5,6,7}, // f - no reordering
    };
};


#endif //HHUGBOY_MBCUNLSINTAX_H
