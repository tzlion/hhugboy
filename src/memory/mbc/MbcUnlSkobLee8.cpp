/*
 * Additional mapper support for hhugboy emulator
 * by taizou 2024
 * This file released under Creative Commons CC0 https://creativecommons.org/publicdomain/zero/1.0/legalcode
 *
 * As part of the hhugboy project it is also licensed under the GNU General Public License v2
 * See "license.txt" in the project root
 */

#include <cstdio>
#include "MbcUnlSkobLee8.h"
#include "../../GB.h"

byte MbcUnlSkobLee8::readMemory(register unsigned short address) {

    if(address >= 0x4000 && address < 0x8000)
    {
        byte data = gbMemMap[address>>12][address&0x0FFF];
        return  data ^ romBankXor;
    }

    return MbcNin5_LogoSwitch::readMemory(address);
}

void MbcUnlSkobLee8::writeMemory(unsigned short address, register byte data) {

    if(address>= 0x2000 && address < 0x3000)
    {
        requestedBankNo = data;

        byte* romBankNoReordering;

        switch(bankReorderMode & 0x07) {
            case 0x05:
                romBankNoReordering = reordering05;
                break;
            case 0x07:
                romBankNoReordering = reordering07;
                break;
            case 0x00:
                romBankNoReordering = noReordering;
        }

        data = switchOrder(data, romBankNoReordering);

        setXorForBank(data);
    }

    if( ( address & 0xF003 ) == 0x5001 ) {

        // FFX is not observed to set reorder mode on the fly but the mapper does support it to some extent

        bankReorderMode = (byte)(0x07 & data);

        char buff[100];
        sprintf(buff,"Bank reorder mode: %X", data);
        debug_win(buff);

        switch (bankReorderMode) {
            // Supported modes
            case 0x00: // None
            case 0x05: // Digimon D-3
            case 0x07: // Default mode used for FFX
                break;
            default:
                sprintf(buff, "Bank reorder mode unsupported - %X", data);
                debug_print(buff);
                break;
        }

        writeMemory(0x2000, requestedBankNo); // fake a bank switch to select the correct bank

        return;

    }

    if (address >= 0x7000 && address < 0x8000) {

        char buff[100];

        int xorNo = (address & 0x0003);
        switch (xorNo) {
            case 0:
                xor00 = data;
                sprintf(buff, "XOR 0: %X", data);
                debug_win(buff);
                break;
            case 1:
                xor01 = data;
                sprintf(buff, "XOR 1: %X", data);
                debug_win(buff);
                break;
            case 2:
                xor02 = data;
                sprintf(buff, "XOR 2: %X", data);
                debug_win(buff);
                break;
            case 3:
                xor03 = data;
                sprintf(buff, "XOR 3: %X", data);
                debug_win(buff);
                break;
        }

        // xor is applied immediately to the current bank
        setXorForBank(rom_bank);

        return;
    }


    MbcNin5::writeMemory(address, data);

}

MbcUnlSkobLee8::MbcUnlSkobLee8() :
// todo: at least ffx boots up with some default xor already applied, can we figure out/do we care what it is
        bankReorderMode(0x0f),
        xor00(0),
        xor01(0),
        xor02(0),
        xor03(0),
        requestedBankNo(1),
        romBankXor(0) {

}

void MbcUnlSkobLee8::resetVars(bool preserveMulticartState) {

    bankReorderMode = 0x0f;
    xor00 = xor01 = xor02 = xor03 = 0;

    romBankXor = 0;

    MbcNin5_LogoSwitch::resetVars(preserveMulticartState);
}

void MbcUnlSkobLee8::readMbcSpecificVarsFromStateFile(FILE *statefile) {
    fread(&(bankReorderMode), sizeof(byte), 1, statefile);
    fread(&(xor00), sizeof(byte), 1, statefile);
    fread(&(xor01), sizeof(byte), 1, statefile);
    fread(&(xor02), sizeof(byte), 1, statefile);
    fread(&(xor03), sizeof(byte), 1, statefile);
    fread(&(romBankXor), sizeof(byte), 1, statefile);
    MbcNin5_LogoSwitch::readMbcSpecificVarsFromStateFile(statefile);
}

void MbcUnlSkobLee8::writeMbcSpecificVarsToStateFile(FILE *statefile) {
    fwrite(&(bankReorderMode), sizeof(byte), 1, statefile);
    fwrite(&(xor00), sizeof(byte), 1, statefile);
    fwrite(&(xor01), sizeof(byte), 1, statefile);
    fwrite(&(xor02), sizeof(byte), 1, statefile);
    fwrite(&(xor03), sizeof(byte), 1, statefile);
    fwrite(&(romBankXor), sizeof(byte), 1, statefile);
    MbcNin5_LogoSwitch::writeMbcSpecificVarsToStateFile(statefile);
}

void MbcUnlSkobLee8::setXorForBank(byte bankNo)
{
    switch(bankNo & 0x03) {
        case 0x00:
            romBankXor = xor00;
            break;
        case 0x01:
            romBankXor = xor01;
            break;
        case 0x02:
            romBankXor = xor02;
            break;
        case 0x03:
            romBankXor = xor03;
            break;
    }
}