//
// Created by Alex on 21/08/2016.
//

#ifndef HHUGBOY_MBCNIN1_H
#define HHUGBOY_MBCNIN1_H


#include "BasicMbc.h"
//-------------------------------------------------------------------------
// for MBC1 and HuC1 (is that okay? what's the difference?)
//-------------------------------------------------------------------------
class MbcNin1 : public BasicMbc {
public:
    virtual void writeMemory(unsigned short address, register byte data) override;

};


#endif //HHUGBOY_MBCNIN1_H
