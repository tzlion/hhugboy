//
// Created by Alex on 23/08/2016.
//

#ifndef HHUGBOY_MBCUNLLBMULTI_H
#define HHUGBOY_MBCUNLLBMULTI_H


#include "MbcNin5.h"
//-------------------------------------------------------------------------
// for multicarts with Last Bible music ex. VFame's ones
//-------------------------------------------------------------------------
class MbcUnlLbMulti : public MbcNin5 {
public:
    virtual void writeMemory(unsigned short address, register byte data) override;
};


#endif //HHUGBOY_MBCUNLLBMULTI_H
