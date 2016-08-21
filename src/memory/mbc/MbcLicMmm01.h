//
// Created by Alex on 21/08/2016.
//

#ifndef HHUGBOY_MBCLICMMM01_H
#define HHUGBOY_MBCLICMMM01_H


#include "BasicMbc.h"
//-------------------------------------------------------------------------
// for MMM01
// Momotarou Collection 2, Taito Variety Pack, Mani collections..
// Support doesn't seem to be implemented properly tho (and many existent roms are in the wrong order)
//-------------------------------------------------------------------------
class MbcLicMmm01 : public BasicMbc {
public:
    virtual void writeMemory(unsigned short address, register byte data) override;

};


#endif //HHUGBOY_MBCLICMMM01_H
