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
#include "Filter.h"

#ifndef SCALE2X_H
#define SCALE2X_H

typedef unsigned long DWORD;
typedef unsigned short WORD;

class Scale2x: public Filter {
    public:
        void filter32(DWORD *pointer,DWORD *source,int width,int height,int pitch);
        void filter16(WORD *pointer,WORD *source,int width,int height,int pitch);
};

#endif
