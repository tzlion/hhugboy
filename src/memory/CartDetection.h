#ifndef HHUGBOY_CARTDETECTION_H
#define HHUGBOY_CARTDETECTION_H


#include "../types.h"
#include "../options.h"
#include "GB_MBC.h"

class CartDetection {
    public:
        Cartridge* processRomInfo(byte* rom, int romFileSize);
    private:
        void setCartridgeAttributesFromHeader(Cartridge *cartridge);
        bool detectUnlicensedCarts(byte *rom, Cartridge *cartridge, int romFileSize);
        bool detectMbc1ComboPacks(Cartridge *cartridge, int romFileSize);
        bool detectFlashCartHomebrew(Cartridge *cartridge, int romFileSize);
        void readHeader(byte* rom, Cartridge* cartridge);
        unlCompatMode detectUnlCompatMode(byte* rom, Cartridge* cartridge, int romFileSize);
        byte detectGbRomSize(int romFileSize);
};

#endif //HHUGBOY_CARTDETECTION_H
