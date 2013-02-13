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
#include <stdio.h>
#include <ddraw.h>

#include <iostream>
#include <fstream>

#include <png.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


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

IDirectDraw7* DD = NULL;
IDirectDrawSurface7* DDSurface = NULL;
IDirectDrawSurface7* BSurface = NULL;
IDirectDrawSurface7* Border_surface = NULL;
IDirectDrawClipper* DDClip = NULL;


RECT target_blt_rect;

char dx_message[60];
int message_time = 0;
gb_system* message_GB = NULL;

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

extern int sizen_w;
extern int sizen_h;

void resize_window(int width, int height)
{
	setWinSize(width,height);
}

DWORD* gfx_pal32 = NULL;
WORD* gfx_pal16 = NULL;

const unsigned short LCD_palette_brown[4] =
 { GBC_RGB(30,31,25),GBC_RGB(20,21,15),GBC_RGB(10,11,5),GBC_RGB(0,0,0) }; 

const unsigned short LCD_palette_green[4] =
 { GBC_RGB(26,31,24),GBC_RGB(16,21,14),GBC_RGB(6,11,4),GBC_RGB(2,3,1) }; 

const unsigned short GB_palette_gray[4] =
 { GBC_RGB(27,28,27),GBC_RGB(17,18,17),GBC_RGB(7,8,7),GBC_RGB(2,2,2) }; 

const unsigned short GBC_DMGBG_palettes[12][4] =
{
 { GBC_RGB(31,31,31),GBC_RGB(20,20,20),GBC_RGB(10,10,10),GBC_RGB(0,0,0) }, // Gray
 { GBC_RGB(31,31,31),GBC_RGB(18,23,18),GBC_RGB(1,16,15),GBC_RGB(0,0,0) }, // Green & blue
 { GBC_RGB(31,31,31),GBC_RGB(26,18,11),GBC_RGB(15,5,5),GBC_RGB(0,0,0) }, // Brown
 { GBC_RGB(31,31,31),GBC_RGB(26,16,15),GBC_RGB(16,7,7),GBC_RGB(0,0,0) }, // Red
 { GBC_RGB(31,31,31),GBC_RGB(24,19,15),GBC_RGB(12,10,8),GBC_RGB(8,4,4) }, // Dark Brown
 { GBC_RGB(31,31,31),GBC_RGB(14,24,20),GBC_RGB(0,14,15),GBC_RGB(0,0,0) }, // Blue 
 { GBC_RGB(31,31,31),GBC_RGB(16,19,17),GBC_RGB(8,12,8),GBC_RGB(0,0,0) }, // Light Green 
 { GBC_RGB(31,31,31),GBC_RGB(26,16,16),GBC_RGB(13,19,22),GBC_RGB(0,0,0) }, // Yellow, Red, Blue 
 { GBC_RGB(31,31,31),GBC_RGB(31,23,15),GBC_RGB(31,8,6),GBC_RGB(0,0,0) }, // Yellow & Red 
 { GBC_RGB(31,31,31),GBC_RGB(31,19,10),GBC_RGB(14,10,5),GBC_RGB(0,0,0) }, // Yellow
 { GBC_RGB(31,31,31),GBC_RGB(16,24,16),GBC_RGB(26,10,10),GBC_RGB(0,0,0) }, // Green & red
 { GBC_RGB(2,0,0),GBC_RGB(3,17,17),GBC_RGB(30,22,22),GBC_RGB(31,31,31) } // Reverse  
};                         

const unsigned short GBC_DMGOBJ0_palettes[12][4] =
{
 { GBC_RGB(31,31,31),GBC_RGB(20,20,20),GBC_RGB(10,10,10),GBC_RGB(0,0,0) }, // Gray
 { GBC_RGB(31,31,31),GBC_RGB(24,14,14),GBC_RGB(16,7,7),GBC_RGB(0,0,0) }, // Red 
 { GBC_RGB(31,31,31),GBC_RGB(25,18,13),GBC_RGB(13,7,7),GBC_RGB(0,0,0) }, // Brown
 { GBC_RGB(31,31,31),GBC_RGB(19,26,19),GBC_RGB(9,17,9),GBC_RGB(0,0,0) }, // Green
 { GBC_RGB(31,31,31),GBC_RGB(28,18,15),GBC_RGB(21,7,7),GBC_RGB(0,0,0) }, // Brown
 { GBC_RGB(31,31,31),GBC_RGB(24,14,14),GBC_RGB(16,7,7),GBC_RGB(0,0,0) }, // Red 
 { GBC_RGB(31,31,31),GBC_RGB(24,14,14),GBC_RGB(16,7,7),GBC_RGB(0,0,0) }, // Red 
 { GBC_RGB(31,31,31),GBC_RGB(26,16,15),GBC_RGB(13,19,22),GBC_RGB(0,0,0) }, // Yellow, Red, Blue
 { GBC_RGB(31,31,31),GBC_RGB(31,23,15),GBC_RGB(31,8,6),GBC_RGB(0,0,0) }, // Yellow & Red 
 { GBC_RGB(31,31,31),GBC_RGB(11,25,29),GBC_RGB(0,13,16),GBC_RGB(0,0,0) }, // Blue 
 { GBC_RGB(31,31,31),GBC_RGB(16,24,16),GBC_RGB(26,10,10),GBC_RGB(0,0,0) }, // Green & red
 { GBC_RGB(2,0,0),GBC_RGB(3,17,17),GBC_RGB(30,22,22),GBC_RGB(31,31,31) } // Reverse 
};

const unsigned short GBC_DMGOBJ1_palettes[12][4] =
{
 { GBC_RGB(31,31,31),GBC_RGB(20,20,20),GBC_RGB(10,10,10),GBC_RGB(0,0,0) }, // Gray
 { GBC_RGB(31,31,31),GBC_RGB(24,14,14),GBC_RGB(16,7,7),GBC_RGB(0,0,0) }, // Red 
 { GBC_RGB(31,31,31),GBC_RGB(25,18,13),GBC_RGB(13,7,7),GBC_RGB(0,0,0) }, // Brown
 { GBC_RGB(31,31,31),GBC_RGB(15,20,23),GBC_RGB(0,11,12),GBC_RGB(0,0,0) }, // Blue
 { GBC_RGB(31,31,31),GBC_RGB(28,18,15),GBC_RGB(19,9,9),GBC_RGB(0,0,0) }, // Brown
 { GBC_RGB(31,31,31),GBC_RGB(19,26,19),GBC_RGB(9,17,9),GBC_RGB(0,0,0) }, // Green
 { GBC_RGB(31,31,31),GBC_RGB(24,14,14),GBC_RGB(16,7,7),GBC_RGB(0,0,0) }, // Red 
 { GBC_RGB(31,31,31),GBC_RGB(26,16,15),GBC_RGB(13,19,22),GBC_RGB(0,0,0) }, // Yellow, Red, Blue
 { GBC_RGB(31,31,31),GBC_RGB(31,23,15),GBC_RGB(31,8,6),GBC_RGB(0,0,0) }, // Yellow & Red 
 { GBC_RGB(31,31,31),GBC_RGB(18,27,21),GBC_RGB(4,11,14),GBC_RGB(0,0,0) }, // Green 
 { GBC_RGB(31,31,31),GBC_RGB(16,24,16),GBC_RGB(26,10,10),GBC_RGB(0,0,0) }, // Green & red
 { GBC_RGB(2,0,0),GBC_RGB(3,17,17),GBC_RGB(30,22,22),GBC_RGB(31,31,31) } // Reverse 
};

int lPitch = 160; // should be 160
int border_lPitch;
int dx_bitcount;

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
           if(dx_bitcount == 16)
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
           if(dx_bitcount == 16)
              gfx_pal16[i] = (red<<rs) | (green<<gs) | (blue<<bs);
           else        
              gfx_pal32[i] = (red<<rs) | (green<<gs) | (blue<<bs);
        }
     }
  } else
  {
     if(dx_bitcount == 16)
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
      if(dx_bitcount==16)
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
      if(dx_bitcount==16)
      {
         delete [] (WORD*)dx_border_buffer_render;
      } else
      {
         delete [] (DWORD*)dx_border_buffer_render;
      }
      dx_border_buffer_render = NULL; 
   }   
      
   SafeRelease(BSurface);
   SafeRelease(Border_surface);
   SafeRelease(DDSurface);
   SafeRelease(DDClip);
   SafeRelease(DD);
   
   DeleteObject(afont);
}

void (*filter_f_32)(DWORD *target,DWORD *src,int width,int height,int pitch) = NULL;
void (*filter_f_16)(WORD *target,WORD *src,int width,int height,int pitch) = NULL;
void (*border_filter_f_32)(DWORD *target,DWORD *src,int width,int height,int pitch) = NULL;
void (*border_filter_f_16)(WORD *target,WORD *src,int width,int height,int pitch) = NULL;
int filter_width = 1;
int filter_height = 1;
int border_filter_width = 1;
int border_filter_height = 1;

void screenshotPng(char* filename) 
{
	int height=144;
	int width=160;
	
	png::image< png::rgb_pixel > image(width,height);
	
	if(dx_bitcount == 16) {
		
		WORD* source = (WORD*)GB->gfx_buffer;
		WORD* init = source;
		for(register int y = 0;y < height;y++)
		{ 
			source = init + y*width;
			for(int x = 0;x < width; x++)
			{
				WORD px = *source++;
				image[y][x] = png::rgb_pixel((0xFF0000&px)/0x10000,(0x00FF00&px)/0x100,(0x0000FF&px));
			}
		}
		
	} else {

		DWORD* source = (DWORD*)GB->gfx_buffer;
		DWORD* init = source;
		for(register int y = 0;y < height;y++)
		{ 
			source = init + y*width;
			for(int x = 0;x < width; x++)
			{   
			    DWORD px = *source++;
				image[y][x] = png::rgb_pixel((0xFF0000&px)/0x10000,(0x00FF00&px)/0x100,(0x0000FF&px));
				//image[y][x] = png::rgb_pixel(x,y,px); // purple gradient
			}
		}

	}

	image.write(filename);
}

// this is what you might call a novelty
// not enabled at the moment
void screenshotHtml(char* filename) 
{
	ofstream myfile;
	myfile.open (filename);
	
	int height=144;
	int width=160;
	
	if(dx_bitcount == 16) {
		
		WORD* source = (WORD*)GB->gfx_buffer;
		WORD* init = source;
		for(register int y = 0;y < height;y++)
		{ 
			myfile << "<div style='line-height:2px;height:2px;'>";
			source = init + y*width;
			for(int x = 0;x < width; x++)
			{
				WORD px = *source++;
				char col[500];
				sprintf(col,"<span style='background-color:#%06X;display:inline-block;line-height:2px;width:2px;height:2px;'></span>",px);
				myfile << col;
			}
			myfile << "</div>";
		}
		
	} else {

		DWORD* source = (DWORD*)GB->gfx_buffer;
		DWORD* init = source;
		for(register int y = 0;y < height;y++)
		{ 
			myfile << "<div style='line-height:2px;height:2px;'>";
			source = init + y*width;
			for(int x = 0;x < width; x++)
			{
				DWORD px = *source++;
				char col[500];
				sprintf(col,"<span style='background-color:#%06X;display:inline-block;line-height:2px;width:2px;height:2px;'></span>",px);
				myfile << col;
			}
			myfile << "</div>";
		}

	}
	
	myfile.close();
}

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
   
	// filter_height indicates scale 
   for(register int y = 0;y < height*filter_height;y++)
   { 
      target = pointer + y*pitch;
      source = init + (y/filter_height)*width;
      for(int x = 0;x < width; x++)
      {
      	 for (int s = 0; s < filter_height - 1; s++) {
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

	// filter_height indicates scale 
   for(register int y = 0;y < height*filter_height;y++)
   { 
      target = pointer + y*pitch;
      source = init + (y/filter_height)*width;
      for(int x = 0;x < width; x++)
      {
      	 for (int s = 0; s < filter_height - 1; s++) {
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
      
   for(register int y=2;y<(height*filter_height)-2;y+=2) 
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

   target = pointer+(height*filter_height-2)*pitch;
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
      
   target = pointer+(height*filter_height-1)*pitch;
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
      
   for(register int y=2;y<(height*filter_height)-2;y+=2) 
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

   target = pointer+(height*filter_height-2)*pitch;
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
    
      
   target = pointer+(height*filter_height-1)*pitch;
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
   
   SafeRelease(BSurface);
   SafeRelease(Border_surface);
   
   ZeroMemory(&ddsd,sizeof(ddsd));
   ddsd.dwSize = sizeof(DDSURFACEDESC2);
   ddsd.dwFlags = DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT;
   ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN|DDSCAPS_VIDEOMEMORY;//DDSCAPS_SYSTEMMEMORY;
   ddsd.dwWidth = 160*filter_width;
   ddsd.dwHeight = 144*filter_height;
   
   ddrval = DD->CreateSurface(&ddsd,&BSurface,NULL);
   if(ddrval != DD_OK) 
   {
      debug_print("DirectDraw Createsurface failed!"); 
   
      Kill_DD();
      return false;
   }

   ddsd.dwWidth = 256*border_filter_width;
   ddsd.dwHeight = 224*border_filter_height;
   ddrval = DD->CreateSurface(&ddsd,&Border_surface,NULL);
   if(ddrval != DD_OK) 
   {
      debug_print("DirectDraw Createsurface failed!"); 
   
      Kill_DD();
      return false;
   } 
   
   ZeroMemory(&ddsd,sizeof(ddsd));
   ddsd.dwSize = sizeof(DDSURFACEDESC2);
   BSurface->Lock(NULL,&ddsd,DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR,NULL);
   
   lPitch = ddsd.lPitch;

   BSurface->Unlock(NULL);
   
   if(dx_bitcount==16)
   {
      lPitch >>= 1;  
       
      switch(options->video_filter)
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

      switch(options->video_filter)
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
      switch(options->video_SGBborder_filter)
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
   return true;
}

bool Init_DD()
{
   HRESULT ddrval;
   DDSURFACEDESC2 ddsd;
   //DDSCAPS2 ddscaps;
      
   ddrval = DirectDrawCreateEx(NULL, (void**)&DD, IID_IDirectDraw7, NULL); 
   if(ddrval!=DD_OK)
   {
      debug_print("DirectDraw Create failed!"); 
   
      return false;
   }
   ddrval = DD->SetCooperativeLevel(hwnd, DDSCL_NORMAL);
   if(ddrval!=DD_OK)
   {
      debug_print("DirectDraw: SetCooperativelevel failed!"); 

      Kill_DD();
      return false;
   }
   
   ddrval = DD->CreateClipper(0,&DDClip,NULL);
   if(ddrval!=DD_OK)
   {
      debug_print("DirectDraw: CreateClipper failed!"); 
   
      Kill_DD();
      return false;
   }
   DDClip->SetHWnd(0,hwnd);
   
   ZeroMemory(&ddsd,sizeof(ddsd));
   ddsd.dwSize = sizeof(DDSURFACEDESC2);
   ddsd.dwFlags = DDSD_CAPS;
   ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
   ddrval = DD->CreateSurface(&ddsd,&DDSurface,NULL);
   if(ddrval != DD_OK) 
   {
      debug_print("DirectDraw: Create main surface failed!"); 

      Kill_DD();
      return false;
   }
   
   DDSurface->SetClipper(DDClip);
   
   ZeroMemory(&ddsd,sizeof(ddsd));
   ddsd.dwSize = sizeof(DDSURFACEDESC2);
   ddsd.dwFlags = DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT;
   ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN|DDSCAPS_VIDEOMEMORY;//DDSCAPS_SYSTEMMEMORY;
   ddsd.dwWidth = 160;
   ddsd.dwHeight = 144;
   
   ddrval = DD->CreateSurface(&ddsd,&BSurface,NULL);
   if(ddrval != DD_OK) 
   {
      debug_print("DirectDraw: Create gb surface failed!"); 
   
      Kill_DD();
      return false;
   }
   ddsd.dwWidth = 256;
   ddsd.dwHeight = 224;   
   ddrval = DD->CreateSurface(&ddsd,&Border_surface,NULL);
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
   BSurface->Blt(NULL,NULL,NULL,DDBLT_COLORFILL,&clrblt);
   Border_surface->Blt(NULL,NULL,NULL,DDBLT_COLORFILL,&clrblt);

   ZeroMemory(&ddsd,sizeof(ddsd));
   ddsd.dwSize = sizeof(DDSURFACEDESC2);
   ddsd.dwFlags = DDSD_PIXELFORMAT;
   BSurface->Lock(NULL,&ddsd,DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR,NULL);
   
   dx_bitcount = ddsd.ddpfPixelFormat.dwRGBBitCount; 
   lPitch = ddsd.lPitch;
   
   BSurface->Unlock(NULL);
   
   DDPIXELFORMAT px;

   px.dwSize = sizeof(px);

   BSurface->GetPixelFormat(&px);
  
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
    if(dx_bitcount == 32 || dx_bitcount == 24) 
    {
      rs += 3;
      gs += 3;
      bs += 3;
    }
  }
  
  if(dx_bitcount == 16)
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

  afont = CreateFont(12,6,2,2,FW_BOLD,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,PROOF_QUALITY,DEFAULT_PITCH|FF_ROMAN,NULL);

  return true;
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
   Border_surface->Lock(NULL,&ddsd,DDLOCK_WRITEONLY|DDLOCK_SURFACEMEMORYPTR,NULL);
   border_lPitch = ddsd.lPitch>>2;

   int temp_w = filter_width;
   int temp_h = filter_height;   
   filter_width = border_filter_width;
   filter_height = border_filter_height;   
   border_filter_f_32((DWORD*)ddsd.lpSurface,(DWORD*)dx_border_buffer_render,256,224,border_lPitch);
   filter_width = temp_w;
   filter_height = temp_h;
      
   Border_surface->Unlock(NULL);   
   
   POINT pt;
   RECT rect;

   GetClientRect(hwnd,&rect);
   pt.x=pt.y=0;
   ClientToScreen(hwnd,&pt);
   OffsetRect(&rect,pt.x,pt.y);

   if(DDSurface->Blt(&rect,Border_surface,NULL,0,NULL) == DDERR_SURFACELOST)
   {
      DDSurface->Restore();
      Border_surface->Restore();
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
   Border_surface->Lock(NULL,&ddsd,DDLOCK_WRITEONLY|DDLOCK_SURFACEMEMORYPTR,NULL);
   border_lPitch = ddsd.lPitch>>1;

   int temp_w = filter_width;
   int temp_h = filter_height;   
   filter_width = border_filter_width;
   filter_height = border_filter_height;   
   border_filter_f_16((WORD*)ddsd.lpSurface,(WORD*)dx_border_buffer_render,256,224,border_lPitch);
   filter_width = temp_w;
   filter_height = temp_h;   
   
   Border_surface->Unlock(NULL);   
   
   POINT pt;
   RECT rect;
   
   GetClientRect(hwnd,&rect);
   pt.x=pt.y=0;
   ClientToScreen(hwnd,&pt);
   OffsetRect(&rect,pt.x,pt.y);
   
   if(DDSurface->Blt(&rect,Border_surface,NULL,0,NULL) == DDERR_SURFACELOST)
   {
      DDSurface->Restore();
      Border_surface->Restore();
   }
}

void draw_screen32()
{  
   DDSURFACEDESC2 ddsd;
   ZeroMemory(&ddsd,sizeof(ddsd));
   ddsd.dwSize = sizeof(DDSURFACEDESC2);
   BSurface->Lock(NULL,&ddsd,DDLOCK_WRITEONLY|DDLOCK_SURFACEMEMORYPTR,NULL);
      
   filter_f_32((DWORD*)ddsd.lpSurface,(DWORD*)GB->gfx_buffer,160,144,lPitch);
  
   BSurface->Unlock(NULL);   
      
   if(options->video_visual_rumble && GB->rumble_counter)
   {
      --GB->rumble_counter;
      if(!(GB->rumble_counter%2))
      {
         target_blt_rect.left-=2;
         target_blt_rect.right-=2;
         change_rect = 1;
      } else change_rect = 0;
   } else change_rect = 0;
   
   if(message_time && GB == message_GB)
   {
      --message_time;
      HDC aDC;
      if(BSurface->GetDC(&aDC)==DD_OK)
      {
         SelectObject(aDC,afont);
         SetBkMode(aDC, TRANSPARENT);
         SetTextColor(aDC,RGB(255,0,0));
         TextOut(aDC,0,0,dx_message,strlen(dx_message));
         BSurface->ReleaseDC(aDC);
      }
   }

   int screen_real_width = target_blt_rect.right - target_blt_rect.left;

   if(multiple_gb && GB == GB2)
   {
       target_blt_rect.left += screen_real_width;
       target_blt_rect.right += screen_real_width;
   }

   if(DDSurface->Blt(&target_blt_rect,BSurface,NULL,0,NULL) == DDERR_SURFACELOST)
   {
      DDSurface->Restore();
      BSurface->Restore();
   }

   if(multiple_gb && GB == GB2)
   {
      target_blt_rect.left -= screen_real_width;
      target_blt_rect.right -= screen_real_width;
   } 
   
   if(change_rect)
   {
     target_blt_rect.left += 2;
     target_blt_rect.right += 2;
   }
}

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

   DDSURFACEDESC2 ddsd;

   ZeroMemory(&ddsd,sizeof(ddsd));
   ddsd.dwSize = sizeof(DDSURFACEDESC2);
   BSurface->Lock(NULL,&ddsd,DDLOCK_WRITEONLY|DDLOCK_SURFACEMEMORYPTR,NULL);

   filter_f_32((DWORD*)ddsd.lpSurface,(DWORD*)dx_buffer_mix,160,144,lPitch);

   BSurface->Unlock(NULL);   

   if(options->video_visual_rumble && GB->rumble_counter)
   {
      --GB->rumble_counter;
      if(!(GB->rumble_counter%2))
      {
         target_blt_rect.left-=2;
         target_blt_rect.right-=2;
         change_rect = 1;
      } else change_rect = 0;
   } else change_rect = 0;
   
   if(message_time && GB == message_GB)
   {
      --message_time;
      HDC aDC;
      if(BSurface->GetDC(&aDC)==DD_OK)
      {
         SelectObject(aDC,afont);
         SetBkMode(aDC, TRANSPARENT);
         SetTextColor(aDC,RGB(255,0,0));
         TextOut(aDC,0,0,dx_message,strlen(dx_message));
         BSurface->ReleaseDC(aDC);
      }
   }
   
   int screen_real_width = target_blt_rect.right - target_blt_rect.left;

   if(multiple_gb && GB == GB2)
   {
       target_blt_rect.left += screen_real_width;
       target_blt_rect.right += screen_real_width;
   }
   
   if(DDSurface->Blt(&target_blt_rect,BSurface,NULL,0,NULL) == DDERR_SURFACELOST)
   {
      DDSurface->Restore();
      BSurface->Restore();
   }
   
   if(multiple_gb && GB == GB2)
   {
      target_blt_rect.left -= screen_real_width;
      target_blt_rect.right -= screen_real_width;
   } 

   if(change_rect)
   {
     target_blt_rect.left += 2;
     target_blt_rect.right += 2;
   }
}

void draw_screen16()
{
   DDSURFACEDESC2 ddsd;
   
   ZeroMemory(&ddsd,sizeof(ddsd));
   ddsd.dwSize = sizeof(DDSURFACEDESC2);
   BSurface->Lock(NULL,&ddsd,DDLOCK_WRITEONLY|DDLOCK_SURFACEMEMORYPTR,NULL);
      
   filter_f_16((WORD*)ddsd.lpSurface,(WORD*)GB->gfx_buffer,160,144,lPitch);
   
   BSurface->Unlock(NULL);   
         
   if(options->video_visual_rumble && GB->rumble_counter)
   {
      --GB->rumble_counter;
      if(!(GB->rumble_counter%2))
      {
         target_blt_rect.left-=2;
         target_blt_rect.right-=2;
         change_rect = 1;
      } else change_rect = 0;
   } else change_rect = 0;

   if(message_time && GB == message_GB)
   {
      --message_time;
      HDC aDC;
      if(BSurface->GetDC(&aDC)==DD_OK)
      {
         SelectObject(aDC,afont);
         SetBkMode(aDC, TRANSPARENT);
         SetTextColor(aDC,RGB(255,0,0));
         TextOut(aDC,0,0,dx_message,strlen(dx_message));

         BSurface->ReleaseDC(aDC);
      }
   }

   int screen_real_width = target_blt_rect.right - target_blt_rect.left;

   if(multiple_gb && GB == GB2)
   {
       target_blt_rect.left += screen_real_width;
       target_blt_rect.right += screen_real_width;
   }

   if(DDSurface->Blt(&target_blt_rect,BSurface,NULL,0,NULL) == DDERR_SURFACELOST)
   {
      DDSurface->Restore();
      BSurface->Restore();
   }

   if(multiple_gb && GB == GB2)
   {
      target_blt_rect.left -= screen_real_width;
      target_blt_rect.right -= screen_real_width;
   } 

   if(change_rect)
   {
     target_blt_rect.left += 2;
     target_blt_rect.right += 2;
   }
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
   
   DDSURFACEDESC2 ddsd;

   ZeroMemory(&ddsd,sizeof(ddsd));
   ddsd.dwSize = sizeof(DDSURFACEDESC2);
   BSurface->Lock(NULL,&ddsd,DDLOCK_WRITEONLY|DDLOCK_SURFACEMEMORYPTR,NULL);
          
   filter_f_16((WORD*)ddsd.lpSurface,(WORD*)dx_buffer_mix,160,144,lPitch);
   
   BSurface->Unlock(NULL);   
              
   if(options->video_visual_rumble && GB->rumble_counter)
   {
      --GB->rumble_counter;
      if(!(GB->rumble_counter%2))
      {
         target_blt_rect.left-=2;
         target_blt_rect.right-=2;
         change_rect = 1;
      } else change_rect = 0;
   } else change_rect = 0;
   
   if(message_time && GB == message_GB)
   {
      --message_time;
      HDC aDC;
      if(BSurface->GetDC(&aDC)==DD_OK)
      {
         SelectObject(aDC,afont);
         SetBkMode(aDC, TRANSPARENT);
         SetTextColor(aDC,RGB(255,0,0));
         TextOut(aDC,0,0,dx_message,strlen(dx_message));
         BSurface->ReleaseDC(aDC);
      }
   }
   
   int screen_real_width = target_blt_rect.right - target_blt_rect.left;

   if(multiple_gb && GB == GB2)
   {
       target_blt_rect.left += screen_real_width;
       target_blt_rect.right += screen_real_width;
   }

   if(DDSurface->Blt(&target_blt_rect,BSurface,NULL,0,NULL) == DDERR_SURFACELOST)
   {
      DDSurface->Restore();
      BSurface->Restore();
   }

   if(multiple_gb && GB == GB2)
   {
      target_blt_rect.left -= screen_real_width;
      target_blt_rect.right -= screen_real_width;
   } 

   if(change_rect)
   {
     target_blt_rect.left+=2;
     target_blt_rect.right+=2;
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
