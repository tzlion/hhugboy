//
// Created by Alex on 20/08/2016.
//

#include <cstdio>
#include "MbcNin7.h"
#include "../../mainloop.h" // Todo: can we not pls. The sensor values actually belong to the cart!

byte MbcNin7::readMemory(register unsigned short address) {

    if(address >= 0xA000 && address < 0xC000)
    {
        switch(address & 0xa0f0)
        {
            case 0xa000:
            case 0xa010:
            case 0xa060:
            case 0xa070:
                return 0;
            case 0xa020:
                // sensor X low byte
                return sensorX & 255;
            case 0xa030:
                // sensor X high byte
                return sensorX >> 8;
            case 0xa040:
                // sensor Y low byte
                return sensorY & 255;
            case 0xa050:
                // sensor Y high byte
                return sensorY >> 8;
            case 0xa080:
                return MBC7_value;
        }
        return 0xff;
    }

    return BasicMbc::readMemory(address);
}

void MbcNin7::readOldMbcSpecificVarsFromStateFile(FILE *statefile) {
    fread(&(MBC7_cs), sizeof(int), 1, statefile);
    fread(&(MBC7_sk), sizeof(int), 1, statefile);
    fread(&(MBC7_state), sizeof(int), 1, statefile);
    fread(&(MBC7_buffer), sizeof(int), 1, statefile);
    fread(&(MBC7_idle), sizeof(int), 1, statefile);
    fread(&(MBC7_count), sizeof(int), 1, statefile);
    fread(&(MBC7_code), sizeof(int), 1, statefile);
    fread(&(MBC7_address), sizeof(int), 1, statefile);
    fread(&(MBC7_writeEnable), sizeof(int), 1, statefile);
    fread(&(MBC7_value), sizeof(int), 1, statefile);
    fread(&sensorX,sizeof(int),1,statefile);
    fread(&sensorY,sizeof(int),1,statefile);
}

void MbcNin7::writeOldMbcSpecificVarsToStateFile(FILE *statefile) {
    fwrite(&(MBC7_cs), sizeof(int), 1, statefile);
    fwrite(&(MBC7_sk), sizeof(int), 1, statefile);
    fwrite(&(MBC7_state), sizeof(int), 1, statefile);
    fwrite(&(MBC7_buffer), sizeof(int), 1, statefile);
    fwrite(&(MBC7_idle), sizeof(int), 1, statefile);
    fwrite(&(MBC7_count), sizeof(int), 1, statefile);
    fwrite(&(MBC7_code), sizeof(int), 1, statefile);
    fwrite(&(MBC7_address), sizeof(int), 1, statefile);
    fwrite(&(MBC7_writeEnable), sizeof(int), 1, statefile);
    fwrite(&(MBC7_value), sizeof(int), 1, statefile);
    fwrite(&sensorX,sizeof(int),1,statefile);
    fwrite(&sensorY,sizeof(int),1,statefile);
}
