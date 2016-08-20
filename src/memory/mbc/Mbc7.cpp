//
// Created by Alex on 20/08/2016.
//

#include "Mbc7.h"
#include "../../mainloop.h" // Todo: can we not pls. The sensor values actually belong to the cart!

byte Mbc7::readMemory(register unsigned short address) {

    if(address >= 0xA000 && address < 0xC000)
    {
        switch(address & 0xa0f0)
        {
            case 0xa000:
            case 0xa010:
            case 0xa060:
            case 0xa070:
                return 0;
            case 0xa020:
                // sensor X low byte
                return sensorX & 255;
            case 0xa030:
                // sensor X high byte
                return sensorX >> 8;
            case 0xa040:
                // sensor Y low byte
                return sensorY & 255;
            case 0xa050:
                // sensor Y high byte
                return sensorY >> 8;
            case 0xa080:
                return MBC7_value;
        }
        return 0xff;
    }

    return Default::readMemory(address);
}
