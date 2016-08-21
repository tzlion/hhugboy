//
// Created by Alex on 20/08/2016.
//

#include "MbcUnlSintax.h"

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
    mbc5Write(address,data,false,true);
}
