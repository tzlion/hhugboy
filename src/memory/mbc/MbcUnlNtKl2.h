
#ifndef HHUGBOY_MBCUNLNTKL2_H
#define HHUGBOY_MBCUNLNTKL2_H


#include "MbcUnlNtK11.h"

class MbcUnlNtKl2: public MbcUnlNtK11 {
    public:
        MbcUnlNtKl2(int originalRomSize);
        virtual void writeMemory(unsigned short address, register byte data) override;
    private:
        byte flippo2[8] = {0,1,2,3,4,7,5,6};
};


#endif //HHUGBOY_MBCUNLNTKL2_H
