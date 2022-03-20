/*
 * GBX format support for hhugboy emulator
 * by taizou 2018
 * This file released under Creative Commons CC0 https://creativecommons.org/publicdomain/zero/1.0/legalcode
 *
 * As part of the hhugboy project it is also licensed under the GNU General Public License v2
 * See "license.txt" in the project root
 */

#ifndef HHUGBOY_GBXPARSER_H
#define HHUGBOY_GBXPARSER_H

#include "../types.h"
#include "../rom.h"

class GbxParser {

    public:

        static bool isGbx(byte* cartROM, int romFileSize);
        static bool parseFooter(byte* cartROM, CartridgeMetadata *cartridge, int romFileSize);

    private:

        static const int MAX_SUPPORTED_MAJOR_VERSION = 1;

        static const int KB = 1024;
        static const int MB = 1024 * KB;

        static int fourBytesToInt(byte* bytePtr);
        static int mapRomSize(int romSize);
        static int mapRamSize(int ramSize);
        static MbcType mapMapper(char* mapperType);

};

#endif //HHUGBOY_GBXPARSER_H
