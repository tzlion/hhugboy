//
// Created by Alex on 21/08/2016.
//

#ifndef HHUGBOY_MBCLICHUCOLL_H
#define HHUGBOY_MBCLICHUCOLL_H


#include "MbcNin1.h"
//-------------------------------------------------------------------------
// for JP Collection Carts - Hudson + Culture Brain
// MBC1 derived
//-------------------------------------------------------------------------
class MbcLicHuColl : public MbcNin1 {
public:
    virtual void writeMemory(unsigned short address, register byte data) override;

};


#endif //HHUGBOY_MBCLICHUCOLL_H
