/*
 * Additional mapper support for hhugboy emulator
 * by taizou 2021
 * This file released under Creative Commons CC0 https://creativecommons.org/publicdomain/zero/1.0/legalcode
 *
 * As part of the hhugboy project it is also licensed under the GNU General Public License v2
 * See "license.txt" in the project root
 */
#ifndef HHUGBOY_MBCUNLNEWGBHK_H
#define HHUGBOY_MBCUNLNEWGBHK_H

#include "MbcNin5.h"

// Simple protection known to be used by "New GB Color" carts with HKxxxx PCBs as follows:
// 口袋怪獸-動作篇 [Pokemon Action Chapter] with HK0819 pcb
// 怪獸 GO!GO! II [Monster Go! Go! II] with HK0701 pcb

class MbcUnlNewGbHk : public MbcNin5 {
    public:
        virtual byte readMemory(unsigned short address) override;
    private:
        byte getValue6(byte digits);
        byte getValue7(byte digits);
        byte reverse[8] = {7,6,5,4,3,2,1,0};
        byte evenBitsTwice[8] = {0,2,4,6,0,2,4,6};
        byte oddBitsTwice[8] = {1,3,5,7,1,3,5,7};
};

#endif //HHUGBOY_MBCUNLNEWGBHK_H
