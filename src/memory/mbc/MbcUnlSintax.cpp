/*
 * Additional mapper support for hhugboy emulator
 * by taizou 2013-2016
 * This file released under Creative Commons CC0 https://creativecommons.org/publicdomain/zero/1.0/legalcode
 *
 * As part of the hhugboy project it is also licensed under the GNU General Public License v2
 * See "license.txt" in the project root
 */

#include <cstdio>
#include "MbcUnlSintax.h"
#include "../../GB.h"

byte MbcUnlSintax::readMemory(register unsigned short address) {

    if(address >= 0x4000 && address < 0x8000)
    {
        byte data = gbMemMap[address>>12][address&0x0FFF];
        return  data ^ romBankXor;
    }

    return gbMemMap[address>>12][address&0x0FFF];
}

void MbcUnlSintax::writeMemory(unsigned short address, register byte data) {

    if(address>= 0x2000 && address < 0x3000)
    {
        sintaxBankNo = data;

        byte* romBankNoReordering;

        switch(sintaxMode & 0x0f) {
            case 0x0D:
                romBankNoReordering = reordering0d;
                break;
            case 0x09:
                romBankNoReordering = reordering09;
                break;
            case 0x00:
                romBankNoReordering = reordering00;
                break;
            case 0x01:
                romBankNoReordering = reordering01;
                break;
            case 0x05:
                romBankNoReordering = reordering05;
                break;
            case 0x07:
                romBankNoReordering = reordering07;
                break;
            case 0x0B:
                romBankNoReordering = reordering0b;
                break;
            case 0x0F:
            default:
                romBankNoReordering = noReordering;
        }

        data = switchOrder(data, romBankNoReordering);

        setXorForBank(sintaxBankNo);
    }

    if( ( address & 0xF0F0 ) == 0x5010 ) {

        // contrary to previous belief it IS possible to change the mode after setting it initially
        // The reason Metal Max was breaking is because it only recognises writes to 5x1x
        // and that game writes to a bunch of other 5xxx addresses before battles

        sintaxMode = (byte)(0x0F & data);

        switch (sintaxMode) {
            // Supported modes
            case 0x00: // Lion King, Golden Sun
            case 0x01: // Langrisser
            case 0x05: // Maple Story, Pokemon Platinum
            case 0x07: // Bynasty Warriors 5
            case 0x09: // ???
            case 0x0B: // Shaolin Legend
            case 0x0D: // Older games
            case 0x0F: // Default mode, no reordering
                break;
            default:
                char buff[100];
                sprintf(buff, "Unknown Sintax Mode %X Addr %X - probably won't work!", data, address);
                debug_print(buff);
                break;
        }

        writeMemory(0x2000, sintaxBankNo); // fake a bank switch to select the correct bank

        return;

    }

    if (address >= 0x7000 && address < 0x8000) {

        int xorNo = (address & 0x00F0) >> 4;
        switch (xorNo) {
            case 2:
                sintaxXor00 = data;
                break;
            case 3:
                sintaxXor01 = data;
                break;
            case 4:
                sintaxXor02 = data;
                break;
            case 5:
                sintaxXor03 = data;
                break;
        }

        // xor is applied immediately to the current bank
        setXorForBank(sintaxBankNo);

        return;
    }


    MbcNin5::writeMemory(address, data);

}

MbcUnlSintax::MbcUnlSintax() :
        sintaxMode(0),
        sintaxXor00(0),
        sintaxXor01(0),
        sintaxXor02(0),
        sintaxXor03(0),
        sintaxBankNo(1) {

}

void MbcUnlSintax::resetVars(bool preserveMulticartState) {

    sintaxMode = 0;
    sintaxXor00 = sintaxXor01 = sintaxXor02 = sintaxXor03 = 0;

    romBankXor = 0;

    AbstractMbc::resetVars(preserveMulticartState);
}

void MbcUnlSintax::readMbcSpecificVarsFromStateFile(FILE *statefile) {
    fread(&(sintaxMode), sizeof(byte), 1, statefile);
    fread(&(sintaxXor00), sizeof(byte), 1, statefile);
    fread(&(sintaxXor01), sizeof(byte), 1, statefile);
    fread(&(sintaxXor02), sizeof(byte), 1, statefile);
    fread(&(sintaxXor03), sizeof(byte), 1, statefile);
    fread(&(romBankXor), sizeof(byte), 1, statefile);
}

void MbcUnlSintax::writeMbcSpecificVarsToStateFile(FILE *statefile) {
    fwrite(&(sintaxMode), sizeof(byte), 1, statefile);
    fwrite(&(sintaxXor00), sizeof(byte), 1, statefile);
    fwrite(&(sintaxXor01), sizeof(byte), 1, statefile);
    fwrite(&(sintaxXor02), sizeof(byte), 1, statefile);
    fwrite(&(sintaxXor03), sizeof(byte), 1, statefile);
    fwrite(&(romBankXor), sizeof(byte), 1, statefile);
}

void MbcUnlSintax::setXorForBank(byte bankNo)
{
    switch(bankNo & 0x03) {
        case 0x00:
            romBankXor = sintaxXor00;
            break;
        case 0x01:
            romBankXor = sintaxXor01;
            break;
        case 0x02:
            romBankXor = sintaxXor02;
            break;
        case 0x03:
            romBankXor = sintaxXor03;
            break;
    }
}