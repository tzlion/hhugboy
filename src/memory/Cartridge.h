/*
   hhugboy Game Boy emulator
   copyright 2013-2016 taizou

   Based on GEST
   Copyright (C) 2003-2010 TM
   This file incorporates code from VisualBoyAdvance
   Copyright (C) 1999-2004 by Forgotten

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
#ifndef HHUGBOY_GB_MBC_H_H
#define HHUGBOY_GB_MBC_H_H

#include "mbc/AbstractMbc.h"
#include "mbc/MbcLicHuc3.h"

class Cartridge {

public:
    Cartridge(byte** gbMemMap, byte** cartRom, byte** cartRam, CartridgeMetadata** metadata);

    void setMbcType(MbcType memory_type);

    byte readMemory(register unsigned short address);
    void writeMemory(unsigned short address, register byte data);

    int determineRamSize();

    AbstractMbc *mbc;

private:

    byte** gbMemMap;
    CartridgeMetadata** metadata;
    byte** cartRam;
    byte** cartRom;

};

#endif //HHUGBOY_GB_MBC_H_H
