
#include <cstdio>
#include "MbcUnlPokeAct.h"
#include "../../debug.h"

byte lastpiss = 00;

byte MbcUnlPokeAct::readMemory(unsigned short address) {

    if (address >= 0x4000 && address <= 0x7fff && rom_bank >= 0x80) {
        // protection values read
        if (address < 0x5000) {
            byte digits = (address >> 4) & 0xff;
            byte reverse[8] = {7,6,5,4,3,2,1,0};

            byte lookedup = 0;
            byte calculated = 0;

            char buff[100];

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
                    calculated = bitwank(digits);
                    switch(digits) {
                        // idk the logic for this one yet
                        case 0x06: lookedup = 0x30; break;
                        case 0x0e: lookedup = 0x32; break;
                        case 0x16: lookedup = 0x70; break;
                        case 0x1e: lookedup = 0x72; break;
                        case 0x26: lookedup = 0x70; break;
                        case 0x2e: lookedup = 0x72; break;
                        case 0x36: lookedup = 0x74; break;
                        case 0x3e: lookedup = 0x76; break;
                        case 0x46: lookedup = 0xb0; break;
                        case 0x4e: lookedup = 0xb2; break;
                        case 0x56: lookedup = 0xf0; break;
                        case 0x5e: lookedup = 0xf2; break;
                        case 0x66: lookedup = 0xf0; break;
                        case 0x6e: lookedup = 0xf2; break;
                        case 0x76: lookedup = 0xf4; break;
                        case 0x7e: lookedup = 0xf6; break;
                        case 0x86: lookedup = 0xb0; break;
                        case 0x8e: lookedup = 0xb2; break;
                        case 0x96: lookedup = 0xf0; break;
                        case 0x9e: lookedup = 0xf2; break;
                        case 0xa6: lookedup = 0xf0; break;
                        case 0xae: lookedup = 0xf2; break;
                        case 0xb6: lookedup = 0xf4; break;
                        case 0xbe: lookedup = 0xf6; break;
                        case 0xc6: lookedup = 0xb8; break;
                        case 0xce: lookedup = 0xba; break;
                        case 0xd6: lookedup = 0xf8; break;
                        case 0xde: lookedup = 0xfa; break;
                        case 0xe6: lookedup = 0xf8; break;
                        case 0xee: lookedup = 0xfa; break;
                        case 0xf6: lookedup = 0xfc; break;
                        case 0xfe: lookedup = 0xfe; break;
                    }
                    if (digits != lastpiss) {
                        lastpiss = digits;
                        sprintf(buff,"digits %x = %x / %x", digits, lookedup, calculated);
                        debug_win(buff);
                    }
                    return lookedup;
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

byte MbcUnlPokeAct::bitwank(byte digits) {
    // here it ORs each pair of bits to create the first half of the returned byte
    // then ANDs each pair of bits to create the second half
    // is there a simpler way to do this? maybe?
    byte evenbits = digits & 0xaa;
    byte oddbits = digits & 0x55;
    byte ors = evenbits | (oddbits << 1);
    byte ands = evenbits & (oddbits << 1);
    byte evensToFirstHalf[8] = {0,2,4,6,1,3,5,7};
    byte evensToSecondHalf[8] = {1,3,5,7,0,2,4,6};
    return (switchOrder(ors, evensToFirstHalf) & 0xf0) | (switchOrder(ands, evensToSecondHalf) & 0x0f);
}
