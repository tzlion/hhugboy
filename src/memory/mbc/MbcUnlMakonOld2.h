
#ifndef HHUGBOY_MBCUNLMAKONOLD2_H
#define HHUGBOY_MBCUNLMAKONOLD2_H


#include "MbcUnlMakonOld1.h"

class MbcUnlMakonOld2: public MbcUnlMakonOld1 {
    public:
        MbcUnlMakonOld2(int originalRomSize);
        virtual void writeMemory(unsigned short address, register byte data) override;
    private:
        byte flippo2[8] = {0,1,2,3,4,7,5,6}; // mario
};


#endif //HHUGBOY_MBCUNLMAKONOLD2_H
