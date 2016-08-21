//
// Created by Alex on 21/08/2016.
//

#ifndef HHUGBOY_MBCUNLLICHENG_H
#define HHUGBOY_MBCUNLLICHENG_H


#include "MbcNin5.h"

class MbcUnlLiCheng : public MbcNin5 {
public:
    virtual void writeMemory(unsigned short address, register byte data) override;

};


#endif //HHUGBOY_MBCUNLLICHENG_H
