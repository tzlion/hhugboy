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

#include <ddraw.h>

#include <string>

using namespace std;

#define VISUAL_RUMBLE_STRENGTH 5

extern DWORD* gfx_pal32;
extern WORD* gfx_pal16;

void mix_gbc_colors();

void initPaletteShifts();
bool initPalettes();

#define SafeRelease(x) if(x) { x->Release(), x=NULL; }

#ifdef ALLOW_DEBUG
void draw_debug_screen();
#endif

class DirectDraw {
    
    public:
        
        void (DirectDraw::*drawBorder)();
		void (DirectDraw::*drawScreen)();
        
        DirectDraw(HWND* inHwnd);
        ~DirectDraw();
        
        void setDrawMode(bool mix);
        
        bool init();
        
        void setBorderFilter(videofiltertype type);
        void setGameboyFilter(videofiltertype type);
        
        void showMessage(wstring message, int duration, gb_system* targetGb);
        
        int getBitCount();
        
        void handleWindowResize();
        void setRect(bool gb2open);
        
        // when this works properly the below can be made private

		int bitCount;
        int rs,gs,bs;
        
        int borderFilterHeight;
        int borderFilterWidth;
        int gameboyFilterHeight;
        int gameboyFilterWidth;
        videofiltertype borderFilterType;
        videofiltertype gameboyFilterType;

    private:
        
        static int ffs(UINT mask);
        
        void initPaletteShifts();
        
        bool changeFilters();

        wstring messageText;
        int messageDuration;
        gb_system* messageGb;
        
		IDirectDrawClipper* ddClip;
		
        int getFilterDimension(videofiltertype type);
        
        int changeRect;
            
		void drawScreenGeneric16(WORD* buffer);
		void drawScreenGeneric32(DWORD* buffer);
		void drawScreen16();
		void drawScreen32();
		void drawScreenMix16();
		void drawScreenMix32();
		
		void drawBorder16();
		void drawBorder32();
		        
        void gbTextOut();
        
		IDirectDraw7* dd;
		IDirectDrawSurface7* ddSurface;
		IDirectDrawSurface7* bSurface;
		IDirectDrawSurface7* borderSurface;       
        
        int lPitch;
        int borderLPitch;
        
        HFONT afont;
        
        RECT targetBltRect;
        
        HWND* hwnd;
        
};

#endif
