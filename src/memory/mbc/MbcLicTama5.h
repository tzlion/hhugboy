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

public:
    virtual void readSgbMbcSpecificVarsFromStateFile(FILE *statefile) override;
    virtual void readMbcSpecificVarsFromStateFile(FILE *statefile) override;
    virtual void writeSgbMbcSpecificVarsToStateFile(FILE *statefile) override;
    virtual void writeMemory(unsigned short address, register byte data) override;
    virtual void writeMbcSpecificVarsToStateFile(FILE *statefile) override;
    virtual byte readMemory(register unsigned short address);
private:
    void updateTamaRtc();

};


#endif //HHUGBOY_MBCLICTAMA5_H
