//
// Created by Alex on 20/03/2017.
//

#include "MbcUnlMakon.h"

void MbcUnlMakon::writeMemory(unsigned short address, register byte data) {
    if ( (address&0xFF00)== 0x1400 && data == 0x55 ) {
        theMode = true;
        return;
    }
    if ( theMode ) {
        if ( (address&0xFF00)== 0x2000 ) {
            int bankStartAddr = data<<0x0D;
            gbMemMap[0x4] = &(*gbCartridge)[bankStartAddr];
            gbMemMap[0x5] = &(*gbCartridge)[bankStartAddr+0x1000];
            return;
        }
        if ( (address&0xFF00)== 0x2400 ) {
            int bankStartAddr = data<<0x0D;
            gbMemMap[0x6] = &(*gbCartridge)[bankStartAddr+0x2000];
            gbMemMap[0x7] = &(*gbCartridge)[bankStartAddr+0x3000];
            return;
        }
    }

    MbcNin5::writeMemory(address, data);
}
