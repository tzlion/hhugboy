/*
 * Additional mapper support for hhugboy emulator
 * by taizou 2017
 * This file released under Creative Commons CC0 https://creativecommons.org/publicdomain/zero/1.0/legalcode
 *
 * As part of the hhugboy project it is also licensed under the GNU General Public License v2
 * See "license.txt" in the project root
 */

#ifndef HHUGBOY_MBCUNLNTOLD2_H
#define HHUGBOY_MBCUNLNTOLD2_H

#include "MbcUnlNtOld1.h"

class MbcUnlNtOld2: public MbcUnlNtOld1 {
    public:
        MbcUnlNtOld2(int originalRomSize);
        virtual void writeMemory(unsigned short address, register byte data) override;
    private:
        byte flippo2[8] = {0,1,2,3,4,7,5,6};
};

#endif //HHUGBOY_MBCUNLNTOLD2_H
