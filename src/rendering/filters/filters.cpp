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

#include "filters.h"
#include "../render.h"

void filter_none(DWORD *pointer,DWORD *source,int width,int height,int pitch)
{
   copy_line32(pointer,source,width*height); 
}

void filter_none(WORD *pointer,WORD *source,int width,int height,int pitch)
{
   // copy_line16(pointer,source,(width*height)); // fails due to pitch differences?
   
   for(register int y = 0;y < height;y++)
   {
      for(int x = 0;x < width; x++)
      {        
         *pointer++ = *source++;
      }
      pointer+=pitch-width;
  }

}

template<typename TYPE>
void softwarexx_tmp(TYPE *pointer, TYPE *source, int width, int height, int pitch)
{
   register TYPE *target;
   TYPE* init = source;
   
	// (pitch/width) indicates scale
   for(register int y = 0;y < height*(pitch/width);y++)
   { 
      target = pointer + y*pitch;
      source = init + (y/(pitch/width))*width;
      for(int x = 0;x < width; x++)
      {
      	 for (int s = 0; s < (pitch/width) - 1; s++) {
      	 	*target++ = *source;
      	 }
         *target++ = *source++;
      }
  }
}

void softwarexx(WORD *pointer,WORD *source,int width,int height,int pitch)
{
   softwarexx_tmp(pointer,source,width,height,pitch);
}

void softwarexx(DWORD *pointer,DWORD *source,int width,int height,int pitch)
{
   softwarexx_tmp(pointer,source,width,height,pitch);
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
}
*/
