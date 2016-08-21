//
// Created by Alex on 20/08/2016.
//

#ifndef HHUGBOY_MBCLICHUC3_H
#define HHUGBOY_MBCLICHUC3_H


#include "BasicMbc.h"
//-------------------------------------------------------------------------
// for HuC-3
//-------------------------------------------------------------------------
class MbcLicHuc3 : public BasicMbc {

public:
    virtual void readOldMbcSpecificVarsFromStateFile(FILE *statefile) override;
    virtual void writeOldMbcSpecificVarsToStateFile(FILE *statefile) override;
    virtual void writeMemory(unsigned short address, register byte data) override;
    virtual byte readMemory(register unsigned short address);

private:
    void updateHuc3Time();
};


#endif //HHUGBOY_MBCLICHUC3_H
