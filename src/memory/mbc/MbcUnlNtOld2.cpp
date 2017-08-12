
#include "MbcUnlNtOld2.h"

MbcUnlNtOld2::MbcUnlNtOld2(int originalRomSize) : MbcUnlNtOld1(originalRomSize)
{
}

void MbcUnlNtOld2::writeMemory(unsigned short address, register byte data) {

    if (address >= 0x2000 && address <= 0x3FFF) {
        if (data == 0) data = 1; // MBC3 stylez
        if (isWeirdMode) {
            data = switchOrder(data,flippo2);
        }
        setRom1Bank(data);
        return;
    }

    if (address >= 0x4000 && address <= 0x4FFF) {
        // todo later. rumble in the jungle
        if((data & 0x02) == 0x02) {
            // Rumble ON
        } else {
            // Rumble OFF
        }
    }

    MbcUnlNtOld1::writeMemory(address, data);
}
