//
// Created by Alex on 20/08/2016.
//

#include "MbcNinCamera.h"

byte MbcNinCamera::readMemory(register unsigned short address) {
    if(address >= 0xA000 && address < 0xC000)
    {
        if(cameraIO) // Camera I/O register in cart RAM area
        {
            if(address == 0xA000)
                return 0x00; // Hardware is ready
            else
                return 0xFF; // others write only
        }
    }

    return BasicMbc::readMemory(address);
}
