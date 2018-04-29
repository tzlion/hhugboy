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

#include "menu.h"

// constructor
menu::menu(HINSTANCE instance)
{
	menuBar = LoadMenu(instance, MAKEINTRESOURCE(ID_MENU));
}

menu::menu()
{
	menuBar = NULL;
}

void menu::init(HINSTANCE instance)
{
	menuBar = LoadMenu(instance, MAKEINTRESOURCE(ID_MENU));
}

void menu::setText(int option, wchar_t* text) {
    ModifyMenuW(menuBar,option,MF_BYCOMMAND,option,text);
}

void menu::checkOption(int option)
{	
    // check one option in a group but uncheck the rest
    
    int group = option & 0xFF00;
    
    int curItem = group;
    
    if (group > 0) {
        // this isnt ideal, we are just unchecking 16 items in the group
        // checkMenuItem is supposed to return -1 if the item doesnt exist but im not sure if it does
        for (int curItem = group; curItem < group + 16; curItem++) {
            CheckMenuItem(menuBar,curItem,MF_UNCHECKED);   
       }
    }
    
    CheckMenuItem(menuBar,option,MF_CHECKED);   
}

void menu::uncheckOption(int option) 
{
    CheckMenuItem(menuBar,option,MF_UNCHECKED);
}

HMENU menu::getMenu()
{
	return menuBar;
}
