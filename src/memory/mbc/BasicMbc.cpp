//
// Created by Alex on 20/08/2016.
//

#include "BasicMbc.h"

byte BasicMbc::readMemory(register unsigned short address) {

    /* if(address >= 0xA000 && address < 0xC000)
     {
        if((!rom->RAMsize && !rom->battery))
           return 0xFF;
     }*/

    //  wchar_t wrmessage[50];
    //  wsprintf(wrmessage,L"MM %X %X",superaddroffset,mem_map[0x0]);
    //  renderer.showMessage(wrmessage,60,GB1);

    /* mem_map[0x0] = &cartridge[superaddroffset];
     mem_map[0x1] = &cartridge[superaddroffset+0x1000];
     mem_map[0x2] = &cartridge[superaddroffset+0x2000];
     mem_map[0x3] = &cartridge[superaddroffset+0x3000];

     mem_map[0x4] = &cartridge[superaddroffset+0x4000];
     mem_map[0x5] = &cartridge[superaddroffset+0x5000];
     mem_map[0x6] = &cartridge[superaddroffset+0x6000];
     mem_map[0x7] = &cartridge[superaddroffset+0x7000];    */

    return gbMemMap[address>>12][address&0x0FFF];
}

void BasicMbc::resetVars() {
    // nothin
}

void BasicMbc::readNewMbcSpecificVarsFromStateFile(FILE *statefile) {

}

void BasicMbc::readOldMbcSpecificVarsFromStateFile(FILE *statefile) {

}

void BasicMbc::writeNewMbcSpecificVarsToStateFile(FILE *statefile) {

}

void BasicMbc::writeOldMbcSpecificVarsToStateFile(FILE *statefile) {

}
