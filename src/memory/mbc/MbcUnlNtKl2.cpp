
#include "MbcUnlNtKl2.h"
#include "../../debug.h"

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

    // -- rumble in the jungle --

    // On a multicart rumble needs to be enabled by the write to 5001 & 80
    // But single carts like DK have it enabled already
    if (address == 0x5001) {
        if (data & 0x80) {
            (*gbRom)->rumble = true;
        } else {
            (*gbRom)->rumble = false;
        }
    }

    if ((*gbRom)->rumble && address >= 0x4000 && address <= 0x5FFF) {
        // In initialised weird-Makon mode it works on the same writes as official rumble carts
        // In uninitialised mode it's different (so official rumble games wouldn't rumble on these carts?)
        if(isWeirdMode ? (data & 0x08) : (data & 0x02)) {
            *gbRumbleCounter += 4;
        } else {
            *gbRumbleCounter = 0;
        }
        // Note that the way this emu handles rumble overall isn't exactly the same as real carts (official or not)
        // On real carts it operates on an on/off basis e.g. write 08 to 4000 to turn it on, 00 to 4000 to turn it off
        // It's not timed/there's no "rumble counter"
        // But most games will continually write to the address anyway, for whatever reason
        // Observed this both on Top Gear Rally (official) and DK5 (not)
        // Rumble doesn't seem to be well documented in any case
        // Have tweaked both this and MBC5 to approximate this behaviour a bit closer
    }

    MbcUnlNtK11::writeMemory(address, data);
}
