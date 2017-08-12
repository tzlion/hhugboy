
#include "MbcUnlNtKl2.h"

MbcUnlNtKl2::MbcUnlNtKl2(int originalRomSize) : MbcUnlNtK11(originalRomSize)
{
}

void MbcUnlNtKl2::writeMemory(unsigned short address, register byte data) {

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
        // doesn't seem to work the same as official rumble carts but i will investigate later
        if((data & 0x02) == 0x02) {
            // Rumble ON
        } else {
            // Rumble OFF
        }
    }

    MbcUnlNtK11::writeMemory(address, data);
}
