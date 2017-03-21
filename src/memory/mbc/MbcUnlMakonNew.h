/*
 * Additional mapper support for hhugboy emulator
 * by taizou 2017
 * This file released under Creative Commons CC0 https://creativecommons.org/publicdomain/zero/1.0/legalcode
 *
 * As part of the hhugboy project it is also licensed under the GNU General Public License v2
 * See "license.txt" in the project root
 */

#ifndef HHUGBOY_MBCUNLMAKONNEW_H
#define HHUGBOY_MBCUNLMAKONNEW_H


#include "MbcNin5.h"

//-------------------------------------------------------------------------
// For newer Makon games e.g. Digimon, Pokemon, H.Potter, fighting games..
//-------------------------------------------------------------------------
class MbcUnlMakonNew : public MbcNin5 {
    public:
        MbcUnlMakonNew();
        virtual void writeMemory(unsigned short address, register byte data) override;

        virtual void resetVars(bool preserveMulticartState) override;

        virtual void readMbcSpecificVarsFromStateFile(FILE *statefile) override;
        virtual void writeMbcSpecificVarsToStateFile(FILE *statefile) override;
    private:
        bool splitMode = false;
};


#endif //HHUGBOY_MBCUNLMAKONNEW_H
