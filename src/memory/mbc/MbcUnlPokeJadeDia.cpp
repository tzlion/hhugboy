/*
 * Additional mapper support for hhugboy emulator
 * by taizou 2018
 * This file released under Creative Commons CC0 https://creativecommons.org/publicdomain/zero/1.0/legalcode
 *
 * As part of the hhugboy project it is also licensed under the GNU General Public License v2
 * See "license.txt" in the project root
 */

#include <cstdio>
#include "MbcUnlPokeJadeDia.h"
#include "../../debug.h"

MbcUnlPokeJadeDia::MbcUnlPokeJadeDia() {
    notRtcRegister = 0;
    registerDValue = 0;
    registerEValue = 0;
}

byte MbcUnlPokeJadeDia::readMemory(unsigned short address) {
    if (address >= 0xa000) {
        if (!RAMenable) {
            // if RAM is not enabled we got nothing for you here
            return 0xff;
        }
        if (notRtcRegister >= 0x08 && notRtcRegister <= 0x0c && !(*gbCartridge)->RTC) {
            // attempting to read from one of the actual RTC registers
            // the cart I have just returns 00 for all, not sure if any version of this mapper exists with RTC populated
            // but if such a cart exists we can just let it fall through to the real MBC3's implementation for now
            return 0;
        }
        // the 3 not-RTC registers
        if (notRtcRegister == 0x0d) return registerDValue;
        if (notRtcRegister == 0x0e) return registerEValue;
        if (notRtcRegister == 0x0f) return 0; // F seems to be write only
    }
    return MbcNin3::readMemory(address);
}

void MbcUnlPokeJadeDia::writeMemory(unsigned short address, register byte data) {
    if (address >= 0x4000 && address <= 0x5fff) {
        // this cart's protection more or less extends MBC3's RTC register functionality
        // using the unused D, E and F registers for shenanigans
        notRtcRegister = data;
    }
    if (address >= 0xa000 && address <= 0xbfff) {
        if (!RAMenable) {
            // if RAM is NOT enabled and you try to write to that area, don't do anything
            // (pretty sure this behaviour should also occur on real MBC3 but haven't tested it yet)
            return;
        }
        switch (notRtcRegister) {
            case 0x0d:
                registerDValue = data;
                return;
            case 0x0e:
                registerEValue = data;
                return;
            case 0x0f:
                // Certain writes to F can manipulate the value in D and E
                // this is basically the weak protection scheme of these carts
                switch (data) {
                    case 0x11:
                        registerDValue--;
                        break;
                    case 0x12:
                        registerEValue--;
                        break;
                    case 0x41:
                        registerDValue += registerEValue;
                        break;
                    case 0x42:
                        registerEValue += registerDValue;
                        break;
                    case 0x51:
                        registerDValue++;
                        break;
                    case 0x52:
                        registerEValue--;
                        break;
                }
                return;
        }
    }
    MbcNin3::writeMemory(address, data);
}

void MbcUnlPokeJadeDia::resetVars(bool preserveMulticartState) {
    notRtcRegister = 0;
    registerDValue = 0;
    registerEValue = 0;
    AbstractMbc::resetVars(preserveMulticartState);
}

void MbcUnlPokeJadeDia::readMbcSpecificVarsFromStateFile(FILE *statefile) {
    MbcNin3::readMbcSpecificVarsFromStateFile(statefile);
    fread(&(notRtcRegister), sizeof(byte), 1, statefile);
    fread(&(registerDValue), sizeof(byte), 1, statefile);
    fread(&(registerEValue), sizeof(byte), 1, statefile);
}

void MbcUnlPokeJadeDia::writeMbcSpecificVarsToStateFile(FILE *statefile) {
    MbcNin3::writeMbcSpecificVarsToStateFile(statefile);
    fwrite(&(notRtcRegister), sizeof(byte), 1, statefile);
    fwrite(&(registerDValue), sizeof(byte), 1, statefile);
    fwrite(&(registerEValue), sizeof(byte), 1, statefile);
}
