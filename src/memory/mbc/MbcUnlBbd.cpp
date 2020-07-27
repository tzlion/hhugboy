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

        return swapDataByte(data);
    }

    return gbMemMap[address>>12][address&0x0FFF];
}

void MbcUnlBbd::writeMemory(unsigned short address, register byte data) {

    if ( ( address & 0xF0FF ) == 0x2080 ) {

        bankSwapMode = (byte)(data & 0x07);

        char buff[100];
        sprintf(buff,"BBD bank reorder mode: %X", data);
        debug_win(buff);

        if ( !isBankSwapModeSupported() ) { // 00 = normal
            sprintf(buff,"Bank reorder mode unsupported - %02x",bankSwapMode);
            debug_print(buff);
        }

    } else if ( ( address & 0xF0FF ) == 0x2001 ) {

        bitSwapMode = (byte)(data & 0x07);

        char buff[100];
        sprintf(buff,"BBD bit scramble mode: %X", data);
        debug_win(buff);

        if ( !isDataSwapModeSupported() ) { // 00 = normal
            sprintf(buff,"Bit scramble mode unsupported - %02x",bitSwapMode);
            debug_print(buff);
        }

    } else if ( ( address & 0xF0FF ) == 0x2000 ) {

        data = swapBankByte( data );

    }

    MbcNin5::writeMemory(address, data);

}

MbcUnlBbd::MbcUnlBbd() :
        bitSwapMode(0),
        bankSwapMode(0)
 {

}

MbcUnlBbd::MbcUnlBbd( byte bbdBitSwapMode, byte bbdBankSwapMode ) : bitSwapMode( bbdBitSwapMode ),  bankSwapMode( bbdBankSwapMode )
{}

void MbcUnlBbd::resetVars(bool preserveMulticartState) {
    bitSwapMode = 0;
    bankSwapMode = 0;
    AbstractMbc::resetVars(preserveMulticartState);
}

    void MbcUnlBbd::readMbcSpecificVarsFromStateFile(FILE *statefile) {
    fread(&(bitSwapMode), sizeof(byte), 1, statefile);
    fread(&(bankSwapMode), sizeof(byte), 1, statefile);
}

void MbcUnlBbd::writeMbcSpecificVarsToStateFile(FILE *statefile) {
    fwrite(&(bitSwapMode), sizeof(byte), 1, statefile);
    fwrite(&(bankSwapMode), sizeof(byte), 1, statefile);
}

byte MbcUnlBbd::swapDataByte(byte data) {
    return switchOrder( data, bbdDataReordering[bitSwapMode] );
}

byte MbcUnlBbd::swapBankByte(byte data) {
    return switchOrder(data, bbdBankReordering[bankSwapMode]);
}

bool MbcUnlBbd::isDataSwapModeSupported() {
    return ( bitSwapMode == 0x07 || bitSwapMode == 0x05 || bitSwapMode == 0x04 || bitSwapMode == 0x00 );
}

bool MbcUnlBbd::isBankSwapModeSupported() {
    return ( bankSwapMode == 0x03 || bankSwapMode == 0x05 || bankSwapMode == 0x00 );
}
