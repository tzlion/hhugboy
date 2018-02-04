/*
   hhugboy Game Boy emulator
   copyright 2013 taizou

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
#ifndef ROM_H
#define ROM_H

#include "types.h"
#include "memory/MbcType.h"

#define ROM_FILENAME_SIZE 1024

struct Header
{
    char name[15];
    char newlic[2];
    byte lic;
    byte carttype;
    int ROMsize;
    int RAMsize;
    byte destcode;
    byte version;
    byte complement;
    bool complementok;
    unsigned short checksum;
    int SGB;
    int CGB; // this gets modified but it shouldn't
};

struct Cartridge
{
   int ROMsize;
   int RAMsize;
    // these are the real/effective values as opposed to the headers which could be wrong
    // maybe something here like:
    // - rom size (the full)
    // - addressable rom size (for multicarts) ...
    // and same for ram
    // or maybe do a struct for rom/ram sizes by header no idk
   Header header;
   MbcType mbcType;
   bool battery;
   bool RTC;
   bool rumble;
};

#endif
