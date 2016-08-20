//
// Created by Alex on 20/08/2016.
//

#include "Huc3.h"

byte Huc3::readMemory(register unsigned short address) {
    if(address >= 0xA000 && address < 0xC000)
    {
        if(HuC3_RAMflag >= 0x0b && HuC3_RAMflag < 0x0e)
        {
            if(HuC3_RAMflag == 0x0D)
                return 1;
            return HuC3_RAMvalue;
        }
        if(!(*gbRom)->RAMsize)
            return 0xFF;
    }

    return Default::readMemory(address);
}
