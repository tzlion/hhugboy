/*
 * Additional mapper support for hhugboy emulator
 * by taizou 2022
 * This file released under Creative Commons CC0 https://creativecommons.org/publicdomain/zero/1.0/legalcode
 *
 * As part of the hhugboy project it is also licensed under the GNU General Public License v2
 * See "license.txt" in the project root
 */

#include <cstdio>
#include "MbcUnlVf001.h"
#include "../../debug.h"

bool configMode = false;
byte runningValue = 00;

byte cur6000 = 00;
byte cur7001 = 00;
byte cur7002 = 00;
byte cur7003 = 00;
byte cur7004 = 00;
byte cur7005 = 00;
byte cur7006 = 00;
byte cur7007 = 00;
byte cur7009 = 00;
byte cur700a = 00;

int sequenceStartBank = 0;
int sequenceStartAddress = 0;
int sequenceLength = 0;
byte sequenceVal1 = 0;
byte sequenceVal2 = 0;
byte sequenceVal3 = 0;
byte sequenceVal4 = 0;
int sequenceBytesLeft = 0;

bool shouldReplace = false;
int replaceStartAddress = 0;
int replaceSourceBank = 0;

byte MbcUnlVf001::readMemory(unsigned short address) {

    // Protection affects ROM reads in 2 ways

    // (1) ROM reads return a specified sequence of bytes triggered by a read from a specified address
    bool inChangeStartBank = (sequenceStartBank == 0 && address < 0x3fff) || (sequenceStartBank == rom_bank && address >= 4000);
    if (inChangeStartBank && address == sequenceStartAddress && sequenceBytesLeft == 0) {
        sequenceBytesLeft = sequenceLength;
    }
    // After the sequence is triggered it will be continued by any read from ROM
    if (sequenceBytesLeft > 0 && address < 0x8000) {
        sequenceBytesLeft--;
        int currentByte = sequenceLength - sequenceBytesLeft;
        if (currentByte == 1) return sequenceVal1;
        if (currentByte == 2) return sequenceVal2;
        if (currentByte == 3) return sequenceVal3;
        if (currentByte == 4) return sequenceVal4;
    }
    // Note I'm not 100% sure on the non-zero bank behaviour here
    // as I observed it on a cart returning 3 bytes from the sequence (instead of the specified 4)
    // immediately after the bank was switched and not actually caring about the specified address
    // But it seems to work this way for now. Needs more testing

    // (2) Partial replacement of bank 0 starting from a specified address
    if (shouldReplace && address >= replaceStartAddress && address < 0x4000) {
        byte* replaceBankPointer = &(*this->gbCartRom)[(replaceSourceBank << 0xe) & (rom_size_mask[(*gbCartridge)->ROMsize])];
        return replaceBankPointer[address];
    }

    return MbcNin5_LogoSwitch::readMemory(address);

}

void MbcUnlVf001::writeMemory(unsigned short address, byte data) {

    // Protection configuration writes
    if (address >= 0x6000 && address < 0x8000) {

        char buffer[420];

        unsigned short effectiveAddress = address & 0xf00f;

        // Enable config mode
        if (effectiveAddress == 0x7000 && data == 0x96) {
            if (configMode) {
                debug_win("Protection config mode enabled when config mode already on");
            } else {
                debug_win("Protection config mode enabled");
            }
            configMode = true;
            runningValue = 00;
            return;
        }

        // Disable config mode
        if (effectiveAddress == 0x700f && data == 0x96) {
            if (!configMode) {
                debug_win("Protection config mode disabled when config mode already off");
            } else {
                debug_win("Protection config mode disabled");
            }
            configMode = false;
            return;
        }

        // If we have any other protection write and config mode was NOT enabled we don't expect that so do nothing
        // (Not verified on cart whether anything actually happens in this case)
        if (!configMode) {
            sprintf(buffer, "Protection address written when config mode off: %04x %02x", address, data);
            debug_win(buffer);
            return;
        }

        // Writes to unknown addresses - do nothing for now
        // (Not verified on cart whether these addresses affect the running value or do anything else)
        if (effectiveAddress >= 0x700b || (effectiveAddress < 0x7000 && effectiveAddress > 0x6000)) {
            sprintf(buffer, "Write to unknown protection address: %04x %02x", address, data);
            debug_win(buffer);
            return;
        }

        // Otherwise, any known write should affect the running value like so
        runningValue = ((runningValue & 1) ? 0x80 : 0) + (runningValue >> 1);
        runningValue = runningValue ^ data;
        sprintf(buffer, "Protection write: Addr %04x Data %02x Running %02x", address, data, runningValue);
        debug_win(buffer);

        // Then the running value is used for some config depending on the address written to

        switch(effectiveAddress) {

            // (1) "byte sequence" related writes

            // set start address & bank
            case 0x7001:
                cur7001 = runningValue;
                break;
            case 0x7002:
                cur7002 = runningValue;
                break;
            case 0x7003:
                cur7003 = runningValue;
                break;

            // set sequence values
            case 0x7004:
                cur7004 = runningValue;
                break;
            case 0x7005:
                cur7005 = runningValue;
                break;
            case 0x7006:
                cur7006 = runningValue;
                break;
            case 0x7007:
                cur7007 = runningValue;
                break;

            // set sequence length & activate the changes
            case 0x7000:
                sequenceStartBank = cur7003;
                sequenceStartAddress = (cur7002 << 8) + cur7001;
                sequenceVal1 = cur7004;
                sequenceVal2 = cur7005;
                sequenceVal3 = cur7006;
                sequenceVal4 = cur7007;
                switch(runningValue & 7) {
                    case 4:
                        sequenceLength = 1;
                        break;
                    case 5:
                        sequenceLength = 2;
                        break;
                    case 6:
                        sequenceLength = 3;
                        break;
                    case 7:
                        sequenceLength = 4;
                        break;
                    default:
                        sprintf(buffer, "Unknown command at 7000: %02x", runningValue);
                        debug_win(buffer);
                        sequenceLength = 0;
                }
                sprintf(
                    buffer,
                    "Sequence set up: bank %02x addr %04x vals %02x %02x %02x %02x count %01x",
                    sequenceStartBank, sequenceStartAddress, sequenceVal1, sequenceVal2, sequenceVal3, sequenceVal4, sequenceLength
                );
                debug_win(buffer);
                break;

            // (2) "bank 0 replacement" related writes

            // set start address
            case 0x7009:
                cur7009 = runningValue;
                break;
            case 0x700a:
                if (runningValue >= 0x40) {
                    sprintf(buffer, "Value outside bank 0 at 700a: %02x", runningValue);
                    debug_win(buffer);
                }
                cur700a = runningValue;
                break;

            // set replacement source bank
            case 0x6000:
                cur6000 = runningValue;
                break;

            // activate the changes
            case 0x7008:
                replaceStartAddress = (cur700a << 8) + cur7009;
                replaceSourceBank = cur6000;
                if ((runningValue & 0xf) == 0xf) { // to pay respects
                    shouldReplace = true;
                } else {
                    shouldReplace = false;
                    sprintf(buffer, "Unknown command at 7008: %02x", runningValue);
                    debug_win(buffer);
                }
                sprintf(
                    buffer,
                    "Bank 0 replacement set up: addr %04x bank %02x enabled %01x",
                    replaceStartAddress, replaceSourceBank, shouldReplace
                );
                debug_win(buffer);
                break;

        }

        return;

    }

    MbcNin5_LogoSwitch::writeMemory(address, data);
}

void MbcUnlVf001::resetVars(bool preserveMulticartState) {
    MbcNin5_LogoSwitch::resetVars(preserveMulticartState);
}

void MbcUnlVf001::readMbcSpecificVarsFromStateFile(FILE *savefile) {
    MbcNin5_LogoSwitch::readMbcSpecificVarsFromStateFile(savefile);
}

void MbcUnlVf001::writeMbcSpecificVarsToStateFile(FILE *savefile) {
    MbcNin5_LogoSwitch::writeMbcSpecificVarsToStateFile(savefile);
}
