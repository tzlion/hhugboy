//
// Created by Alex on 20/08/2016.
//

#ifndef HHUGBOY_MBCLICTAMA5_H
#define HHUGBOY_MBCLICTAMA5_H


#include "BasicMbc.h"
//-------------------------------------------------------------------------
// for Bandai TAMA5 (Tamagotchi3)
//-------------------------------------------------------------------------

class MbcLicTama5 : public BasicMbc {
    byte readMemory(register unsigned short address);
private:
    void updateTamaRtc();

public:
    virtual void readNewMbcSpecificVarsFromStateFile(FILE *statefile) override;
    virtual void readOldMbcSpecificVarsFromStateFile(FILE *statefile) override;
    virtual void writeNewMbcSpecificVarsToStateFile(FILE *statefile) override;

    virtual void writeMemory(unsigned short address, register byte data) override;

    virtual void writeOldMbcSpecificVarsToStateFile(FILE *statefile) override;
};


#endif //HHUGBOY_MBCLICTAMA5_H
