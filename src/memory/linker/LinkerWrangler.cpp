
#include "LinkerWrangler.h"
#include "../../ui/dialogs/LinkerLog.h"

#include "stdio.h"
#include "string.h"

#include "windows.h"

bool LinkerWrangler::readBank0 = false;
byte LinkerWrangler::bank0[0x4000];
bool LinkerWrangler::readBank1 = false;
byte LinkerWrangler::bank1[0x4000];

LibInitLinker LinkerWrangler::libInitLinker;
LibDeinitLinker LinkerWrangler::libDeinitLinker;
LibReadBlock LinkerWrangler::libReadBlock;
LibReadByte LinkerWrangler::libReadByte;
LibWriteByte LinkerWrangler::libWriteByte;
LibSetLogger LinkerWrangler::libSetLogger;
LibIsLinkerActive LinkerWrangler::libIsLinkerActive;
LibGetBank0 LinkerWrangler::libGetBank0;
HINSTANCE LinkerWrangler::libgblink;

bool LinkerWrangler::shouldReadThroughLinker(unsigned short address)
{
    if (libgblink == nullptr || !libIsLinkerActive()) {
        return false;
    }
    if (address < 0x8000) {
        return READ_ROM_THRU_LINKER;
    }
    if (address < 0xc000) {
        return READ_RAM_THRU_LINKER;
    }
    return true;
}

byte LinkerWrangler::readThroughLinker(unsigned short address)
{
    if (address < 0x4000 && USE_PREDUMPED_BANK_0) {
        return libGetBank0()[address];
    }
    if (address < 0x4000 && CACHE_BANK_0) {
        if (!readBank0) {
            LinkerLog::addMessage("DUMPING BANK 0");
            libReadBlock(bank0, 0, 0x4000);
            readBank0=true;
        }
        return bank0[address];
    }
    if (address < 0x8000 && CACHE_BANK_1) {
        if (!readBank1) {
            LinkerLog::addMessage("DUMPING BANK 1");
            libReadBlock(bank1, 0x4000, 0x4000);
            readBank1=true;
        }
        return bank1[address-0x4000];
    }
    byte datum = libReadByte(address);
    if (address >= 0x8000) {
        char buffer[420];
        sprintf(buffer, "read %02x from %04x", datum, address);
        LinkerLog::addMessage(buffer);
    }
    return datum;
}

bool LinkerWrangler::shouldWriteThroughLinker(unsigned short address, byte data)
{
    if (libgblink == nullptr || !libIsLinkerActive()) {
        return false;
    }
    return true;
}

bool LinkerWrangler::writeThroughLinker(unsigned short address, byte data)
{
    readBank1 = false;
    char buffer[420];
    sprintf(buffer, "write %02x to %04x", data, address);
    LinkerLog::addMessage(buffer);
    libWriteByte(address, data);
}


bool LinkerWrangler::initLinker()
{
    libgblink = LoadLibrary("libgblink.dll");

    if (libgblink != NULL) {
        libSetLogger = (LibSetLogger)GetProcAddress(libgblink, "SetLogger");
        libInitLinker = (LibInitLinker)GetProcAddress(libgblink, "InitLinker");
        libDeinitLinker = (LibDeinitLinker)GetProcAddress(libgblink, "DeinitLinker");
        libReadByte = (LibReadByte)GetProcAddress(libgblink, "ReadByte");
        libReadBlock = (LibReadBlock)GetProcAddress(libgblink, "ReadBlock");
        libWriteByte = (LibWriteByte)GetProcAddress(libgblink, "WriteByte");
        libIsLinkerActive = (LibIsLinkerActive)GetProcAddress(libgblink, "IsLinkerActive");
        libGetBank0 = (LibGetBank0)GetProcAddress(libgblink, "GetBank0");
    } else {
        LinkerLog::addMessage("Unable to load libgblink.dll");
        return false;
    }

    libSetLogger(LinkerLog::addMessage);
    libInitLinker();
}

bool LinkerWrangler::deinitLinker()
{
    libDeinitLinker();
}
