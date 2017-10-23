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

//#define ALLOW_DEBUG

#include "ui/menu.h"
#include "rendering/directdraw.h"
#include "config.h"

extern bool paused;
extern bool menupause; 
extern int sizen_w;
extern int sizen_h;

extern int timer_id;
extern HWND hwndCtrl;    

extern menu emuMenu;

extern DirectDraw renderer;
extern Palette palette;

extern int current_controller;

void menuAction(int menuOption);
void keyAction(int key);

void setWinSize(int width, int height);

extern gb_system* GB;
extern gb_system* GB1;
extern gb_system* GB2;

extern HWND hwnd;
extern HWND hwndCtrl;

extern program_configuration* options;

extern char title_text[ROM_FILENAME_SIZE + 8];
extern wchar_t w_title_text[ROM_FILENAME_SIZE + 16];

extern wstring gb1_loaded_file_name;

extern int ramsize[10];

extern const wchar_t* prg_version;
