//
// Created by Alex on 20/08/2016.
//

#ifndef HHUGBOY_MBCUNLSINTAX_H
#define HHUGBOY_MBCUNLSINTAX_H


#include "BasicMbc.h"
#include "MbcNin5.h"

//-------------------------------------------------------------------------
// for SiNTAX
//-------------------------------------------------------------------------
class MbcUnlSintax : public MbcNin5 {
public:
    MbcUnlSintax();
    virtual byte readMemory(register unsigned short address) override;
    virtual void writeMemory(unsigned short address, register byte data) override;

    virtual void resetVars(bool preserveMulticartState) override;

    virtual void readMbcSpecificVarsFromStateFile(FILE *savefile) override;
    virtual void writeMbcSpecificVarsToStateFile(FILE *savefile) override;

private:

    void setXorForBank(byte bankNo);

    byte sintax_mode;
    byte sintax_xor2;
    byte sintax_xor3;
    byte sintax_xor4;
    byte sintax_xor5;
};

inline byte switchOrder( byte input, byte* reorder )
{
    byte newbyte=0;
    for( byte x=0;x<8;x++ ) {
        newbyte += ( ( input >> ( 7 - reorder[x] ) ) & 1 ) << ( 7 - x );
    }

    return newbyte;
}

#endif //HHUGBOY_MBCUNLSINTAX_H
