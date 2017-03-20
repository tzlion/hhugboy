//
// Created by Alex on 20/03/2017.
//

#ifndef HHUGBOY_MBCUNLMAKON_H
#define HHUGBOY_MBCUNLMAKON_H


#include "MbcNin5.h"

class MbcUnlMakonNew : public MbcNin5 {
    public:
        virtual void writeMemory(unsigned short address, register byte data) override;
    private:
        bool theMode = false;
};


#endif //HHUGBOY_MBCUNLMAKON_H
