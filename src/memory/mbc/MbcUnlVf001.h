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

    public:
        virtual byte readMemory(register unsigned short address) override;
        virtual void writeMemory(unsigned short address, register byte data) override;

        virtual void resetVars(bool preserveMulticartState) override;

        virtual void readMbcSpecificVarsFromStateFile(FILE *savefile) override;
        virtual void writeMbcSpecificVarsToStateFile(FILE *savefile) override;

};


#endif //HHUGBOY_MBCUNLVF001_H
