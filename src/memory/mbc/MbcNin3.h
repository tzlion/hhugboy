//
// Created by Alex on 20/08/2016.
//

#ifndef HHUGBOY_MBCNIN3_H
#define HHUGBOY_MBCNIN3_H

#include "BasicMbc.h"

//-------------------------------------------------------------------------
// for MBC3
//-------------------------------------------------------------------------
class MbcNin3 : public BasicMbc {
public:
    virtual void readOldMbcSpecificVarsFromStateFile(FILE *statefile) override;
    virtual void writeOldMbcSpecificVarsToStateFile(FILE *statefile) override;

private:
    byte readMemory(register unsigned short address) override;
};


#endif //HHUGBOY_MBCNIN3_H
