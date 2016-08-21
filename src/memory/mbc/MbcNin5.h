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
protected:
public:
    virtual void writeMemory(unsigned short address, register byte data) override;

protected:
    void mbc5Write(register unsigned short address, register byte data, bool isNiutoude, bool isSintax);
    void setXorForBank(byte bankNo);
};


#endif //HHUGBOY_MBCNIN5_H
