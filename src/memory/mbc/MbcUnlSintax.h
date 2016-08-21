//
// Created by Alex on 20/08/2016.
//

#ifndef HHUGBOY_MBCUNLSINTAX_H
#define HHUGBOY_MBCUNLSINTAX_H


#include "BasicMbc.h"
#include "MbcNin5.h"

//-------------------------------------------------------------------------
// for SiNTAX
//-------------------------------------------------------------------------
class MbcUnlSintax : public MbcNin5 {
public:
    virtual byte readMemory(register unsigned short address) override;
    virtual void writeMemory(unsigned short address, register byte data) override;
};


#endif //HHUGBOY_MBCUNLSINTAX_H
