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

#ifndef CPU_H
#define CPU_H

#include "types.h"

#define PUSH(n) (writememory(--SP.W,(n)));
#define POP(n) ((n) = readmemory(SP.W++));

/*
extern byte A;
extern byte F; 
extern word BC;
extern word DE;
extern word HL;
extern word SP;   

extern unsigned short flags; */
// flags in x86 order
// size word because lahf loads flags to ah

//inline void PUSH(unsigned short data) { writememory(--SP.W,data>>8); writememory(--SP.W,data); }
//inline void POP(unsigned short& target) { target = readword(SP.W); SP.W+=2; }

extern int emulating;
extern int sgb_mode;

const int cycles[256] =
{ 
//0   1   2   3   4   5  6   7   8   9   A  B   C   D  E   F
  4, 12,  8,  8,  4,  4, 8,  4, 20,  8,  8, 8,  4,  4, 8,  4, // 0
  4, 12,  8,  8,  4,  4, 8,  4, 12,  8,  8, 8,  4,  4, 8,  4, // 1
  8, 12,  8,  8,  4,  4, 8,  4,  8,  8,  8, 8,  4,  4, 8,  4, // 2 
  8, 12,  8,  8, 12, 12,12,  4,  8,  8,  8, 8,  4,  4, 8,  4, // 3
  4,  4,  4,  4,  4,  4, 8,  4,  4,  4,  4, 4,  4,  4, 8,  4, // 4
  4,  4,  4,  4,  4,  4, 8,  4,  4,  4,  4, 4,  4,  4, 8,  4, // 5
  4,  4,  4,  4,  4,  4, 8,  4,  4,  4,  4, 4,  4,  4, 8,  4, // 6
  8,  8,  8,  8,  8,  8, 4,  8,  4,  4,  4, 4,  4,  4, 8,  4, // 7
  4,  4,  4,  4,  4,  4, 8,  4,  4,  4,  4, 4,  4,  4, 8,  4, // 8
  4,  4,  4,  4,  4,  4, 8,  4,  4,  4,  4, 4,  4,  4, 8,  4, // 9
  4,  4,  4,  4,  4,  4, 8,  4,  4,  4,  4, 4,  4,  4, 8,  4, // A
  4,  4,  4,  4,  4,  4, 8,  4,  4,  4,  4, 4,  4,  4, 8,  4, // B
  8, 12, 12, 16, 12, 16, 8, 16,  8, 16, 12, 8, 12, 24, 8, 16, // C 
  8, 12, 12,  0, 12, 16, 8, 16,  8, 16, 12, 0, 12,  0, 8, 16, // D
  12, 12, 8,  0,  0, 16, 8, 16, 16,  4, 16, 0,  0,  0, 8, 16, // E
  12, 12, 8,  4,  0, 16, 8, 16, 12,  8, 16, 4,  0,  0, 8, 16  // F
};

const int CB_cycles[256] =
{
//0  1  2  3  4  5   6  7  8  9  A  B  C  D   E  F
  8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // 0
  8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // 1
  8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // 2
  8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // 3
  8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8, // 4
  8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8, // 5
  8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8, // 6  
  8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8, // 7
  8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // 8
  8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // 9
  8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // A
  8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // B
  8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // C
  8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // D
  8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // E
  8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8  // F
};

const int zero_table[256] =
{
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
/*
extern int CFLAG;
extern int HFLAG;
extern int ZFLAG;
extern int NFLAG;
*/
#define SYS_AUTO 0
#define SYS_GB   1
#define SYS_GBP  2
#define SYS_GBA  3
#define SYS_GBC  4
#define SYS_SGB  5
#define SYS_SGB2 6

bool init_gb2();

extern int multiple_gb;
extern int gb_speed_another;

const int STOP = 1;
const int HALT = 2;
const int HALT2 = 3;

const int CYCLES_DIV = 256; // 256
const int CYCLES_TIMER_MODE0 = 1024; // 1024
const int CYCLES_TIMER_MODE1 = 16; // 16
const int CYCLES_TIMER_MODE2 = 64; // 64
const int CYCLES_TIMER_MODE3 = 256; // 256
const int CYCLES_SGB_TIMEOUT = 66666;

const int CYCLES_SERIAL_GB = 512;
const int CYCLES_SERIAL_GBC = 16;

const int CYCLES_LCD_MODE0 = 375; // 376 / 375
const int CYCLES_LCD_MODE1 = 456; // 456
const int CYCLES_LCD_MODE2 = 82; // 80 / 82
const int CYCLES_LCD_MODE3 = 172; // 172 

extern int cycles_SGB;

extern int cur_cycle;
extern byte opcode;

extern int GB1_state_slot;
extern int GB2_state_slot;

#endif
