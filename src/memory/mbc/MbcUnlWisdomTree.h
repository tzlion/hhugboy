/*
 * Additional mapper support for hhugboy emulator
 * by NewRisingSun 2020
 * This file released under Creative Commons CC0 https://creativecommons.org/publicdomain/zero/1.0/legalcode
 *
 * As part of the hhugboy project it is also licensed under the GNU General Public License v2
 * See "license.txt" in the project root
 */

#ifndef HHUGBOY_MBCUNLWISDOMTREE_H
#define HHUGBOY_MBCUNLWISDOMTREE_H


#include "AbstractMbc.h"
class MbcUnlWisdomTree : public AbstractMbc {
public:
    virtual byte readMemory(register unsigned short address) override;
    virtual void writeMemory(unsigned short address, register byte data) override;
};


#endif //HHUGBOY_MBCUNLWISDOMTREE_H
