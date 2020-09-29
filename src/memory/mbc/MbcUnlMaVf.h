/*
 * Additional mapper support for hhugboy emulator
 * by taizou 2020
 * This file released under Creative Commons CC0 https://creativecommons.org/publicdomain/zero/1.0/legalcode
 *
 * As part of the hhugboy project it is also licensed under the GNU General Public License v2
 * See "license.txt" in the project root
 */

#ifndef HHUGBOY_MBCUNLMAVF_H
#define HHUGBOY_MBCUNLMAVF_H

#include "MbcUnlBbd.h"

// "ma" "GB Full Colour Chinese" carts with vast fame games - mainland china releases
// DSHGGB-81 PCB observed so far for Harvest Moon 6..
// No consistent logos
// * HM6 has "digi" logo swap
// * LOTR has "TD-SOFT" logo swap

class MbcUnlMaVf : public MbcUnlBbd {
    
public:
    MbcUnlMaVf();
    virtual void writeMemory(unsigned short address, register byte data) override;

    virtual void resetVars(bool preserveMulticartState) override;

protected:
    virtual byte swapDataByte( byte data ) override;
    virtual byte swapBankByte( byte data ) override;

    virtual bool isDataSwapModeSupported() override;
    virtual bool isBankSwapModeSupported() override;

    byte maVfDataReordering[8][8] = {
            {0,1,2,3,4,5,6,7},
            {0,2,1,3,4,6,5,7},
            {0,6,5,3,4,2,1,7},
            {0,1,5,3,4,6,2,7},
            {0,1,6,3,4,5,2,7},
            {0,6,2,3,4,1,5,7},
            {0,2,5,3,4,1,6,7},
            {0,6,1,3,4,2,5,7},
    };
    
};

#endif //HHUGBOY_MBCUNLMAVF_H
