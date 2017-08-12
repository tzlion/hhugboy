/*
   hhugboy Game Boy emulator
   copyright 2013-2016 taizou

   Based on GEST
   Copyright (C) 2003-2010 TM
   This file incorporates code from VisualBoyAdvance
   Copyright (C) 1999-2004 by Forgotten

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <cstring>
#include "GB.h"
#include "config.h"
#include "strings.h"


void gb_system::processRomInfo() {

    readHeader();
    detectWeirdCarts();

    mbc->setMemoryReadWrite(rom->mbcType);
}

void gb_system::setCartridgeType(byte value)
{
    rom->carttype = value;
    rom->RTC = false;
    rom->rumble = false;
    rom->mbcType = MEMORY_DEFAULT;

    switch(value)
    {
        case 0x00: //"ROM"
            rom->battery = false;
            rom->mbcType = MEMORY_ROMONLY;
            break;

        case 0x01: //"MBC1"
            rom->battery = false;
            rom->mbcType = MEMORY_MBC1;
            break;

        case 0x02: //"MBC1+RAM"
            rom->battery = false;
            rom->mbcType = MEMORY_MBC1;
            break;

        case 0x03: //"MBC1+RAM+BATTERY"
            rom->battery = true;
            rom->mbcType = MEMORY_MBC1;
            break;

        case 0x05: //"MBC2"
            rom->battery = false;
            rom->mbcType = MEMORY_MBC2;
            break;

        case 0x06: //"MBC2+BATTERY"
            rom->battery = true;
            rom->mbcType = MEMORY_MBC2;
            break;

        case 0x08: //"ROM+RAM";
            rom->battery = false;
            rom->mbcType = MEMORY_ROMONLY;
            break;

        case 0x09: //"ROM+RAM+BATTERY"
            rom->battery = true;
            rom->mbcType = MEMORY_ROMONLY;
            break;

        case 0x0B: //"MMM01"
            rom->battery = false;
            rom->mbcType = MEMORY_MMM01;
            break;

        case 0x0C: //"MMM01+RAM"
            rom->battery = false;
            rom->mbcType = MEMORY_MMM01;
            break;

        case 0x0D: //"MMM01+RAM+BATTERY"
            rom->battery = true;
            rom->mbcType = MEMORY_MMM01;
            break;

        case 0x0F: //"MBC3+TIMER+BATTERY"
            rom->battery = true;
            rom->RTC = true;
            rom->mbcType = MEMORY_MBC3;
            break;

        case 0x10: //"MBC3+TIMER+RAM+BATTERY"
            rom->battery = true;
            rom->RTC = true;
            rom->mbcType = MEMORY_MBC3;
            break;

        case 0x11: //"MBC3"
            rom->battery = false;
            rom->mbcType = MEMORY_MBC3;
            break;

        case 0x12: //"MBC3+RAM"
            rom->battery = false;
            rom->mbcType = MEMORY_MBC3;
            break;

        case 0x13: //"MBC3+RAM+BATTERY"
            rom->battery = true;
            rom->mbcType = MEMORY_MBC3;
            break;

        case 0x15: //"MBC4"
            rom->battery = false;
            break;

        case 0x16: //"MBC4+RAM"
            rom->battery = false;
            break;

        case 0x17: //"MBC4+RAM+BATTERY"
            rom->battery = true;
            break;

        case 0x19: //"MBC5"
            rom->battery = false;
            rom->mbcType = MEMORY_MBC5;
            break;

        case 0x1A: //"MBC5+RAM"
            rom->battery = false;
            rom->mbcType = MEMORY_MBC5;
            break;

        case 0x1B: //"MBC5+RAM+BATTERY"
            rom->battery = true;
            rom->mbcType = MEMORY_MBC5;
            break;

        case 0x1C: //"MBC5+RUMBLE"
            rom->battery = false;
            rom->rumble = true;
            rom->mbcType = MEMORY_MBC5;
            break;

        case 0x1D: //"MBC5+RUMBLE+RAM"
            rom->battery = false;
            rom->rumble = true;
            rom->mbcType = MEMORY_MBC5;
            break;

        case 0x1E: //"MBC5+RUMBLE+RAM+BATTERY"
            rom->battery = true;
            rom->rumble = true;
            rom->mbcType = MEMORY_MBC5;
            break;

        case 0x22: //Kirby's Tilt'n'Tumble
            rom->battery = true;
            rom->mbcType = MEMORY_MBC7;
            break;

        case 0x59: //Game Boy Smart Card
            rom->battery = false;
            rom->mbcType = MEMORY_MBC1;
            break;

        case 0xBE: //Pocket Voice Recorder
            rom->battery = false;
            rom->ROMsize++;
            rom->mbcType = MEMORY_MBC5;
            break;

        case 0xEA: //SONIC5
            rom->battery = false;
            rom->mbcType = MEMORY_MBC1;
            break;

        case 0xFC: //"POCKET CAMERA"
            rom->battery = true;
            rom->mbcType = MEMORY_CAMERA;
            break;

        case 0xFD: //"Bandai TAMA5"
            rom->battery = true;
            rom->mbcType = MEMORY_TAMA5;
            break;

        case 0xFE: //"Hudson HuC-3"
            rom->battery = true;
            rom->mbcType = MEMORY_HUC3;
            break;

        case 0xFF: //"Hudson HuC-1+RAM+BATTERY"
            rom->battery = true;
            rom->mbcType = MEMORY_MBC1;
            break;

        default: //"Unknown"
            rom->battery = false;
            debug_print(str_table[ERROR_ROM_TYPE]);
            break;
    }
}

void gb_system::readHeader()
{
    byte rominfo[30];
    memcpy(rominfo,cartridge+0x0134,0x1C);

    int addr = 0;
    for(;addr<=14; ++addr)
        rom->name[addr] = rominfo[addr];

    if(rominfo[addr] == 0x80)
        rom->CGB = 1;
    else if(rominfo[addr] == 0xC0)
        rom->CGB = 2; // gbc only
    else
        rom->CGB = 0;
    // modes changed when reset

    ++addr; rom->newlic[0] = rominfo[addr];
    ++addr; rom->newlic[1] = rominfo[addr];

    ++addr;
    if(rominfo[addr] == 0x03)
        rom->SGB = 1;
    else
        rom->SGB = 0;

    ++addr;
    setCartridgeType(rominfo[addr]);

    byte romsize = rominfo[addr+1];
    if((romsize > 8 && romsize < 0x52) || romsize > 0x54)
        romsize = 0;
    rom->ROMsize = romsize;
    byte ramsize = rominfo[addr+2];
    rom->RAMsize = ramsize;
    if(rom->RAMsize >= 8)
        rom->RAMsize = 1;

    addr+=3; rom->destcode = rominfo[addr];
    ++addr; rom->lic = rominfo[addr];
    ++addr; rom->version = rominfo[addr];
    ++addr; rom->complement = rominfo[addr];
    ++addr; rom->checksum=(rominfo[addr]<<8);
    ++addr; rom->checksum|=rominfo[addr];

    // check complement
    byte cmpl=0;
    for(addr=0;addr<=0x19;++addr)
        cmpl+=rominfo[addr];
    cmpl+=25; rom->complementok = !cmpl;
}

unlCompatMode gb_system::detectUnlCompatMode()
{
    byte logo1[0x30];
    byte logo2[0x30];
    memcpy(logo1,cartridge+0x0104,0x30); // Real logo
    memcpy(logo2,cartridge+0x0184,0x30); // Unlicensed game's logo. Sometimes.

    int logoChecksum= 0;
    for(int lb=0;lb<0x30;++lb) {
        logoChecksum+=logo2[lb];
    }
    //char buff[1000];
    //sprintf(buff,"%d",logoChecksum);
    //debug_print(buff);

    switch ( logoChecksum ) {
        case 4048: // "GK.RX" = Gaoke(Hitek) x Ruanxin
            // (All known hacked versions of Hitek games are Li Cheng so have the Niutoude logo instead)
            return UNL_HITEK;
        case 4639: // BBD
        case 5092: // Fiver Firm (publisher of e'Fighter Hot, appears in subsequent BBD fighting games)
            // Games from BBD, Sintax and related developers (probably anything built with Gamtec's SDK) have the bank
            // number as the last byte of each bank. If that number matches the actual bank number, then this is
            // PROBABLY a decrypted rom & we don't have to apply the swapping stuff
            // There MAY be some BBD games that this check fails on, but it works for everything dumped so far (afaik)
            if ( cartridge[0x7fff] != 01 || cartridge[0xbfff] != 02 )
                return UNL_BBD;
            else
                return UNL_NONE;
        case 4876: // Niutoude (Li Cheng)
            // Also appears in Li Cheng games:
            // 5152 = odd logo from Digimon Fight
            // 3746 = not a logo at all; data from Cap vs SNK (its logo is at 0x0904 instead)
            // But since I don't think those are LC-exclusive, you gotta select manual mode for those games for now
            return UNL_NIUTOUDE;
        case 4125: // Sintax "Kwichvu" (corrupted Nintendo)
        case 4138: // Slight variation on Sintax, seen in Harry
            // Similar check to BBD here to detect fixes/hacks/reprints/etc
            if ( ( cartridge[0x7fff] != 01 && cartridge[0x7fff] != 00 ) )
                return UNL_SINTAX;
            else
                return UNL_NONE;
    }

    if (
        (strstr(rom->name,"POKEMON_GLDAAUJ")&&romFileSize==4194304) || // SL 36 in 1 w/Pokemon GS
        (strstr(rom->name,"TIMER MONSTER")&&romFileSize==16777216||romFileSize==(8388608) ) // V.Fame 12in1 Silver / 18in1
    ) {
        return UNL_LBMULTI;
    }

    return UNL_NONE;
}

int gb_system::detectWeirdCarts()
{
    unlCompatMode unlMode = options->unl_compat_mode;
    if ( unlMode == UNL_AUTO ) {
        unlMode = detectUnlCompatMode();
    }

    switch(unlMode) {

        case UNL_HITEK:
            rom->mbcType = MEMORY_HITEK;
            break;
        case UNL_BBD:
            rom->mbcType = MEMORY_BBD;
            break;
        case UNL_NIUTOUDE:
            rom->battery = true;
            rom->mbcType = MEMORY_NIUTOUDE;
            rom->ROMsize=07; // assumption for now
            rom->RAMsize=03; // assumption for now; Sango5 doesnt work with smaller
            rom->carttype=0x1B;
            break;
        case UNL_SINTAX:
            rom->battery = true;
            rom->mbcType = MEMORY_SINTAX;
            rom->ROMsize=07; // assumption for now
            rom->RAMsize=03; // assumption for now
            rom->carttype=0x1B; // same
            break;
        case UNL_LBMULTI:
            rom->RAMsize = 9; // Doesn't really exist shh
            rom->mbcType = MEMORY_LBMULTI;
            break;
        case UNL_NONE: default:
            break;
    }

    // Rumble force for Makon games
    if(!strcmp(rom->newlic,"MK")||!strcmp(rom->newlic,"GC"))
    {
        rom->rumble = 1;
    }

    if(!strcmp(rom->name,"GB SMART CARD"))
    {
        rom->ROMsize = 0;
    }

    // BHGOS MultiCart
    if(!strcmp(rom->name,"MultiCart"))
    {
        rom->mbcType = MEMORY_DEFAULT;

        rom->RAMsize = 3;
        rom->ROMsize = 2;
    }

    // Makon/NT multicarts with menu in Pocket Bomberman
    if(!strcmp(rom->name,"POKEBOM USA") && romFileSize > 512*1024) {
        if(cartridge[0x102] == 0xE0) {
            // 23 in 1 with mario
            rom->mbcType = MEMORY_MAKONOLD2;
            rom->rumble = true;
        }
        if(cartridge[0x102] == 0xC0) {
            // 25 in 1 with rockman
            rom->mbcType = MEMORY_MAKONOLD1;
            rom->battery = true;
            rom->RAMsize = 2;
        }
    }

    if(!strcmp(rom->name,"ROCKMAN 99")) {
        if (cartridge[0x8001] == 0xB7) {
            // old dubious dump
            rom->mbcType = MEMORY_ROCKMAN8;
        } else {
            rom->mbcType = MEMORY_MAKONOLD1;
            rom->ROMsize = 3;
        }
    }

    if(!strcmp(rom->name,"SUPER MARIO 3") || !strcmp(rom->name,"DONKEY\x09KONG 5")) {
        if(romFileSize < 512*1024) {
            debug_print("This ROM is probably an underdump or patch and may not work properly");
        }
        rom->mbcType = MEMORY_MAKONOLD2;
        rom->rumble = true;
        rom->ROMsize = 4;
    }

    char ball_name[16] = { 0x42,0x61,0x6C,0x6C,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x00,(char)0x80 };
    // Ball (Bung)(PD)[C] and Capman (Lik-Sang)(PD)[C] and Fix & Foxi [C][t1]
    if(!strcmp(rom->name,ball_name) || strstr(rom->name,"CAPMAN") || !strcmp(rom->name,"LUPO +3HI"))
    {
        rom->mbcType = MEMORY_DEFAULT;
    }
    else
        //Bugs Bunny - Crazy Castle 3 (J)[C][t2]
    if(!strcmp(rom->name,"BUGS CC3 CRACK"))
    {
        rom->mbcType = MEMORY_DEFAULT;
        rom->RAMsize=1;
    }
    else
        //Pulsar (Freedom GB Contest 2001)(PD)[C]
    if(strstr(rom->name,"PULSAR"))
    {
        rom->mbcType = MEMORY_MBC5;
    }
    else
        //Pokemon Red-Blue 2-in-1 (Unl)[S][h1]
    if(!strcmp(rom->name,"POKEMON RED") && rom->ROMsize == 6)
    {
        rom->ROMsize = 7;
        rom->mbcType = MEMORY_POKE;
    }
    else
        // SGB Pack
    if(!strcmp(rom->name,"SGBPACK"))
    {
        rom->ROMsize = 0;
    }
    else
        // Dragon Ball Z Goku (Chinese)
    if(!strcmp(rom->name,"GB DBZ GOKOU"))
    {
        rom->carttype = 3;
        rom->battery = true;
        rom->mbcType = MEMORY_MBC1;
    }
    else
        // Dragon Ball Z Goku 2 (English)
    if(!strcmp(rom->name,"GB DBZ GOKOU 2") && rom->ROMsize == 05)
    {
        rom->mbcType = MEMORY_DBZTRANS;
    }
    else
        // Bokujou Monogatari 3 Chinese
    if(!strcmp(rom->name,"BOKUMONOGB3BWAJ") || !strcmp(rom->name,"BOYGIRLD640BWAJ"))
    {
        rom->ROMsize = 6;
    }
    else
        // Monsters GO!GO!GO!!
    if(!strcmp(rom->name,"POCKET MONSTER"))
    {
        rom->ROMsize = 4;
    }
    else
        // Sonic 3D Blast 5
    if(strstr(rom->name,"SONIC5"))
    {
        rom->ROMsize = 3;
        rom->RAMsize = 0;
    }
    else
        // Collection Carts
    if(!strcmp(rom->name,"BOMCOL") || !strcmp(rom->name,"BOMSEL") || !strcmp(rom->name,"GENCOL") || strstr(rom->name,"MOMOCOL") || strstr(rom->name,"SUPERCHINESE 12"))
    {
        rom->mbcType = MEMORY_BC;
        if(strstr(rom->name,"MOMOCOL2"))
            rom->mbcType = MEMORY_MMM01;
    }
    else
    if(strstr(rom->name,"MORTALKOMBATI&I"))
    {
        rom->mbcType = MEMORY_MK12;
    }
    else
        // Gameboy Camera
    if(!strcmp(rom->name,"GAMEBOYCAMERA"))
    {
        rom->ROMsize = 5;
        rom->RAMsize = 4;
        rom->mbcType = MEMORY_CAMERA;
    }

    // Digimon 3 saving
    if(!strcmp(rom->name,"DIGIMON") && rom->checksum == 0xE11B)
        rom->battery = true;
    else
        // Joust & Defender (U)[C][t1]
    if(!strcmp(rom->name,"DEFENDER/JOUST") && rom->checksum == 0xB110)
        rom->RAMsize = 1;

    if(!strcmp(rom->name,"TETRIS") && romFileSize > 32768 && rom->ROMsize==0)
    {
        rom->mbcType = MEMORY_MBC1;
        rom->ROMsize = 2;
    } else
    if(!strcmp(rom->name,"\0") && romFileSize > 32768 && rom->ROMsize==0)
    {
        rom->mbcType = MEMORY_8IN1;
        rom->ROMsize = 4;
    } else
    if(strstr(rom->name,"TUWAMONO") && romFileSize == 524288) // Joryu Janshi Ni Chousen (J)[C] (bad)
    {
        debug_print("Bad dump!");
        rom->ROMsize--;
    }  else
    if(!strcmp(rom->name,"SGBPACK") && romFileSize > 32768)
    {
        rom->ROMsize = 6;
        rom->mbcType = MEMORY_POKE;
    } else
    if(romFileSize == 262144 && rom->ROMsize == 4)
        rom->ROMsize--;

}
