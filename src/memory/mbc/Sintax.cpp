//
// Created by Alex on 20/08/2016.
//

#include "Sintax.h"

byte Sintax::readMemory(register unsigned short address) {

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
