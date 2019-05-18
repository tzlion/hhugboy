/*
 * Additional mapper support for hhugboy emulator
 * by taizou 2013-2019
 * This file released under Creative Commons CC0 https://creativecommons.org/publicdomain/zero/1.0/legalcode
 *
 * As part of the hhugboy project it is also licensed under the GNU General Public License v2
 * See "license.txt" in the project root
 */

#include "MbcUnlLiCheng.h"

void MbcUnlLiCheng::writeMemory(unsigned short address, register byte data) {
    // Writes to certain addresses in the ROM bank switch range should be ignored
    // The carts do garbage writes to these addresses to select the wrong ROM banks if they're run as MBC5
    if (address > 0x2000 && address < 0x3000) {
        // different carts can be configured differently here, annoyingly
        // e.g. tested zoids ignores writes with 0x80 set and cannon fodder ignores writes with 0x800
        // i have not yet encountered a game that fails if you skip both, but...
        if ((address & 0x80) || (address & 0x800)) {
            return;
        }
    }
    MbcNin5::writeMemory(address,data);
}
