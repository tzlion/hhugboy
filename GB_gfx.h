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

#ifndef GFX_H
#define GFX_H

#include "types.h"

extern byte tile_signed_trans[256];
extern byte invert_table[256];

extern unsigned short line_buffer[160];

const int VID_EN_BG = 1;
const int VID_EN_WIN = 2;
const int VID_EN_SPRITE = 4;

extern int video_enable;

#endif
