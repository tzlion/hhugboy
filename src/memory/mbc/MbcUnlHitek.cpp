/*
 * Additional mapper support for hhugboy emulator
 * by taizou 2016
 * This file released under Creative Commons CC0 https://creativecommons.org/publicdomain/zero/1.0/legalcode
 *
 * As part of the hhugboy project it is also licensed under the GNU General Public License v2
 * See "license.txt" in the project root
 */

#include "MbcUnlHitek.h"

void MbcUnlHitek::writeMemory(unsigned short address, register byte data) {

    if  ( ( address & 0xF000 ) == 0x3000 ) {
        // Hitek carts (at least Shui Hu Zhuan) start in a 'locked' mode that only allows bank 00-1f to be accessed
        // ShuiHu is a 1mb game so this effectively prevents reading of half of the rom
        // The game writes 01 to 3001 in its initialisation which unlocks the cart allowing all banks to be selected
        // There's probably no need to emulate this behaviour tho
        // Also I haven't yet tested if writes of other values or to other 3xxx addresses have any effect
        // So just ignoring them all for now
        return;
    }

    // Additional observed behaviour on Hitek: Writing 00 to 0x2000 loads bank 01 like MBC3, not bank 00 like MBC5
    // Not tested on BBD
    // No games known to rely on this & it's also not emulated for now

    MbcUnlBbd::writeMemory(address, data);

}

// Hitek carts (again only ShuiHu tested so far) also start up w both modes set to 7 by default
// Unlike BBD,Sintax etc which start up in your standard MBC-clone mode and require their weird shit to be switched ON
MbcUnlHitek::MbcUnlHitek() : MbcUnlBbd( 7, 7 )
{
}

void MbcUnlHitek::resetVars(bool preserveMulticartState) {
    bitSwapMode = 7;
    bankSwapMode = 7;
    AbstractMbc::resetVars(preserveMulticartState);
}

byte MbcUnlHitek::swapDataByte(byte data) {
    return switchOrder(data, hitekDataReordering[bitSwapMode]);
}

byte MbcUnlHitek::swapBankByte(byte data) {
    return switchOrder(data, hitekBankReordering[bankSwapMode]);
}

bool MbcUnlHitek::isDataSwapModeSupported() {
    return true;
}

bool MbcUnlHitek::isBankSwapModeSupported() {
    return true;
}
