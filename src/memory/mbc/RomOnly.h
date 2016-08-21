//
// Created by Alex on 21/08/2016.
//

#ifndef HHUGBOY_ROMONLY_H
#define HHUGBOY_ROMONLY_H


#include "AbstractMbc.h"
//-------------------------------------------------------------------------
// Read: for most MBCs
// Write: for ROM only only
//-------------------------------------------------------------------------
class RomOnly : public AbstractMbc {
public:
    virtual byte readMemory(register unsigned short address) override;
    virtual void writeMemory(unsigned short address, register byte data) override;
};


#endif //HHUGBOY_ROMONLY_H
