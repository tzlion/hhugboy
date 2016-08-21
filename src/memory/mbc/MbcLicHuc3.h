//
// Created by Alex on 20/08/2016.
//

#ifndef HHUGBOY_MBCLICHUC3_H
#define HHUGBOY_MBCLICHUC3_H


#include "BasicMbc.h"
//-------------------------------------------------------------------------
// for HuC3
//-------------------------------------------------------------------------
class MbcLicHuc3 : public BasicMbc {
    byte readMemory(register unsigned short address);

public:
    virtual void readOldMbcSpecificVarsFromStateFile(FILE *statefile) override;
    virtual void writeOldMbcSpecificVarsToStateFile(FILE *statefile) override;
};


#endif //HHUGBOY_MBCLICHUC3_H
