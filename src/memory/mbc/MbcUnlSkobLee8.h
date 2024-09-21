/*
 * Additional mapper support for hhugboy emulator
 * by taizou 2024
 * This file released under Creative Commons CC0 https://creativecommons.org/publicdomain/zero/1.0/legalcode
 *
 * As part of the hhugboy project it is also licensed under the GNU General Public License v2
 * See "license.txt" in the project root
 */

#ifndef HHUGBOY_MBCUNLSKOBLEE8_H
#define HHUGBOY_MBCUNLSKOBLEE8_H

#include "BasicMbc.h"
#include "MbcNin5_LogoSwitch.h"

// For SKOB LEE8 PCB carts: Space Fighter X/Final Fantasy X/Fantasy War. Digimon D-3
// Note unlike most GBC protected carts these boot up with some address scrambling and XORs already applied
// To dump one of these carts "normally" you will need to write 00 to 7000, 7001, 7002, 7003 and 5001 before dumping
// PCB config on all observed carts so far: R1 bridged, R2 empty, C1 empty, C2 100 nF capacitor, C3 empty
// Different configurations might be possible, might change the initial state or something
class MbcUnlSkobLee8 : public MbcNin5_LogoSwitch {
public:
    MbcUnlSkobLee8();
    virtual byte readMemory(register unsigned short address) override;
    virtual void writeMemory(unsigned short address, register byte data) override;

    virtual void resetVars(bool preserveMulticartState) override;

    virtual void readMbcSpecificVarsFromStateFile(FILE *savefile) override;
    virtual void writeMbcSpecificVarsToStateFile(FILE *savefile) override;

private:

    void setXorForBank(byte bankNo);

    byte bankReorderMode;
    byte xor00;
    byte xor01;
    byte xor02;
    byte xor03;
    byte requestedBankNo;
    byte romBankXor;

    // 5 and 7 are functionally identical for known carts
    // the actual order of bits 1 and 3 is unknown though as no known game has a >1MB ROM
    byte reordering05[8] = {1,3,2,0,5,4,7,6};
    byte reordering07[8] = {1,3,2,0,5,4,7,6};

    byte noReordering[8] = {0,1,2,3,4,5,6,7};
};


#endif //HHUGBOY_MBCUNLSKOBLEE8_H
