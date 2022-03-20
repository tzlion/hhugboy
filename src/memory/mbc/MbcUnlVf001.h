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

// Vast Fame mostly Taiwan releases
// PCBs inc: G6R16M01, G6R16M02, KGB-126, VF001
// Also mainland "SL" releases use this mapper with a config difference
// PCBs inc: SL-01-16, SL-004

class MbcUnlVf001 : public MbcNin5_LogoSwitch {

    private:

        bool configMode;
        byte runningValue;

        byte cur6000;
        byte cur700x[15];

        byte sequenceStartBank;
        unsigned short sequenceStartAddress;
        byte sequenceLength;
        byte sequence[4];
        byte sequenceBytesLeft;

        bool shouldReplace;
        unsigned short replaceStartAddress;
        byte replaceSourceBank;

        void init();

    public:

        MbcUnlVf001();

        virtual byte readMemory(register unsigned short address) override;
        virtual void writeMemory(unsigned short address, register byte data) override;

        virtual void resetVars(bool preserveMulticartState) override;

        virtual void readMbcSpecificVarsFromStateFile(FILE *statefile) override;
        virtual void writeMbcSpecificVarsToStateFile(FILE *statefile) override;

};

#endif //HHUGBOY_MBCUNLVF001_H
