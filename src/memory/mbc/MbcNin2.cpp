//
// Created by Alex on 21/08/2016.
//

#include "MbcNin2.h"

void MbcNin2::writeMemory(unsigned short address, register byte data) {
    if(address < 0x2000)// Is it a RAM bank enable/disable?
    {
        if(!(address&0x0100))
            RAMenable =  (data&0x0F) == 0x0A;
        return;
    }

    if(address < 0x4000) // Is it a ROM bank switch?
    {
        //if(address&0x0100)
        {
            data &= 0x0F;
            if(data==0)
                data=1;
            if(data > maxROMbank[(*gbRom)->ROMsize])
                data = maxROMbank[(*gbRom)->ROMsize];

            rom_bank = data;

            int cadr = data<<14;
            gbMemMap[0x4] = &(*gbCartridge)[cadr];
            gbMemMap[0x5] = &(*gbCartridge)[cadr+0x1000];
            gbMemMap[0x6] = &(*gbCartridge)[cadr+0x2000];
            gbMemMap[0x7] = &(*gbCartridge)[cadr+0x3000];
        }
        return;
    }

    if(address < 0x6000) // Is it a RAM bank switch?
        return;

    if(address < 0x8000)
        return;

    /*  if(address >= 0xA000 && address < 0xC000)
      {
         if(!RAMenable || !rom->battery)
            return;
      }*/

    gbMemMap[address>>12][address&0x0FFF] = data;

}
