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
        byte origData = data;

        byte* romBankNoReordering;

        switch(sintax_mode & 0x0f) {
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
            default:
                romBankNoReordering = noReordering;
        }

        data = switchOrder(data, romBankNoReordering);

        setXorForBank(origData);
    }

    if(address >= 0x5000 && address < 0x6000) {

        // check mode was not already set; if it was, ignore it (otherwise Metal Max breaks)
        if (sintax_mode == 0) {

            switch (0x0F & data) {
                // Supported modes
                case 0x00: // Lion King, Golden Sun
                case 0x01: // Langrisser
                case 0x05: // Maple Story, Pokemon Platinum
                case 0x07: // Bynasty Warriors 5
                case 0x09: // ???
                case 0x0B: // Shaolin Legend
                case 0x0D: // Older games
                    break;
                default:
                    char buff[100];
                    sprintf(buff, "Unknown Sintax Mode %X Addr %X - probably won't work!", data, address);
                    debug_print(buff);
                    break;
            }
            sintax_mode = data;

            writeMemory(0x2000, 01); // fake a bank switch to select the correct bank 1

            return;
        }
    }

    if (address >= 0x7000 && address < 0x8000) {

        int xorNo = (address & 0x00F0) >> 4;
        switch (xorNo) {
            case 2:
                sintax_xor2 = data;
                break;
            case 3:
                sintax_xor3 = data;
                break;
            case 4:
                sintax_xor4 = data;
                break;
            case 5:
                sintax_xor5 = data;
                break;
        }

        if (romBankXor == 0) {
            setXorForBank(4);
        }


        return;
    }


    MbcNin5::writeMemory(address, data);

}

MbcUnlSintax::MbcUnlSintax() :
        sintax_mode(0),
        sintax_xor2(0),
        sintax_xor3(0),
        sintax_xor4(0),
        sintax_xor5(0) {

}

void MbcUnlSintax::resetVars(bool preserveMulticartState) {

    sintax_mode = 0;
    sintax_xor2 = sintax_xor3 = sintax_xor4 = sintax_xor5 = 0;

    romBankXor = 0;

    AbstractMbc::resetVars(preserveMulticartState);
}

void MbcUnlSintax::readMbcSpecificVarsFromStateFile(FILE *statefile) {
    fread(&(sintax_mode), sizeof(byte), 1, statefile);
    fread(&(sintax_xor2), sizeof(byte), 1, statefile);
    fread(&(sintax_xor3), sizeof(byte), 1, statefile);
    fread(&(sintax_xor4), sizeof(byte), 1, statefile);
    fread(&(sintax_xor5), sizeof(byte), 1, statefile);
    fread(&(romBankXor), sizeof(byte), 1, statefile);
}

void MbcUnlSintax::writeMbcSpecificVarsToStateFile(FILE *statefile) {
    fwrite(&(sintax_mode), sizeof(byte), 1, statefile);
    fwrite(&(sintax_xor2), sizeof(byte), 1, statefile);
    fwrite(&(sintax_xor3), sizeof(byte), 1, statefile);
    fwrite(&(sintax_xor4), sizeof(byte), 1, statefile);
    fwrite(&(sintax_xor5), sizeof(byte), 1, statefile);
    fwrite(&(romBankXor), sizeof(byte), 1, statefile);
}

void MbcUnlSintax::setXorForBank(byte bankNo)
{
    switch(bankNo & 0x0F) {
        case 0x00: case 0x04: case 0x08: case 0x0C:
            romBankXor = sintax_xor2;
            break;
        case 0x01: case 0x05: case 0x09: case 0x0D:
            romBankXor = sintax_xor3;
            break;
        case 0x02: case 0x06: case 0x0A: case 0x0E:
            romBankXor = sintax_xor4;
            break;
        case 0x03: case 0x07: case 0x0B: case 0x0F:
            romBankXor = sintax_xor5;
            break;
    }
}