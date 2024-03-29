/*
 * Additional mapper support for hhugboy emulator
 * by taizou 2022
 * This file released under Creative Commons CC0 https://creativecommons.org/publicdomain/zero/1.0/legalcode
 *
 * As part of the hhugboy project it is also licensed under the GNU General Public License v2
 * See "license.txt" in the project root
 */

#include <cstdio>
#include <cstring>
#include "MbcUnlVf001.h"
#include "../../debug.h"

#define MORE_DEBUG_MESSAGING false

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
        byte currentByte = sequenceLength - sequenceBytesLeft;
        return sequence[currentByte - 1];
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
            } else if (MORE_DEBUG_MESSAGING) {
                debug_win("Protection config mode enabled");
            }
            configMode = true;
            runningValue = (*gbCartridge)->mbcConfig[0];
            return;
        }

        // Disable config mode
        if (effectiveAddress == 0x700f && data == 0x96) {
            if (!configMode) {
                debug_win("Protection config mode disabled when config mode already off");
            } else if (MORE_DEBUG_MESSAGING) {
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

        if (MORE_DEBUG_MESSAGING) {
            sprintf(buffer, "Protection write: Addr %04x Data %02x Running %02x", address, data, runningValue);
            debug_win(buffer);
        }

        // Then the running value is stored until one of the protection modes is activated
        // At which point it will be used for some config depending on the address written to
        if (effectiveAddress >= 0x7000) {
            byte offset700x = effectiveAddress & 0xf;
            cur700x[offset700x] = runningValue;
        } else if (effectiveAddress == 0x6000) {
            cur6000 = runningValue;
        }

        // Byte sequence activation
        // Uses values from the following writes:
        // 7001-7002  Start address
        // 7003       Start bank
        // 7004-7007  Sequence values
        // 7000       Sequence length
        if (effectiveAddress == 0x7000) {
            sequenceStartBank = cur700x[3];
            sequenceStartAddress = (cur700x[2] << 8) + cur700x[1];
            sequence[0] = cur700x[4];
            sequence[1] = cur700x[5];
            sequence[2] = cur700x[6];
            sequence[3] = cur700x[7];
            switch(cur700x[0] & 7) {
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
            if (MORE_DEBUG_MESSAGING) {
                sprintf(
                    buffer,
                    "Sequence set up: bank %02x addr %04x vals %02x %02x %02x %02x count %01x",
                    sequenceStartBank, sequenceStartAddress, sequence[0], sequence[1], sequence[2], sequence[3], sequenceLength
                );
                debug_win(buffer);
            }
        }

        // Bank 0 replacement activation
        // Uses values from the following writes:
        // 7009-700a  Start address
        // 6000       Replacement data source bank
        if (effectiveAddress == 0x7008) {
            replaceStartAddress = (cur700x[10] << 8) + cur700x[9];
            replaceSourceBank = cur6000;
            if ((cur700x[8] & 0xf) == 0xf) { // to pay respects
                shouldReplace = true;
            } else {
                shouldReplace = false;
                sprintf(buffer, "Unknown command at 7008: %02x", runningValue);
                debug_win(buffer);
            }
            if (cur700x[10] >= 0x40) {
                sprintf(buffer, "Value outside bank 0 at 700a: %02x", runningValue);
                debug_win(buffer);
            }
            if (MORE_DEBUG_MESSAGING) {
                sprintf(
                    buffer,
                    "Bank 0 replacement set up: addr %04x bank %02x enabled %01x",
                    replaceStartAddress, replaceSourceBank, shouldReplace
                );
                debug_win(buffer);
            }
        }

        return;

    }

    MbcNin5_LogoSwitch::writeMemory(address, data);
}

void MbcUnlVf001::init() {
    configMode = false;
    runningValue = 0;

    cur6000 = 0;
    byte zeroes15[15] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    memcpy(cur700x, zeroes15, sizeof zeroes15);

    sequenceStartBank = 0;
    sequenceStartAddress = 0;
    sequenceLength = 0;
    byte zeroes4[4] = {0, 0, 0, 0};
    memcpy(sequence, zeroes4, sizeof zeroes4);
    sequenceBytesLeft = 0;

    shouldReplace = false;
    replaceStartAddress = 0;
    replaceSourceBank = 0;
}

MbcUnlVf001::MbcUnlVf001() {
    init();
}

void MbcUnlVf001::resetVars(bool preserveMulticartState) {
    init();
    MbcNin5_LogoSwitch::resetVars(preserveMulticartState);
}

void MbcUnlVf001::readMbcSpecificVarsFromStateFile(FILE *statefile) {
    MbcNin5_LogoSwitch::readMbcSpecificVarsFromStateFile(statefile);
    fread(&(configMode),            sizeof(bool),             1, statefile);
    fread(&(runningValue),          sizeof(byte),             1, statefile);
    fread(&(cur6000),               sizeof(byte),             1, statefile);
    fread(&(cur700x),               sizeof(byte),            15, statefile);
    fread(&(sequenceStartBank),     sizeof(byte),             1, statefile);
    fread(&(sequenceStartAddress),  sizeof(unsigned short),   1, statefile);
    fread(&(sequenceLength),        sizeof(byte),             1, statefile);
    fread(&(sequence),              sizeof(byte),             4, statefile);
    fread(&(shouldReplace),         sizeof(bool),             1, statefile);
    fread(&(replaceStartAddress),   sizeof(unsigned short),   1, statefile);
    fread(&(replaceSourceBank),     sizeof(byte),             1, statefile);
}

void MbcUnlVf001::writeMbcSpecificVarsToStateFile(FILE *statefile) {
    MbcNin5_LogoSwitch::writeMbcSpecificVarsToStateFile(statefile);
    fwrite(&(configMode),            sizeof(bool),             1, statefile);
    fwrite(&(runningValue),          sizeof(byte),             1, statefile);
    fwrite(&(cur6000),               sizeof(byte),             1, statefile);
    fwrite(&(cur700x),               sizeof(byte),            15, statefile);
    fwrite(&(sequenceStartBank),     sizeof(byte),             1, statefile);
    fwrite(&(sequenceStartAddress),  sizeof(unsigned short),   1, statefile);
    fwrite(&(sequenceLength),        sizeof(byte),             1, statefile);
    fwrite(&(sequence),              sizeof(byte),             4, statefile);
    fwrite(&(shouldReplace),         sizeof(bool),             1, statefile);
    fwrite(&(replaceStartAddress),   sizeof(unsigned short),   1, statefile);
    fwrite(&(replaceSourceBank),     sizeof(byte),             1, statefile);
}
