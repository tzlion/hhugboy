/*
 * Additional mapper support for hhugboy emulator
 * by taizou 2021
 * This file released under Creative Commons CC0 https://creativecommons.org/publicdomain/zero/1.0/legalcode
 *
 * As part of the hhugboy project it is also licensed under the GNU General Public License v2
 * See "license.txt" in the project root
 */
#ifndef HHUGBOY_MBCUNLPOKEACT_H
#define HHUGBOY_MBCUNLPOKEACT_H

#include "MbcNin5.h"

// Simple protection only known to be used by 口袋怪獸-動作篇 with HK0819 pcb

class MbcUnlPokeAct : public MbcNin5 {
    public:
        virtual byte readMemory(unsigned short address) override;
        virtual void writeMemory(unsigned short address, register byte data) override;
};

#endif //HHUGBOY_MBCUNLPOKEACT_H
