/*
 * Additional mapper support for hhugboy emulator
 * by taizou 2020
 * This file released under Creative Commons CC0 https://creativecommons.org/publicdomain/zero/1.0/legalcode
 *
 * As part of the hhugboy project it is also licensed under the GNU General Public License v2
 * See "license.txt" in the project root
 */

#include "MbcUnlMaVf.h"

void MbcUnlMaVf::writeMemory(unsigned short address, register byte data) {
    MbcUnlBbd::writeMemory(address, data);
}

MbcUnlMaVf::MbcUnlMaVf() : MbcUnlBbd( 0, 0 )
{
}

void MbcUnlMaVf::resetVars(bool preserveMulticartState) {
    bitSwapMode = 0;
    bankSwapMode = 0;
    AbstractMbc::resetVars(preserveMulticartState);
}

byte MbcUnlMaVf::swapDataByte(byte data) {
    return switchOrder(data, maVfDataReordering[bitSwapMode]);
}

byte MbcUnlMaVf::swapBankByte(byte data) {
    return data; // not supported or at least documented for this mapper
}

bool MbcUnlMaVf::isDataSwapModeSupported() {
    return true;
}

bool MbcUnlMaVf::isBankSwapModeSupported() {
    return (bankSwapMode == 0); // not supported or at least documented for this mapper

}
