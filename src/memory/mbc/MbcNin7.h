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
    virtual void readMbcSpecificVarsFromStateFile(FILE *statefile) override;
    virtual void writeMbcSpecificVarsToStateFile(FILE *statefile) override;
    virtual void writeMemory(unsigned short address, register byte data) override;
    virtual byte readMemory(register unsigned short address) override;
    MbcNin7();
private:
    int MBC7_cs; // chip select
    int MBC7_sk; // ?
    int MBC7_state; // mapper state
    int MBC7_buffer; // buffer for receiving serial data
    int MBC7_idle; // idle state
    int MBC7_count; // count of bits received
    int MBC7_code; // command received
    int MBC7_address; // address received
    int MBC7_writeEnable; // write enable
    int MBC7_value; // value to return on ram
};


#endif //HHUGBOY_MBCNIN7_H
