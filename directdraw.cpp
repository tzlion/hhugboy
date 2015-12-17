/*
   hhugboy Game Boy emulator
   copyright 2013 taizou

   Based on GEST
   Copyright (C) 2003-2010 TM
   This file incorporates code from VisualBoyAdvance
   Copyright (C) 1999-2004 by Forgotten

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
#define UNICODE

#include <stdio.h>
#include <ddraw.h>

#include <iostream>
#include <fstream>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <string>
#include <math.h>
using namespace std;

#include "directdraw.h"
#include "GB.h"

#include "filters.h"

#include "cpu.h"
#include "debug.h"
#include "SGB.h"
#include "strings.h"
#include "render.h"
#include "config.h"

#include "main.h"

int RGB_BIT_MASK = 0;

DirectDraw::DirectDraw(HWND* inHwnd)
{
   //debug_print("Emu Center HX DirectDraw ON");
   this->borderFilterWidth = this->borderFilterHeight = this->gameboyFilterWidth = this->gameboyFilterHeight = 1;
   this->borderFilterType = this->gameboyFilterType = VIDEO_FILTER_NONE;
   this->hwnd = inHwnd;
   //RECT this->targetBltRect;
   this->lPitch = 160;
   this->changeRect = 0;
}

DirectDraw::~DirectDraw()
{
    if(this->dxBufferMix != NULL) { 
        if(this->bitCount==16) {
            delete [] (WORD*)this->dxBufferMix;
        } else {
            delete [] (DWORD*)this->dxBufferMix;
        }
        this->dxBufferMix = NULL; 
    }         
    if(this->dxBorderBufferRender != NULL) { 
        if(this->bitCount==16) {
            delete [] (WORD*)this->dxBorderBufferRender;
        } else {
            delete [] (DWORD*)this->dxBorderBufferRender;
        }
        this->dxBorderBufferRender = NULL; 
    }   
      
    SafeRelease(this->bSurface);
    SafeRelease(this->borderSurface);
    SafeRelease(this->ddSurface);
    SafeRelease(this->ddClip);
    SafeRelease(this->dd);
    
    DeleteObject(this->afont);
}

void DirectDraw::setDrawMode(bool mix) 
{
	if (!mix) {
		if(this->bitCount==16) {
			this->drawScreen = &DirectDraw::drawScreen16;
		} else {
			this->drawScreen = &DirectDraw::drawScreen32;
		}
	} else {
		if(this->bitCount==16) {
			this->drawScreen = &DirectDraw::drawScreenMix16;
		} else {
			this->drawScreen = &DirectDraw::drawScreenMix32;
		}
	}
	
}

bool DirectDraw::init(Palette* palette)
{
    HRESULT ddrval;
    DDSURFACEDESC2 ddsd;
    //DDSCAPS2 ddscaps;
    
    ddrval = DirectDrawCreateEx(NULL, (void**)&(this->dd), IID_IDirectDraw7, NULL); 
    if(ddrval!=DD_OK)
    {
        debug_print("DirectDraw Create failed!"); 
        return false;
    }
    ddrval = this->dd->SetCooperativeLevel(*hwnd, DDSCL_NORMAL);
    if(ddrval!=DD_OK)
    {
        debug_print("DirectDraw: SetCooperativelevel failed!"); 
        return false;
    }
    
    ddrval = dd->CreateClipper(0,&(this->ddClip),NULL);
    if(ddrval!=DD_OK)
    {
        debug_print("DirectDraw: CreateClipper failed!"); 
        return false;
    }
    ddClip->SetHWnd(0,*(this->hwnd));
    
    ZeroMemory(&ddsd,sizeof(ddsd));
    ddsd.dwSize = sizeof(DDSURFACEDESC2);
    ddsd.dwFlags = DDSD_CAPS;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
    ddrval = this->dd->CreateSurface(&ddsd,&(this->ddSurface),NULL);
    if(ddrval != DD_OK) 
    {
        debug_print("DirectDraw: Create main surface failed!"); 
        return false;
    }
    
    this->ddSurface->SetClipper(this->ddClip);
    
    ZeroMemory(&ddsd,sizeof(ddsd));
    ddsd.dwSize = sizeof(DDSURFACEDESC2);
    ddsd.dwFlags = DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT;
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN|DDSCAPS_VIDEOMEMORY;//DDSCAPS_SYSTEMMEMORY;
    ddsd.dwWidth = 160;
    ddsd.dwHeight = 144;
    
    ddrval = this->dd->CreateSurface(&ddsd,&(this->bSurface),NULL);
    if(ddrval != DD_OK) 
    {
        debug_print("DirectDraw: Create gb surface failed!"); 
        return false;
    }
    ddsd.dwWidth = 256;
    ddsd.dwHeight = 224;   
    ddrval = this->dd->CreateSurface(&ddsd,&(this->borderSurface),NULL);
    if(ddrval != DD_OK) 
    {
        debug_print("DirectDraw: Create border surface failed!"); 
        return false;
    }   
    
    // empty the new surface
    DDBLTFX clrblt;
    ZeroMemory(&clrblt,sizeof(DDBLTFX));
    clrblt.dwSize=sizeof(DDBLTFX);
    clrblt.dwFillColor = RGB(0,0,0);
    this->bSurface->Blt(NULL,NULL,NULL,DDBLT_COLORFILL,&clrblt);
    this->borderSurface->Blt(NULL,NULL,NULL,DDBLT_COLORFILL,&clrblt);
    
    ZeroMemory(&ddsd,sizeof(ddsd));
    ddsd.dwSize = sizeof(DDSURFACEDESC2);
    ddsd.dwFlags = DDSD_PIXELFORMAT;
    this->bSurface->Lock(NULL,&ddsd,DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR,NULL);
    
    this->bitCount = ddsd.ddpfPixelFormat.dwRGBBitCount; 
    this->lPitch = ddsd.lPitch;
    
    this->bSurface->Unlock(NULL);
    
    this->palette = palette;
    
    this->applyPaletteShifts();
    
    if (!this->palette->initPalettes(this->bitCount)) return false;
    
	this->setDrawMode(false);
    
    if(this->bitCount  == 16) {
        this->dxBufferMix = new WORD[140*166];     
        this->dxBorderBufferRender = new WORD[256*224];
        
        this->drawBorder = &DirectDraw::drawBorder16;
        this->gameboyFilter16 = &filter_none;
        
        this->lPitch >>= 1;
    } else {
        this->dxBufferMix = new DWORD[140*166];  
        this->dxBorderBufferRender = new DWORD[256*224];
        
        this->drawBorder = &DirectDraw::drawBorder32;
        this->gameboyFilter32 = &filter_none;     
        
        this->lPitch >>= 2;
    }
    
    if(!this->dxBufferMix || !this->dxBorderBufferRender) {
        debug_print(str_table[ERROR_MEMORY]); 
        return false;
    }
    
    SetCurrentDirectory(options->program_directory.c_str()); // hmmmmmmm
    AddFontResource(L"PCPaintBoldSmall.ttf");
    
    return true;
}

void DirectDraw::applyPaletteShifts()
{
    DDPIXELFORMAT px;
    
    px.dwSize = sizeof(px);
    
    this->bSurface->GetPixelFormat(&px);
    
    int rs = ffs(px.dwRBitMask);
    int gs = ffs(px.dwGBitMask);
    int bs = ffs(px.dwBBitMask);
    
    RGB_BIT_MASK = 0x421;
    
    if((px.dwFlags&DDPF_RGB) != 0 && px.dwRBitMask == 0xF800 && px.dwGBitMask == 0x07E0 && px.dwBBitMask == 0x001F) {
        gs++;
        RGB_BIT_MASK = 0x821;
    } else if((px.dwFlags&DDPF_RGB) != 0 && px.dwRBitMask == 0x001F && px.dwGBitMask == 0x07E0 && px.dwBBitMask == 0xF800) {
        gs++;
        RGB_BIT_MASK = 0x821;
    } else if(this->bitCount == 32 || this->bitCount == 24) {// 32-bit or 24-bit
        rs += 3;
        gs += 3;
        bs += 3;
    }
    
    this->palette->setPaletteShifts(rs, gs, bs);
}

int DirectDraw::ffs(UINT mask)
{
    int m = 0;
    if(mask) {
        while (!(mask & (1 << m)))
            m++;
        return m;
    }
    return 0;
}

void DirectDraw::showMessage(wstring message, int duration, gb_system* targetGb)
{
    this->messageText = message;
    this->messageDuration = duration;
    this->messageGb = targetGb;
}

void DirectDraw::gbTextOut()
{ // note use of GB here
    if(this->messageDuration && GB == messageGb) {
        --this->messageDuration;
        HDC aDC;
        if(this->bSurface->GetDC(&aDC)==DD_OK) {
            SelectObject(aDC,this->afont);
            SetBkMode(aDC, TRANSPARENT);
            SetTextColor(aDC,RGB(255,0,128));

            TextOut(aDC,3*this->gameboyFilterWidth,3*this->gameboyFilterHeight,this->messageText.c_str(),this->messageText.length());
            TextOut(aDC,1*this->gameboyFilterWidth,1*this->gameboyFilterHeight,this->messageText.c_str(),this->messageText.length());
            TextOut(aDC,1*this->gameboyFilterWidth,3*this->gameboyFilterHeight,this->messageText.c_str(),this->messageText.length());
            TextOut(aDC,3*this->gameboyFilterWidth,1*this->gameboyFilterHeight,this->messageText.c_str(),this->messageText.length());
            
            TextOut(aDC,3*this->gameboyFilterWidth,2*this->gameboyFilterHeight,this->messageText.c_str(),this->messageText.length());
            TextOut(aDC,1*this->gameboyFilterWidth,2*this->gameboyFilterHeight,this->messageText.c_str(),this->messageText.length());
            TextOut(aDC,2*this->gameboyFilterWidth,3*this->gameboyFilterHeight,this->messageText.c_str(),this->messageText.length());
            TextOut(aDC,2*this->gameboyFilterWidth,1*this->gameboyFilterHeight,this->messageText.c_str(),this->messageText.length());
            
            SetTextColor(aDC,RGB(255,255,255));
            TextOut(aDC,2*this->gameboyFilterWidth,2*this->gameboyFilterHeight,this->messageText.c_str(),this->messageText.length());
            this->bSurface->ReleaseDC(aDC);
        }
    }

}

// get the filter width/height for the selected filter type (currently always the same)
int DirectDraw::getFilterDimension(videofiltertype type)
{
    switch (type) {
        case VIDEO_FILTER_SOFTXX:
            return 8;
        case VIDEO_FILTER_SCALE2X:
        case VIDEO_FILTER_SOFT2X:
        case VIDEO_FILTER_BLUR:
            return 2;
        case VIDEO_FILTER_SCALE3X:
            return 3;
        case VIDEO_FILTER_NONE:
        default:
            return 1;
    }
}

void DirectDraw::setBorderFilter(videofiltertype type) 
{
    this->borderFilterWidth = this->borderFilterHeight = this->getFilterDimension(type);
    this->borderFilterType = type;
    this->changeFilters();
}

void DirectDraw::setGameboyFilter(videofiltertype type) 
{
    this->gameboyFilterWidth = this->gameboyFilterHeight = this->getFilterDimension(type);
    this->gameboyFilterType = type;
    this->changeFilters();
}

bool DirectDraw::changeFilters()
{
	HRESULT ddrval;
	DDSURFACEDESC2 ddsd;
	
	SafeRelease(this->bSurface);
	SafeRelease(this->borderSurface);
	
	ZeroMemory(&ddsd,sizeof(ddsd));
	ddsd.dwSize = sizeof(DDSURFACEDESC2);
	ddsd.dwFlags = DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN|DDSCAPS_VIDEOMEMORY;//DDSCAPS_SYSTEMMEMORY;
	ddsd.dwWidth = 160*this->gameboyFilterWidth;
	ddsd.dwHeight = 144*this->gameboyFilterHeight;

	ddrval = this->dd->CreateSurface(&ddsd,&(this->bSurface),NULL);
	if(ddrval != DD_OK) {
		debug_print("DirectDraw Createsurface failed!"); 
		return false;
	}

	ddsd.dwWidth = 256*this->borderFilterWidth;
	ddsd.dwHeight = 224*this->borderFilterHeight;
	ddrval = this->dd->CreateSurface(&ddsd,&(this->borderSurface),NULL);
	if(ddrval != DD_OK)  {
		debug_print("DirectDraw Createsurface failed!"); 
		return false;
	} 
	   
	ZeroMemory(&ddsd,sizeof(ddsd));
	ddsd.dwSize = sizeof(DDSURFACEDESC2);
	this->bSurface->Lock(NULL,&ddsd,DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR,NULL);
   
	this->lPitch = ddsd.lPitch;
	
	this->bSurface->Unlock(NULL);
	
	int effectiveBitCount = this->bitCount == 16 ? 16 : 32;
	
	switch(this->gameboyFilterType){
		case VIDEO_FILTER_SOFT2X:
		case VIDEO_FILTER_SOFTXX:
			this->gameboyFilter16 = softwarexx;      
			this->gameboyFilter32 = softwarexx;      
			break;
		case VIDEO_FILTER_SCALE2X:
			this->gameboyFilter16 = Scale2x;      
			this->gameboyFilter32 = Scale2x;   
			break;   
		case VIDEO_FILTER_SCALE3X:
			this->gameboyFilter16 = Scale3x;      
			this->gameboyFilter32 = Scale3x;    
			break;           
		case VIDEO_FILTER_NONE:
		default:
			this->gameboyFilter16 = filter_none;
			this->gameboyFilter32 = filter_none;
			break;
	}   
	
	switch(this->borderFilterType) { 
		case VIDEO_FILTER_SOFT2X:
		case VIDEO_FILTER_SOFTXX:
			this->borderFilter16 = softwarexx;    
            this->borderFilter32 = softwarexx;      
			break;
		case VIDEO_FILTER_SCALE2X:
			this->borderFilter16 = Scale2x;   
            this->borderFilter32 = Scale2x;       
			break;  
		case VIDEO_FILTER_SCALE3X:
			this->borderFilter16 = Scale3x; 
            this->borderFilter32 = Scale3x;      
			break;            
		case VIDEO_FILTER_NONE:
		default:
			this->borderFilter16 = filter_none;
			this->borderFilter32 = filter_none;
			break;
	}    
	
	this->lPitch >>= ( effectiveBitCount / 16 );
    
   if(GB1->romloaded && sgb_mode)
		(this->*DirectDraw::drawBorder)();  // totally not sure about this either 
	
	//afont = CreateFont(12*renderer.gameboyFilterHeight,6*renderer.gameboyFilterWidth,2,2,FW_BOLD,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,DEFAULT_PITCH|FF_SWISS,NULL);   
	this->afont = CreateFont(8*this->gameboyFilterHeight,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_DEFAULT_PRECIS,
                       CLIP_DEFAULT_PRECIS,NONANTIALIASED_QUALITY,DEFAULT_PITCH|FF_SWISS,L"PCPaint Bold Small");   

	return true;
}


int DirectDraw::getBitCount()
{
	return this->bitCount;
}

void DirectDraw::handleWindowResize()
{
    this->setRect(false);
    
    // where are we getting multiple_gb and sgb_mode from in this scope .. Also border_uploaded
    
    if(multiple_gb) {
        int width = this->targetBltRect.right - this->targetBltRect.left;
        this->targetBltRect.right = this->targetBltRect.left + width / 2;
    }
    
    if(sgb_mode || (options->GBC_SGB_border != OFF && border_uploaded)) {
        double width = ((double)(this->targetBltRect.right-this->targetBltRect.left)/256.0);
        double height = ((double)(this->targetBltRect.bottom-this->targetBltRect.top)/224.0);
        
        this->targetBltRect.left += (long)round(48.0*width); 
        this->targetBltRect.right = this->targetBltRect.left + (long)round(160.0*width); 
        this->targetBltRect.top += (long)round(40.0*height);
        this->targetBltRect.bottom = this->targetBltRect.top + (long)round(144.0*height);
        
        (this->*DirectDraw::drawBorder)();  
        if(sgb_mask == 1) (*this.*drawScreen)();
    }    
}

// Does something with a rectangle or idfk
// Also should we use something else instead of options->video_size
void DirectDraw::setRect(bool gb2open)
{
    POINT pt;
    GetClientRect(*(this->hwnd),&(this->targetBltRect));
    pt.x=pt.y=0;
    ClientToScreen(*(this->hwnd),&pt);
    OffsetRect(&(this->targetBltRect),pt.x,pt.y);       
    if (gb2open) {
        this->targetBltRect.right-=160*options->video_size;
    }
}

// draw the screen without mixing frames
void DirectDraw::drawScreen32() {
   this->drawScreenGeneric((DWORD*)GB->gfx_buffer);
}

// draw the screen mixing frames
void DirectDraw::drawScreenMix32()
{   
	DWORD* current = (DWORD*)GB->gfx_buffer;
	DWORD* old = (DWORD*)GB->gfx_buffer_old;
	DWORD* older = (DWORD*)GB->gfx_buffer_older;
	DWORD* oldest = (DWORD*)GB->gfx_buffer_oldest;
	
	DWORD* target = (DWORD*)dxBufferMix;
	
	DWORD mix_temp1 = 0;
	DWORD mix_temp2 = 0;
	
	if(options->video_mix_frames == MIX_FRAMES_MORE && !(GB->gbc_mode || sgb_mode)) { // Options and modes and stuff ugh
		for(int y = 0;y < 144*160;y++) {// mix it
			mix_temp1 = ((*current) + (*old)) >> 1;
			mix_temp2 = ((*older) + (*oldest)) >> 1;
			
			*target = ((mix_temp1*3 + mix_temp2) >> 2);
			
			++target;
			++current;
			++old;
			++older;
			++oldest;
		}
	
		void* temp1 = GB->gfx_buffer;
		void* temp2 = GB->gfx_buffer_older;
		GB->gfx_buffer = GB->gfx_buffer_oldest;
		GB->gfx_buffer_older = GB->gfx_buffer_old;
		GB->gfx_buffer_old = temp1;
		GB->gfx_buffer_oldest = temp2;
	} else {
		for(int y = 0;y < 144*160;y++) {// mix it
			*target++ = ((*current++) + (*old++)) >> 1;
		}
		
		void* temp = GB->gfx_buffer;
		GB->gfx_buffer = GB->gfx_buffer_old;
		GB->gfx_buffer_old = temp;
	}

    this->drawScreenGeneric((DWORD*)dxBufferMix);
}

void DirectDraw::drawScreen16()
{  
   this->drawScreenGeneric((WORD*)GB->gfx_buffer);
}

void DirectDraw::drawScreenMix16()
{
	WORD* current = (WORD*)GB->gfx_buffer;
	WORD* old = (WORD*)GB->gfx_buffer_old;
	WORD* older = (WORD*)GB->gfx_buffer_older;
	WORD* oldest = (WORD*)GB->gfx_buffer_oldest;
	
	WORD* target = (WORD*)dxBufferMix;
	
	WORD mix_temp1 = 0;
	WORD mix_temp2 = 0;
	
	WORD mask = ~RGB_BIT_MASK;
   
	if(options->video_mix_frames == MIX_FRAMES_MORE && !(GB->gbc_mode || sgb_mode)) {
		for(int y = 0;y < 144*160;y++) {// mix it
			mix_temp1 = ((*current&mask)>>1) + ((*old&mask)>>1);
			mix_temp2 = ((*older&mask)>>1) + ((*oldest&mask)>>1);
			
			*target++ = ((((mix_temp1&mask)>>1) + ((mix_temp1&mask)>>1)&mask)>>1) +
			((((mix_temp1&mask)>>1) + ((mix_temp2&mask)>>1)&mask)>>1);
			
			++current;
			++old;
			++older;
			++oldest;
		}
		
		void* temp1 = GB->gfx_buffer;
		void* temp2 = GB->gfx_buffer_older;
		GB->gfx_buffer = GB->gfx_buffer_oldest;
		GB->gfx_buffer_older = GB->gfx_buffer_old;
		GB->gfx_buffer_old = temp1;
		GB->gfx_buffer_oldest = temp2;
	} else {
		for(int y = 0;y < 144*160;y++){ // mix it
			*target++ = (((*current++)&mask)>>1)+(((*old++)&mask)>>1);
		}
		
		void* temp = GB->gfx_buffer;
		GB->gfx_buffer = GB->gfx_buffer_old;
		GB->gfx_buffer_old = temp;
	}
   
	this->drawScreenGeneric((WORD*)dxBufferMix);
}

void DirectDraw::gameboyFilter(WORD *target,WORD *src,int width,int height,int pitch)
{
    this->gameboyFilter16(target,src,width,height,pitch);
}

void DirectDraw::gameboyFilter(DWORD *target,DWORD *src,int width,int height,int pitch)
{
    this->gameboyFilter32(target,src,width,height,pitch);
}

template<typename TYPE>
void DirectDraw::drawScreenGeneric(TYPE* buffer)
{
	DDSURFACEDESC2 ddsd;
	
	ZeroMemory(&ddsd,sizeof(ddsd));
	ddsd.dwSize = sizeof(DDSURFACEDESC2);
	renderer.bSurface->Lock(NULL,&ddsd,DDLOCK_WRITEONLY|DDLOCK_SURFACEMEMORYPTR,NULL);
	
	this->gameboyFilter((TYPE*)ddsd.lpSurface,buffer,160,144,this->lPitch);
	
	this->bSurface->Unlock(NULL);   
	// Options accessed in here
	if(options->video_visual_rumble && GB->rumble_counter) {
		--GB->rumble_counter;
		if(!(GB->rumble_counter%2)) {
			this->targetBltRect.left-=VISUAL_RUMBLE_STRENGTH;
			this->targetBltRect.right-=VISUAL_RUMBLE_STRENGTH;
			this->changeRect = 1;
		} else this->changeRect = 0;
	} else this->changeRect = 0;
	this->gbTextOut();
	
	int screen_real_width = this->targetBltRect.right - this->targetBltRect.left;
	// multiple_gb accessed
	if(multiple_gb && GB == GB2) {
		this->targetBltRect.left += screen_real_width;
		this->targetBltRect.right += screen_real_width;
	}
	
	if(this->ddSurface->Blt(&(this->targetBltRect),this->bSurface,NULL,0,NULL) == DDERR_SURFACELOST)	{
		this->ddSurface->Restore();
		this->bSurface->Restore();
	}
	
	if(multiple_gb && GB == GB2) {
		this->targetBltRect.left -= screen_real_width;
		this->targetBltRect.right -= screen_real_width;
	} 
	
	if(this->changeRect){
		this->targetBltRect.left += VISUAL_RUMBLE_STRENGTH;
		this->targetBltRect.right += VISUAL_RUMBLE_STRENGTH;
    }
}


void DirectDraw::drawBorder32()
{
	unsigned short* source = sgb_border_buffer; // sgb_border_buffer == ?
	DWORD* target = (DWORD*)(this->dxBorderBufferRender);
	
	for(register int y=0;y<256*224;y+=8) { 
		*target++ = *(this->palette->gfxPal32+*source++);
		*target++ = *(this->palette->gfxPal32+*source++);
		*target++ = *(this->palette->gfxPal32+*source++);
		*target++ = *(this->palette->gfxPal32+*source++);
		*target++ = *(this->palette->gfxPal32+*source++);
		*target++ = *(this->palette->gfxPal32+*source++);
		*target++ = *(this->palette->gfxPal32+*source++);
		*target++ = *(this->palette->gfxPal32+*source++);                                                                                                                                                   
	} 
	
	DDSURFACEDESC2 ddsd;
	
	ZeroMemory(&ddsd,sizeof(ddsd));
	ddsd.dwSize = sizeof(DDSURFACEDESC2);
	this->borderSurface->Lock(NULL,&ddsd,DDLOCK_WRITEONLY|DDLOCK_SURFACEMEMORYPTR,NULL);
	this->borderLPitch = ddsd.lPitch>>2;
	
	int temp_w = gameboyFilterWidth;
	int temp_h = gameboyFilterHeight;   
	this->gameboyFilterWidth = this->borderFilterWidth;
	this->gameboyFilterHeight = this->borderFilterHeight;   
	this->borderFilter32((DWORD*)ddsd.lpSurface,(DWORD*)(this->dxBorderBufferRender),256,224,this->borderLPitch);
	this->gameboyFilterWidth = temp_w;
	this->gameboyFilterHeight = temp_h;
	
	this->borderSurface->Unlock(NULL);   
	
	POINT pt;
	RECT rect;
	
	GetClientRect(*(this->hwnd),&rect);
	pt.x=pt.y=0;
	ClientToScreen(*(this->hwnd),&pt);
	OffsetRect(&rect,pt.x,pt.y);
	
	if(this->ddSurface->Blt(&rect,this->borderSurface,NULL,0,NULL) == DDERR_SURFACELOST){
	    this->ddSurface->Restore();
		this->borderSurface->Restore();
	}
}

void DirectDraw::drawBorder16()
{
	WORD* target = (WORD*)(this->dxBorderBufferRender);
	unsigned short* source = sgb_border_buffer;
	
	for(register int y=0;y<256*224;y+=8) { 
		*target++ = *(this->palette->gfxPal16+*source++);
		*target++ = *(this->palette->gfxPal16+*source++);
		*target++ = *(this->palette->gfxPal16+*source++);
		*target++ = *(this->palette->gfxPal16+*source++);
		*target++ = *(this->palette->gfxPal16+*source++);
		*target++ = *(this->palette->gfxPal16+*source++);
		*target++ = *(this->palette->gfxPal16+*source++);
		*target++ = *(this->palette->gfxPal16+*source++);                                                                                                                                                   
	} 
	
	DDSURFACEDESC2 ddsd;
	
	ZeroMemory(&ddsd,sizeof(ddsd));
	ddsd.dwSize = sizeof(DDSURFACEDESC2);
	this->borderSurface->Lock(NULL,&ddsd,DDLOCK_WRITEONLY|DDLOCK_SURFACEMEMORYPTR,NULL);
	this->borderLPitch = ddsd.lPitch>>1;
	
	int temp_w = this->gameboyFilterWidth;
	int temp_h = this->gameboyFilterHeight;   
	this->gameboyFilterWidth = this->borderFilterWidth;
	this->gameboyFilterHeight = this->borderFilterHeight;   
	this->borderFilter16((WORD*)ddsd.lpSurface,(WORD*)(this->dxBorderBufferRender),256,224,this->borderLPitch);
	this->gameboyFilterWidth = temp_w;
	this->gameboyFilterHeight = temp_h;   
	
	this->borderSurface->Unlock(NULL);   
	
	POINT pt;
	RECT rect;
	
	GetClientRect(*(this->hwnd),&rect);
	pt.x=pt.y=0;
	ClientToScreen(*(this->hwnd),&pt);
	OffsetRect(&rect,pt.x,pt.y);
	
	if(this->ddSurface->Blt(&rect,this->borderSurface,NULL,0,NULL) == DDERR_SURFACELOST){
		this->ddSurface->Restore();
		this->borderSurface->Restore();
	}
}

#ifdef ALLOW_DEBUG
void DirectDraw::drawDebugScreen()
{
   DDBLTFX clrblt;
   ZeroMemory(&clrblt,sizeof(DDBLTFX));
   clrblt.dwSize=sizeof(DDBLTFX);
   clrblt.dwFillColor=RGB(0,0,0);
   this->BSurface->Blt(NULL,NULL,NULL,DDBLT_COLORFILL,&clrblt);

   char chregs[60];
   HDC aDC;

   if(this->BSurface->GetDC(&aDC)==DD_OK)
   {
      SetBkColor(aDC, RGB(0,0,0));//TRANSPARENT);
      SetTextColor(aDC,RGB(255,255,255));
      sprintf(chregs,"A:  %X | BC: %X", A,BC.W);
      TextOut(aDC,5,0,chregs,strlen(chregs));
      sprintf(chregs,"DE: %X | HL: %X", DE.W,HL.W);
      TextOut(aDC,5,20,chregs,strlen(chregs));
      sprintf(chregs,"PC: %X | F: %X | SP: %X", PC.W,F,SP.W);
      TextOut(aDC,5,40,chregs,strlen(chregs));
      sprintf(chregs,"opcode: %X", opcode);
      TextOut(aDC,5,60,chregs,strlen(chregs));
                     
      sprintf(chregs,"C: %X | H: %X | Z: %X | N: %X", CFLAG,HFLAG,ZFLAG,NFLAG);
      TextOut(aDC,5,80,chregs,strlen(chregs));
           
      sprintf(chregs,"IME: %X",IME);
      TextOut(aDC,5,100,chregs,strlen(chregs));

      this->BSurface->ReleaseDC(aDC);
   }
        
   if(this->DDSurface->Blt(&(this->targetBltRect),this->BSurface,NULL,0,NULL) == DDERR_SURFACELOST)
   {
      this->DDSurface->Restore();
      this->BSurface->Restore();
   }
}
#endif
