//
// Created by Alex on 21/08/2016.
//

#ifndef HHUGBOY_MBCNIN5_H
#define HHUGBOY_MBCNIN5_H


#include "BasicMbc.h"
//-------------------------------------------------------------------------
// for MBC5 and MBC5 rumble
//-------------------------------------------------------------------------
class MbcNin5 : public BasicMbc {
public:
    virtual void writeMemory(unsigned short address, register byte data) override;
};


#endif //HHUGBOY_MBCNIN5_H
