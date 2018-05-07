
#ifndef HHUGBOY_LINKERWRANGLER_H
#define HHUGBOY_LINKERWRANGLER_H

#include "../../types.h"
#include "GbLinker.h"

class LinkerWrangler {
    private:
        static bool readBank0;
        static U8 bank0[0x4000];
        static bool readBank1;
        static U8 bank1[0x4000];
        static byte last4000;
    public:
        static bool shouldReadThroughLinker(unsigned short address);
        static byte readThroughLinker(unsigned short address);
        static bool shouldWriteThroughLinker(unsigned short address, byte data);
        static bool writeThroughLinker(unsigned short address, byte data);
};

#endif //HHUGBOY_LINKERWRANGLER_H
