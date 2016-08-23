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
    virtual void readMbcSpecificVarsFromStateFile(FILE *statefile) override;
    virtual void writeMemory(unsigned short address, register byte data) override;
    virtual void writeMbcSpecificVarsToStateFile(FILE *statefile) override;
    virtual byte readMemory(register unsigned short address) override;
private:
    void rtcUpdate();
};


#endif //HHUGBOY_MBCNIN3_H
