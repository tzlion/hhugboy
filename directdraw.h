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
#ifndef GEST_DIRECTDRAW_H
#define GEST_DIRECTDRAW_H

#include "types.h"

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "GB.h"

//31-63-31 565
#define RGB16(red,green,blue) ( (WORD)(((red)<<11)|((green)<<5)|(blue)) )
#define GBC_RGB(red,green,blue) ( (WORD)(((blue)<<10)|((green)<<5)|(red)) )

#define VISUAL_RUMBLE_STRENGTH 5

extern int filter_width;
extern int filter_height;

extern int border_filter_width;
extern int border_filter_height;

extern HWND hwnd;

extern DWORD* gfx_pal32;
extern WORD* gfx_pal16;

void mix_gbc_colors();

extern void (*draw_border)();

bool change_filter();

extern RECT target_blt_rect;

extern wchar_t dx_message[60];
extern int message_time;
extern gb_system* message_GB;

extern const unsigned short GBC_DMGBG_palettes[12][4];
extern const unsigned short GBC_DMGOBJ0_palettes[12][4];
extern const unsigned short GBC_DMGOBJ1_palettes[12][4];

extern const unsigned short LCD_palette_brown[4];
extern const unsigned short LCD_palette_green[4];
extern const unsigned short GB_palette_gray[4];

extern int lPitch;
extern int dx_bitcount;

#define SafeRelease(x) if(x) { x->Release(), x=NULL; }

void Kill_DD();

bool Init_DD();

void gbTextOut();

extern void (*draw_screen)();
void draw_screen_mix16();
void draw_screen_mix32();
void draw_screen16();
void draw_screen32();
void draw_screen_generic16(WORD* buffer);
void draw_screen_generic32(DWORD* buffer);

#ifdef ALLOW_DEBUG
void draw_debug_screen();
#endif

#endif
