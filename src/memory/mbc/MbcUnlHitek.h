/*
 * Additional mapper support for hhugboy emulator
 * by taizou 2016
 * This file released under Creative Commons CC0 https://creativecommons.org/publicdomain/zero/1.0/legalcode
 *
 * As part of the hhugboy project it is also licensed under the GNU General Public License v2
 * See "license.txt" in the project root
 */
#ifndef HHUGBOY_MBCUNLHITEK_H
#define HHUGBOY_MBCUNLHITEK_H

#include "MbcNin5.h"
#include "MbcUnlBbd.h"

class MbcUnlHitek : public MbcUnlBbd {
public:
    MbcUnlHitek();
    virtual void writeMemory(unsigned short address, register byte data) override;

    virtual void resetVars(bool preserveMulticartState) override;

protected:
    virtual byte swapDataByte( byte data ) override;
    virtual byte swapBankByte( byte data ) override;

    virtual bool isDataSwapModeSupported() override;
    virtual bool isBankSwapModeSupported() override;

    byte hitekDataReordering[8][8] = {
            {0,1,2,3,4,5,6,7},
            {0,5,6,3,4,2,1,7},
            {0,6,5,3,4,1,2,7},
            {0,6,2,3,4,5,1,7},
            {0,5,2,3,4,6,1,7},
            {0,5,2,3,4,1,6,7},
            {0,2,6,3,4,1,5,7},
            {0,2,6,3,4,5,1,7},
    };
    byte hitekBankReordering[8][8] = {
            {0,1,2,3,4,5,6,7},
            {0,1,2,3,7,6,5,4},
            {0,1,2,3,4,7,6,5},
            {0,1,2,3,5,4,7,6},
            {0,1,2,3,6,5,4,7},
            {0,1,2,3,6,7,4,5},
            {0,1,2,3,5,6,7,4},
            {0,1,2,3,6,4,7,5},
    };


};


#endif //HHUGBOY_MBCUNLHITEK_H
