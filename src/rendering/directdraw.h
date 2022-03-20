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

#include "../types.h"

#define WIN32_LEAN_AND_MEAN
#define UNICODE

#include <windows.h>
#include "../GB.h"
#include "../options.h"

#include "Renderer.h"
#include "palette.h"

#include "filters/filters.h"
#include "DirectDrawStuff.h"


#include <string>

using namespace std;


#define SafeRelease(x) if(x) { x->Release(), x=NULL; }

class DirectDraw: public Renderer {
    
    public:
        
        void (DirectDraw::*drawBorder)();
		void (DirectDraw::*drawScreen)();
		
#ifdef ALLOW_DEBUG
		void (DirectDraw::*drawDebugScreen)();
#endif
        DirectDraw(HWND* inHwnd);
        ~DirectDraw();
        
        void setDrawMode(bool mix);
        
        bool init(Palette* palette);
        
        void setBorderFilter(videofiltertype type);
        void setGameboyFilter(videofiltertype type);
        
        void showMessage(wstring message, int duration, gb_system* targetGb);
        
        int getBitCount();
        
        void handleWindowResize();
        void setRect(bool gb2open);
        
        void toggleFiltering(bool on);

    private:

        DirectDrawStuff* directDrawStuff;
        
		void *dxBorderBufferRender;
		void *dxBufferMix;

        void applyPaletteShifts();
        static int ffs(UINT mask);
        
        bool changeFilters();

        wstring messageText;
        int messageDuration;
        gb_system* messageGb;
            
		template<typename TYPE>
		void drawScreenGeneric(TYPE* buffer);
		void drawScreen16();
		void drawScreen32();
        template<typename TYPE>
        void drawScreenMixGeneric(TYPE* buffer);
		void drawScreenMix16();
		void drawScreenMix32();

        template<typename TYPE>
        void drawBorderGeneric(TYPE* buffer, TYPE* paletteSrc);
		void drawBorder16();
		void drawBorder32();
		
		void gameboyFilter(WORD *target,WORD *src,int width,int height,int pitch);
		void gameboyFilter(DWORD *target,DWORD *src,int width,int height,int pitch);
		void borderzFilter(WORD *target,WORD *src,int width,int height,int pitch);
		void borderzFilter(DWORD *target,DWORD *src,int width,int height,int pitch);
		        
        void gbTextOut();

	    bool createSurfaces(int gbWidthMulti, int gbHeightMulti, int borderWidthMulti, int borderHeightMulti);

        int oddframe;

    int borderFilterHeight;
        int borderFilterWidth;
        int gameboyFilterHeight;
        int gameboyFilterWidth;

        HFONT afont;
        
        RECT targetBltRect;
        
        HWND* hwnd;
        
		int bitCount;
		
		Palette* palette;
		
		Filter* gbFilter;
		Filter* borderFilter;
        
};

#endif
