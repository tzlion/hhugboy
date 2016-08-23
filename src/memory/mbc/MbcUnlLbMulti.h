//
// Created by Alex on 23/08/2016.
//

#ifndef HHUGBOY_MBCUNLLBMULTI_H
#define HHUGBOY_MBCUNLLBMULTI_H


#include "MbcNin5.h"
//-------------------------------------------------------------------------
// for multicarts with Last Bible music ex. VFame's ones
//-------------------------------------------------------------------------
class MbcUnlLbMulti : public MbcNin5 {
public:
    virtual void writeMemory(unsigned short address, register byte data) override;
    virtual void resetVars(bool preserveMulticartState) override;
    virtual void writeMbcSpecificVarsToStateFile(FILE *statefile) override;
    virtual void readMbcSpecificVarsFromStateFile(FILE *statefile) override;

private:
    byte vfmultimode=0;
    byte vfmultibank=0;
    byte vfmultimem=0;
    byte vfmultifinal=0;
};


#endif //HHUGBOY_MBCUNLLBMULTI_H
