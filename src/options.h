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

#ifndef OPTIONS_H
#define OPTIONS_H

#define SYS_AUTO 0
#define SYS_GB   1
#define SYS_GBP  2
#define SYS_GBA  3
#define SYS_GBC  4
#define SYS_SGB  5
#define SYS_SGB2 6

// Ideally what I would like to do is have these as an enum too ^
// and use everything in this file for menu ID generation
// but you can only use #defined constants for that
// and it feels like i'd be taking a step backwards if i turned all these enums into #defines
// so HMM

enum SGBborderoption
{
    OFF = 0,
    GBC_WITH_SGB_BORDER = 1,
    GBC_WITH_INITIAL_SGB_BORDER = 2
};

enum GBpaletteoption
{
    BLACK_WHITE = 0,
    GRAY = 1,
    LCD_BROWN = 2,
    LCD_GREEN = 3
};

enum videofiltertype
{
    VIDEO_FILTER_NONE = 0,
    VIDEO_FILTER_SOFT2X = 1,
    VIDEO_FILTER_SCALE2X = 2,
    VIDEO_FILTER_BLUR = 3,
    VIDEO_FILTER_SCALE3X = 4,
    VIDEO_FILTER_SOFTXX = 5
};

enum soundvolumetype
{
	VOLUME_025X = 25,
	VOLUME_05X = 50,
    VOLUME_1X = 100,
    VOLUME_2X = 200,
    VOLUME_3X = 300,
    VOLUME_4X = 400
};

enum _controllernumber
{
    PLAYER1 = 0,
    PLAYER2 = 1,
    PLAYER3 = 2,
    PLAYER4 = 3
};

enum _keys
{
    BUTTON_A = 0,
    BUTTON_B,
    BUTTON_START,
    BUTTON_SELECT,
    BUTTON_TURBO_A,
    BUTTON_TURBO_B,
    BUTTON_TURBO_START,
    BUTTON_TURBO_SELECT,
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_LEFT,
    BUTTON_RIGHT
};

enum _specialkeys
{
    BUTTON_L = 0,
    BUTTON_R,
    BUTTON_SPEEDUP,
    BUTTON_SENSOR_LEFT,
    BUTTON_SENSOR_RIGHT,
    BUTTON_SENSOR_UP,
    BUTTON_SENSOR_DOWN
};

enum mixframestype
{
    MIX_FRAMES_OFF = 0,
    MIX_FRAMES_ON = 1,
    MIX_FRAMES_MORE = 2
};

enum unlCompatMode
{
	UNL_AUTO,
	UNL_NONE,
	UNL_NIUTOUDE,
	UNL_SINTAX,
	UNL_BBD,
	UNL_HITEK,
	UNL_LBMULTI,
	UNL_NTOLD1,
	UNL_NTOLD2,
	UNL_MBC1NOSAVE,
	UNL_MBC1SAVE,
	UNL_MBC5NOSAVE,
	UNL_MBC5SAVE,
	UNL_DBZTR,
	UNL_MAKONNEW
};

#endif
