
#ifndef HHUGBOY_MBCUNLNTOLD2_H
#define HHUGBOY_MBCUNLNTOLD2_H


#include "MbcUnlNtOld1.h"

class MbcUnlNtOld2: public MbcUnlNtOld1 {
    public:
        MbcUnlNtOld2(int originalRomSize);
        virtual void writeMemory(unsigned short address, register byte data) override;
    private:
        byte flippo2[8] = {0,1,2,3,4,7,5,6};
};


#endif //HHUGBOY_MBCUNLNTOLD2_H
