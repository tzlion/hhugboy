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
    if (address > 0x2000 && address < 0x3000 && (address & 0x80)) {
        return;
    }
    MbcNin5::writeMemory(address,data);
}
