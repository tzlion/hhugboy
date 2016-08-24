//
// Created by Alex on 21/08/2016.
//
#include "MbcNin5.h"

#include <stdio.h>
#include "../../GB.h"
#include "../../main.h"
// ^ can we not

// todo: ugh @ this
void MbcNin5::mbc5Write(register unsigned short address, register byte data, bool isNiutoude, bool isSintax) {

    if(address < 0x2000)// Is it a RAM bank enable/disable?
    {
        RAMenable = ( (data&0x0A) == 0x0A ? 1 : 0);
        return;
    }

    if(address < 0x3000)
    {
        // 2100 needs to be not-ignored (for Cannon Fodder's sound)
        // but 2180 DOES need to be ignored (for FF DX3)
        // Determined to find the right number here
        if (isNiutoude && address > 0x2100) {
            return;
        }
        byte origData = data;

        if (isSintax) {
            switch(sintax_mode & 0x0f) {
                // Maybe these could go in a config file, so new ones can be added easily?
                case 0x0D: {
                    byte flips[] = {6,7,0,1,2,3,4,5};
                    data = switchOrder( data, flips );
                    //data = ((data & 0x03) << 6 ) + ( data >> 2 );
                    break;
                }
                case 0x09: {
                    //	byte flips[] = {4,5,2,3,7,6,1,0}; // Monkey..no
                    byte flips[] = {3,2,5,4,7,6,1,0};
                    data = switchOrder( data, flips );
                    break;
                }

                case 0x00: { // 0x10=lion 0x00 hmmmmm // 1 and 0 unconfirmed
                    byte flips[] = {0,7,2,1,4,3,6,5};
                    data = switchOrder( data, flips );
                    break;
                }

                case 0x01: {
                    byte flips[] = {7,6,1,0,3,2,5,4};
                    data = switchOrder( data, flips );
                    break;
                }

                case 0x05: {
                    byte flips[] = {0,1,6,7,4,5,2,3}; // Not 100% on this one
                    data = switchOrder( data, flips );
                    break;
                }

                case 0x07: {
                    byte flips[] = {5,7,4,6,2,3,0,1}; // 5 and 7 unconfirmed
                    data = switchOrder( data, flips );
                    break;
                }

                case 0x0B: {
                    byte flips[] = {5,4,7,6,1,0,3,2}; // 5 and 6 unconfirmed
                    data = switchOrder( data, flips );
                    break;
                }

            }

            setXorForBank(origData);

        }


        // Set current xor so we dont have to figure it out on every read

        rom_bank = data|(MBChi<<8);
        cart_address = rom_bank<<14;

        cart_address &= rom_size_mask[(*gbRom)->ROMsize];

        cart_address += multicartOffset;

        MBClo = data;

        gbMemMap[0x4] = &(*gbCartridge)[cart_address];
        gbMemMap[0x5] = &(*gbCartridge)[cart_address+0x1000];
        gbMemMap[0x6] = &(*gbCartridge)[cart_address+0x2000];
        gbMemMap[0x7] = &(*gbCartridge)[cart_address+0x3000];

        //  if(origData == 0x69) {
        //    	char buff[100];
        //		sprintf(buff,"%X %X %X",origData,data,cart_address);
        //		debug_print(buff);
        //  }



        return;
    }

    if(address < 0x4000) // Is it a ROM bank switch?
    {
        data = data&1;

        rom_bank = MBClo|(data<<8);

        cart_address = rom_bank<<14;

        cart_address &= rom_size_mask[(*gbRom)->ROMsize];

        cart_address += multicartOffset;

        MBChi = data;

        gbMemMap[0x4] = &(*gbCartridge)[cart_address];
        gbMemMap[0x5] = &(*gbCartridge)[cart_address+0x1000];
        gbMemMap[0x6] = &(*gbCartridge)[cart_address+0x2000];
        gbMemMap[0x7] = &(*gbCartridge)[cart_address+0x3000];

        return;
    }

    if(address < 0x6000) // Is it a RAM bank switch?
    {

        // sintaxs not entirely understood addressing thing hi

        // check sintax_mode was not already set; if it was, ignore it (otherwise Metal Max breaks)
        if (isSintax && sintax_mode ==0 && address >= 0x5000 ) {

            switch(0x0F & data) {
                case 0x0D: // old
                case 0x09: // ???
                case 0x00:// case 0x10: // LiON, GoldenSun
                case 0x01: // LANGRISSER
                case 0x05: // Maple, PK Platinum
                case 0x07: // Bynasty5
                case 0x0B: // Shaolin
                    // These are all supported
                    break;

                default:
                    char buff[100];
                    sprintf(buff,"Unknown Sintax Mode %X Addr %X - probably won't work!",data,address);
                    debug_print(buff);
                    break;
            }
            sintax_mode=data;

            mbc5Write(0x2000,01,false,true); // force a fake bank switch

            return;

        }

        if((*gbRom)->rumble)
        {
            if(data&0x08)
                *gbRumbleCounter = 4;
            data &= 0x07;
        }

        if((*gbRom)->RAMsize <= 2) // no need to change it if there isn't over 8KB ram
            return;

        data &= 0x0F;

        if(data > maxRAMbank[(*gbRom)->RAMsize])
            data = maxRAMbank[(*gbRom)->RAMsize];

        ram_bank = data;

        int madr = (data<<13) + multicartRamOffset;
        gbMemMap[0xA] = &(*gbCartRam)[madr];
        gbMemMap[0xB] = &(*gbCartRam)[madr+0x1000];
        return;
    }

    if(address<0x8000)
    {
        if ( isSintax && address >= 0x7000 ) {

            int xorNo = ( address & 0x00F0 ) >> 4;
            switch (xorNo) {
                case 2:
                    sintax_xor2 = data;
                    break;
                case 3:
                    sintax_xor3 = data;
                    break;
                case 4:
                    sintax_xor4 = data;
                    break;
                case 5:
                    sintax_xor5 = data;
                    break;
            }

            if (*gbRomBankXor == 0 ) {
                setXorForBank(4);
            }


            return;

        }


        /*  if(++bc_select == 2 && rom->ROMsize>1)
          {
             MBChi = (data&0xFF);

             cart_address = (MBChi<<1)<<14;

             mem_map[0x0] = &cartridge[cart_address];
             mem_map[0x1] = &cartridge[cart_address+0x1000];
             mem_map[0x2] = &cartridge[cart_address+0x2000];
             mem_map[0x3] = &cartridge[cart_address+0x3000];

             mem_map[0x4] = &cartridge[cart_address+0x4000];
             mem_map[0x5] = &cartridge[cart_address+0x5000];
             mem_map[0x6] = &cartridge[cart_address+0x6000];
             mem_map[0x7] = &cartridge[cart_address+0x7000];
          }*/
        return;
    }

    /*  if(address >= 0xA000 && address < 0xC000)
      {
         if(!RAMenable || !rom->RAMsize)
            return;
      }*/

    gbMemMap[address>>12][address&0x0FFF] = data;

}

void MbcNin5::writeMemory(unsigned short address, register byte data) {
    mbc5Write(address, data, false, false);
}

// This should belong to sintax
void MbcNin5::setXorForBank(byte bankNo)
{
    switch(bankNo & 0x0F) {
        case 0x00: case 0x04: case 0x08: case 0x0C:
            *gbRomBankXor = sintax_xor2;
            break;
        case 0x01: case 0x05: case 0x09: case 0x0D:
            *gbRomBankXor = sintax_xor3;
            break;
        case 0x02: case 0x06: case 0x0A: case 0x0E:
            *gbRomBankXor = sintax_xor4;
            break;
        case 0x03: case 0x07: case 0x0B: case 0x0F:
            *gbRomBankXor = sintax_xor5;
            break;
    }

    //char buff[200];
    //	sprintf(buff,"bank no %x abbr %x xor %x",bankNo,bankNo&0x0F,mbc->rom_bank_xor);
    //	debug_print(buff);
}