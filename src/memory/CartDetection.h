#ifndef HHUGBOY_CARTDETECTION_H
#define HHUGBOY_CARTDETECTION_H


#include "../types.h"
#include "../options.h"
#include "GB_MBC.h"

class CartDetection {
    public:
        void processRomInfo(byte* cartridge, Cartridge* rom, int romFileSize);
    private:
        void setCartridgeAttributesFromHeader(Cartridge *rom);
        bool detectUnlicensedCarts(byte *cartridge, Cartridge *rom, int romFileSize);
        bool detectMbc1ComboPacks(Cartridge *rom, int romFileSize);
        bool detectFlashCartHomebrew(Cartridge *rom, int romFileSize);
        void readHeader(byte* cartridge, Cartridge* rom);
        unlCompatMode detectUnlCompatMode(byte* cartridge, Cartridge* rom, int romFileSize);
        byte detectGbRomSize(int romFileSize);
};

#endif //HHUGBOY_CARTDETECTION_H
