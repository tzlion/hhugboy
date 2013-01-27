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
#ifndef STRINGS_H
#define STRINGS_H

#define ERROR_CFG_FILE        0
#define ERROR_CFG_FILE_READ   1
#define ERROR_MEMORY          2
#define ERROR_DDRAW           3
#define ERROR_DINPUT          4
#define ERROR_FMOD            5
#define ERROR_SAVE_FILE_READ  6
#define ERROR_SAVE_FILE_WRITE 7
#define ERROR_ROM_LOAD        8 
#define GG_CHT_FORMAT         9
#define ABOUT_STR             10
#define STATE_SLOT            11
#define NOT_A_ROM             12
#define SAVE_TO_SLOT          13
#define SAVE_FAILED           14
#define SAVE_OK               15
#define LOAD_FROM_SLOT        16
#define ERROR_ROM_SIZE        17
#define ERROR_OPEN_FILE       18
#define ERROR_FIND_FILE       19
#define ERROR_BAD_ZIP         20
#define ERROR_FIND_ROM        21
#define ERROR_OPEN_ZIP_FILE   22
#define ERROR_READ_FILE       23
#define ERROR_FSEEK           24
#define ERROR_READ_ROM_INFO   25
#define ERROR_READ_ROM_TO_MEMORY 26
#define ERROR_UNK_OPCODE      27
#define ERROR_OPCODE          28
#define ERROR_ROM_TYPE        29
#define GB_ROMS               30
#define ALL_FILES             31
#define ROM_INFO              32
#define STR_YES               33
#define STR_NO                34
#define GBC_ONLY              35
#define STR_JAPAN             36
#define STR_NON_JAPAN         37
#define CHECK_OK              38
#define CHECK_WRONG           39

extern char* str_table[40];

#endif
