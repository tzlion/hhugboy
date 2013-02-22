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

#ifndef GFX_H
#define GFX_H

#include "types.h"

#include "GB.h"

extern byte tile_signed_trans[256];
extern byte invert_table[256];

extern unsigned short line_buffer[160];

const int VID_EN_BG = 1;
const int VID_EN_WIN = 2;
const int VID_EN_SPRITE = 4;

void screenshotPng(char* filename, gb_system* gameboy);
void screenshotHtml(char* filename);

extern int video_enable;


//31-63-31 565
#define RGB16(red,green,blue) ( (WORD)(((red)<<11)|((green)<<5)|(blue)) )
#define GBC_RGB(red,green,blue) ( (WORD)(((blue)<<10)|((green)<<5)|(red)) )


extern const unsigned short GBC_DMGBG_palettes[12][4];
extern const unsigned short GBC_DMGOBJ0_palettes[12][4];
extern const unsigned short GBC_DMGOBJ1_palettes[12][4];

extern const unsigned short LCD_palette_brown[4];
extern const unsigned short LCD_palette_green[4];
extern const unsigned short GB_palette_gray[4];

#endif
