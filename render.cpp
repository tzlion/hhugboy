/*
   unGEST Game Boy emulator
   copyright 2013 taizou

   Based on GEST
   Copyright (C) 2003-2010 TM

   This file is part of unGEST.

   unGEST is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   unGEST is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with unGEST.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "render.h"
#include "GB.h"

void fill_gfx_buffers(unsigned long val)
{
   int count = 160*144;
      
   if(GB->gfx_bit_count == 16)
   {
      fill_line16((WORD*)GB->gfx_buffer1, val,count);
      fill_line16((WORD*)GB->gfx_buffer2, val,count);
      fill_line16((WORD*)GB->gfx_buffer3, val,count);
      fill_line16((WORD*)GB->gfx_buffer4, val,count);
   } else
   {
      fill_line32((DWORD*)GB->gfx_buffer1, val,count);
      fill_line32((DWORD*)GB->gfx_buffer2, val,count);
      fill_line32((DWORD*)GB->gfx_buffer3, val,count);
      fill_line32((DWORD*)GB->gfx_buffer4, val,count);
   }
}

void LCDoff_fill_gfx_buffer(unsigned long val)
{
   int count = 160*144;

   if(GB->gfx_bit_count == 16)
   {
      fill_line16((WORD*)GB->gfx_buffer, val,count);
   } else
   {
      fill_line32((DWORD*)GB->gfx_buffer, val,count);
   }
}

void fill_line16(unsigned short* adr, unsigned long val, int count)
{
   __asm__ __volatile__ 
   (    
      "cld\n"  
      "rep\n"
      "stosl\n"    
      : "=c" (count), "=D" (val)  // dummy values, because register is corrupted
      : "c" (count>>1), "a" (val), "D" (adr)
      : "memory" );
}

void fill_line32(unsigned long* adr, unsigned long val, int count)
{            
   __asm__ __volatile__ 
   (       
      "cld\n"  
      "rep\n"
      "stosl\n"    
      : "=c" (count), "=D" (val)  // dummy values, because register is corrupted
      : "c" (count), "a" (val), "D" (adr)
      : "memory" );
}

void copy_line16(unsigned short* target, unsigned short* src, int count)
{
   unsigned long dummy;
   __asm__ __volatile__
   (
      "cld\n"
      "rep\n"
      "movsd\n"
      : "=c" (count), "=D" (dummy), "=S" (dummy)  // dummy values, because register is corrupted
      : "c" (count>>1), "S" (src), "D" (target)
      : "memory" );
}

void copy_line32(unsigned long* target, unsigned long* src, int count)
{
   unsigned long dummy;
   __asm__ __volatile__
   (
      "cld\n"
      "rep\n"
      "movsd\n"
      : "=c" (count), "=D" (dummy), "=S" (dummy)  // dummy values, because register is corrupted
      : "c" (count), "S" (src), "D" (target)
      : "memory" );
}

