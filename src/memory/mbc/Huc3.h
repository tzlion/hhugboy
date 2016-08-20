//
// Created by Alex on 20/08/2016.
//

#ifndef HHUGBOY_HUC3_H
#define HHUGBOY_HUC3_H


#include "Default.h"
//-------------------------------------------------------------------------
// for HuC3
//-------------------------------------------------------------------------
class Huc3 : public Default {
    byte readMemory(register unsigned short address);
};


#endif //HHUGBOY_HUC5_H
