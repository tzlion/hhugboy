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
// default English language

#include "strings.h"

wchar_t* str_table[40] =
{
 L"Writing config file failed!",
 L"Error reading config file!",
 L"Error: Not enough memory!",
 L"DirectDraw Init failed! Quitting...",
 L"DirectInput init failed",
 L"FSOUND_Init failed!",
 L"Reading save file failed!",
 L"Writing save file failed!",
 L"Error loading rom!",
 L"GG cheat format: xxx-xxx-xxx or xxx-xxx",
 L"hhugboy v%s\nbased on GEST 1.1.1 by TM\nmodified by taizou",
 L"Current slot:",
 L"Not a GB rom!",
 L"Save to slot", // shouldn't be very long
 L"failed!",
 L"OK!",
 L"Load from slot", // shouldn't be very long // that's what he said
 L"Unknown ROM size! (Really a Game Boy ROM?)",
 L"Cannot open file!",
 L"Finding file from zip failed!",
 L"Bad zip file!",
 L"No ROM image found inside ZIP file!",
 L"Error opening rom file from zip",
 L"Error reading file!",
 L"Error: fseek failed",
 L"Error: Read ROM info failed",
 L"Error: Read ROM into memory failed! ROM size in header might be wrong.",
 L"Unknown opcode!",
 L"opcode",
 L"Unidentified ROM type!",
 L"GB roms",
 L"All files",
 L"ROM name: \t%s\nGBC feature: \t%s\nNew licensee: \t%s\nSGB feature: \t%s\nCartridge type: \t%X\nROM size: \t\t%d KBytes\nRAM size: \t\t%d KBytes\nCountry: \t\t%X (%s)\nOld licensee: \t%X (%s)\nVersion: \t\t%X\nComplement check:\t%X %s\nChecksum: \t%X",
 L"yes",
 L"no",
 L"GBC only",
 L"Japan",
 L"non-Japan",
 L"(OK)",
 L"(Wrong)"
};
