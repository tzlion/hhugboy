
#include "MbcUnlMakonOld2.h"

MbcUnlMakonOld2::MbcUnlMakonOld2(int originalRomSize) : MbcUnlMakonOld1(originalRomSize)
{
}

void MbcUnlMakonOld2::writeMemory(unsigned short address, register byte data) {

    if (address >= 0x2000 && address <= 0x3FFF) {
        if (data == 0) data = 1; // MBC3 stylez
        if (isWeirdMode) {
            data = switchOrder(data,flippo2);
        }
        setRom1Bank(data);
        return;
    }

    MbcUnlMakonOld1::writeMemory(address, data);
}
