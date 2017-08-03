
#ifndef HHUGBOY_MBCUNLMAKONOLD_H
#define HHUGBOY_MBCUNLMAKONOLD_H


#include "BasicMbc.h"

class MbcUnlMakonOld: public BasicMbc {
    public:
        MbcUnlMakonOld();
        virtual void writeMemory(unsigned short address, register byte data) override;

        virtual void resetVars(bool preserveMulticartState) override;

        virtual void readMbcSpecificVarsFromStateFile(FILE *savefile) override;
        virtual void writeMbcSpecificVarsToStateFile(FILE *savefile) override;

    private:
        bool isWeirdMode;
};


#endif //HHUGBOY_MBCUNLMAKONOLD_H
