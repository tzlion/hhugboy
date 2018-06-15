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
    rambankno = 0;
    ramd = 0;
    rame = 0;
}

byte MbcUnlPokeJadeDia::readMemory(unsigned short address) {
    byte data = MbcNin3::readMemory(address);
    if (address >= 0xa000) {
        if (!RAMenable) {
            return 0xff;
        }
        if (rambankno == 0x0d) data= ramd;
        else if (rambankno == 0x0e) data= rame;
        else if (rambankno >= 0x04) {
            char msg[69];
            sprintf(msg,"trying to read undefined ram bank %02x", rambankno);
            debug_win(msg);
            debug_print(msg);
        }
    }
    return data;
}

void MbcUnlPokeJadeDia::writeMemory(unsigned short address, register byte data) {
    if (address == 0x4000 && address <= 0x5fff) {
        rambankno = data;
    }
    if (address >= 0xa000 && !RAMenable) {
        return;
    }
    if (address >= 0xa000 && rambankno >= 0x08) {
        if (rambankno == 0x0d) ramd = data;
        if (rambankno == 0x0e) rame = data;
        if (rambankno == 0x0f) {
            // now observed:
            // 11 - reduces D by 1
            // 12 - reduces E by 1
            // 41 - adds E to D
            // 42 - adds D to E
            // 51 - increases D by 1
            // 52 - increases E by 1
            if (data == 0x51) {
                ramd++;
            } else if (data == 0x12) {
                rame--;
            } else {
                char msg[69];
                sprintf(msg,"UNKNOWN RAM BANK F WRITE. %02x to %04x", rambankno, data, address);
                debug_print(msg);
            }
        }

        char msg[69];
        sprintf(msg,"%02x RAM BANK WRITE. %02x to %04x", rambankno, data, address);
        debug_win(msg);
        return;
    }
    MbcNin3::writeMemory(address, data);
}

void MbcUnlPokeJadeDia::resetVars(bool preserveMulticartState) {
    rambankno = 0;
    ramd = 0;
    rame = 0;
    AbstractMbc::resetVars(preserveMulticartState);
}

void MbcUnlPokeJadeDia::readMbcSpecificVarsFromStateFile(FILE *statefile) {
    fread(&(rambankno), sizeof(byte), 1, statefile);
    fread(&(ramd), sizeof(byte), 1, statefile);
    fread(&(rame), sizeof(byte), 1, statefile);
}

void MbcUnlPokeJadeDia::writeMbcSpecificVarsToStateFile(FILE *statefile) {
    fwrite(&(rambankno), sizeof(byte), 1, statefile);
    fwrite(&(ramd), sizeof(byte), 1, statefile);
    fwrite(&(rame), sizeof(byte), 1, statefile);
}
