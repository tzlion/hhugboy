/*
 * Additional mapper support for hhugboy emulator
 * by taizou 2022
 * This file released under Creative Commons CC0 https://creativecommons.org/publicdomain/zero/1.0/legalcode
 *
 * As part of the hhugboy project it is also licensed under the GNU General Public License v2
 * See "license.txt" in the project root
 */

#ifndef HHUGBOY_MBCUNLVF001_H
#define HHUGBOY_MBCUNLVF001_H

#include "MbcNin5_LogoSwitch.h"

class MbcUnlVf001 : public MbcNin5_LogoSwitch {

    private:

        bool configMode = false;
        byte runningValue = 0;

        byte cur6000 = 0;
        byte cur700x[15] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

        byte sequenceStartBank = 0;
        unsigned short sequenceStartAddress = 0;
        byte sequenceLength = 0;
        byte sequence[4] = {0, 0, 0, 0};
        byte sequenceBytesLeft = 0;

        bool shouldReplace = false;
        unsigned short replaceStartAddress = 0;
        byte replaceSourceBank = 0;

    public:

        virtual byte readMemory(register unsigned short address) override;
        virtual void writeMemory(unsigned short address, register byte data) override;

        virtual void resetVars(bool preserveMulticartState) override;

        virtual void readMbcSpecificVarsFromStateFile(FILE *savefile) override;
        virtual void writeMbcSpecificVarsToStateFile(FILE *savefile) override;

};


#endif //HHUGBOY_MBCUNLVF001_H
