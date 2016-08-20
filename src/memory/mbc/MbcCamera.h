//
// Created by Alex on 20/08/2016.
//

#ifndef HHUGBOY_MBCCAMERA_H
#define HHUGBOY_MBCCAMERA_H


#include "Default.h"
//-------------------------------------------------------------------------
// for GB Pocket Camera
//-------------------------------------------------------------------------
class MbcCamera : public Default {
    byte readMemory(register unsigned short address);
};


#endif //HHUGBOY_MBCCAMERA_H
