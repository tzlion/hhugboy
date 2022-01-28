/*
 * Additional mapper support for hhugboy emulator
 * by NewRisingSun 2020
 * This file released under Creative Commons CC0 https://creativecommons.org/publicdomain/zero/1.0/legalcode
 *
 * As part of the hhugboy project it is also licensed under the GNU General Public License v2
 * See "license.txt" in the project root
 */

#ifndef HHUGBOY_MBCUNLSACHENMMC1_H
#define HHUGBOY_MBCUNLSACHENMMC1_H


#include "BasicMbc.h"
class MbcUnlSachenMMC1 : public BasicMbc {
public:
        virtual byte readMemory(unsigned short address) override;
	virtual void writeMemory(unsigned short address, register byte data) override;
        virtual void resetVars(bool preserveMulticartState) override;
        virtual void readMbcSpecificVarsFromStateFile(FILE *statefile) override;
        virtual void writeMbcSpecificVarsToStateFile(FILE *statefile) override;
private:
	byte	outerBank;
	byte	outerMask;
	bool	locked;
	byte	unlockCount;
};


#endif //HHUGBOY_MBCUNLSACHENMMC1_H
