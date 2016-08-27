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
    MbcLicTama5();

    virtual void resetVars(bool preserveMulticartState) override;

    virtual void readMbcSpecificVarsFromSaveFile(FILE *savefile) override;

    virtual void writeMbcSpecificVarsToSaveFile(FILE *savefile) override;

private:
    void updateTamaRtc();
    int tama_flag;
    byte tama_time;
    int tama_val4;
    int tama_val5;
    int tama_val6;
    int tama_val7;
    int tama_count;
    int tama_change_clock;
    int tama_month;

};


#endif //HHUGBOY_MBCLICTAMA5_H
