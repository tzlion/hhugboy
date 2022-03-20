/*
   hhugboy Game Boy emulator
   copyright 2013-2018 taizou

   Based on GEST
   Copyright (C) 2003-2010 TM

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef HHUGBOY_CARTDETECTION_H
#define HHUGBOY_CARTDETECTION_H


#include "../types.h"
#include "../options.h"
#include "GB_MBC.h"

class CartDetection {
    public:
        CartridgeMetadata* processRomInfo(byte* rom, int romFileSize);
    private:
        void setCartridgeAttributesFromHeader(CartridgeMetadata *cartridge);
        bool detectUnlicensedCarts(byte *rom, CartridgeMetadata *cartridge, int romFileSize);
        bool detectMbc1ComboPacks(CartridgeMetadata *cartridge, int romFileSize);
        bool detectFlashCartHomebrew(CartridgeMetadata *cartridge, int romFileSize);
        void readHeader(byte* rom, CartridgeMetadata* cartridge, int romFileSize);
        unlCompatMode detectUnlCompatMode(byte* rom, CartridgeMetadata* cartridge, int romFileSize);
        byte detectGbRomSize(int romFileSize);
};

#endif //HHUGBOY_CARTDETECTION_H
