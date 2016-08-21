//
// Created by Alex on 20/08/2016.
//

#ifndef HHUGBOY_MBCUNLSINTAX_H
#define HHUGBOY_MBCUNLSINTAX_H


#include "BasicMbc.h"
//-------------------------------------------------------------------------
// for SiNTAX
//-------------------------------------------------------------------------
class MbcUnlSintax : public BasicMbc {
    byte readMemory(register unsigned short address) override;
};


#endif //HHUGBOY_MBCUNLSINTAX_H
