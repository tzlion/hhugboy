
#ifndef HHUGBOY_MBCUNLMAKONOLD_H
#define HHUGBOY_MBCUNLMAKONOLD_H



#include "MbcNin1.h"
class MbcUnlMakonOld: public MbcNin1 {
   public:
        MbcUnlMakonOld(int originalRomSize);
        virtual void writeMemory(unsigned short address, register byte data) override;

        virtual void resetVars(bool preserveMulticartState) override;

        virtual void readMbcSpecificVarsFromStateFile(FILE *savefile) override;
        virtual void writeMbcSpecificVarsToStateFile(FILE *savefile) override;

        int originalRomSize;

    private:
        bool isWeirdMode;
};


#endif //HHUGBOY_MBCUNLMAKONOLD_H
