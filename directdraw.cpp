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

using namespace std;

#include "directdraw.h"
#include "GB.h"
#include "scale2x.h"
#include "scale3x.h"
#include "cpu.h"
#include "debug.h"
#include "SGB.h"
#include "strings.h"
#include "render.h"
#include "config.h"

#include "main.h"

int RGB_BIT_MASK = 0;

int change_rect = 0;

void* dx_buffer_mix = NULL;

void *dx_border_buffer_render;

RECT target_blt_rect;

void draw_screen16();
void draw_screen32();
void draw_border32();
void draw_border16();

HFONT afont;

int bs,gs,rs;

static int ffs(UINT mask)
{
  int m = 0;
  if(mask) 
  {
    while (!(mask & (1 << m)))
      m++;
    
    return m;
  }
  
  return 0;
}

DWORD* gfx_pal32 = NULL;
WORD* gfx_pal16 = NULL;

int lPitch = 160; // should be 160
int border_lPitch;

void mix_gbc_colors()
{
  if(GB1->gbc_mode && options->video_GBCBGA_real_colors)
  {
     if(GB1->system_type == SYS_GBA)
     {
        for(int i=0;i<0x10000;++i)
        {
           int red_init = (i & 0x1F);
           int green_init = ((i & 0x3E0) >> 5);
           int blue_init = ((i & 0x7C00) >> 10);
         
           if(red_init < 0x19) red_init -= 4; else red_init -= 3; 
           if(green_init < 0x19) green_init -= 4; else green_init -= 3;
           if(blue_init < 0x19) blue_init -= 4; else blue_init -= 3;
           if(red_init < 0) red_init = 0;       
           if(green_init < 0) green_init = 0;
           if(blue_init < 0) blue_init = 0;
        
           int red = ((red_init*12+green_init+blue_init)/14);
           int green = ((green_init*12+blue_init+red_init)/14);
           int blue = ((blue_init*12+red_init+green_init)/14);
           if(renderer.bitCount == 16)
              gfx_pal16[i] = (red<<rs) | (green<<gs) | (blue<<bs);              
           else
              gfx_pal32[i] = (red<<rs) | (green<<gs) | (blue<<bs);
        }
     }     
     else
     {
        for(int i=0;i<0x10000;++i)
        {
           int red_init = (i & 0x1F);
           int green_init = ((i & 0x3E0) >> 5);
           int blue_init = ((i & 0x7C00) >> 10);
         
           if(red_init && red_init < 0x10) red_init += 2; else if(red_init) red_init += 3; 
           if(green_init && green_init < 0x10) green_init += 2; else if(green_init) green_init += 3;
           if(blue_init && blue_init < 0x10) blue_init += 2; else if(blue_init) blue_init += 3;
           if(red_init >= 0x1F) red_init = 0x1E;       
           if(green_init >= 0x1F) green_init = 0x1E;
           if(blue_init >= 0x1F) blue_init = 0x1E;
        
           int red = ((red_init*10+green_init*3+blue_init)/14);
           int green = ((green_init*10+blue_init*2+red_init*2)/14);
           int blue = ((blue_init*10+red_init*2+green_init*2)/14);
           if(renderer.bitCount == 16)
              gfx_pal16[i] = (red<<rs) | (green<<gs) | (blue<<bs);
           else        
              gfx_pal32[i] = (red<<rs) | (green<<gs) | (blue<<bs);
        }
     }
  } else
  {
     if(renderer.bitCount == 16)
     {
        for(int i=0;i<0x10000;++i)
           gfx_pal16[i] = ((i & 0x1F) << rs) | (((i & 0x3E0) >> 5) << gs) | (((i & 0x7C00) >> 10) << bs);
     } else
     {
        for(int i=0;i<0x10000;++i)
           gfx_pal32[i] = ((i & 0x1F) << rs) | (((i & 0x3E0) >> 5) << gs) | (((i & 0x7C00) >> 10) << bs);
     }  
  }
}

void Kill_DD()
{
   if(gfx_pal32 != NULL) 
   { 
      delete [] gfx_pal32; 
      gfx_pal32 = NULL; 
   }
   if(gfx_pal16 != NULL) 
   { 
      delete [] gfx_pal16; 
      gfx_pal16 = NULL; 
   }
   if(dx_buffer_mix != NULL) 
   { 
      if(renderer.bitCount==16)
      {
         delete [] (WORD*)dx_buffer_mix;
      } else
      {
         delete [] (DWORD*)dx_buffer_mix;
      }
      dx_buffer_mix = NULL; 
   }         
   if(dx_border_buffer_render != NULL) 
   { 
      if(renderer.bitCount==16)
      {
         delete [] (WORD*)dx_border_buffer_render;
      } else
      {
         delete [] (DWORD*)dx_border_buffer_render;
      }
      dx_border_buffer_render = NULL; 
   }   
      
   SafeRelease(renderer.bSurface);
   SafeRelease(renderer.borderSurface);
   SafeRelease(renderer.ddSurface);
   SafeRelease(renderer.ddClip);
   SafeRelease(renderer.dd);
   
   DeleteObject(afont);
}

void (*filter_f_32)(DWORD *target,DWORD *src,int width,int height,int pitch) = NULL;
void (*filter_f_16)(WORD *target,WORD *src,int width,int height,int pitch) = NULL;
void (*border_filter_f_32)(DWORD *target,DWORD *src,int width,int height,int pitch) = NULL;
void (*border_filter_f_16)(WORD *target,WORD *src,int width,int height,int pitch) = NULL;


void filter_none_32(DWORD *pointer,DWORD *source,int width,int height,int pitch)
{
   copy_line32(pointer,source,width*height); 
}

void filter_none_16(WORD *pointer,WORD *source,int width,int height,int pitch)
{
   copy_line16(pointer,source,width*height);
}

void softwarexx_16(WORD *pointer,WORD *source,int width,int height,int pitch)
{
   register WORD *target;
   WORD* init = source;
   
	// renderer.gameboyFilterHeight indicates scale 
   for(register int y = 0;y < height*renderer.gameboyFilterHeight;y++)
   { 
      target = pointer + y*pitch;
      source = init + (y/renderer.gameboyFilterHeight)*width;
      for(int x = 0;x < width; x++)
      {
      	 for (int s = 0; s < renderer.gameboyFilterHeight - 1; s++) {
      	 	*target++ = *source;
      	 }
         *target++ = *source++;
      }
  }
}

void softwarexx_32(DWORD *pointer,DWORD *source,int width,int height,int pitch)
{
   register DWORD *target;
   DWORD* init = source;

	// renderer.gameboyFilterHeight indicates scale 
   for(register int y = 0;y < height*renderer.gameboyFilterHeight;y++)
   { 
      target = pointer + y*pitch;
      source = init + (y/renderer.gameboyFilterHeight)*width;
      for(int x = 0;x < width; x++)
      {
      	 for (int s = 0; s < renderer.gameboyFilterHeight - 1; s++) {
      	 	*target++ = *source;
      	 }
         *target++ = *source++;
      }
  }
}

/*
void blur_32(DWORD *pointer,DWORD *source,int width,int height,int pitch)
{
   register DWORD *target = pointer;
   DWORD* init = source;

   *target++ = (((*(source+width)) + ((*source)<<1) + *(source+1))>>2);
   *target++ = (((*(source+width)) + ((*source)<<1) + *(source+1))>>2);
   *source++;    
   for(int x=1;x<width-1;x++)
   {
      *target++ = (((*(source+width)) + ((*source)<<1) + *(source-1))>>2);
      *target++ = (((*(source+width)) + ((*source)<<1) + *(source+1))>>2);
      *source++;                                    
   } 
   *target++ = (((*(source+width)) + ((*source)<<1) + *(source-1))>>2);
   *target++ = (((*(source+width)) + ((*source)<<1) + *(source-1))>>2);
   *source++;     
      
   target = pointer+pitch;      
   source=init; 
   
   *target++ = (((*(source+width)) + ((*source)<<1) + *(source+1))>>2);
   *target++ = (((*(source+width)) + ((*source)<<1) + *(source+1))>>2);
   *source++;     
   for(int x=1;x<width-1;x++)
   {
      *target++ = (((*(source+width)) + ((*source)<<1) + *(source-1))>>2);
      *target++ = (((*(source+width)) + ((*source)<<1) + *(source+1))>>2);
      *source++;                                    
   } 
   *target++ = (((*(source+width)) + ((*source)<<1) + *(source-1))>>2);
   *target++ = (((*(source+width)) + ((*source)<<1) + *(source-1))>>2);
   *source++;     
      
   for(register int y=2;y<(height*renderer.gameboyFilterHeight)-2;y+=2) 
   { 
      target = pointer+y*pitch;
      source = init+(y>>1)*width;
      
      *target++ = (((*(source-width)) + ((*source)<<1) + *(source+1))>>2);
      *target++ = (((*(source-width)) + ((*source)<<1) + *(source+1))>>2);
      *source++;                  
          
      for(int x=1;x<width-1;x++)
      {
         *target++ = (((*(source-width)) + ((*source)<<1) + *(source-1))>>2);
         *target++ = (((*(source-width)) + ((*source)<<1) + *(source+1))>>2);
         *source++;                                    
      } 
      *target++ = (((*(source-width)) + ((*source)<<1) + *(source-1))>>2);
      *target++ = (((*(source-width)) + ((*source)<<1) + *(source-1))>>2);
      *source++;                     
      
      target = pointer+(y+1)*pitch;
      source = init+(y>>1)*width;
            
      *target++ = (((*(source+width)) + ((*source)<<1) + *(source+1))>>2);
      *target++ = (((*(source+width)) + ((*source)<<1) + *(source+1))>>2);
      *source++;    
          
      for(int x=1;x<width-1;x++)
      {
         *target++ = (((*(source+width)) + ((*source)<<1) + *(source-1))>>2);
         *target++ = (((*(source+width)) + ((*source)<<1) + *(source+1))>>2);
         *source++;                                    
      } 
      *target++ = (((*(source+width)) + ((*source)<<1) + *(source-1))>>2);
      *target++ = (((*(source+width)) + ((*source)<<1) + *(source-1))>>2);
      *source++;           
   } 

   target = pointer+(height*renderer.gameboyFilterHeight-2)*pitch;
   source = init+(height-1)*width; 

   *target++ = (((*(source-width)) + ((*source)<<1) + *(source+1))>>2);
   *target++ = (((*(source-width)) + ((*source)<<1) + *(source+1))>>2);
   *source++;                 
   for(int x=1;x<width-1;x++)
   {
      *target++ = (((*(source-width)) + ((*source)<<1) + *(source-1))>>2);
      *target++ = (((*(source-width)) + ((*source)<<1) + *(source+1))>>2);
      *source++;                                    
   } 
   *target++ = (((*(source-width)) + ((*source)<<1) + *(source-1))>>2);
   *target++ = (((*(source-width)) + ((*source)<<1) + *(source-1))>>2);
   *source++;       
      
   target = pointer+(height*renderer.gameboyFilterHeight-1)*pitch;
   source = init+(height-1)*width;   

   *target++ = (((*(source-width)) + ((*source)<<1) + *(source+1))>>2);
   *target++ = (((*(source-width)) + ((*source)<<1) + *(source+1))>>2);
   *source++;   
   for(int x=1;x<width-1;x++)
   {
      *target++ = (((*(source-width)) + ((*source)<<1) + *(source-1))>>2);
      *target++ = (((*(source-width)) + ((*source)<<1) + *(source+1))>>2);
      *source++;                                    
   } 
   *target++ = (((*(source-width)) + ((*source)<<1) + *(source-1))>>2);
   *target++ = (((*(source-width)) + ((*source)<<1) + *(source-1))>>2);
   //*source++;         
}

void blur_16(WORD *pointer,WORD *source,int width,int height,int pitch)
{
   register WORD *target=pointer;
   WORD* init = source;
   int mask = ~RGB_BIT_MASK;

   *target++ = (((((*(source+width))&mask)>>1)&mask)>>1) + ((((*source)&mask)&mask)>>1) + (((((*(source+1))&mask)>>1)&mask)>>1);
   *target++ = (((((*(source+width))&mask)>>1)&mask)>>1) + ((((*source)&mask)&mask)>>1) + (((((*(source+1))&mask)>>1)&mask)>>1);
   *source++; 
   for(int x=1;x<width-1;x++)
   {
      *target++ = (((((*(source+width))&mask)>>1)&mask)>>1) + ((((*source)&mask)&mask)>>1) + (((((*(source-1))&mask)>>1)&mask)>>1);
      *target++ = (((((*(source+width))&mask)>>1)&mask)>>1) + ((((*source)&mask)&mask)>>1) + (((((*(source+1))&mask)>>1)&mask)>>1);
      *source++;                                    
   } 
   *target++ = (((((*(source+width))&mask)>>1)&mask)>>1) + ((((*source)&mask)&mask)>>1) + (((((*(source-1))&mask)>>1)&mask)>>1);
   *target++ = (((((*(source+width))&mask)>>1)&mask)>>1) + ((((*source)&mask)&mask)>>1) + (((((*(source-1))&mask)>>1)&mask)>>1);
   *source++;  
      
   target = pointer+pitch;      
   source=init; 
   
   *target++ = (((((*(source+width))&mask)>>1)&mask)>>1) + ((((*source)&mask)&mask)>>1) + (((((*(source+1))&mask)>>1)&mask)>>1);
   *target++ = (((((*(source+width))&mask)>>1)&mask)>>1) + ((((*source)&mask)&mask)>>1) + (((((*(source+1))&mask)>>1)&mask)>>1);
   *source++; 
   for(int x=1;x<width-1;x++)
   {
      *target++ = (((((*(source+width))&mask)>>1)&mask)>>1) + ((((*source)&mask)&mask)>>1) + (((((*(source-1))&mask)>>1)&mask)>>1);
      *target++ = (((((*(source+width))&mask)>>1)&mask)>>1) + ((((*source)&mask)&mask)>>1) + (((((*(source+1))&mask)>>1)&mask)>>1);      
      *source++;                                    
   } 
   *target++ = (((((*(source+width))&mask)>>1)&mask)>>1) + ((((*source)&mask)&mask)>>1) + (((((*(source-1))&mask)>>1)&mask)>>1);
   *target++ = (((((*(source+width))&mask)>>1)&mask)>>1) + ((((*source)&mask)&mask)>>1) + (((((*(source-1))&mask)>>1)&mask)>>1);
   *source++;     
      
   for(register int y=2;y<(height*renderer.gameboyFilterHeight)-2;y+=2) 
   { 
      target = pointer+y*pitch;
      source = init+(y>>1)*width;
      
      *target++ = (((((*(source-width))&mask)>>1)&mask)>>1) + ((((*source)&mask)&mask)>>1) + (((((*(source+1))&mask)>>1)&mask)>>1);
      *target++ = (((((*(source-width))&mask)>>1)&mask)>>1) + ((((*source)&mask)&mask)>>1) + (((((*(source+1))&mask)>>1)&mask)>>1);      
      *source++;                  
      for(int x=1;x<width-1;x++)
      {
         *target++ = (((((*(source-width))&mask)>>1)&mask)>>1) + ((((*source)&mask)&mask)>>1) + (((((*(source-1))&mask)>>1)&mask)>>1);
         *target++ = (((((*(source-width))&mask)>>1)&mask)>>1) + ((((*source)&mask)&mask)>>1) + (((((*(source+1))&mask)>>1)&mask)>>1);
         *source++;                                    
      } 
      *target++ = (((((*(source-width))&mask)>>1)&mask)>>1) + ((((*source)&mask)&mask)>>1) + (((((*(source-1))&mask)>>1)&mask)>>1);
      *target++ = (((((*(source-width))&mask)>>1)&mask)>>1) + ((((*source)&mask)&mask)>>1) + (((((*(source-1))&mask)>>1)&mask)>>1);      
      *source++;                     
      
      target = pointer+(y+1)*pitch;
      source = init+(y>>1)*width;
            
      *target++ = (((((*(source+width))&mask)>>1)&mask)>>1) + ((((*source)&mask)&mask)>>1) + (((((*(source+1))&mask)>>1)&mask)>>1);
      *target++ = (((((*(source+width))&mask)>>1)&mask)>>1) + ((((*source)&mask)&mask)>>1) + (((((*(source+1))&mask)>>1)&mask)>>1);      
      *source++;     
      for(int x=1;x<width-1;x++)
      {
         *target++ = (((((*(source+width))&mask)>>1)&mask)>>1) + ((((*source)&mask)&mask)>>1) + (((((*(source-1))&mask)>>1)&mask)>>1);
         *target++ = (((((*(source+width))&mask)>>1)&mask)>>1) + ((((*source)&mask)&mask)>>1) + (((((*(source+1))&mask)>>1)&mask)>>1);
         *source++;                                    
      } 
      *target++ = (((((*(source+width))&mask)>>1)&mask)>>1) + ((((*source)&mask)&mask)>>1) + (((((*(source-1))&mask)>>1)&mask)>>1);
      *target++ = (((((*(source+width))&mask)>>1)&mask)>>1) + ((((*source)&mask)&mask)>>1) + (((((*(source-1))&mask)>>1)&mask)>>1);      
      *source++;           
   } 

   target = pointer+(height*renderer.gameboyFilterHeight-2)*pitch;
   source = init+(height-1)*width; 

   *target++ = (((((*(source-width))&mask)>>1)&mask)>>1) + ((((*source)&mask)&mask)>>1) + (((((*(source+1))&mask)>>1)&mask)>>1);
   *target++ = (((((*(source-width))&mask)>>1)&mask)>>1) + ((((*source)&mask)&mask)>>1) + (((((*(source+1))&mask)>>1)&mask)>>1);
   *source++;               
   for(int x=1;x<width-1;x++)
   {
      *target++ = (((((*(source-width))&mask)>>1)&mask)>>1) + ((((*source)&mask)&mask)>>1) + (((((*(source-1))&mask)>>1)&mask)>>1);
      *target++ = (((((*(source-width))&mask)>>1)&mask)>>1) + ((((*source)&mask)&mask)>>1) + (((((*(source+1))&mask)>>1)&mask)>>1);
      *source++;                                    
   } 
   *target++ = (((((*(source-width))&mask)>>1)&mask)>>1) + ((((*source)&mask)&mask)>>1) + (((((*(source-1))&mask)>>1)&mask)>>1);
   *target++ = (((((*(source-width))&mask)>>1)&mask)>>1) + ((((*source)&mask)&mask)>>1) + (((((*(source-1))&mask)>>1)&mask)>>1);
   *source++;               
    
      
   target = pointer+(height*renderer.gameboyFilterHeight-1)*pitch;
   source = init+(height-1)*width;   

   *target++ = (((((*(source-width))&mask)>>1)&mask)>>1) + ((((*source)&mask)&mask)>>1) + (((((*(source+1))&mask)>>1)&mask)>>1);
   *target++ = (((((*(source-width))&mask)>>1)&mask)>>1) + ((((*source)&mask)&mask)>>1) + (((((*(source+1))&mask)>>1)&mask)>>1);
   *source++;               
   for(int x=1;x<width-1;x++)
   {
      *target++ = (((((*(source-width))&mask)>>1)&mask)>>1) + ((((*source)&mask)&mask)>>1) + (((((*(source-1))&mask)>>1)&mask)>>1);
      *target++ = (((((*(source-width))&mask)>>1)&mask)>>1) + ((((*source)&mask)&mask)>>1) + (((((*(source+1))&mask)>>1)&mask)>>1);
      *source++;                                    
   } 
   *target++ = (((((*(source-width))&mask)>>1)&mask)>>1) + ((((*source)&mask)&mask)>>1) + (((((*(source-1))&mask)>>1)&mask)>>1);
   *target++ = (((((*(source-width))&mask)>>1)&mask)>>1) + ((((*source)&mask)&mask)>>1) + (((((*(source-1))&mask)>>1)&mask)>>1);
   //*source++;                    
}*/

bool change_filter()
{
   HRESULT ddrval;
   DDSURFACEDESC2 ddsd;
   
   SafeRelease(renderer.bSurface);
   SafeRelease(renderer.borderSurface);
   
   ZeroMemory(&ddsd,sizeof(ddsd));
   ddsd.dwSize = sizeof(DDSURFACEDESC2);
   ddsd.dwFlags = DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT;
   ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN|DDSCAPS_VIDEOMEMORY;//DDSCAPS_SYSTEMMEMORY;
   ddsd.dwWidth = 160*renderer.gameboyFilterWidth;
   ddsd.dwHeight = 144*renderer.gameboyFilterHeight;

   ddrval = renderer.dd->CreateSurface(&ddsd,&renderer.bSurface,NULL);
   if(ddrval != DD_OK) 
   {
      debug_print("DirectDraw Createsurface failed!"); 
   
      Kill_DD();
      return false;
   }

   ddsd.dwWidth = 256*renderer.borderFilterWidth;
   ddsd.dwHeight = 224*renderer.borderFilterHeight;
   ddrval = renderer.dd->CreateSurface(&ddsd,&renderer.borderSurface,NULL);
   if(ddrval != DD_OK) 
   {
      debug_print("DirectDraw Createsurface failed!"); 
   
      Kill_DD();
      return false;
   } 
   
   ZeroMemory(&ddsd,sizeof(ddsd));
   ddsd.dwSize = sizeof(DDSURFACEDESC2);
   renderer.bSurface->Lock(NULL,&ddsd,DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR,NULL);
   
   lPitch = ddsd.lPitch;

   renderer.bSurface->Unlock(NULL);
   
   if(renderer.bitCount==16)
   {
      lPitch >>= 1;  
       
      switch(renderer.gameboyFilterType)
      {
      case VIDEO_FILTER_SOFT2X:
      case VIDEO_FILTER_SOFTXX:
         filter_f_16 = softwarexx_16;      
      break;
      case VIDEO_FILTER_SCALE2X:
         filter_f_16 = Scale2x16;      
      break;   
      case VIDEO_FILTER_SCALE3X:
         filter_f_16 = Scale3x16;      
      break;           
  /*    case VIDEO_FILTER_BLUR:
         filter_f_16 = blur_16;    
      break;     */
      case VIDEO_FILTER_NONE:
      default:
         filter_f_16 = filter_none_16;
      break;
      }   
      switch(options->video_SGBborder_filter)
      {
      case VIDEO_FILTER_SOFT2X:
      case VIDEO_FILTER_SOFTXX:
         border_filter_f_16 = softwarexx_16;      
      break;
      case VIDEO_FILTER_SCALE2X:
         border_filter_f_16 = Scale2x16;      
      break;  
      case VIDEO_FILTER_SCALE3X:
         border_filter_f_16 = Scale3x16;      
      break;            
   /*   case VIDEO_FILTER_BLUR:
         border_filter_f_16 = blur_16;    
      break;       */
      case VIDEO_FILTER_NONE:
      default:
         border_filter_f_16 = filter_none_16;
      break;
      }         
   }
   else
   {
      lPitch >>= 2;

      switch(renderer.gameboyFilterType)
      {
      case VIDEO_FILTER_SOFT2X:
      case VIDEO_FILTER_SOFTXX:
         filter_f_32 = softwarexx_32;      
      break;
      case VIDEO_FILTER_SCALE2X:
         filter_f_32 = Scale2x32;      
      break;      
      case VIDEO_FILTER_SCALE3X:
         filter_f_32 = Scale3x32;      
      break;          
  /*    case VIDEO_FILTER_BLUR:
         filter_f_32 = blur_32;    
      break;  */
      case VIDEO_FILTER_NONE:
      default:
         filter_f_32 = filter_none_32;
      break;
      }
      switch(renderer.borderFilterType)
      {
      case VIDEO_FILTER_SOFT2X:
      case VIDEO_FILTER_SOFTXX:
         border_filter_f_32 = softwarexx_32;      
      break;
      case VIDEO_FILTER_SCALE2X:
         border_filter_f_32 = Scale2x32;      
      break;     
      case VIDEO_FILTER_SCALE3X:
         border_filter_f_32 = Scale3x32;      
      break;         
   /*   case VIDEO_FILTER_BLUR:
         border_filter_f_32 = blur_32;    
      break;    */
      case VIDEO_FILTER_NONE:
      default:
         border_filter_f_32 = filter_none_32;
      break;
      }       
   }    
   if(GB1->romloaded && sgb_mode)
      draw_border();  
      
    //afont = CreateFont(12*renderer.gameboyFilterHeight,6*renderer.gameboyFilterWidth,2,2,FW_BOLD,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,DEFAULT_PITCH|FF_SWISS,NULL);   
    afont = CreateFont(8*renderer.gameboyFilterHeight,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,NONANTIALIASED_QUALITY,DEFAULT_PITCH|FF_SWISS,L"PCPaint Bold Small");   
      
   return true;
}

bool Init_DD()
{
   HRESULT ddrval;
   DDSURFACEDESC2 ddsd;
   //DDSCAPS2 ddscaps;
      
   ddrval = DirectDrawCreateEx(NULL, (void**)&renderer.dd, IID_IDirectDraw7, NULL); 
   if(ddrval!=DD_OK)
   {
      debug_print("DirectDraw Create failed!"); 
   
      return false;
   }
   ddrval = renderer.dd->SetCooperativeLevel(hwnd, DDSCL_NORMAL);
   if(ddrval!=DD_OK)
   {
      debug_print("DirectDraw: SetCooperativelevel failed!"); 

      Kill_DD();
      return false;
   }
   
   ddrval = renderer.dd->CreateClipper(0,&renderer.ddClip,NULL);
   if(ddrval!=DD_OK)
   {
      debug_print("DirectDraw: CreateClipper failed!"); 
   
      Kill_DD();
      return false;
   }
   renderer.ddClip->SetHWnd(0,hwnd);
   
   ZeroMemory(&ddsd,sizeof(ddsd));
   ddsd.dwSize = sizeof(DDSURFACEDESC2);
   ddsd.dwFlags = DDSD_CAPS;
   ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
   ddrval = renderer.dd->CreateSurface(&ddsd,&renderer.ddSurface,NULL);
   if(ddrval != DD_OK) 
   {
      debug_print("DirectDraw: Create main surface failed!"); 

      Kill_DD();
      return false;
   }
   
   renderer.ddSurface->SetClipper(renderer.ddClip);
   
   ZeroMemory(&ddsd,sizeof(ddsd));
   ddsd.dwSize = sizeof(DDSURFACEDESC2);
   ddsd.dwFlags = DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT;
   ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN|DDSCAPS_VIDEOMEMORY;//DDSCAPS_SYSTEMMEMORY;
   ddsd.dwWidth = 160;
   ddsd.dwHeight = 144;
   
   ddrval = renderer.dd->CreateSurface(&ddsd,&renderer.bSurface,NULL);
   if(ddrval != DD_OK) 
   {
      debug_print("DirectDraw: Create gb surface failed!"); 
   
      Kill_DD();
      return false;
   }
   ddsd.dwWidth = 256;
   ddsd.dwHeight = 224;   
   ddrval = renderer.dd->CreateSurface(&ddsd,&renderer.borderSurface,NULL);
   if(ddrval != DD_OK) 
   {
      debug_print("DirectDraw: Create border surface failed!"); 

      Kill_DD();
      return false;
   }   
   
   // empty the new surface
   DDBLTFX clrblt;
   ZeroMemory(&clrblt,sizeof(DDBLTFX));
   clrblt.dwSize=sizeof(DDBLTFX);
   clrblt.dwFillColor = RGB(0,0,0);
   renderer.bSurface->Blt(NULL,NULL,NULL,DDBLT_COLORFILL,&clrblt);
   renderer.borderSurface->Blt(NULL,NULL,NULL,DDBLT_COLORFILL,&clrblt);

   ZeroMemory(&ddsd,sizeof(ddsd));
   ddsd.dwSize = sizeof(DDSURFACEDESC2);
   ddsd.dwFlags = DDSD_PIXELFORMAT;
   renderer.bSurface->Lock(NULL,&ddsd,DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR,NULL);
   
   renderer.bitCount = ddsd.ddpfPixelFormat.dwRGBBitCount; 
   lPitch = ddsd.lPitch;
   
   renderer.bSurface->Unlock(NULL);
   
   DDPIXELFORMAT px;

   px.dwSize = sizeof(px);

   renderer.bSurface->GetPixelFormat(&px);
  
   rs = ffs(px.dwRBitMask);
   gs = ffs(px.dwGBitMask);
   bs = ffs(px.dwBBitMask);

  RGB_BIT_MASK = 0x421;
  
  if((px.dwFlags&DDPF_RGB) != 0 && px.dwRBitMask == 0xF800 && px.dwGBitMask == 0x07E0 && px.dwBBitMask == 0x001F) 
  {
     gs++;
     RGB_BIT_MASK = 0x821;
  } else if((px.dwFlags&DDPF_RGB) != 0 && px.dwRBitMask == 0x001F && px.dwGBitMask == 0x07E0 && px.dwBBitMask == 0xF800) 
  {
     gs++;
     RGB_BIT_MASK = 0x821;
  } else 
  {
    // 32-bit or 24-bit
    if(renderer.bitCount == 32 || renderer.bitCount == 24) 
    {
      rs += 3;
      gs += 3;
      bs += 3;
    }
  }
  
  if(renderer.bitCount  == 16)
  {
     gfx_pal16 = new WORD[0x10000];
     for(int i=0;i<0x10000;++i)
        gfx_pal16[i] = ((i & 0x1F) << rs) | (((i & 0x3E0) >> 5) << gs) | (((i & 0x7C00) >> 10) << bs);
        
     dx_buffer_mix = new WORD[140*166];     
     dx_border_buffer_render = new WORD[256*224];

     gb_system::gfx_bit_count = 16;     
     if(!GB->init_gfx() || !gfx_pal16 || !dx_buffer_mix || !dx_border_buffer_render)
     {
        debug_print(str_table[ERROR_MEMORY]); 
        return false;
     }
     fill_gfx_buffers(0UL); 
          
     draw_screen = draw_screen16;
     draw_border = draw_border16;
     filter_f_16 = filter_none_16;
     
     lPitch >>= 1;
  } else
  {
     gfx_pal32 = new DWORD[0x10000];

     for(int i=0;i<0x10000;++i)
        gfx_pal32[i] = ((i & 0x1F) << rs) | (((i & 0x3E0) >> 5) << gs) | (((i & 0x7C00) >> 10) << bs);
     
     dx_buffer_mix = new DWORD[140*166];  
     dx_border_buffer_render = new DWORD[256*224];

     gb_system::gfx_bit_count = 32;
     if(!GB->init_gfx() || !gfx_pal32 || !dx_buffer_mix || !dx_border_buffer_render)
     {
        debug_print(str_table[ERROR_MEMORY]); 
        return false;
     }
     fill_gfx_buffers(0UL);

     draw_screen = draw_screen32; 
     draw_border = draw_border32;
     filter_f_32 = filter_none_32;     
     
     lPitch >>= 2;
  }
  
  SetCurrentDirectory(options->program_directory.c_str());
  AddFontResource(L"PCPaintBoldSmall.ttf");

  return true;
}


void gbTextOut() {
    if(renderer.messageDuration && GB == renderer.messageGb)
   {
      --renderer.messageDuration;
      HDC aDC;
      if(renderer.bSurface->GetDC(&aDC)==DD_OK)
      {
         SelectObject(aDC,afont);
         SetBkMode(aDC, TRANSPARENT);
         SetTextColor(aDC,RGB(255,0,128));
        
         
         TextOut(aDC,3*renderer.gameboyFilterWidth,3*renderer.gameboyFilterHeight,renderer.messageText.c_str(),renderer.messageText.length());
         TextOut(aDC,1*renderer.gameboyFilterWidth,1*renderer.gameboyFilterWidth,renderer.messageText.c_str(),renderer.messageText.length());
         TextOut(aDC,1*renderer.gameboyFilterWidth,3*renderer.gameboyFilterWidth,renderer.messageText.c_str(),renderer.messageText.length());
         TextOut(aDC,3*renderer.gameboyFilterWidth,1*renderer.gameboyFilterWidth,renderer.messageText.c_str(),renderer.messageText.length());
         
         TextOut(aDC,3*renderer.gameboyFilterWidth,2*renderer.gameboyFilterHeight,renderer.messageText.c_str(),renderer.messageText.length());
         TextOut(aDC,1*renderer.gameboyFilterWidth,2*renderer.gameboyFilterWidth,renderer.messageText.c_str(),renderer.messageText.length());
         TextOut(aDC,2*renderer.gameboyFilterWidth,3*renderer.gameboyFilterWidth,renderer.messageText.c_str(),renderer.messageText.length());
         TextOut(aDC,2*renderer.gameboyFilterWidth,1*renderer.gameboyFilterWidth,renderer.messageText.c_str(),renderer.messageText.length());
         
          SetTextColor(aDC,RGB(255,255,255));
         TextOut(aDC,2*renderer.gameboyFilterWidth,2*renderer.gameboyFilterWidth,renderer.messageText.c_str(),renderer.messageText.length());
         renderer.bSurface->ReleaseDC(aDC);
      }
   }
}

void (*draw_screen)();

void (*draw_border)();

void draw_border32()
{
   unsigned short* source = sgb_border_buffer;
   DWORD* target = (DWORD*)dx_border_buffer_render;
   
   for(register int y=0;y<256*224;y+=8) 
   { 
      *target++ = *(gfx_pal32+*source++);
      *target++ = *(gfx_pal32+*source++);
      *target++ = *(gfx_pal32+*source++);
      *target++ = *(gfx_pal32+*source++);
      *target++ = *(gfx_pal32+*source++);
      *target++ = *(gfx_pal32+*source++);
      *target++ = *(gfx_pal32+*source++);
      *target++ = *(gfx_pal32+*source++);                                                                                                                                                   
   } 
   
   DDSURFACEDESC2 ddsd;

   ZeroMemory(&ddsd,sizeof(ddsd));
   ddsd.dwSize = sizeof(DDSURFACEDESC2);
   renderer.borderSurface->Lock(NULL,&ddsd,DDLOCK_WRITEONLY|DDLOCK_SURFACEMEMORYPTR,NULL);
   border_lPitch = ddsd.lPitch>>2;

   int temp_w = renderer.gameboyFilterWidth;
   int temp_h = renderer.gameboyFilterHeight;   
   renderer.gameboyFilterWidth = renderer.borderFilterWidth;
   renderer.gameboyFilterHeight = renderer.borderFilterHeight;   
   border_filter_f_32((DWORD*)ddsd.lpSurface,(DWORD*)dx_border_buffer_render,256,224,border_lPitch);
   renderer.gameboyFilterWidth = temp_w;
   renderer.gameboyFilterHeight = temp_h;
      
   renderer.borderSurface->Unlock(NULL);   
   
   POINT pt;
   RECT rect;

   GetClientRect(hwnd,&rect);
   pt.x=pt.y=0;
   ClientToScreen(hwnd,&pt);
   OffsetRect(&rect,pt.x,pt.y);

   if(renderer.ddSurface->Blt(&rect,renderer.borderSurface,NULL,0,NULL) == DDERR_SURFACELOST)
   {
      renderer.ddSurface->Restore();
      renderer.borderSurface->Restore();
   }
}

void draw_border16()
{
   WORD* target = (WORD*)dx_border_buffer_render;
   unsigned short* source = sgb_border_buffer;
      
   for(register int y=0;y<256*224;y+=8) 
   { 
      *target++ = *(gfx_pal16+*source++);
      *target++ = *(gfx_pal16+*source++);
      *target++ = *(gfx_pal16+*source++);
      *target++ = *(gfx_pal16+*source++);
      *target++ = *(gfx_pal16+*source++);
      *target++ = *(gfx_pal16+*source++);
      *target++ = *(gfx_pal16+*source++);
      *target++ = *(gfx_pal16+*source++);                                                                                                                                                   
   } 
   
   DDSURFACEDESC2 ddsd;

   ZeroMemory(&ddsd,sizeof(ddsd));
   ddsd.dwSize = sizeof(DDSURFACEDESC2);
   renderer.borderSurface->Lock(NULL,&ddsd,DDLOCK_WRITEONLY|DDLOCK_SURFACEMEMORYPTR,NULL);
   border_lPitch = ddsd.lPitch>>1;

   int temp_w = renderer.gameboyFilterWidth;
   int temp_h = renderer.gameboyFilterHeight;   
   renderer.gameboyFilterWidth = renderer.borderFilterWidth;
   renderer.gameboyFilterHeight = renderer.borderFilterHeight;   
   border_filter_f_16((WORD*)ddsd.lpSurface,(WORD*)dx_border_buffer_render,256,224,border_lPitch);
   renderer.gameboyFilterWidth = temp_w;
   renderer.gameboyFilterHeight = temp_h;   
   
   renderer.borderSurface->Unlock(NULL);   
   
   POINT pt;
   RECT rect;
   
   GetClientRect(hwnd,&rect);
   pt.x=pt.y=0;
   ClientToScreen(hwnd,&pt);
   OffsetRect(&rect,pt.x,pt.y);
   
   if(renderer.ddSurface->Blt(&rect,renderer.borderSurface,NULL,0,NULL) == DDERR_SURFACELOST)
   {
      renderer.ddSurface->Restore();
      renderer.borderSurface->Restore();
   }
}

// draw the screen without mixing frames
void draw_screen32()
{  
   draw_screen_generic32((DWORD*)GB->gfx_buffer);
}

// draw the screen mixing frames
void draw_screen_mix32()
{   
   DWORD* current = (DWORD*)GB->gfx_buffer;
   DWORD* old = (DWORD*)GB->gfx_buffer_old;
   DWORD* older = (DWORD*)GB->gfx_buffer_older;
   DWORD* oldest = (DWORD*)GB->gfx_buffer_oldest;

   DWORD* target = (DWORD*)dx_buffer_mix;

   DWORD mix_temp1 = 0;
   DWORD mix_temp2 = 0;

   if(options->video_mix_frames == MIX_FRAMES_MORE && !(GB->gbc_mode || sgb_mode))
   {
      for(int y = 0;y < 144*160;y++) // mix it
      {
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
   } else
   {
      for(int y = 0;y < 144*160;y++) // mix it
      {
        *target++ = ((*current++) + (*old++)) >> 1;
      }

      void* temp = GB->gfx_buffer;
      GB->gfx_buffer = GB->gfx_buffer_old;
      GB->gfx_buffer_old = temp;
   }

    draw_screen_generic32((DWORD*)dx_buffer_mix);
}

void draw_screen_generic32(DWORD* buffer)
{
       DDSURFACEDESC2 ddsd;

   ZeroMemory(&ddsd,sizeof(ddsd));
   ddsd.dwSize = sizeof(DDSURFACEDESC2);
   renderer.bSurface->Lock(NULL,&ddsd,DDLOCK_WRITEONLY|DDLOCK_SURFACEMEMORYPTR,NULL);

   filter_f_32((DWORD*)ddsd.lpSurface,buffer,160,144,lPitch);

   renderer.bSurface->Unlock(NULL);   

   if(options->video_visual_rumble && GB->rumble_counter)
   {
      --GB->rumble_counter;
      if(!(GB->rumble_counter%2))
      {
         target_blt_rect.left-=VISUAL_RUMBLE_STRENGTH;
         target_blt_rect.right-=VISUAL_RUMBLE_STRENGTH;
         change_rect = 1;
      } else change_rect = 0;
   } else change_rect = 0;
   
    gbTextOut();
   
   int screen_real_width = target_blt_rect.right - target_blt_rect.left;

   if(multiple_gb && GB == GB2)
   {
       target_blt_rect.left += screen_real_width;
       target_blt_rect.right += screen_real_width;
   }
   
   if(renderer.ddSurface->Blt(&target_blt_rect,renderer.bSurface,NULL,0,NULL) == DDERR_SURFACELOST)
   {
      renderer.ddSurface->Restore();
      renderer.bSurface->Restore();
   }
   
   if(multiple_gb && GB == GB2)
   {
      target_blt_rect.left -= screen_real_width;
      target_blt_rect.right -= screen_real_width;
   } 

   if(change_rect)
   {
     target_blt_rect.left += VISUAL_RUMBLE_STRENGTH;
     target_blt_rect.right += VISUAL_RUMBLE_STRENGTH;
   }
}

void draw_screen16()
{  
   draw_screen_generic16((WORD*)GB->gfx_buffer);
}

void draw_screen_mix16()
{
   WORD* current = (WORD*)GB->gfx_buffer;
   WORD* old = (WORD*)GB->gfx_buffer_old;
   WORD* older = (WORD*)GB->gfx_buffer_older;
   WORD* oldest = (WORD*)GB->gfx_buffer_oldest;

   WORD* target = (WORD*)dx_buffer_mix;

   WORD mix_temp1 = 0;
   WORD mix_temp2 = 0;
   
   WORD mask = ~RGB_BIT_MASK;
   
 /*  for(register int y=0;y<144*160;y+=10) // mix it
   { 
      *target++ = (((*mix_source++)&mask)>>1)+(((*old++)&mask)>>1);
      *target++ = (((*mix_source++)&mask)>>1)+(((*old++)&mask)>>1);
      *target++ = (((*mix_source++)&mask)>>1)+(((*old++)&mask)>>1);
      *target++ = (((*mix_source++)&mask)>>1)+(((*old++)&mask)>>1);
      *target++ = (((*mix_source++)&mask)>>1)+(((*old++)&mask)>>1);
      *target++ = (((*mix_source++)&mask)>>1)+(((*old++)&mask)>>1);
      *target++ = (((*mix_source++)&mask)>>1)+(((*old++)&mask)>>1);
      *target++ = (((*mix_source++)&mask)>>1)+(((*old++)&mask)>>1);
      *target++ = (((*mix_source++)&mask)>>1)+(((*old++)&mask)>>1);
      *target++ = (((*mix_source++)&mask)>>1)+(((*old++)&mask)>>1);
   }*/

   if(options->video_mix_frames == MIX_FRAMES_MORE && !(GB->gbc_mode || sgb_mode))
   {
      for(int y = 0;y < 144*160;y++) // mix it
      {
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
   } else
   {
      for(int y = 0;y < 144*160;y++) // mix it
      {
        *target++ = (((*current++)&mask)>>1)+(((*old++)&mask)>>1);
      }

      void* temp = GB->gfx_buffer;
      GB->gfx_buffer = GB->gfx_buffer_old;
      GB->gfx_buffer_old = temp;
   }
   
    draw_screen_generic16((WORD*)dx_buffer_mix);
}

void draw_screen_generic16(WORD* buffer)
{
   DDSURFACEDESC2 ddsd;

   ZeroMemory(&ddsd,sizeof(ddsd));
   ddsd.dwSize = sizeof(DDSURFACEDESC2);
   renderer.bSurface->Lock(NULL,&ddsd,DDLOCK_WRITEONLY|DDLOCK_SURFACEMEMORYPTR,NULL);
          
   filter_f_16((WORD*)ddsd.lpSurface,buffer,160,144,lPitch);
   
   renderer.bSurface->Unlock(NULL);   
              
   if(options->video_visual_rumble && GB->rumble_counter)
   {
      --GB->rumble_counter;
      if(!(GB->rumble_counter%2))
      {
         target_blt_rect.left-=VISUAL_RUMBLE_STRENGTH;
         target_blt_rect.right-=VISUAL_RUMBLE_STRENGTH;
         change_rect = 1;
      } else change_rect = 0;
   } else change_rect = 0;
   
    gbTextOut();
   
   int screen_real_width = target_blt_rect.right - target_blt_rect.left;

   if(multiple_gb && GB == GB2)
   {
       target_blt_rect.left += screen_real_width;
       target_blt_rect.right += screen_real_width;
   }

   if(renderer.ddSurface->Blt(&target_blt_rect,renderer.bSurface,NULL,0,NULL) == DDERR_SURFACELOST)
   {
      renderer.ddSurface->Restore();
      renderer.bSurface->Restore();
   }

   if(multiple_gb && GB == GB2)
   {
      target_blt_rect.left -= screen_real_width;
      target_blt_rect.right -= screen_real_width;
   } 

   if(change_rect)
   {
     target_blt_rect.left+=VISUAL_RUMBLE_STRENGTH;
     target_blt_rect.right+=VISUAL_RUMBLE_STRENGTH;
   }   
}

#ifdef ALLOW_DEBUG
void draw_debug_screen()
{
   DDBLTFX clrblt;
   ZeroMemory(&clrblt,sizeof(DDBLTFX));
   clrblt.dwSize=sizeof(DDBLTFX);
   clrblt.dwFillColor=RGB(0,0,0);
   BSurface->Blt(NULL,NULL,NULL,DDBLT_COLORFILL,&clrblt);

   char chregs[60];
   HDC aDC;

   if(BSurface->GetDC(&aDC)==DD_OK)
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

      BSurface->ReleaseDC(aDC);
   }
        
   if(DDSurface->Blt(&target_blt_rect,BSurface,NULL,0,NULL) == DDERR_SURFACELOST)
   {
      DDSurface->Restore();
      BSurface->Restore();
   }
}
#endif


DirectDraw::DirectDraw()
{
   //debug_print("Emu Center HX DirectDraw ON");
   borderFilterWidth = borderFilterHeight = gameboyFilterWidth = gameboyFilterHeight = 1;
   borderFilterType = gameboyFilterType = VIDEO_FILTER_NONE;
}

void DirectDraw::showMessage(wstring message, int duration, gb_system* targetGb)
{
    messageText = message;
    messageDuration = duration;
    messageGb = targetGb;
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
    borderFilterWidth = borderFilterHeight = getFilterDimension(type);
    borderFilterType = type;
    change_filter();
}

void DirectDraw::setGameboyFilter(videofiltertype type) 
{
    gameboyFilterWidth = gameboyFilterHeight = getFilterDimension(type);
    gameboyFilterType = type;
    change_filter();
}

int DirectDraw::getBitCount()
{
	return bitCount;
}
