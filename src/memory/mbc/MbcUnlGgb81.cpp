/*
 * Additional mapper support for hhugboy emulator
 * by taizou 2021
 * This file released under Creative Commons CC0 https://creativecommons.org/publicdomain/zero/1.0/legalcode
 *
 * As part of the hhugboy project it is also licensed under the GNU General Public License v2
 * See "license.txt" in the project root
 */

#include "MbcUnlGgb81.h"

void MbcUnlGgb81::writeMemory(unsigned short address, register byte data) {
    MbcUnlBbd::writeMemory(address, data);
}

MbcUnlGgb81::MbcUnlGgb81() : MbcUnlBbd( 0, 0 )
{
}

void MbcUnlGgb81::resetVars(bool preserveMulticartState) {
    bitSwapMode = 0;
    bankSwapMode = 0;
    AbstractMbc::resetVars(preserveMulticartState);
}

byte MbcUnlGgb81::swapDataByte(byte data) {
    return switchOrder(data, ggb81DataReordering[bitSwapMode]);
}

byte MbcUnlGgb81::swapBankByte(byte data) {
    return data; // not supported or at least documented for this mapper
}

bool MbcUnlGgb81::isDataSwapModeSupported() {
    return true;
}

bool MbcUnlGgb81::isBankSwapModeSupported() {
    return (bankSwapMode == 0); // not supported or at least documented for this mapper
}
