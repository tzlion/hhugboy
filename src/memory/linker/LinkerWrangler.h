
#ifndef HHUGBOY_LINKERWRANGLER_H
#define HHUGBOY_LINKERWRANGLER_H

#include <windef.h>
#include "../../types.h"

#define READ_ROM_THRU_LINKER false
#define READ_RAM_THRU_LINKER true
#define CACHE_BANK_0 false
#define CACHE_BANK_1 false
#define USE_PREDUMPED_BANK_0 false

typedef bool(__cdecl *LibInitLinker)();
typedef void(__cdecl *LibDeinitLinker)();
typedef void(__cdecl *LibReadBlock)(byte * dest, unsigned short addr, int len);
typedef byte(__cdecl *LibReadByte)(unsigned short addr);
typedef void(__cdecl *LibWriteByte)(unsigned short addr, byte val);
typedef void(__cdecl *LibSetLogger)(void(*)(const char*));
typedef bool(__cdecl *LibIsLinkerActive)();
typedef byte*(__cdecl *LibGetBank0)();

class LinkerWrangler {
    private:
        static bool readBank0;
        static byte bank0[0x4000];
        static bool readBank1;
        static byte bank1[0x4000];
        static bool libLoaded;
        static LibInitLinker libInitLinker;
        static LibDeinitLinker libDeinitLinker;
        static LibReadBlock libReadBlock;
        static LibReadByte libReadByte;
        static LibWriteByte libWriteByte;
        static LibSetLogger libSetLogger;
        static LibIsLinkerActive libIsLinkerActive;
        static LibGetBank0 libGetBank0;
        static HINSTANCE libgblink;
public:
        static bool shouldReadThroughLinker(unsigned short address);
        static byte readThroughLinker(unsigned short address);
        static bool shouldWriteThroughLinker(unsigned short address, byte data);
        static void writeThroughLinker(unsigned short address, byte data);
        static bool initLinker();
        static void deinitLinker();
};

#endif //HHUGBOY_LINKERWRANGLER_H
