/*
   hhugboy Game Boy emulator
   copyright 2013 taizou

   Based on GEST
   Copyright (C) 2003-2010 TM
   This file incorporates code from the Scale2x project
   Copyright (C) 2001, 2002, 2003, 2004 Andrea Mazzoleni

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

#ifndef __SCALE3X_H
#define __SCALE3X_H

typedef unsigned char scale3x_uint8;
typedef unsigned short scale3x_uint16;
typedef unsigned scale3x_uint32;

typedef unsigned long DWORD;
typedef unsigned short WORD;

void Scale3x32(DWORD *dstPtr,DWORD *srcPtr,int width,int height,int pitch);
void Scale3x16(WORD *dstPtr,WORD *srcPtr,int width,int height,int pitch);

#endif

