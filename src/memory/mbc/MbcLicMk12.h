//
// Created by Alex on 21/08/2016.
//

#ifndef HHUGBOY_MBCLICMK12_H
#define HHUGBOY_MBCLICMK12_H


#include "MbcNin1.h"
//-------------------------------------------------------------------------
// for Mortal Kombat 1&2
// another MBC1 derived collection cart, similar to HuColl but not quite?
// Works only with "Mortal Kombat I & II (UE) [a1][!]" currently
//-------------------------------------------------------------------------
class MbcLicMk12 : public MbcNin1 {
public:
    virtual void writeMemory(unsigned short address, register byte data) override;

};


#endif //HHUGBOY_MBCLICMK12_H
