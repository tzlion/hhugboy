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

#define ROM_FILENAME_SIZE 1024

enum { ROM, MBC1, MBC2, MMM01, MBC3, MBC4, MBC5, MBC7, Camera, TAMA5, HuC3, HuC1, UnknownCart, Niutoude };

struct GBrom
{
   char name[15];
   unsigned short newlic;
   byte lic;
   byte carttype;
   int ROMsize;
   int RAMsize;
   int bankType;
   byte destcode;
   byte version;
   byte complement;
   bool complementok;
   unsigned short checksum;
   bool battery;
   bool RTC;
   bool rumble;
   int CGB;
   int SGB;
};

#endif
