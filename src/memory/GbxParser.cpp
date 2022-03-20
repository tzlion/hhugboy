/*
 * GBX format support for hhugboy emulator
 * by taizou 2018
 * This file released under Creative Commons CC0 https://creativecommons.org/publicdomain/zero/1.0/legalcode
 *
 * As part of the hhugboy project it is also licensed under the GNU General Public License v2
 * See "license.txt" in the project root
 */

#include <cstdio>
#include <cstring>
#include "GbxParser.h"
#include "../debug.h"

bool GbxParser::isGbx(byte* cartROM, int romFileSize)
{
    byte signature[4] = {'G', 'B', 'X', '!'};
    return !memcmp(cartROM + romFileSize - 4, signature, 4);
}

bool GbxParser::parseFooter(byte* cartROM, Cartridge *cartridge, int romFileSize)
{
    char msg[420];

    int footerMajVer = fourBytesToInt(cartROM + romFileSize - 12);
    int footerMinVer = fourBytesToInt(cartROM + romFileSize - 8);
    int footerSize = fourBytesToInt(cartROM + romFileSize - 16);

    sprintf(msg,"GBX footer found - ver %d.%d, size %d bytes", footerMajVer, footerMinVer, footerSize);
    debug_win(msg);

    if (footerMajVer != MAX_SUPPORTED_MAJOR_VERSION) {
        debug_win("GBX version not supported!!");
        return false;
    }
    if (footerSize > romFileSize || footerSize < 16) {
        debug_win("invalid footer size!!");
        return false;
    }

    byte footer[footerSize];
    memcpy(footer, cartROM + romFileSize - footerSize, footerSize);
    memset(cartROM + romFileSize - footerSize, 0, footerSize);

    char mapper[5];
    memcpy(mapper, footer, 4);
    mapper[4] = 0x00;
    int romSize = fourBytesToInt(footer+8);
    int ramSize = fourBytesToInt(footer+12);

    sprintf(msg,"Mapper %s / Batt %d / Rumble %d / RTC %d / ROM %d bytes / RAM %d bytes",mapper, footer[4], footer[5], footer[6], romSize, ramSize);
    debug_win(msg);

    cartridge->mbcType = mapMapper(mapper);
    cartridge->ROMsize = mapRomSize(romSize);
    cartridge->RAMsize = mapRamSize(ramSize);
    cartridge->battery = (bool)footer[4];
    cartridge->rumble = (bool)footer[5];
    cartridge->RTC = (bool)footer[6];
    memcpy(cartridge->mbcConfig, footer +16, 32);

    sprintf(msg,"Mapped to internal MBC type %d / ROM size %d / RAM size %d",cartridge->mbcType, cartridge->ROMsize, cartridge->RAMsize);
    debug_win(msg);

    return true;
}

int GbxParser::mapRomSize(int romSize)
{
    switch(romSize) {
        case 32 * KB:
            return 0x00;
        case 64 * KB:
            return 0x01;
        case 128 * KB:
            return 0x02;
        case 256 * KB:
            return 0x03;
        case 512 * KB:
            return 0x04;
        case MB:
            return 0x05;
        case 2 * MB:
            return 0x06;
        case 4 * MB:
            return 0x07;
        case 8 * MB:
            return 0x08;
        default:
            debug_win("Unsupported ROM size!");
            return 0x00;
    }
}

int GbxParser::mapRamSize(int ramSize)
{
    switch(ramSize) {
        case 0:
            return 0x00;
        case 2 * KB:
            return 0x01;
        case 8 * KB:
            return 0x02;
        case 32 * KB:
            return 0x03;
        case 128 * KB:
            return 0x04;
        case 64 * KB:
            return 0x05;
        default:
            debug_win("Unsupported RAM size!");
            return 0x00;
    }
}

MbcType GbxParser::mapMapper(char* mapperType)
{
    if (!strcmp(mapperType,"ROM")) return MEMORY_ROMONLY;
    if (!strcmp(mapperType,"MBC1")) return MEMORY_MBC1;
    if (!strcmp(mapperType,"MBC2")) return MEMORY_MBC2;
    if (!strcmp(mapperType,"MBC3")) return MEMORY_MBC3;
    if (!strcmp(mapperType,"MBC5")) return MEMORY_MBC5;
    if (!strcmp(mapperType,"MBC7")) return MEMORY_MBC7;
    if (!strcmp(mapperType,"MB1M")) return MEMORY_MBC1MULTI;
    if (!strcmp(mapperType,"MMM1")) return MEMORY_MMM01;
    if (!strcmp(mapperType,"CAMR")) return MEMORY_CAMERA;
    if (!strcmp(mapperType,"HUC1")) return MEMORY_MBC1; // ??
    if (!strcmp(mapperType,"HUC3")) return MEMORY_HUC3;
    if (!strcmp(mapperType,"TAM5")) return MEMORY_TAMA5;
    if (!strcmp(mapperType,"BBD")) return MEMORY_BBD;
    if (!strcmp(mapperType,"HITK")) return MEMORY_HITEK;
    if (!strcmp(mapperType,"SNTX")) return MEMORY_SINTAX;
    if (!strcmp(mapperType,"NTO1")) return MEMORY_NTOLD1;
    if (!strcmp(mapperType,"NTO2")) return MEMORY_NTOLD2;
    if (!strcmp(mapperType,"NTN")) return MEMORY_NTNEW;
    if (!strcmp(mapperType,"LICH")) return MEMORY_NIUTOUDE;
    if (!strcmp(mapperType,"LBMC")) return MEMORY_LBMULTI;
    if (!strcmp(mapperType,"LIBA")) return MEMORY_DBZTRANS;
    if (!strcmp(mapperType,"PKJD")) return MEMORY_POKEJD;
    if (!strcmp(mapperType,"WISD")) return MEMORY_WISDOMTREE;
    if (!strcmp(mapperType,"SAM1")) return MEMORY_SACHENMMC1;
    if (!strcmp(mapperType,"SAM2")) return MEMORY_SACHENMMC2;
    if (!strcmp(mapperType,"M161")) return MEMORY_M161;
    if (!strcmp(mapperType,"ROCK")) return MEMORY_ROCKET;
    if (!strcmp(mapperType,"NGHK")) return MEMORY_NEWGBHK;
    if (!strcmp(mapperType,"GB81")) return MEMORY_GGB81;
    if (!strcmp(mapperType,"VF01")) return MEMORY_VF001;
    debug_win("Unsupported mapper type!");
    return MEMORY_DEFAULT;
}

int GbxParser::fourBytesToInt(byte* bytePtr)
{
    return (bytePtr[0] << 24) | (bytePtr[1] << 16) | (bytePtr[2] << 8) | (bytePtr[3]);
}
