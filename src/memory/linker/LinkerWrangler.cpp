
#include <cstdio>
#include "LinkerWrangler.h"
#include "GbLinker.h"
#include "../../types.h"
#include "../../ui/dialogs/LinkerLog.h"

bool LinkerWrangler::readBank0 = false;
U8 LinkerWrangler::bank0[0x4000];
bool LinkerWrangler::readBank1 = false;
U8 LinkerWrangler::bank1[0x4000];
byte LinkerWrangler::last4000 = 00;

bool LinkerWrangler::shouldReadThroughLinker(unsigned short address)
{
    if (!GbLinker::linkerActive) {
        return false;
    }
    if (address < 0x8000 || last4000 < 0x08) {
        return false;
    }
    return true;
}

byte LinkerWrangler::readThroughLinker(unsigned short address)
{
    // super fights still doesnt work like this.. does that mean bank 0 changes maybe?
    if (address < 0x4000) {
        if (!readBank0) {
            LinkerLog::addMessage("DUMPING BANK 0");
            GbLinker::readBlock(bank0,0,0x4000);
            readBank0=true;
        }
        return bank0[address];
    }
    if (address < 0x8000) {
        // for PB Dreams this is slower.. you could probably cache the banks for an official game
        if (!readBank1) {
            LinkerLog::addMessage("DUMPING BANK 1");
            GbLinker::readBlock(bank1,0x4000,0x4000);
            readBank1=true;
        }
        return bank1[address-0x4000];
    }
    if (address<0x4000) return GbLinker::bank0[address]; // quicker maybe // tho not sure if working
    // super fighter 2001 doesnt work this way but ping ball dreams works pretty well
    byte datum = GbLinker::readByte(address);
    if (address >= 0x8000) {
        char fart[420];
        sprintf(fart, "read %02x from %04x", datum, address);
        LinkerLog::addMessage(fart);
    }
    return datum;

}

bool LinkerWrangler::shouldWriteThroughLinker(unsigned short address, byte data)
{
    if (!GbLinker::linkerActive) {
        return false;
    }
    if (address == 0x4000) {
        return true;
    }
    return false;
}

bool LinkerWrangler::writeThroughLinker(unsigned short address, byte data)
{
    readBank1 = false;
    if (address == 0x4000) {
        last4000 = data;
    }
    char fart[420];
    sprintf(fart, "write %02x to %04x", data, address);
    LinkerLog::addMessage(fart);
    GbLinker::writeByte(address,data);
}
