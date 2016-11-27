/*
 * Additional mapper support for hhugboy emulator
 * by taizou 2016
 * This file released under Creative Commons CC0 https://creativecommons.org/publicdomain/zero/1.0/legalcode
 *
 * As part of the hhugboy project it is also licensed under the GNU General Public License v2
 * See "license.txt" in the project root
 */

// todo this was mostly copied from bbd

#include <cstdio>
#include "MbcUnlHitek.h"
#include "../../debug.h"

byte MbcUnlHitek::readMemory(register unsigned short address) {

    if(address >= 0x4000 && address < 0x8000)
    {
        // Do the reorder..
        byte data = gbMemMap[address>>12][address&0x0FFF];

        return switchOrder(data,dataReordering[bbdBitSwapMode]);
    }

    return gbMemMap[address>>12][address&0x0FFF];
}

void MbcUnlHitek::writeMemory(unsigned short address, register byte data) {

    if  ( ( address & 0xF000 ) == 0x3000 ) {
        // 01 to 3001 'unlocks' but idc for now
        return;
    }

    if ( ( address & 0xF0FF ) == 0x2080 ) {

        bbdBankSwapMode = (byte)(data & 0x07);

    } else if ( ( address & 0xF0FF ) == 0x2001 ) {

        bbdBitSwapMode = (byte)(data & 0x07);

    } else if ( ( address & 0xF0FF ) == 0x2000 ) {

        data = switchOrder(data,bankReordering[bbdBankSwapMode]);

        // also: 00 loads bank 01 like MBC3

    }

    MbcNin5::writeMemory(address, data);

}

MbcUnlHitek::MbcUnlHitek() :
        bbdBitSwapMode(7), // default
        bbdBankSwapMode(7)
{
}

void MbcUnlHitek::resetVars(bool preserveMulticartState) {
    bbdBitSwapMode = 7;
    bbdBankSwapMode = 7;
    AbstractMbc::resetVars(preserveMulticartState);
}

void MbcUnlHitek::readMbcSpecificVarsFromStateFile(FILE *statefile) {
    fread(&(bbdBitSwapMode), sizeof(byte), 1, statefile);
    fread(&(bbdBankSwapMode), sizeof(byte), 1, statefile);
}

void MbcUnlHitek::writeMbcSpecificVarsToStateFile(FILE *statefile) {
    fwrite(&(bbdBitSwapMode), sizeof(byte), 1, statefile);
    fwrite(&(bbdBankSwapMode), sizeof(byte), 1, statefile);
}