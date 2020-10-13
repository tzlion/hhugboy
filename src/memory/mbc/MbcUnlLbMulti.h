/*
 * Additional mapper support for hhugboy emulator
 * by taizou 2016
 * This file released under Creative Commons CC0 https://creativecommons.org/publicdomain/zero/1.0/legalcode
 *
 * As part of the hhugboy project it is also licensed under the GNU General Public License v2
 * See "license.txt" in the project root
 */

#ifndef HHUGBOY_MBCUNLLBMULTI_H
#define HHUGBOY_MBCUNLLBMULTI_H


#include "MbcNin5_LogoSwitch.h"
//-------------------------------------------------------------------------
// for multicarts with Last Bible music ex. VFame's ones
//-------------------------------------------------------------------------
class MbcUnlLbMulti : public MbcNin5_LogoSwitch {
public:
    virtual void writeMemory(unsigned short address, register byte data) override;
    virtual void resetVars(bool preserveMulticartState) override;
    virtual void writeMbcSpecificVarsToStateFile(FILE *statefile) override;
    virtual void readMbcSpecificVarsFromStateFile(FILE *statefile) override;

private:
    byte multiCommand=0;
    byte multiRomSelect=0;
    byte multiRamSelect=0;
    byte multiOtherStuff=0;
    bool mbc1Mode = false;
};


#endif //HHUGBOY_MBCUNLLBMULTI_H
