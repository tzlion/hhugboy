/*
 * Additional mapper support for hhugboy emulator
 * by taizou 2021
 * This file released under Creative Commons CC0 https://creativecommons.org/publicdomain/zero/1.0/legalcode
 *
 * As part of the hhugboy project it is also licensed under the GNU General Public License v2
 * See "license.txt" in the project root
 */

#ifndef HHUGBOY_MBCUNLGGB81_H
#define HHUGBOY_MBCUNLGGB81_H

#include "MbcUnlBbd.h"

// Variant of BBD protection used by some secondary releases of Vast Fame games
// Digimon Pocket "Game GB Color" mainland China release (DSHGGB-81 PCB)
// Harvest Moon 6 & Lord of the Rings "GB全彩中文" mainland China releases (both DSHGGB-81 PCB)
// Digimon Sapphire "New Game Color Advance" English worldwide release (BC-R1616T3P PCB)
// DSHGGB-81 has logo swap, BC-R1616T3P does not

class MbcUnlGgb81 : public MbcUnlBbd {
    
public:
    MbcUnlGgb81();
    virtual void writeMemory(unsigned short address, register byte data) override;

protected:
    bool shouldEnableLogoSwap() override;

    virtual byte swapDataByte( byte data ) override;
    virtual byte swapBankByte( byte data ) override;

    virtual bool isDataSwapModeSupported() override;
    virtual bool isBankSwapModeSupported() override;

    byte ggb81DataReordering[8][8] = {
            {0,1,2,3,4,5,6,7},
            {0,2,1,3,4,6,5,7},
            {0,6,5,3,4,2,1,7},
            {0,1,5,3,4,6,2,7},
            {0,1,6,3,4,5,2,7},
            {0,6,2,3,4,1,5,7},
            {0,2,5,3,4,1,6,7},
            {0,6,1,3,4,2,5,7},
    };

private:
    static const byte PCB_TYPE_DSHGGB81 = 0;
    static const byte PCB_TYPE_BCR1616T3P = 1;
    
};

#endif //HHUGBOY_MBCUNLGGB81_H
