//
// Created by Alex on 21/08/2016.
//

#ifndef HHUGBOY_MBCUNLPOKE2IN1_H
#define HHUGBOY_MBCUNLPOKE2IN1_H


#include "BasicMbc.h"
//-------------------------------------------------------------------------
// for Pokemon Red & Blue 2-in-1
//-------------------------------------------------------------------------
class MbcUnlPoke2in1 : public BasicMbc {
public:
    virtual void writeMemory(unsigned short address, register byte data) override;

};


#endif //HHUGBOY_MBCUNLPOKE2IN1_H
