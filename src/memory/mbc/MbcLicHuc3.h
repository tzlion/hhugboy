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
    virtual void readMbcSpecificVarsFromStateFile(FILE *statefile) override;
    virtual void writeMbcSpecificVarsToStateFile(FILE *statefile) override;
    virtual void writeMemory(unsigned short address, register byte data) override;
    virtual byte readMemory(register unsigned short address);
    MbcLicHuc3();

    virtual void resetVars(bool preserveMulticartState) override;

    virtual void readMbcSpecificVarsFromSaveFile(FILE *savefile) override;

    virtual void writeMbcSpecificVarsToSaveFile(FILE *savefile) override;

private:
    void updateHuc3Time();

    int HuC3_register[8];
    int HuC3_RAMvalue;
    int HuC3_RAMaddress;
    int HuC3_address;
    int HuC3_RAMflag;

    int HuC3_flag;
    int HuC3_shift;

    time_t HuC3_last_time;
    unsigned int HuC3_time;

};


#endif //HHUGBOY_MBCLICHUC3_H
