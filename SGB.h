/*
GEST - Gameboy emulator
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
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#ifndef SGB_H
#define SGB_H

#include "types.h"

extern int border_uploaded;

extern byte *sgb_borderchar;
extern byte *sgb_border;

extern int bit_received;
extern int bits_received;
extern int packets_received;
extern int sgb_state; 
extern int sgb_index;
extern int sgb_command;
extern int sgb_multiplayer;
extern int sgb_fourplayers;
extern int sgb_nextcontrol;
extern int sgb_readingcontrol;
extern int sgb_CGB_support;
extern int sgb_mask;
extern int sgb_file_transfer;

extern unsigned short sgb_border_buffer[256*224];

extern unsigned short sgb_palette[8*16];
extern unsigned short sgb_palette_memory[512*4];
extern byte sgb_ATF[20 * 18];
extern byte sgb_ATF_list[45 * 20 * 18];
extern byte sgb_buffer[16*7];
extern int sgb_multiplayer;
extern int sgb_fourplayers;
extern int sgb_nextcontrol;
extern int sgb_readingcontrol;

bool sgb_init();
void sgb_end();
void sgb_transfer(byte);
void sgb_reset();
void sgb_reset_state();

#endif
