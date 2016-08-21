//
// Created by Alex on 20/08/2016.
//

#ifndef HHUGBOY_MBCNINCAMERA_H
#define HHUGBOY_MBCNINCAMERA_H


#include "BasicMbc.h"
//-------------------------------------------------------------------------
// for GB Pocket Camera
//-------------------------------------------------------------------------
class MbcNinCamera : public BasicMbc {
    byte readMemory(register unsigned short address) override;
};


#endif //HHUGBOY_MBCNINCAMERA_H
