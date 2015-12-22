#include <windows.h>

class Palette {
    public:
        DWORD* gfxPal32;
        WORD* gfxPal16;

        bool initPalettes(int bitCount);
        void setPaletteShifts(int rs, int gs, int bs);
        void mixGbcColours();
        ~Palette();
    private:
        int rs,gs,bs;
        int bitCount;
};
