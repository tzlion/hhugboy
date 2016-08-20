//
// Created by Alex on 20/08/2016.
//

#ifndef HHUGBOY_MBCDEFAULT_H
#define HHUGBOY_MBCDEFAULT_H


#include "../../types.h"
#include "Mbc.h"
//-------------------------------------------------------------------------
// Read: for most MBCs
//-------------------------------------------------------------------------
class Default : public Mbc {
public:
    virtual byte readMemory(register unsigned short address);
    virtual void resetVars();
};


#endif //HHUGBOY_MBCDEFAULT_H
