//
// Created by Alex on 20/08/2016.
//

#ifndef HHUGBOY_MBCNIN7_H
#define HHUGBOY_MBCNIN7_H


#include "BasicMbc.h"
//-------------------------------------------------------------------------
// for MBC7
//-------------------------------------------------------------------------
class MbcNin7 : public BasicMbc {
public:
    virtual void readOldMbcSpecificVarsFromStateFile(FILE *statefile) override;
    virtual void writeOldMbcSpecificVarsToStateFile(FILE *statefile) override;
    virtual void writeMemory(unsigned short address, register byte data) override;
    virtual byte readMemory(register unsigned short address) override;
};


#endif //HHUGBOY_MBCNIN7_H
