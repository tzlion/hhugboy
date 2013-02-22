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
#define UNICODE

#include <windows.h>
#include "GB.h"
#include "options.h"


#include <string>

using namespace std;

#define VISUAL_RUMBLE_STRENGTH 5

extern HWND hwnd;

extern DWORD* gfx_pal32;
extern WORD* gfx_pal16;

void mix_gbc_colors();

extern void (*draw_border)();

bool change_filter();

extern RECT target_blt_rect;

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

class DirectDraw {
    
    public:
        
        DirectDraw();
        void setBorderFilter(videofiltertype type);
        void setGameboyFilter(videofiltertype type);
        
        void showMessage(wstring message, int duration, gb_system* targetGb);
        
        // when this works properly the below can be made private
        int borderFilterHeight;
        int borderFilterWidth;
        int gameboyFilterHeight;
        int gameboyFilterWidth;
        videofiltertype borderFilterType;
        videofiltertype gameboyFilterType;
        
        wstring messageText;
        int messageDuration;
        gb_system* messageGb;
        
    private:
        int getFilterDimension(videofiltertype type);
};

#endif
