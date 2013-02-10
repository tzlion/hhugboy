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
#include "debug.h"
#include "config.h"

#include <windows.h>
#include <stdio.h>
#include <string>

using namespace std;

void debug_print(const char* message)
{
   MessageBox(NULL, message , emu_title, 0);
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
