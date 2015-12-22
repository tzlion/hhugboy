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

#include "devices.h"
#include "debug.h"
#include "GB.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

unsigned short (*serial_function)(int);

unsigned short serial_device_none(int stat)
{
   return 0xFF;
}

/*unsigned short serial_device_link(int stat)
{
   return 0xFF; //link_receive_data(stat,NULL);
}*/

int barcode_index = 0;
int barcode_state = 0;

int connected_device = DEVICE_NONE;
int external_clock = 0;

const int BARCODE_LEN = 18;
byte barcode[BARCODE_LEN] = // this is a "barcode" created out of random data, format = ?
{1,7,0,2,1,4,9,4,0,1,7,0,2,1,4,9,4,0};

void random_code()
{
   barcode[4] = barcode[4+9] = rand()%10;
   barcode[5] = barcode[5+9] = rand()%10;
   barcode[6] = barcode[6+9] = rand()%10;
   barcode[7] = barcode[7+9] = rand()%10;
   barcode[8] = barcode[8+9] = rand()%10; 
}

void reset_devices()
{
   barcode_index = 0;
   barcode_state = 1;
   srand(time(0));
   external_clock = 0;
   
   if(connected_device == DEVICE_NONE)
      serial_function = serial_device_none;
   else
   if(connected_device == DEVICE_BARCODE)
      serial_function = serial_device_barcode;  
}

unsigned short serial_device_barcode(int stat) // this is all guessing :)
{
   if(stat == 0)
   {
      barcode_index=0;

      switch(GB->memory[0xff01])
      {
         case 0x10:
            barcode_state = 2;
            external_clock = 1;
            return 0x10;
         case 0x07:
            barcode_state = 1;
            return 0xff;
         case 0x55:
            barcode_state = 3;
            return 0xff;
         default:
         {
            barcode_state = 0;
            return 0xff;
         }
      }
   }
   else
   { 
      if(barcode_state == 2)
         return 0x07; // power test?
      else 
      if(barcode_state == 1)
      {
         if(++barcode_index >= BARCODE_LEN) 
         {
            barcode_index = 0;
            random_code();
            barcode_state = 0;
         }
         return barcode[barcode_index]; 
      } else
          return 0xff;
   }
}
