
#include "MbcUnlPokeAct.h"

byte MbcUnlPokeAct::readMemory(unsigned short address) {

    if (address >= 0x4000 && address <= 0x7fff && rom_bank >= 0x80) {
        // protection values read
        if (address < 0x5000) {
            byte digits = (address >> 4) & 0xff;
            byte reverse[8] = {7,6,5,4,3,2,1,0};
            switch (digits & 7) {
                case 0:
                    return digits;
                case 1:
                    return digits ^ 0xaa;
                case 2:
                    return digits ^ 0x55;
                case 3:
                    return ((digits >> 1) + (digits << 7)) & 0xff;
                case 4:
                    return ((digits << 1) + (digits >> 7)) & 0xff;
                case 5:
                    return switchOrder(digits, reverse);
                case 6:
                    switch(digits) {
                        // idk the logic for this one yet
                        case 0x06: return 0x30;
                        case 0x0e: return 0x32;
                        case 0x16: return 0x70;
                        case 0x1e: return 0x72;
                        case 0x26: return 0x70;
                        case 0x2e: return 0x72;
                        case 0x36: return 0x74;
                        case 0x3e: return 0x76;
                        case 0x46: return 0xb0;
                        case 0x4e: return 0xb2;
                        case 0x56: return 0xf0;
                        case 0x5e: return 0xf2;
                        case 0x66: return 0xf0;
                        case 0x6e: return 0xf2;
                        case 0x76: return 0xf4;
                        case 0x7e: return 0xf6;
                        case 0x86: return 0xb0;
                        case 0x8e: return 0xb2;
                        case 0x96: return 0xf0;
                        case 0x9e: return 0xf2;
                        case 0xa6: return 0xf0;
                        case 0xae: return 0xf2;
                        case 0xb6: return 0xf4;
                        case 0xbe: return 0xf6;
                        case 0xc6: return 0xb8;
                        case 0xce: return 0xba;
                        case 0xd6: return 0xf8;
                        case 0xde: return 0xfa;
                        case 0xe6: return 0xf8;
                        case 0xee: return 0xfa;
                        case 0xf6: return 0xfc;
                        case 0xfe: return 0xfe;
                    }
                case 7:
                    switch(digits) {
                        // idk the logic for this one yet either
                        case 0x07: return 0xd2;
                        case 0x0f: return 0xf0;
                        case 0x17: return 0x96;
                        case 0x1f: return 0xb4;
                        case 0x27: return 0x96;
                        case 0x2f: return 0xb4;
                        case 0x37: return 0xd2;
                        case 0x3f: return 0xf0;
                        case 0x47: return 0x5a;
                        case 0x4f: return 0x78;
                        case 0x57: return 0x1e;
                        case 0x5f: return 0x3c;
                        case 0x67: return 0x1e;
                        case 0x6f: return 0x3c;
                        case 0x77: return 0x5a;
                        case 0x7f: return 0x78;
                        case 0x87: return 0x5a;
                        case 0x8f: return 0x78;
                        case 0x97: return 0x1e;
                        case 0x9f: return 0x3c;
                        case 0xa7: return 0x1e;
                        case 0xaf: return 0x3c;
                        case 0xb7: return 0x5a;
                        case 0xbf: return 0x78;
                        case 0xc7: return 0xd2;
                        case 0xcf: return 0xf0;
                        case 0xd7: return 0x96;
                        case 0xdf: return 0xb4;
                        case 0xe7: return 0x96;
                        case 0xef: return 0xb4;
                        case 0xf7: return 0xd2;
                        case 0xff: return 0xf0;
                    }
            }
        } else {
            return 0xff;
        }
    }

    // additional behaviour observed on the Pokemon Action Chapter cart (HK0819 pcb):
    // when rom banks 20-5f are selected, reads give either all ff or alternating 4 bytes of 00/ff depending on the cart
    // banks 60-7f give the 2nd half of the rom data (which does not seem to actually be used by the game)
    // Monster Go Go 2 (HK0701 pcb) just maps the whole rom normally at 00-3f and mirrored normally at 40-7f
    // Action Chapter doesn't rely on this behaviour & it would break Go Go 2 so i haven't emulated it
    // just noting it here in case any future dump needs it

    return MbcNin5::readMemory(address);
}
