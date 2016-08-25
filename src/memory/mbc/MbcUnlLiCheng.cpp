//
// Created by Alex on 21/08/2016.
//

#include "MbcUnlLiCheng.h"

void MbcUnlLiCheng::writeMemory(unsigned short address, register byte data) {
    // 2100 needs to be not-ignored (for Cannon Fodder's sound)
    // but 2180 DOES need to be ignored (for FF DX3)
    // Determined to find the right number here
    if (address > 0x2100 && address < 0x3000) {
        return;
    }
    MbcNin5::writeMemory(address,data);
}
