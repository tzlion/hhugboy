#include <cstdio>
#include "MbcUnlMakonOld.h"
#include "../../debug.h"



void MbcUnlMakonOld::writeMemory(unsigned short address, register byte data) {

    // Maybe this should extend MBC1 idk

    if (address >= 0x2000 && address <= 0x3FFF) {

        if (data == 0) data = 1; // MBC1 stylez

        if (isWeirdMode) {

            byte oldata=data;

       //     char ass[69];
         //   sprintf(ass,"%02x",data);
           // if(data>=0x08)debug_print(ass);

           //byte flippo[8] = {0,1,2,4,3,6,5,7}; // rocco
            byte flippo[8] = {0,1,2,3,4,7,5,6}; // mario??????? doesnt work


            data = switchOrder(data,flippo);

           // data &= 0x1f; // mario????

            char ass[69];
            sprintf(ass,"Mapped %02x to %02x",oldata,data);
            //debug_print(ass);
        }

        rom_bank = data;

        int bankAddress = rom_bank<<14;

        bankAddress &= rom_size_mask[(*gbRom)->ROMsize];

        bankAddress += multicartOffset;

        gbMemMap[0x4] = &(*gbCartridge)[bankAddress];
        gbMemMap[0x5] = &(*gbCartridge)[bankAddress+0x1000];
        gbMemMap[0x6] = &(*gbCartridge)[bankAddress+0x2000];
        gbMemMap[0x7] = &(*gbCartridge)[bankAddress+0x3000];

        return;
    }

    if (address >= 0x5000 && address <= 0x5FFF) {
        if (data == 0x10 &&( address == 0x50EF || address == 0x5003)) {
            isWeirdMode = true;
            // bripro observed write of 20 to 5xxx tho.. and writes to 5003 too
            // OK so when loading the level it writes 00 to 5003 hm
            // Maybe check this behav for addresses e.g. is it addres & 5003 = whatever?
        } else if (data == 0x00 && address == 0x5003) {
            isWeirdMode = false;
        } else if (address == 0x5001) {
            data &= 0x3f;
            multicartOffset = (data << 0x0f);
            //char ass[69];
            //sprintf(ass,"%02x",data);
            //debug_print(ass);
            if(multicartOffset>0) {
                resetRomMemoryMap(true);
            }
        } else if (address == 0x5002) {
            (*gbRom)->ROMsize = 0x04; // dubious but works 5now
        } //else {
           // char ass[69];
         //   sprintf(ass,"%02x to %04x",data,address);
          //  debug_print(ass);
        //}
        return;
    }

    if (address < 0x8000) {
        // ignore anything else in the cart area for now
        return;
    }

    gbMemMap[address>>12][address&0x0FFF] = data;
}

MbcUnlMakonOld::MbcUnlMakonOld() :
        isWeirdMode(false) {
}

void MbcUnlMakonOld::resetVars(bool preserveMulticartState) {
    isWeirdMode = false;
    AbstractMbc::resetVars(preserveMulticartState);
}

void MbcUnlMakonOld::readMbcSpecificVarsFromStateFile(FILE *statefile) {
    fread(&(isWeirdMode), sizeof(bool), 1, statefile);
}

void MbcUnlMakonOld::writeMbcSpecificVarsToStateFile(FILE *statefile) {
    fwrite(&(isWeirdMode), sizeof(bool), 1, statefile);
}

