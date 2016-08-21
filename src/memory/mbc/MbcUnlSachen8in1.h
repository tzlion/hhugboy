//
// Created by Alex on 21/08/2016.
//

#ifndef HHUGBOY_MBCUNLSACHEN8IN1_H
#define HHUGBOY_MBCUNLSACHEN8IN1_H


#include "BasicMbc.h"
//-------------------------------------------------------------------------
// for Sachen 8in1
// todo: Sky Ace, Flea War and Explosive don't boot?
//-------------------------------------------------------------------------
class MbcUnlSachen8in1 : public BasicMbc {
public:
    virtual void writeMemory(unsigned short address, register byte data) override;

};


#endif //HHUGBOY_MBCUNLSACHEN8IN1_H
