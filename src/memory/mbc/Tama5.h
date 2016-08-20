//
// Created by Alex on 20/08/2016.
//

#ifndef HHUGBOY_TAMA5_H
#define HHUGBOY_TAMA5_H


#include "Default.h"
//-------------------------------------------------------------------------
// for Bandai TAMA5 (Tamagotchi3)
//-------------------------------------------------------------------------

class Tama5 : public Default {
    byte readMemory(register unsigned short address);
private:
    void updateTamaRtc();
};


#endif //HHUGBOY_TAMA5_H
