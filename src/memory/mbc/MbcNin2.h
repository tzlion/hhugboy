//
// Created by Alex on 21/08/2016.
//

#ifndef HHUGBOY_MBCNIN2_H
#define HHUGBOY_MBCNIN2_H


#include "BasicMbc.h"
//-------------------------------------------------------------------------
// for MBC2
//-------------------------------------------------------------------------
class MbcNin2 : public BasicMbc {
public:
    virtual void writeMemory(unsigned short address, register byte data) override;

};


#endif //HHUGBOY_MBCNIN2_H
