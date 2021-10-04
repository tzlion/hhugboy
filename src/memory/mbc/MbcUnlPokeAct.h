
#ifndef HHUGBOY_MBCUNLPOKEACT_H
#define HHUGBOY_MBCUNLPOKEACT_H

#include "MbcNin5.h"

class MbcUnlPokeAct : public MbcNin5 {

    public:
        virtual byte readMemory(unsigned short address) override;
        virtual void writeMemory(unsigned short address, register byte data) override;

};


#endif //HHUGBOY_MBCUNLPOKEACT_H
