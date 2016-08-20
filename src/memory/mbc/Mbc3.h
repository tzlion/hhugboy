//
// Created by Alex on 20/08/2016.
//

#ifndef HHUGBOY_MBC3_H
#define HHUGBOY_MBC3_H

#include "Default.h"

//-------------------------------------------------------------------------
// for MBC3
//-------------------------------------------------------------------------
class Mbc3 : public Default {
    byte readMemory(register unsigned short address);
};


#endif //HHUGBOY_MBC3_H
