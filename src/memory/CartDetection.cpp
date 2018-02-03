/*
   hhugboy Game Boy emulator
   copyright 2013-2017 taizou

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
#include "CartDetection.h"
#include "../GB.h"
#include "../config.h"
#include "../ui/strings.h"

void CartDetection::processRomInfo(byte* cartridge, GBrom* rom, int romFileSize)
{
    readHeader(cartridge, rom);
    detectWeirdCarts(cartridge, rom, romFileSize);
}

void CartDetection::setCartridgeType(byte value, GBrom* rom)
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

void CartDetection::readHeader(byte* cartridge, GBrom* rom)
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
    setCartridgeType(rominfo[addr], rom);

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

unlCompatMode CartDetection::detectUnlCompatMode(byte* cartridge, GBrom* rom, int romFileSize)
{
    byte logo1[0x30];
    byte logo2[0x30];
    memcpy(logo1,cartridge+0x0104,0x30); // Real logo
    memcpy(logo2,cartridge+0x0184,0x30); // Unlicensed game's logo. Sometimes.

    int logoChecksum= 0;
    for(int lb=0;lb<0x30;++lb) {
        logoChecksum+=logo2[lb];
    }

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
        (strstr(rom->name,"TIMER MONSTER")&&(romFileSize==16777216||romFileSize==8388608) ) // V.Fame 12in1 Silver / 18in1
    ) {
        return UNL_LBMULTI;
    }

    // Makon/NT multicarts with menu in Pocket Bomberman
    if(!strcmp(rom->name,"POKEBOM USA") && romFileSize > 512*1024) {
        if(cartridge[0x102] == 0xE0) {
            // 23 in 1 with mario
            return UNL_NTOLD2;
        }
        if(cartridge[0x102] == 0xC0) {
            // 25 in 1 with rockman
            return UNL_NTOLD1;
        }
    }

    if((!strcmp(rom->name," - TRUMP  BOY -") || !strcmp(rom->name,"QBILLION")) && romFileSize > 512*1024) {
        return UNL_NTOLD2;
    }

    // Rockman 8
    if(!strcmp(rom->name,"ROCKMAN 99") && !strstr(rom->newlic,"MK")) {
        if (cartridge[0x8001] != 0xB7) { // Exclude old dump
            return UNL_NTOLD1;
        }
    }

    // Makon early GBC single carts
    if (
        strstr(rom->newlic,"MK") // Makon GBC (un)licensee code (but later games share this code so we gotta check the title too)
        && (!strcmp(rom->name,"SONIC 7") || !strcmp(rom->name,"SUPER MARIO 3") || !strcmp(rom->name,"DONKEY\x09KONG 5") || !strcmp(rom->name,"ROCKMAN 99"))
        && rom->ROMsize == 3 // Untouched ROMs all have 256k in header, assume anything with a 'fixed' ROM size is patched
    ) {
        return UNL_NTOLD2;
    }

    // Sonic 3D Blast 5, Super Donkey Kong 3
    if(strstr(rom->name,"SONIC5")) {
        return UNL_MBC1NOSAVE;
    }

    // Dragon Ball Z Goku 2 (English)
    if(!strcmp(rom->name,"GB DBZ GOKOU 2") && rom->ROMsize == 05) {
        return UNL_DBZTR;
    }

    return UNL_NONE;
}

byte CartDetection::detectGbRomSize(int romFileSize) {
    if (romFileSize > 4096 * 1024)
        return 0x08;
    if (romFileSize > 2048 * 1024)
        return 0x07;
    if (romFileSize > 1024 * 1024)
        return 0x06;
    if (romFileSize > 512 * 1024)
        return 0x05;
    if (romFileSize > 256 * 1024)
        return 0x04;
    if (romFileSize > 128 * 1024)
        return 0x03;
    if (romFileSize > 64 * 1024)
        return 0x02;
    if (romFileSize > 32 * 1024)
        return 0x01;
    return 0x00;
}

void CartDetection::detectWeirdCarts(byte* cartridge, GBrom* rom, int romFileSize)
{
    unlCompatMode unlMode = options->unl_compat_mode;
    if ( unlMode == UNL_AUTO ) {
        unlMode = detectUnlCompatMode(cartridge, rom, romFileSize);
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
        case UNL_NTOLD1:
            if(romFileSize > 512*1024) {
                // enable battery for multicart
                rom->battery = true;
                rom->RAMsize = 2;
            }
            rom->ROMsize = detectGbRomSize(romFileSize);
            rom->mbcType = MEMORY_NTOLD1;
            break;
        case UNL_NTOLD2:
            if((!strcmp(rom->name,"SUPER MARIO 3") || !strcmp(rom->name,"DONKEY\x09KONG 5")) && romFileSize < 512*1024) {
                debug_print("This ROM is probably an underdump or patch and may not work properly");
            }
            rom->ROMsize = detectGbRomSize(romFileSize);
            rom->rumble = true; // Multicarts technically start in the 'rumble off' state but ehhhh
            rom->mbcType = MEMORY_NTOLD2;
            break;
        case UNL_MBC1SAVE:
            rom->battery = true;
            rom->RAMsize = 03;
            rom->ROMsize = detectGbRomSize(romFileSize);
            rom->mbcType = MEMORY_MBC1;
            rom->carttype = 0x03;
            break;
        case UNL_MBC1NOSAVE:
            rom->battery = false;
            rom->RAMsize = 00;
            rom->ROMsize = detectGbRomSize(romFileSize);
            rom->mbcType = MEMORY_MBC1;
            rom->carttype = 0x01;
            break;
        case UNL_MBC5SAVE:
            rom->battery = true;
            rom->RAMsize = 03;
            rom->ROMsize = detectGbRomSize(romFileSize);
            rom->mbcType = MEMORY_MBC5;
            rom->carttype = 0x1B;
            break;
        case UNL_MBC5NOSAVE:
            rom->battery = false;
            rom->RAMsize = 00;
            rom->ROMsize = detectGbRomSize(romFileSize);
            rom->mbcType = MEMORY_MBC5;
            rom->carttype = 0x19;
            break;
        case UNL_DBZTR:
            rom->mbcType = MEMORY_DBZTRANS;
            break;
        case UNL_NONE: default:
            otherCartDetection(cartridge, rom, romFileSize);
            break;
    }

    // Rumble force for misc Makon games
    if(!strcmp(rom->newlic,"MK")||!strcmp(rom->newlic,"GC")) {
        rom->rumble = 1;
    }
}

void CartDetection::otherCartDetection(byte* cartridge, GBrom* rom, int romFileSize)
{
    // ============= LICENSED =============

    // Momotarou Collection 2 but doesn't seem to actually work with any of the dumps I have
    if(strstr(rom->name,"MOMOCOL2")) {
        rom->mbcType = MEMORY_MMM01;
    }

    // Collection Carts
    if(!strcmp(rom->name,"BOMCOL") || !strcmp(rom->name,"BOMSEL") || !strcmp(rom->name,"GENCOL") || strstr(rom->name,"MOMOCOL") || strstr(rom->name,"SUPERCHINESE 12")) {
        rom->mbcType = MEMORY_BC;
        return;
    }

    // Mortal Kombat I & II (UE) [a1][!]
    if(strstr(rom->name,"MORTALKOMBATI&I")) {
        rom->mbcType = MEMORY_MK12;
        return;
    }

    // Gameboy Camera
    if(!strcmp(rom->name,"GAMEBOYCAMERA")) {
        rom->ROMsize = 5;
        rom->RAMsize = 4;
        rom->mbcType = MEMORY_CAMERA;
        return;
    }

    // Joryu Janshi Ni Chousen (J)[C] (bad)
    // Probably no point in detecting this since it just crashes after the title screen
    if(strstr(rom->name,"TUWAMONO") && romFileSize == 524288) {
        debug_print("Bad dump!");
        rom->ROMsize--;
        return;
    }

    // ============ UNLICENSED ============

    // Rockman 8 (Unl) [p1][b1]
    if(!strcmp(rom->name,"ROCKMAN 99") && cartridge[0x8001] == 0xB7) {
        rom->mbcType = MEMORY_ROCKMAN8;
        return;
    }

    // Gameboy Smart Card (CCL Copier) (Unl)
    if(!strcmp(rom->name,"GB SMART CARD")) {
        rom->ROMsize = 0;
        return;
    }

    // Monsters GO!GO!GO!!
    if(!strcmp(rom->name,"POCKET MONSTER")) {
        rom->ROMsize = 4;
        return;
    }

    // Digimon 3 saving
    if(!strcmp(rom->name,"DIGIMON") && rom->checksum == 0xE11B) {
        rom->battery = true;
        return;
    }

    // Sachen 8 in 1
    if(!strcmp(rom->name,"\0") && romFileSize > 32768 && rom->ROMsize==0) {
        rom->mbcType = MEMORY_8IN1;
        rom->ROMsize = 4;
        return;
    }

    // Captain Knick-Knack (Sachen) [!] - has the Tetris header for some reason
    // Magic Maze has it too but is 32k so works
    if(!strcmp(rom->name,"TETRIS") && romFileSize > 32768 && rom->ROMsize==0) {
        rom->mbcType = MEMORY_MBC1;
        rom->ROMsize = 2;
        return;
    }

    // ======= CHINESE TRANSLATIONS =======

    // Dragon Ball Z Goku (Chinese)
    if(!strcmp(rom->name,"GB DBZ GOKOU")) {
        rom->carttype = 3;
        rom->battery = true;
        rom->mbcType = MEMORY_MBC1;
        return;
    }

    // Bokujou Monogatari 3 Chinese
    if(!strcmp(rom->name,"BOKUMONOGB3BWAJ") || !strcmp(rom->name,"BOYGIRLD640BWAJ")) {
        rom->ROMsize = 6;
        return;
    }

    // ========= CRACKED, TRAINED =========

    // Fix & Foxi [C][t1]
    if(!strcmp(rom->name,"LUPO +3HI")) {
        rom->mbcType = MEMORY_DEFAULT;
        return;
    }

    // Bugs Bunny - Crazy Castle 3 (J)[C][t2]
    if(!strcmp(rom->name,"BUGS CC3 CRACK")) {
        rom->mbcType = MEMORY_DEFAULT;
        rom->RAMsize=1;
        return;
    }

    // Joust & Defender (U)[C][t1]
    if(!strcmp(rom->name,"DEFENDER/JOUST") && rom->checksum == 0xB110) {
        rom->RAMsize = 1;
        return;
    }

    // ============= HOMEBREW =============

    // BHGOS MultiCart
    if(!strcmp(rom->name,"MultiCart")) {
        rom->mbcType = MEMORY_DEFAULT;
        rom->RAMsize = 3;
        rom->ROMsize = 2;
        return;
    }

    // Ball (Bung)(PD)[C] and Capman (Lik-Sang)(PD)[C]
    if(!strcmp(rom->name,"Ball          \x00\x80") || strstr(rom->name,"CAPMAN")) {
        rom->mbcType = MEMORY_DEFAULT;
        return;
    }

    // Pulsar (Freedom GB Contest 2001)(PD)[C]
    if(strstr(rom->name,"PULSAR")) {
        rom->mbcType = MEMORY_MBC5;
        return;
    }

    // =============== ETC. ===============

    // Pokemon Red-Blue 2-in-1 (Unl)[S] -- Duz's Pokemon
    if(!strcmp(rom->name,"POKEMON RED") && rom->ROMsize == 6) {
        rom->ROMsize = 7;
        rom->mbcType = MEMORY_POKE;
        return;
    }

    // Pokemon Red-Blue 2-in-1 (Unl) [S][a1] -- Duz's SGB Pack
    if(!strcmp(rom->name,"SGBPACK") && romFileSize > 32768) {
        rom->ROMsize = 6;
        rom->mbcType = MEMORY_POKE;
        return;
    }

    // Other "SGB Pack" (????)
    if(!strcmp(rom->name,"SGBPACK")) {
        rom->ROMsize = 0;
        return;
    }

    // Not sure what this is for
    // A couple Rocket Games games trip this check but seem to work without it?
    if(romFileSize == 262144 && rom->ROMsize == 4) {
        rom->ROMsize--;
        return;
    }
}
