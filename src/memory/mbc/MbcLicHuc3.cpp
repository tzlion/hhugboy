//
// Created by Alex on 20/08/2016.
//

#include <cstdio>
#include "MbcLicHuc3.h"

byte MbcLicHuc3::readMemory(register unsigned short address) {
    if(address >= 0xA000 && address < 0xC000)
    {
        if(HuC3_RAMflag >= 0x0b && HuC3_RAMflag < 0x0e)
        {
            if(HuC3_RAMflag == 0x0D)
                return 1;
            return HuC3_RAMvalue;
        }
        if(!(*gbRom)->RAMsize)
            return 0xFF;
    }

    return BasicMbc::readMemory(address);
}

void MbcLicHuc3::readOldMbcSpecificVarsFromStateFile(FILE *statefile) {
    fread(&(HuC3_time), sizeof(unsigned int), 1, statefile);
    fread(&(HuC3_last_time), sizeof(time_t), 1, statefile);
    fread(&(rtc).s, sizeof(int), 1, statefile);

    //fread(HuC3_register,sizeof(int),8,statefile);
    fread(&(HuC3_RAMvalue), sizeof(int), 1, statefile);
    //fread(&HuC3_address,sizeof(int),1,statefile);
    fread(&(HuC3_RAMflag), sizeof(int), 1, statefile);
}

void MbcLicHuc3::writeOldMbcSpecificVarsToStateFile(FILE *statefile) {
    fwrite(&(HuC3_time), sizeof(unsigned int), 1, statefile);
    fwrite(&(HuC3_last_time), sizeof(time_t), 1, statefile);
    fwrite(&(rtc).s, sizeof(int), 1, statefile);

    //fwrite(HuC3_register,sizeof(int),8,statefile);
    fwrite(&(HuC3_RAMvalue), sizeof(int), 1, statefile);
    //fwrite(&HuC3_address,sizeof(int),1,statefile);
    fwrite(&(HuC3_RAMflag), sizeof(int), 1, statefile);
}
