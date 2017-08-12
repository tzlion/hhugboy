
#ifndef HHUGBOY_MBCUNLNTKL1_H
#define HHUGBOY_MBCUNLNTKL1_H



#include "MbcNin1.h"
class MbcUnlNtK11: public BasicMbc {
   public:
        MbcUnlNtK11(int originalRomSize);
        virtual void writeMemory(unsigned short address, register byte data) override;

        virtual void resetVars(bool preserveMulticartState) override;

        virtual void readMbcSpecificVarsFromStateFile(FILE *savefile) override;
        virtual void writeMbcSpecificVarsToStateFile(FILE *savefile) override;

        int originalRomSize;

    protected:
        void handleOldMakonCartModeSet(unsigned short address, byte data);
        bool isWeirdMode;

    private:
        byte flippo1[8] = {0,1,2,4,3,6,5,7};
};


#endif //HHUGBOY_MBCUNLNTKL1_H
