/*
 * Additional mapper support for hhugboy emulator
 * by NewRisingSun 2020
 * This file released under Creative Commons CC0 https://creativecommons.org/publicdomain/zero/1.0/legalcode
 *
 * As part of the hhugboy project it is also licensed under the GNU General Public License v2
 * See "license.txt" in the project root
 */

#ifndef HHUGBOY_MBCNIN5_LOGOSWITCH_H
#define HHUGBOY_MBCNIN5_LOGOSWITCH_H

#include "MbcNin5.h"
//-------------------------------------------------------------------------
// MBC5 class that shows a logo at $0184 during bootstrap
//-------------------------------------------------------------------------
class MbcNin5_LogoSwitch : public MbcNin5 {
public:
    virtual void resetVars(bool preserveMulticartState) override;
    virtual byte readMemory(register unsigned short address) override;
    virtual void signalMemoryWrite(unsigned short address, register byte data) override;
    virtual void readMbcSpecificVarsFromStateFile(FILE *savefile) override;
    virtual void writeMbcSpecificVarsToStateFile(FILE *savefile) override;
private:
    byte logoMode;
    byte logoCount;
};

#endif //HHUGBOY_MBCNIN5_LOGOSWITCH_H
