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
#include "debug.h"

#define UNICODE
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdio.h>
#include <string>

#include <stdlib.h>

#include "config.h"

using namespace std;

void debug_print(const char* message)
{
    wchar_t wmessage[1000];
    mbstowcs(wmessage,message,1000);
    MessageBox(NULL, wmessage , w_emu_title, 0);
}

void debug_print(const wchar_t* message)
{
   MessageBox(NULL, message , w_emu_title, 0);
}



void debug_log(const string message)
{
   SetCurrentDirectory(options->program_directory.c_str());
   ofstream logfile("gest.log",ios_base::app);

   if(logfile.fail())
   {
     debug_print("Error: Can't open log file!");
     return;
   }

   logfile << message;
   
   logfile.close();
}
