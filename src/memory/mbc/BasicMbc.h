//
// Created by Alex on 20/08/2016.
//

#ifndef HHUGBOY_BASICMBC_H
#define HHUGBOY_BASICMBC_H


#include "../../types.h"
#include "AbstractMbc.h"

//-------------------------------------------------------------------------
// Read: for most MBCs
//-------------------------------------------------------------------------
class BasicMbc : public AbstractMbc {
public:
    virtual byte readMemory(register unsigned short address) override;
    virtual void resetVars() override;
    virtual void readNewMbcSpecificVarsFromStateFile(FILE *statefile) override;
    virtual void readOldMbcSpecificVarsFromStateFile(FILE *statefile) override;
    virtual void writeNewMbcSpecificVarsToStateFile(FILE *statefile) override;
    virtual void writeOldMbcSpecificVarsToStateFile(FILE *statefile) override;

    virtual void writeMemory(unsigned short address, register byte data) override;
};


#endif //HHUGBOY_BASICMBC_H
