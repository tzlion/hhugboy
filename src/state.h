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
#ifndef STATE_H
#define STATE_H

#include "types.h"

#include "options.h"

extern int emulating;
extern int sgb_mode;

bool init_gb2();

extern int multiple_gb;
extern int gb_speed_another;

extern int cycles_SGB;

extern int cur_cycle;
extern byte opcode;

extern int GB1_state_slot;
extern int GB2_state_slot;

#endif
