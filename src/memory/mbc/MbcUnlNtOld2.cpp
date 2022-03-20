/*
 * Additional mapper support for hhugboy emulator
 * by taizou 2017
 * This file released under Creative Commons CC0 https://creativecommons.org/publicdomain/zero/1.0/legalcode
 *
 * As part of the hhugboy project it is also licensed under the GNU General Public License v2
 * See "license.txt" in the project root
 */

#include "MbcUnlNtOld2.h"
#include "../../debug.h"

MbcUnlNtOld2::MbcUnlNtOld2(int originalRomSize) : MbcUnlNtOld1(originalRomSize)
{
}

void MbcUnlNtOld2::writeMemory(unsigned short address, register byte data) {

    if (address >= 0x2000 && address <= 0x3FFF) {
        if (data == 0) data = 1; // MBC3 stylez
        if (isWeirdMode) {
            data = switchOrder(data,flippo2);
        }
        setRom1Bank(data);
        return;
    }

    // -- rumble in the jungle --

    // On a multicart rumble needs to be enabled by the write to 5001 & 80
    // But single carts like DK have it enabled already
    if (address == 0x5001) {
        if (data & 0x80) {
            (*gbCartridge)->rumble = true;
        } else {
            (*gbCartridge)->rumble = false;
        }
    }

    if ((*gbCartridge)->rumble && address >= 0x4000 && address <= 0x5FFF) {
        // In initialised weird-Makon mode it works on the same writes as official rumble carts
        // In uninitialised mode it's different (so official rumble games wouldn't rumble on these carts?)
        if(isWeirdMode ? (data & 0x08) : (data & 0x02)) {
            *isVibrating = 1;
        } else {
            *isVibrating = 0;
        }
    }

    MbcUnlNtOld1::writeMemory(address, data);
}
