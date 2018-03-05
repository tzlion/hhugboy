/*
 * Additional mapper support for hhugboy emulator
 * by taizou 2017
 * This file released under Creative Commons CC0 https://creativecommons.org/publicdomain/zero/1.0/legalcode
 *
 * As part of the hhugboy project it is also licensed under the GNU General Public License v2
 * See "license.txt" in the project root
 */

#ifndef HHUGBOY_MBCUNLNTOLD1_H
#define HHUGBOY_MBCUNLNTOLD1_H

#include "MbcNin1.h"

class MbcUnlNtOld1: public BasicMbc {
   public:
        MbcUnlNtOld1(int originalRomSize);
        virtual void writeMemory(unsigned short address, register byte data) override;

        virtual void resetVars(bool preserveMulticartState) override;

        virtual void readMbcSpecificVarsFromStateFile(FILE *savefile) override;
        virtual void writeMbcSpecificVarsToStateFile(FILE *savefile) override;

        int originalRomSize;

    protected:
        void handleOldMakonCartModeSet(unsigned short address, byte data);
        bool isWeirdMode;

    private:
        byte flippo1[8] = {0,1,2,4,3,6,5,7};
};

#endif //HHUGBOY_MBCUNLNTOLD1_H
