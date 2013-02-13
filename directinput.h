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

#ifndef DIRECTINPUT_H
#define DIRECTINPUT_H

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

extern HINSTANCE hinst;
extern HWND hwnd;

#define SafeRelease(x) if(x) { x->Release(), x=NULL; }

void Check_KBInput(int);
void check_system_keys();
bool Init_DI();
void Kill_DI();

bool Init_DI_change(HWND);
bool Init_DI_change_joy(HWND);
void Kill_DI_change();

int check_change_keys(int, int);
extern int soft_reset;
extern const char* key_names[256];
int check_change_joypad(int index);

extern int autofire_delay[4][4];

const int AUTOFIRE_DELAY_FASTEST = 0;
const int AUTOFIRE_DELAY_FAST = 4;
const int AUTOFIRE_DELAY_MEDIUM = 10;
const int AUTOFIRE_DELAY_SLOW = 20;

#endif
