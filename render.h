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
#ifndef RENDER_H
#define RENDER_H

typedef unsigned short WORD;
typedef unsigned long DWORD;

void fill_gfx_buffers(unsigned long val);

void LCDoff_fill_gfx_buffer(unsigned long val);

extern void (*draw_border)();

void fill_line16(unsigned short* adr, unsigned long val, int count);

void fill_line32(unsigned long* adr, unsigned long val, int count);

void copy_line16(unsigned short* target, unsigned short* src, int count);

void copy_line32(unsigned long* target, unsigned long* src, int count);

extern void (*draw_screen)();

extern DWORD* gfx_pal32;
extern WORD* gfx_pal16;

void mix_gbc_colors();

extern const unsigned short GBC_DMGBG_palettes[12][4];
extern const unsigned short GBC_DMGOBJ0_palettes[12][4];
extern const unsigned short GBC_DMGOBJ1_palettes[12][4];

extern const unsigned short LCD_palette_brown[4];
extern const unsigned short LCD_palette_green[4];
extern const unsigned short GB_palette_gray[4];

#endif
