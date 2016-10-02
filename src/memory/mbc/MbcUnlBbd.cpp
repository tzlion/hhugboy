/*
 * Additional mapper support for hhugboy emulator
 * by taizou 2016
 * This file released under Creative Commons CC0 https://creativecommons.org/publicdomain/zero/1.0/legalcode
 *
 * As part of the hhugboy project it is also licensed under the GNU General Public License v2
 * See "license.txt" in the project root
 */

#include <cstdio>
#include "MbcUnlBbd.h"
#include "../../debug.h"

byte MbcUnlBbd::readMemory(register unsigned short address) {

    if(address >= 0x4000 && address < 0x8000)
    {
        // Do the reorder..
        byte data = gbMemMap[address>>12][address&0x0FFF];

        if ( bbdBitSwapMode == 0x07 ) {
            return switchOrder(data, dataReordering07);
        } else if ( bbdBitSwapMode == 0x05 ) {
            return switchOrder(data, dataReordering05);
        }else if ( bbdBitSwapMode == 0x04 ) {
            return switchOrder(data, dataReordering04);
        }
    }

    return gbMemMap[address>>12][address&0x0FFF];
}

void MbcUnlBbd::writeMemory(unsigned short address, register byte data) {

    // Games from BBD, Sintax and related developers (probably anything built with Gamtec's SDK) have the bank number as
    // the last byte of each bank. If that number matches the actual bank number, then this is PROBABLY a decrypted rom
    // & we don't have to apply the swapping stuff
    // This isn't really the best way to do it since if it's decrypted then it shouldn't be running through the BBD
    // mapper at all, but the way the existing rom loading/detection code is set up makes this kinda awful to do there
    // There MAY be some BBD games that this check fails on, but it works for everything dumped so far (afaik)

    if ( gbMemMap[0x7][0x0FFF] != rom_bank ) {

        if ( address == 0x2080 ) {

            bbdBankSwapMode = (byte)(data & 0x07);

            if ( bbdBankSwapMode != 0x03 && bbdBankSwapMode != 0x05 && bbdBankSwapMode != 0x00 ) { // 00 = normal
                char buff[1000];
                sprintf(buff,"BBD bankswap mode unsupported - %02x",bbdBankSwapMode);
                debug_print(buff);
            }

        } else if(address== 0x2001 ) {

            bbdBitSwapMode = (byte)(data & 0x07);
            if ( bbdBitSwapMode != 0x07 && bbdBitSwapMode != 0x05 && bbdBitSwapMode != 0x04 && bbdBitSwapMode != 0x00 ) { // 00 = normal
                char buff[1000];
                sprintf(buff,"BBD bitswap mode unsupported - %02x",bbdBitSwapMode);
                debug_print(buff);
            }

        } else if ( address == 0x2000 ) {

            if ( bbdBankSwapMode == 0x03 ) {
                data = switchOrder(data, bankReordering03);
            } else if ( bbdBankSwapMode == 0x05 ) {
                data = switchOrder(data, bankReordering05);
            }

        }

    }

    MbcNin5::writeMemory(address, data);

}

MbcUnlBbd::MbcUnlBbd() :
        bbdBitSwapMode(0),
        bbdBankSwapMode(0)
 {

}

void MbcUnlBbd::resetVars(bool preserveMulticartState) {
    bbdBitSwapMode = 0;
    bbdBankSwapMode = 0;
    AbstractMbc::resetVars(preserveMulticartState);
}

void MbcUnlBbd::readMbcSpecificVarsFromStateFile(FILE *statefile) {
    fread(&(bbdBitSwapMode), sizeof(byte), 1, statefile);
    fread(&(bbdBankSwapMode), sizeof(byte), 1, statefile);
}

void MbcUnlBbd::writeMbcSpecificVarsToStateFile(FILE *statefile) {
    fwrite(&(bbdBitSwapMode), sizeof(byte), 1, statefile);
    fwrite(&(bbdBankSwapMode), sizeof(byte), 1, statefile);
}