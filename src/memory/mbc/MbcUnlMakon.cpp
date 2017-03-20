//
// Created by Alex on 20/03/2017.
//

#include <cstdio>
#include "MbcUnlMakon.h"
#include "../../debug.h"

void MbcUnlMakon::writeMemory(unsigned short address, register byte data) {
    if ( (address&0xFF00)== 0x1400 && data == 0x55 ) {
        theMode = true;
        return;
    }
    if ( theMode ) {
        if ( (address&0xFF00)== 0x2000 ) {
            int bankStartAddr = data<<0x0D;
            bankStartAddr &= rom_size_mask[(*gbRom)->ROMsize];
            if ( bankStartAddr < 0x4000 ) bankStartAddr += 0x4000;
            gbMemMap[0x4] = &(*gbCartridge)[bankStartAddr];
            gbMemMap[0x5] = &(*gbCartridge)[bankStartAddr+0x1000];
            return;
        }
        if ( (address&0xFF00)== 0x2400 ) {
            int bankStartAddr = data<<0x0D;
            bankStartAddr &= rom_size_mask[(*gbRom)->ROMsize];
            if ( bankStartAddr < 0x4000 ) bankStartAddr += 0x4000;
            gbMemMap[0x6] = &(*gbCartridge)[bankStartAddr+0x0000];
            gbMemMap[0x7] = &(*gbCartridge)[bankStartAddr+0x1000];
            return;
        }
    }

     //if(data == 0x69) {
      // 	char buff[100];
    	//	sprintf(buff,"Unk write %02X %04X",data,address);
    	//	debug_print(buff);
      //}

    MbcNin5::writeMemory(address, data);
}
