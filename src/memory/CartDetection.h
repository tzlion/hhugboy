#ifndef HHUGBOY_CARTDETECTION_H
#define HHUGBOY_CARTDETECTION_H


#include "../types.h"
#include "../options.h"
#include "GB_MBC.h"

class CartDetection {
    public:
        CartDetection(gb_mbc* mbc, byte* cartridge, GBrom* rom, int romFileSize);
        void processRomInfo();
    private:
        gb_mbc* mbc;
        byte* cartridge;
        GBrom* rom;
        int romFileSize;
        void setCartridgeType(byte value);
        void detectWeirdCarts();
        void otherCartDetection();
        void readHeader();
        unlCompatMode detectUnlCompatMode();
        byte detectGbRomSize();
};

#endif //HHUGBOY_CARTDETECTION_H
