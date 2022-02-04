
#include <cstdio>
#include "LinkerWrangler.h"
#include "GbLinker.h"
#include "../../ui/dialogs/LinkerLog.h"

bool LinkerWrangler::readBank0 = false;
U8 LinkerWrangler::bank0[0x4000];
bool LinkerWrangler::readBank1 = false;
U8 LinkerWrangler::bank1[0x4000];

bool LinkerWrangler::shouldReadThroughLinker(unsigned short address)
{
    if (!GbLinker::linkerActive) {
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
        return GbLinker::bank0[address];
    }
    if (address < 0x4000 && CACHE_BANK_0) {
        if (!readBank0) {
            LinkerLog::addMessage("DUMPING BANK 0");
            GbLinker::readBlock(bank0,0,0x4000);
            readBank0=true;
        }
        return bank0[address];
    }
    if (address < 0x8000 && CACHE_BANK_1) {
        if (!readBank1) {
            LinkerLog::addMessage("DUMPING BANK 1");
            GbLinker::readBlock(bank1,0x4000,0x4000);
            readBank1=true;
        }
        return bank1[address-0x4000];
    }
    byte datum = GbLinker::readByte(address);
    if (address >= 0x8000) {
        char buffer[420];
        sprintf(buffer, "read %02x from %04x", datum, address);
        LinkerLog::addMessage(buffer);
    }
    return datum;
}

bool LinkerWrangler::shouldWriteThroughLinker(unsigned short address, byte data)
{
    if (!GbLinker::linkerActive) {
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
    GbLinker::writeByte(address,data);
}

bool LinkerWrangler::initLinker()
{
    GbLinker::initLinker();
}

bool LinkerWrangler::deinitLinker()
{
    GbLinker::deinitLinker();
}
