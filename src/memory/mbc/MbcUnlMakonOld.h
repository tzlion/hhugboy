
#ifndef HHUGBOY_MBCUNLMAKONOLD_H
#define HHUGBOY_MBCUNLMAKONOLD_H



#include "MbcNin1.h"
class MbcUnlMakonOld: public BasicMbc {
   public:
        MbcUnlMakonOld(int originalRomSize);
        virtual void writeMemory(unsigned short address, register byte data) override;

        virtual void resetVars(bool preserveMulticartState) override;

        virtual void readMbcSpecificVarsFromStateFile(FILE *savefile) override;
        virtual void writeMbcSpecificVarsToStateFile(FILE *savefile) override;

        int originalRomSize;

    private:
        void handleOldMakonCartModeSet(unsigned short address, byte data);
        bool isWeirdMode;
        byte flippo1[8] = {0,1,2,4,3,6,5,7}; // rockman
        byte flippo2[8] = {0,1,2,3,4,7,5,6}; // mario
};


#endif //HHUGBOY_MBCUNLMAKONOLD_H
