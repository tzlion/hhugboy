//
// Created by Alex on 20/08/2016.
//

#include <cstdio>
#include "MbcUnlSintax.h"
#include "../../GB.h"

byte MbcUnlSintax::readMemory(register unsigned short address) {

    if(address >= 0x4000 && address < 0x8000)
    {
        byte data = gbMemMap[address>>12][address&0x0FFF];

        //char buff[100];
        //sprintf(buff,"MBCLo %X Addr %X Data %X XOR %X XOR'd data %X",MBClo,address,data,rom_bank_xor, data ^ rom_bank_xor);
        //debug_print(buff);

        return  data ^ *gbRomBankXor;
    }

    return gbMemMap[address>>12][address&0x0FFF];
}

void MbcUnlSintax::writeMemory(unsigned short address, register byte data) {

    if(address>= 0x2000 && address < 0x3000)
    {
        byte origData = data;

        switch(sintax_mode & 0x0f) {
            // Maybe these could go in a config file, so new ones can be added easily?
            case 0x0D: {
                byte flips[] = {6,7,0,1,2,3,4,5};
                data = switchOrder( data, flips );
                //data = ((data & 0x03) << 6 ) + ( data >> 2 );
                break;
            }
            case 0x09: {
                //	byte flips[] = {4,5,2,3,7,6,1,0}; // Monkey..no
                byte flips[] = {3,2,5,4,7,6,1,0};
                data = switchOrder( data, flips );
                break;
            }

            case 0x00: { // 0x10=lion 0x00 hmmmmm // 1 and 0 unconfirmed
                byte flips[] = {0,7,2,1,4,3,6,5};
                data = switchOrder( data, flips );
                break;
            }

            case 0x01: {
                byte flips[] = {7,6,1,0,3,2,5,4};
                data = switchOrder( data, flips );
                break;
            }

            case 0x05: {
                byte flips[] = {0,1,6,7,4,5,2,3}; // Not 100% on this one
                data = switchOrder( data, flips );
                break;
            }

            case 0x07: {
                byte flips[] = {5,7,4,6,2,3,0,1}; // 5 and 7 unconfirmed
                data = switchOrder( data, flips );
                break;
            }

            case 0x0B: {
                byte flips[] = {5,4,7,6,1,0,3,2}; // 5 and 6 unconfirmed
                data = switchOrder( data, flips );
                break;
            }

        }

        setXorForBank(origData);
    }

    // Set current xor so we dont have to figure it out on every read

    if(address >= 0x5000 && address < 0x6000) {

        // sintaxs not entirely understood addressing thing hi

        // check sintax_mode was not already set; if it was, ignore it (otherwise Metal Max breaks)
        if (sintax_mode == 0) {

            switch (0x0F & data) {
                case 0x0D: // old
                case 0x09: // ???
                case 0x00:// case 0x10: // LiON, GoldenSun
                case 0x01: // LANGRISSER
                case 0x05: // Maple, PK Platinum
                case 0x07: // Bynasty5
                case 0x0B: // Shaolin
                    // These are all supported
                    break;

                default:
                    char buff[100];
                    sprintf(buff, "Unknown Sintax Mode %X Addr %X - probably won't work!", data, address);
                    debug_print(buff);
                    break;
            }
            sintax_mode = data;

            mbc5Write(0x2000, 01, false, true); // force a fake bank switch

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

        if (*gbRomBankXor == 0) {
            setXorForBank(4);
        }


        return;
    }


    mbc5Write(address, data, false, true);

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

    *gbRomBankXor = 0;

    AbstractMbc::resetVars(preserveMulticartState);
}

void MbcUnlSintax::readMbcSpecificVarsFromStateFile(FILE *statefile) {
    fread(&(sintax_mode), sizeof(byte), 1, statefile);
    fread(&(sintax_xor2), sizeof(byte), 1, statefile);
    fread(&(sintax_xor3), sizeof(byte), 1, statefile);
    fread(&(sintax_xor4), sizeof(byte), 1, statefile);
    fread(&(sintax_xor5), sizeof(byte), 1, statefile);
    fread(&(*gbRomBankXor), sizeof(byte), 1, statefile);
}

void MbcUnlSintax::writeMbcSpecificVarsToStateFile(FILE *statefile) {
    fwrite(&(sintax_mode), sizeof(byte), 1, statefile);
    fwrite(&(sintax_xor2), sizeof(byte), 1, statefile);
    fwrite(&(sintax_xor3), sizeof(byte), 1, statefile);
    fwrite(&(sintax_xor4), sizeof(byte), 1, statefile);
    fwrite(&(sintax_xor5), sizeof(byte), 1, statefile);
    fwrite(&(*gbRomBankXor), sizeof(byte), 1, statefile);
}

// This should belong to sintax
void MbcUnlSintax::setXorForBank(byte bankNo)
{
    switch(bankNo & 0x0F) {
        case 0x00: case 0x04: case 0x08: case 0x0C:
            *gbRomBankXor = sintax_xor2;
            break;
        case 0x01: case 0x05: case 0x09: case 0x0D:
            *gbRomBankXor = sintax_xor3;
            break;
        case 0x02: case 0x06: case 0x0A: case 0x0E:
            *gbRomBankXor = sintax_xor4;
            break;
        case 0x03: case 0x07: case 0x0B: case 0x0F:
            *gbRomBankXor = sintax_xor5;
            break;
    }

    //char buff[200];
    //	sprintf(buff,"bank no %x abbr %x xor %x",bankNo,bankNo&0x0F,mbc->rom_bank_xor);
    //	debug_print(buff);
}