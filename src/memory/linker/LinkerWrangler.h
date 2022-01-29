
#ifndef HHUGBOY_LINKERWRANGLER_H
#define HHUGBOY_LINKERWRANGLER_H

#include "../../types.h"
#include "GbLinker.h"

#define READ_ROM0_THRU_LINKER true
#define READ_ROM1_THRU_LINKER false
#define READ_RAM_THRU_LINKER true
#define CACHE_BANK_0 false
#define CACHE_BANK_1 false
#define USE_PREDUMPED_BANK_0 false

class LinkerWrangler {
    private:
        static bool readBank0;
        static U8 bank0[0x4000];
        static bool readBank1;
        static U8 bank1[0x4000];
    public:
        static bool shouldReadThroughLinker(unsigned short address);
        static byte readThroughLinker(unsigned short address);
        static bool shouldWriteThroughLinker(unsigned short address, byte data);
        static bool writeThroughLinker(unsigned short address, byte data);
};

#endif //HHUGBOY_LINKERWRANGLER_H
