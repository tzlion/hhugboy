/*
 * Additional mapper support for hhugboy emulator
 * by taizou 2021
 * This file released under Creative Commons CC0 https://creativecommons.org/publicdomain/zero/1.0/legalcode
 *
 * As part of the hhugboy project it is also licensed under the GNU General Public License v2
 * See "license.txt" in the project root
 */

#include "MbcUnlNewGbHk.h"

byte MbcUnlNewGbHk::readMemory(unsigned short address) {

    if (address >= 0x4000 && address <= 0x7fff && rom_bank >= 0x80) {
        // protection values read
        if (address < 0x5000) {
            byte digits = (address >> 4) & 0xff;
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
                    return getValue6(digits);
                case 7:
                    return getValue7(digits);
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

byte MbcUnlNewGbHk::getValue6(byte digits) {
    // OR each pair of bits to create the first half of the returned byte
    // then AND each pair to create the second half
    byte evenbits = switchOrder(digits, evenBitsTwice);
    byte oddbits = switchOrder(digits, oddBitsTwice);
    return ((evenbits | oddbits) & 0xf0) | ((evenbits & oddbits) & 0x0f);
}

byte MbcUnlNewGbHk::getValue7(byte digits) {
    // XOR each pair of bits & invert the result to create the first half of the returned byte
    // then XOR each pair without inverting to create the second half
    byte evenbits = switchOrder(digits, evenBitsTwice);
    byte oddbits = switchOrder(digits, oddBitsTwice);
    return (((evenbits ^ oddbits) ^ 0xff) & 0xf0) | ((evenbits ^ oddbits) & 0x0f);
}
