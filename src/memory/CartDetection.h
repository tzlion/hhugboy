#ifndef HHUGBOY_CARTDETECTION_H
#define HHUGBOY_CARTDETECTION_H


#include "../types.h"
#include "../options.h"
#include "GB_MBC.h"

class CartDetection {
    public:
        void processRomInfo(byte* cartridge, GBrom* rom, int romFileSize);
    private:
        void setCartridgeAttributesFromHeader(GBrom *rom);
        bool detectUnlicensedCarts(byte *cartridge, GBrom *rom, int romFileSize);
        bool detectMbc1ComboPacks(GBrom *rom, int romFileSize);
        bool detectFlashCartHomebrew(GBrom *rom, int romFileSize);
        void readHeader(byte* cartridge, GBrom* rom);
        unlCompatMode detectUnlCompatMode(byte* cartridge, GBrom* rom, int romFileSize);
        byte detectGbRomSize(int romFileSize);
};

#endif //HHUGBOY_CARTDETECTION_H
