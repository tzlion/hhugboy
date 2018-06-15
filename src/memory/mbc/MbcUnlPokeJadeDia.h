/*
 * Additional mapper support for hhugboy emulator
 * by taizou 2018
 * This file released under Creative Commons CC0 https://creativecommons.org/publicdomain/zero/1.0/legalcode
 *
 * As part of the hhugboy project it is also licensed under the GNU General Public License v2
 * See "license.txt" in the project root
 */

#ifndef HHUGBOY_MBCUNLPOKEJADEDIA_H
#define HHUGBOY_MBCUNLPOKEJADEDIA_H


#include "MbcNin3.h"

class MbcUnlPokeJadeDia : public MbcNin3 {

    public:
        MbcUnlPokeJadeDia();
        virtual byte readMemory(unsigned short address) override;
        virtual void writeMemory(unsigned short address, register byte data) override;

        virtual void resetVars(bool preserveMulticartState) override;

        virtual void readMbcSpecificVarsFromStateFile(FILE *statefile) override;
        virtual void writeMbcSpecificVarsToStateFile(FILE *statefile) override;

    private:
        byte rambankno;
        byte ramd;
        byte rame;
};


#endif //HHUGBOY_MBCUNLPOKEJADEDIA_H
