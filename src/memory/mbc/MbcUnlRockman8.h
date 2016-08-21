//
// Created by Alex on 21/08/2016.
//

#ifndef HHUGBOY_MBCUNLROCKMAN8_H
#define HHUGBOY_MBCUNLROCKMAN8_H


#include "BasicMbc.h"
//-------------------------------------------------------------------------
// for Rockman8
// only game that requires memory echo emulation ?
//-------------------------------------------------------------------------
class MbcUnlRockman8 : public BasicMbc {
public:
    virtual void writeMemory(unsigned short address, register byte data) override;

};


#endif //HHUGBOY_MBCUNLROCKMAN8_H
