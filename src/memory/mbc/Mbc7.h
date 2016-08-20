//
// Created by Alex on 20/08/2016.
//

#ifndef HHUGBOY_MBC7_H
#define HHUGBOY_MBC7_H


#include "Default.h"
//-------------------------------------------------------------------------
// for MBC7
//-------------------------------------------------------------------------
class Mbc7 : public Default {
    byte readMemory(register unsigned short address);
};


#endif //HHUGBOY_MBC7_H
