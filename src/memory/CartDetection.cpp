/*
   hhugboy Game Boy emulator
   copyright 2013-2018 taizou

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
#include <algorithm>
#include "CartDetection.h"
#include "../GB.h"
#include "../config.h"
#include "GbxParser.h"

Cartridge* CartDetection::processRomInfo(byte* rom, int romFileSize)
{
    Cartridge* cartridge = new Cartridge();
    readHeader(rom, cartridge, romFileSize);

    if (GbxParser::isGbx(rom, romFileSize)) {
        bool parseSuccess = GbxParser::parseFooter(rom, cartridge, romFileSize);
        if (parseSuccess) {
            return cartridge;
        } else {
            debug_print("Couldn't process GBX format ROM");
        }
    }

    setCartridgeAttributesFromHeader(cartridge);
    detectMbc1ComboPacks(cartridge, romFileSize);
    detectUnlicensedCarts(rom, cartridge, romFileSize);
    detectFlashCartHomebrew(cartridge, romFileSize);

    return cartridge;
}

void CartDetection::setCartridgeAttributesFromHeader(Cartridge *cartridge)
{
    cartridge->RTC = false;
    cartridge->rumble = false;
    cartridge->battery = false;
    //cartridge->mbcType = MEMORY_DEFAULT; // Don't do this here anymore, as readHeader may already have detected an MBC type from the header position in the ROM
    cartridge->ROMsize = 0;
    cartridge->RAMsize = 0;

    if (cartridge->header.ROMsize <= 8) {
        // Some docs list ROM size values of 0x52, 0x53, 0x54 but nothing uses that in the entire GoodGBX set
        // and Nintendo's docs from 1999 don't mention them
        cartridge->ROMsize = cartridge->header.ROMsize;
    }

    if (cartridge->header.RAMsize <= 5) {
        // 5 is used by Japanese Pokemon Crystal only
        // 8 was a duplicate of 2, only found in some homebrew/hacks but they don't actually need it
        cartridge->RAMsize = cartridge->header.RAMsize;
    }

    switch(cartridge->header.carttype)
    {
        case 0x00: // ROM
            cartridge->mbcType = MEMORY_ROMONLY;
            break;

        case 0x01: // MBC1
            cartridge->mbcType = MEMORY_MBC1;
            break;

        case 0x02: // MBC1+RAM
            cartridge->mbcType = MEMORY_MBC1;
            break;

        case 0x03: // MBC1+RAM+BATTERY
            cartridge->battery = true;
            cartridge->mbcType = MEMORY_MBC1;
            break;

        case 0x05: // MBC2
            cartridge->mbcType = MEMORY_MBC2;
            break;

        case 0x06: // MBC2+BATTERY
            cartridge->battery = true;
            cartridge->mbcType = MEMORY_MBC2;
            break;

        case 0x08: // ROM+RAM;
            cartridge->mbcType = MEMORY_ROMONLY;
            break;

        case 0x09: // ROM+RAM+BATTERY
            cartridge->battery = true;
            cartridge->mbcType = MEMORY_ROMONLY;
            break;

        case 0x0B: // MMM01
            cartridge->mbcType = MEMORY_MMM01;
            break;

        case 0x0C: // MMM01+RAM
            cartridge->mbcType = MEMORY_MMM01;
            break;

        case 0x0D: // MMM01+RAM+BATTERY
            cartridge->battery = true;
            cartridge->mbcType = MEMORY_MMM01;
            break;

        case 0x0F: // MBC3+TIMER+BATTERY
            cartridge->battery = true;
            cartridge->RTC = true;
            cartridge->mbcType = MEMORY_MBC3;
            break;

        case 0x10: // MBC3+TIMER+RAM+BATTERY
	    if (strstr(cartridge->header.name,"TETRIS SET")) // Mani 4-in-1 Tetris Set uses value 0x10 as well, so detect the one cart using this mapper via game name
		cartridge->mbcType = MEMORY_M161;
	    else {
		cartridge->battery = true;
		cartridge->RTC = true;
		cartridge->mbcType = MEMORY_MBC3;
	    }
            break;

        case 0x11: // MBC3
	    // Mani 4-in-1 put 0x11 there as well even though it uses MMM01. So if MMM01 was already detected by readHeader, don't override that
            if (cartridge->mbcType !=MEMORY_MMM01) cartridge->mbcType = MEMORY_MBC3;
            break;

        case 0x12: // MBC3+RAM
            cartridge->mbcType = MEMORY_MBC3;
            break;

        case 0x13: // MBC3+RAM+BATTERY
            cartridge->battery = true;
            cartridge->mbcType = MEMORY_MBC3;
            break;

        case 0x19: // MBC5
            cartridge->mbcType = MEMORY_MBC5;
            break;

        case 0x1A: // MBC5+RAM
            cartridge->mbcType = MEMORY_MBC5;
            break;

        case 0x1B: // MBC5+RAM+BATTERY
            cartridge->battery = true;
            cartridge->mbcType = MEMORY_MBC5;
            break;

        case 0x1C: // MBC5+RUMBLE
            cartridge->rumble = true;
            cartridge->mbcType = MEMORY_MBC5;
            break;

        case 0x1D: // MBC5+RUMBLE+RAM
            cartridge->rumble = true;
            cartridge->mbcType = MEMORY_MBC5;
            break;

        case 0x1E: // MBC5+RUMBLE+RAM+BATTERY
            cartridge->battery = true;
            cartridge->rumble = true;
            cartridge->mbcType = MEMORY_MBC5;
            break;

        case 0x22: // MBC7+SENSOR+RUMBLE+RAM+BATTERY
            cartridge->battery = true;
            cartridge->mbcType = MEMORY_MBC7;
            break;

        case 0xFC: // POCKET CAMERA
            cartridge->battery = true;
            cartridge->mbcType = MEMORY_CAMERA;
            break;

        case 0xFD: // Bandai TAMA5
            cartridge->battery = true;
            cartridge->mbcType = MEMORY_TAMA5;
            break;

        case 0xFE: // Hudson HuC-3
            cartridge->battery = true;
            cartridge->mbcType = MEMORY_HUC3;
            break;

        case 0xFF: // Hudson HuC-1+RAM+BATTERY
            cartridge->battery = true;
            cartridge->mbcType = MEMORY_MBC1;
            break;
    }
}

void CartDetection::readHeader(byte* rom, Cartridge* cartridge, int romFileSize)
{
    byte rominfo[30];
    cartridge->mbcType = MEMORY_DEFAULT; // Was originally in setCartridgeAttributesFromHeader, but since we are already detecting three MBC types here, put it here as well.
    
    // Determine whether we have a scrambled header, or a header at the end of the file. If so, we have already detected Sachen MMC1/2 or MMM01.
    int logoChecksum0104Scrambled =0; for(int lb=0;lb<0x30;++lb) { int address =0x104 +lb; address =address &~0x53 | address >>6 &0x01 | address >>3 &0x02 | address <<3 &0x10 | address <<6 &0x40; logoChecksum0104Scrambled+=rom[address] ; }
    int logoChecksum0184Scrambled =0; for(int lb=0;lb<0x30;++lb) { int address =0x184 +lb; address =address &~0x53 | address >>6 &0x01 | address >>3 &0x02 | address <<3 &0x10 | address <<6 &0x40; logoChecksum0184Scrambled+=rom[address] ; }
    if (logoChecksum0104Scrambled ==5542 || logoChecksum0104Scrambled ==7484) cartridge->mbcType =MEMORY_SACHENMMC2;
    if (logoChecksum0184Scrambled ==5542 || logoChecksum0184Scrambled ==7484) cartridge->mbcType =MEMORY_SACHENMMC1;
    
    int logoChecksumEnd =0;
    int cartridgeHeaderAtEnd =(romFileSize &~0x7FFF) -0x8000;
    if (cartridgeHeaderAtEnd >=0) {
	for(int lb=0; lb <0x30; ++lb) logoChecksumEnd +=rom[cartridgeHeaderAtEnd +0x0104 +lb];
	byte cartridgeType =rom[cartridgeHeaderAtEnd +0x147];
	if (logoChecksumEnd ==5446 && (cartridgeType ==0x0B || cartridgeType ==0x0C || cartridgeType ==0x0D || cartridgeType ==0x11)) cartridge->mbcType =MEMORY_MMM01;
    }
    
    // Read unscrambled header from the correct file position
    switch (cartridge->mbcType) {
	    case MEMORY_MMM01:
		memcpy(rominfo,rom+(romFileSize &~0x7FFF) -0x8000 +0x0134,0x1C);
		break;
	    case MEMORY_SACHENMMC1:
	    case MEMORY_SACHENMMC2:
		for (int i =0; i <0x1C; i++) {
			int address =i +0x0134;
			address =address &~0x53 |
				address >>6 &0x01 |
				address >>3 &0x02 |
				address <<3 &0x10 |
				address <<6 &0x40
			;
			rominfo[i] =rom[address];
		}
		break;
	    default: // Unscrambled from beginning of file
		memcpy(rominfo,rom+0x0134,0x1C);
		break;
    }

    int addr = 0;
    for(;addr<=14; ++addr)
        cartridge->header.name[addr] = rominfo[addr];

    if(rominfo[addr] == 0x80)
        cartridge->header.CGB = 1;
    else if(rominfo[addr] == 0xC0)
        cartridge->header.CGB = 2; // gbc only
    else
        cartridge->header.CGB = 0;
    // modes changed when reset

    ++addr; cartridge->header.newlic[0] = rominfo[addr];
    ++addr; cartridge->header.newlic[1] = rominfo[addr];

    ++addr;
    if(rominfo[addr] == 0x03)
        cartridge->header.SGB = 1;
    else
        cartridge->header.SGB = 0;

    ++addr;
    cartridge->header.carttype = rominfo[addr];

    cartridge->header.ROMsize = rominfo[addr+1];
    cartridge->header.RAMsize = rominfo[addr+2];

    addr+=3; cartridge->header.destcode = rominfo[addr];
    ++addr; cartridge->header.lic = rominfo[addr];
    ++addr; cartridge->header.version = rominfo[addr];
    ++addr; cartridge->header.complement = rominfo[addr];
    ++addr; cartridge->header.checksum=(rominfo[addr]<<8);
    ++addr; cartridge->header.checksum|=rominfo[addr];

    // check complement
    byte cmpl=0;
    for(addr=0;addr<=0x19;++addr)
        cmpl+=rominfo[addr];
    cmpl+=25; cartridge->header.complementok = !cmpl;
}

unlCompatMode CartDetection::detectUnlCompatMode(byte* rom, Cartridge* cartridge, int romFileSize)
{
    if (cartridge->mbcType ==MEMORY_MMM01) return UNL_NONE; // Might be misdetected as something else, so don't even try
    int logoChecksum0104=0; for(int lb=0;lb<0x30;++lb) logoChecksum0104+=rom[0x0104 +lb];
    int logoChecksum0184=0; for(int lb=0;lb<0x30;++lb) logoChecksum0184+=rom[0x0184 +lb];
    int logoChecksum0104Scrambled =0; for(int lb=0;lb<0x30;++lb) { int address =0x104 +lb; address =address &~0x53 | address >>6 &0x01 | address >>3 &0x02 | address <<3 &0x10 | address <<6 &0x40; logoChecksum0104Scrambled+=rom[address] ; }

			
    int logoChecksum0184Scrambled =0; for(int lb=0;lb<0x30;++lb) { int address =0x184 +lb; address =address &~0x53 | address >>6 &0x01 | address >>3 &0x02 | address <<3 &0x10 | address <<6 &0x40; logoChecksum0184Scrambled+=rom[address] ; }
    
    /*char buff[100];
    sprintf(buff,"logo: 0104=%d, 0184=%d", logoChecksum0104, logoChecksum0184);
    debug_win(buff);*/

    if (logoChecksum0104Scrambled ==5542 || logoChecksum0104Scrambled ==7484) return UNL_SACHENMMC2;
    if (logoChecksum0184Scrambled ==5542 || logoChecksum0184Scrambled ==7484) return UNL_SACHENMMC1;
    
    switch ( logoChecksum0104 ) {
	case 2756: // Rocket Games
	case 4850: // Smartcom
	    return UNL_ROCKET;
    }    
    switch ( logoChecksum0184 ) {
        case 4048: // "GK.RX" = Gaoke(Hitek) x Ruanxin
            // (All known hacked versions of Hitek games are Li Cheng so have the Niutoude logo instead)
            return UNL_HITEK;
        case 4639: // BBD
        case 5092: // Fiver Firm (publisher of e'Fighter Hot, appears in subsequent BBD fighting games)
            // Games from BBD, Sintax and related developers (probably anything built with Gamtec's SDK) have the bank
            // number as the last byte of each bank. If that number matches the actual bank number, then this is
            // PROBABLY a decrypted rom & we don't have to apply the swapping stuff
            // There MAY be some BBD games that this check fails on, but it works for everything dumped so far (afaik)
            if ( rom[0x7fff] != 01 || rom[0xbfff] != 02 )
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
            if ( ( rom[0x7fff] != 01 && rom[0x7fff] != 00 ) )
                return UNL_SINTAX;
            else
                return UNL_NONE;
    }

    if (
        (strstr(cartridge->header.name,"POKEMON_GLDAAUJ")&&romFileSize==4194304) || // SL 36 in 1 w/Pokemon GS
        (strstr(cartridge->header.name,"TIMER MONSTER")&&(romFileSize==16777216||romFileSize==8388608) ) // V.Fame 12in1 Silver / 18in1
    ) {
        return UNL_LBMULTI;
    }

    // Makon/NT multicarts with menu in Pocket Bomberman
    if(!strcmp(cartridge->header.name,"POKEBOM USA") && romFileSize > 512*1024) {
        if(rom[0x102] == 0xE0) {
            // 23 in 1 with mario
            return UNL_NTOLD2;
        }
        if(rom[0x102] == 0xC0) {
            // 25 in 1 with rockman
            return UNL_NTOLD1;
        }
    }

    if((!strcmp(cartridge->header.name," - TRUMP  BOY -") || !strcmp(cartridge->header.name,"QBILLION")) && romFileSize > 512*1024) {
        return UNL_NTOLD2;
    }

    // Rockman 8
    if(!strcmp(cartridge->header.name,"ROCKMAN 99") && !strstr(cartridge->header.newlic,"MK")) {
        if (rom[0x8001] != 0xB7) { // Exclude old dump
            return UNL_NTOLD1;
        }
    }

    // Makon early GBC single carts
    if (
        strstr(cartridge->header.newlic,"MK") // Makon GBC (un)licensee code (but later games share this code so we gotta check the title too)
        && (!strcmp(cartridge->header.name,"SONIC 7") || !strcmp(cartridge->header.name,"SUPER MARIO 3") || !strcmp(cartridge->header.name,"DONKEY\x09KONG 5") || !strcmp(cartridge->header.name,"ROCKMAN 99"))
        && cartridge->ROMsize == 3 // Untouched ROMs all have 256k in header, assume anything with a 'fixed' ROM size is patched
    ) {
        return UNL_NTOLD2;
    }

    // Sonic 3D Blast 5, Super Donkey Kong 3
    // Also has a cart type of "EA" but this is a consequence of there being code in the header area
    if(strstr(cartridge->header.name,"SONIC5")) {
        return UNL_MBC1NOSAVE;
    }

    // Dragon Ball Z Goku 2 (English)
    if(!strcmp(cartridge->header.name,"GB DBZ GOKOU 2") && cartridge->ROMsize == 05) {
        return UNL_DBZTR;
    }

    // ------- older dumps, previously detected by GEST, may be hacked/patched/bad -------
    // Pocket Voice Recorder
    if (cartridge->header.carttype == 0xBE) {
        return UNL_MBC5SAVE;
    }
    // Gameboy Smart Card (CCL Copier) (Unl)
    // Some vers have an ID in the header area so it appears as cart type 0x59
    if(!strcmp(cartridge->header.name,"GB SMART CARD")) {
        return UNL_MBC1NOSAVE;
    }
    // Rockman 8 (Unl) [p1][b1]
    if(!strcmp(cartridge->header.name,"ROCKMAN 99") && rom[0x8001] == 0xB7) {
        return UNL_RM8OLD;
    }
    // Captain Knick-Knack (Sachen) [!] - has the Tetris header for some reason
    // Magic Maze has it too but is 32k so works
    if(!strcmp(cartridge->header.name,"TETRIS") && romFileSize > 32768 && cartridge->ROMsize==0) {
        return UNL_MBC1NOSAVE;
    }
    // Bokujou Monogatari 3 Chinese
    if((strstr(cartridge->header.name,"BOKUMONOGB3BWAJ") || strstr(cartridge->header.name,"BOYGIRLD640BWAJ")) && cartridge->ROMsize == 1) {
        return UNL_MBC5SAVE;
    }
    // Dragon Ball Z Goku (Chinese)
    if(!strcmp(cartridge->header.name,"GB DBZ GOKOU") && cartridge->ROMsize == 4) {
        return UNL_MBC1SAVE;
    }
    // Monsters GO!GO!GO!!
    if(!strcmp(cartridge->header.name,"POCKET MONSTER") && cartridge->ROMsize == 3) {
        return UNL_MBC1NOSAVE;
    }
    // Digimon 3 saving
    if(!strcmp(cartridge->header.name,"DIGIMON") && cartridge->header.checksum == 0xE11B) {
        return UNL_MBC5SAVE;
    }
    // Wisdom Tree
    static const char strWisdomTree1[12] ="WISDOM TREE";
    static const char strWisdomTree2[12] ="WISDOM\0TREE";
    
    if(rom[0x147] ==0xC0 && rom[0x14A] ==0xD1 ||
       std::search(rom, rom +romFileSize, strWisdomTree1, strWisdomTree1 +11) !=(rom +romFileSize) ||
       std::search(rom, rom +romFileSize, strWisdomTree2, strWisdomTree2 +11) !=(rom +romFileSize)) {
        debug_win("Detected: Wisdom Tree");
	return UNL_WISDOMTREE;
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

bool CartDetection::detectUnlicensedCarts(byte *rom, Cartridge *cartridge, int romFileSize)
{
    unlCompatMode unlMode = options->unl_compat_mode;
    if ( unlMode == UNL_AUTO ) {
        unlMode = detectUnlCompatMode(rom, cartridge, romFileSize);
    }

    switch(unlMode) {
        case UNL_HITEK:
            cartridge->mbcType = MEMORY_HITEK;
            break;
        case UNL_BBD:
            cartridge->mbcType = MEMORY_BBD;
            break;
        case UNL_NIUTOUDE:
            cartridge->battery = true;
            cartridge->mbcType = MEMORY_NIUTOUDE;
            cartridge->ROMsize=07; // assumption for now
            cartridge->RAMsize=03; // assumption for now; Sango5 doesnt work with smaller
            break;
        case UNL_SINTAX:
            cartridge->battery = true;
            cartridge->mbcType = MEMORY_SINTAX;
            cartridge->ROMsize=07; // assumption for now
            cartridge->RAMsize=03; // assumption for now
            break;
        case UNL_NTNEW:
            cartridge->battery = true; // not all of them have battery + RAM, but some that have it don't declare it
            cartridge->RAMsize = 3; // most with RAM seem to have a 32k chip on board even if <=8k is actually used
            cartridge->mbcType = MEMORY_NTNEW;
            break;
        case UNL_LBMULTI:
            cartridge->RAMsize = 9; // Doesn't really exist shh
            cartridge->mbcType = MEMORY_LBMULTI;
            break;
        case UNL_NTOLD1:
            if(romFileSize > 512*1024) {
                // enable battery for multicart
                cartridge->battery = true;
                cartridge->RAMsize = 2;
            }
            cartridge->ROMsize = detectGbRomSize(romFileSize);
            cartridge->mbcType = MEMORY_NTOLD1;
            break;
        case UNL_NTOLD2:
            if((!strcmp(cartridge->header.name,"SUPER MARIO 3") || !strcmp(cartridge->header.name,"DONKEY\x09KONG 5")) && romFileSize < 512*1024) {
                debug_print("This ROM is probably an underdump or patch and may not work properly");
            }
            cartridge->ROMsize = detectGbRomSize(romFileSize);
            cartridge->rumble = true; // Multicarts technically start in the 'rumble off' state but ehhhh
            cartridge->mbcType = MEMORY_NTOLD2;
            break;
        case UNL_MBC1SAVE:
            cartridge->battery = true;
            cartridge->RAMsize = 03;
            cartridge->ROMsize = detectGbRomSize(romFileSize);
            cartridge->mbcType = MEMORY_MBC1;
            break;
        case UNL_MBC1NOSAVE:
            cartridge->battery = false;
            cartridge->RAMsize = 00;
            cartridge->ROMsize = detectGbRomSize(romFileSize);
            cartridge->mbcType = MEMORY_MBC1;
            break;
        case UNL_MBC3SAVE:
            cartridge->battery = true;
            cartridge->RAMsize = 03;
            cartridge->ROMsize = detectGbRomSize(romFileSize);
            cartridge->mbcType = MEMORY_MBC3;
            break;
        case UNL_MBC5SAVE:
            cartridge->battery = true;
            cartridge->RAMsize = 03;
            cartridge->ROMsize = detectGbRomSize(romFileSize);
            cartridge->mbcType = MEMORY_MBC5;
            break;
        case UNL_MBC5NOSAVE:
            cartridge->battery = false;
            cartridge->RAMsize = 00;
            cartridge->ROMsize = detectGbRomSize(romFileSize);
            cartridge->mbcType = MEMORY_MBC5;
            break;
        case UNL_POKEJD:
            cartridge->ROMsize = detectGbRomSize(romFileSize);
            cartridge->mbcType = MEMORY_POKEJD;
            break;
        case UNL_DBZTR: // Can't be manually selected currently
            cartridge->mbcType = MEMORY_DBZTRANS;
            break;
        case UNL_RM8OLD: // Can't be manually selected currently
            cartridge->mbcType = MEMORY_ROCKMAN8;
            break;
	case UNL_WISDOMTREE:
	    cartridge->ROMsize = detectGbRomSize(romFileSize);
	    cartridge->mbcType = MEMORY_WISDOMTREE;
	    break;
	case UNL_SACHENMMC1:
	    cartridge->ROMsize = detectGbRomSize(romFileSize);
	    cartridge->mbcType = MEMORY_SACHENMMC1;
	    break;
	case UNL_SACHENMMC2:
	    cartridge->ROMsize = detectGbRomSize(romFileSize);
	    cartridge->mbcType = MEMORY_SACHENMMC2;
	    break;
	case UNL_ROCKET:
	    cartridge->mbcType = MEMORY_ROCKET;
	    break;
        case UNL_NONE: default:
            break;
    }

    // Rumble force for misc Makon games
    if(!strcmp(cartridge->header.newlic,"MK")||!strcmp(cartridge->header.newlic,"GC")) {
        cartridge->rumble = 1;
    }

    return unlMode != UNL_NONE;
}

/**
 * Fix homebrew, cracks, trainers etc that were designed to run on a flashcart and have incorrect header values
 */
bool CartDetection::detectFlashCartHomebrew(Cartridge *cartridge, int romFileSize)
{
    // Trainers expecting some RAM where the original cart had none
    // - Fix & Foxi - Episode 1 Lupo (E) (M3) [C][t1]
    // - Bugs Bunny - Crazy Castle 3 (J)[C][t2]
    // - Joust & Defender (U)[C][t1]
    if(!strcmp(cartridge->header.name,"BUGS CC3 CRACK") || !strcmp(cartridge->header.name,"LUPO +3HI") ||
       (!strcmp(cartridge->header.name,"DEFENDER/JOUST") && cartridge->header.checksum == 0xB110)) {
        cartridge->mbcType = MEMORY_DEFAULT;
        cartridge->RAMsize = 1;
        return true;
    }

    // BHGOS MultiCart
    if(!strcmp(cartridge->header.name,"MultiCart")) {
        cartridge->mbcType = MEMORY_DEFAULT;
        cartridge->RAMsize = 3;
        cartridge->ROMsize = 2;
        return true;
    }

    // Ball (Bung)(PD)[C] and Capman (Lik-Sang)(PD)[C]
    if(!strcmp(cartridge->header.name,"Ball          \x00\x80") || strstr(cartridge->header.name,"CAPMAN")) {
        cartridge->mbcType = MEMORY_DEFAULT;
        return true;
    }

    // Pulsar (Freedom GB Contest 2001)(PD)[C]
    if(strstr(cartridge->header.name,"PULSAR")) {
        cartridge->mbcType = MEMORY_MBC5;
        return true;
    }

    // Duz's Pokemon & Duz's SGB Pack
    if((!strcmp(cartridge->header.name,"POKEMON RED") && cartridge->ROMsize == 6) || !strcmp(cartridge->header.name,"SGBPACK")) {
        cartridge->mbcType = MEMORY_POKE;
        cartridge->ROMsize = detectGbRomSize(romFileSize);
        return true;
    }

    return false;
}

/**
 * Detect licensed MBC1 multicarts (they use the regular MBC1 cart type values in the header)
 */
bool CartDetection::detectMbc1ComboPacks(Cartridge *cartridge, int romFileSize)
{
    // Maintain support for Mortal Kombat I & II (UE) [a1][!] .. for now
    if(strstr(cartridge->header.name,"MORTALKOMBATI&I") && romFileSize == 540672) {
        cartridge->mbcType = MEMORY_MK12;
        return true;
    }

    // momocol should have a battery, the others not
    // (momocol2 uses MMM01 and doesn't currently work)
    if(!strcmp(cartridge->header.name,"BOMCOL") || !strcmp(cartridge->header.name,"BOMSEL") || !strcmp(cartridge->header.name,"GENCOL")
       || strstr(cartridge->header.name,"MOMOCOL") && !strstr(cartridge->header.name,"MOMOCOL2") || strstr(cartridge->header.name,"SUPERCHINESE 12")
       || strstr(cartridge->header.name,"MORTALKOMBATI&I")) {
        cartridge->mbcType = MEMORY_MBC1MULTI;
        return true;
    }

    return false;
}
