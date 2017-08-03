
#ifndef HHUGBOY_MBCUNLMAKONOLD_H
#define HHUGBOY_MBCUNLMAKONOLD_H


#include "BasicMbc.h"

class MbcUnlMakonOld: public BasicMbc {
    public:
        virtual void writeMemory(unsigned short address, register byte data) override;
};


#endif //HHUGBOY_MBCUNLMAKONOLD_H
