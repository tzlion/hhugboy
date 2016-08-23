//
// Created by Alex on 23/08/2016.
//

#include "MbcUnlLbMulti.h"

#include "../../GB.h"
#include "../../main.h"
// ^ can we not

#include <cstdio>


void MbcUnlLbMulti::writeMemory(unsigned short address, register byte data) {

    bool vfmulti = true;
    if ( vfmulti && !bc_select ) {

        if ( /*address & 0xF000 == 0x5000*/ address >= 0x5000 && address <= 0x5FFF ) {
            vfmultimode = data;
        }
        if ( /*address & 0xF000 == 0x7000*/ address >= 0x7000 && address <= 0x7FFF ) {
            bool effectChange = false;
            switch( vfmultimode ) {
                case 0xAA:
                    if ( vfmultibank == 0 ) {
                        vfmultibank = data;
                    } else {
                        effectChange = true;
                    }
                    break;
                case 0xBB:
                    vfmultimem = data;
                    break;
                case 0x55:
                    vfmultifinal = data;
                    effectChange = true;
                    break;
            }
            if ( effectChange ) {

                byte size = vfmultifinal & 0x07;
                byte eightMegBankNo = ( vfmultifinal & 0x08 ) >> 3; // 0 or 1
                byte doReset = ( vfmultifinal & 0x80 ) >> 7; // 0 or 1 again

                int addroffset = vfmultibank << 15;
                addroffset += (eightMegBankNo << 0x17);
                multicartOffset = addroffset;

                wchar_t wrmessage[50];
                wsprintf(wrmessage,L"MM %X %X",multicartOffset,vfmultibank);
                renderer.showMessage(wrmessage,60,GB1);

                gbMemMap[0x0] = &(*gbCartridge)[addroffset];
                gbMemMap[0x1] = &(*gbCartridge)[addroffset+0x1000];
                gbMemMap[0x2] = &(*gbCartridge)[addroffset+0x2000];
                gbMemMap[0x3] = &(*gbCartridge)[addroffset+0x3000];

                gbMemMap[0x4] = &(*gbCartridge)[addroffset+0x4000];
                gbMemMap[0x5] = &(*gbCartridge)[addroffset+0x5000];
                gbMemMap[0x6] = &(*gbCartridge)[addroffset+0x6000];
                gbMemMap[0x7] = &(*gbCartridge)[addroffset+0x7000];

                // todo: Do the memory switch
                if ( doReset ) {
                    deferredReset = true;
                }

                if ( vfmultifinal>0) bc_select = 1;

                vfmultimode=0; vfmultibank=0; vfmultimem=0; vfmultifinal = 0;

                return;
            }
        }
    }


    MbcNin5::writeMemory(address, data);
}

void MbcUnlLbMulti::resetVars(bool preserveMulticartState) {

    if ( !preserveMulticartState ) {
        vfmultimode=0;
        vfmultibank=0;
        vfmultimem=0;
        vfmultifinal=0;
    }

    AbstractMbc::resetVars(preserveMulticartState);
}

void MbcUnlLbMulti::writeMbcSpecificVarsToStateFile(FILE *statefile) {
    fwrite(&(vfmultimode), sizeof(byte), 1, statefile);
    fwrite(&(vfmultibank), sizeof(byte), 1, statefile);
    fwrite(&(vfmultimem), sizeof(byte), 1, statefile);
    fwrite(&(vfmultifinal), sizeof(byte), 1, statefile);
    fwrite(&(multicartOffset),sizeof(int),1,statefile);
    fwrite(&(bc_select),sizeof(int),1,statefile);
}

void MbcUnlLbMulti::readMbcSpecificVarsFromStateFile(FILE *statefile) {
    fread(&(vfmultimode), sizeof(byte), 1, statefile);
    fread(&(vfmultibank), sizeof(byte), 1, statefile);
    fread(&(vfmultimem), sizeof(byte), 1, statefile);
    fread(&(vfmultifinal), sizeof(byte), 1, statefile);
    fread(&(multicartOffset),sizeof(int),1,statefile);
    fread(&(bc_select),sizeof(int),1,statefile);

    resetRomMemoryMap(true);
}
