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

char* str_table[40] =
{
 "Writing config file failed!",
 "Error reading config file!",
 "Error: Not enough memory!",
 "DirectDraw Init failed! Quitting...",
 "DirectInput init failed",
 "FSOUND_Init failed!",
 "Reading save file failed!",
 "Writing save file failed!",
 "Error loading rom!",
 "GG cheat format: xxx-xxx-xxx or xxx-xxx",
 "hhugboy v%s\nbased on GEST 1.1.1 by TM\nmodified by taizou",
 "Current slot:",
 "Not a GB rom!",
 "Save to slot", // shouldn't be very long
 "failed!",
 "OK!",
 "Load from slot", // shouldn't be very long // that's what he said
 "Unknown ROM size! (Really a Game Boy ROM?)",
 "Cannot open file!",
 "Finding file from zip failed!",
 "Bad zip file!",
 "No ROM image found inside ZIP file!",
 "Error opening rom file from zip",
 "Error reading file!",
 "Error: fseek failed",
 "Error: Read ROM info failed",
 "Error: Read ROM into memory failed! ROM size in header might be wrong.",
 "Unknown opcode!",
 "opcode",
 "Unidentified ROM type!",
 "GB roms",
 "All files",
 "Rom name: \t%s\nGBC feature: \t%s\nNew Licensee: \t%X\nSGB feature: \t%s\nCartridge Type: \t%X\nROM Size: \t%d KBytes\nRAM Size: \t%d KBytes\nCountry: \t\t%X ,%s\nLicensee: \t%X - %s\nVersion: \t\t%X\nComplement check: %X - %s\nChecksum: \t%X",
 "yes",
 "no",
 "GBC only",
 "Japan",
 "non-Japan",
 "(OK)",
 "(Wrong)"
};
