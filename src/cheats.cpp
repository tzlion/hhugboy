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
#define UNICODE

#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "cheats.h"

// Cheats --------------------------------------------
int number_of_cheats = 0;
gg_cheats cheat[MAXGGCHEATS];

wchar_t gg_cheat_values[23] = L"0123456789ABCDEFabcdef";

int rol_l(int number,int count) // roll left
{
   while(count)
   {
      int bit = number&0x80;
      number <<= 1;
      number &= 0xFF;
      number |= (bit>>7);
      --count;
   }  
   return number;
}

////////////////////////////////////////
// bool add_cheat(char* cheat_str) :
// checks if cheat_str is valid cheat
// and adds it to cheats
////////////////////////////////////////
int parseStringPart( wchar_t* string, int start, int length )
{
    // so heres some fun shit
    // if you call this again with a length less than or equal to the length in the previous call
    // then you get the extra part of the previous string appended onto it
    // idk im too tired to deal with this fucking language right now
    char parser_str[10];
    for(int i=0;i<length;++i)
        parser_str[i] = string[i+start]; 
    return (int)strtol(parser_str, NULL, 16);
}

bool add_cheat(wchar_t* cheat_str)
{
   int len = wcslen(cheat_str);
   
    // Simple cheat format addr=val
    if ( len == 7 && cheat_str[4] == '=' ) {
        // validate string - should be hex digits
        for(int j=0;j<7;++j) {
            if(j != 4 && wcschr(gg_cheat_values,cheat_str[j]) == NULL) {
                return false;
            }
        }
        // parse the string
        int newval = parseStringPart( cheat_str, 5, 2 );
        int addr = parseStringPart( cheat_str, 0, 4 );
        // regenerate the cheat name from the parsed values - just in case it fucked up somehow
        wchar_t dx_message[50];
        wsprintf(dx_message,L"%04X=%02X",addr,newval);;
        cheat_str = dx_message;
        // populate the cheats list
        cheat[number_of_cheats].address = addr;
        cheat[number_of_cheats].new_value = newval;
        wcscpy(cheat[number_of_cheats].str,cheat_str);
        ++number_of_cheats;
        return true;
    }
      
   if((len != 11) && (len != 7))
      return false;
   
   if(len == 11)
   {
      for(int j=0;j<11;++j)
         if(j != 3 && j != 7)
            if(wcschr(gg_cheat_values,cheat_str[j]) == NULL)
               return false;
   
      if((cheat_str[3] != '-') || (cheat_str[7] != '-'))
         return false;
      
      if(number_of_cheats == MAXGGCHEATS)
         return false;
         
      cheat[number_of_cheats].long_code = 1;

      wcscpy(cheat[number_of_cheats].str,cheat_str);
      
      int nums[3];
      char parser_str[4];

      for(int i=0;i<3;++i)
         parser_str[i] = cheat_str[i];
      nums[0] = (int)strtol(parser_str, NULL, 16);
      
      for(int i=0;i<3;++i)
         parser_str[i] = cheat_str[i+4];
      nums[1] = (int)strtol(parser_str, NULL, 16);
      
      for(int i=0;i<3;++i)
         parser_str[i] = cheat_str[i+8];
      nums[2] = (int)strtol(parser_str, NULL, 16);
      
      cheat[number_of_cheats].new_value = ((nums[0]&0xFF0)>>4);
      cheat[number_of_cheats].address = (((nums[1]&0x00F)<<12)|((nums[0]&0x00F)<<8)|((nums[1]&0xFF0)>>4))^0xF000;
      cheat[number_of_cheats].old_value = rol_l((((nums[2]&0xF00)>>4)|(nums[2]&0x00F)),6)^0xBA; 
                                          // ?,but it seems to work
      ++number_of_cheats;
      
   } else
   {
      for(int j=0;j<7;++j)
         if(j != 3)
            if(wcschr(gg_cheat_values,cheat_str[j]) == NULL)
               return false;
   
      if(cheat_str[3] != '-')
         return false;
      
      if(number_of_cheats == MAXGGCHEATS)
         return false;
      
      cheat[number_of_cheats].long_code = 0;
      
      wcscpy(cheat[number_of_cheats].str,cheat_str);
      
      int nums[2];
      char parser_str[4];

      for(int i=0;i<3;++i)
         parser_str[i] = cheat_str[i];
      nums[0] = (int)strtol(parser_str, NULL, 16);
      
      for(int i=0;i<3;++i)
         parser_str[i] = cheat_str[i+4];
      nums[1] = (int)strtol(parser_str, NULL, 16);

      cheat[number_of_cheats].new_value = ((nums[0]&0xFF0)>>4);
      cheat[number_of_cheats].address = (((nums[1]&0x00F)<<12)|((nums[0]&0x00F)<<8)|((nums[1]&0xFF0)>>4))^0xF000;

      ++number_of_cheats;   
   }

   return true;
}

void remove_cheat(int index)
{
   if(!number_of_cheats)
      return;
      
   index += 1;
      
   for(int k=index; k < MAXGGCHEATS; k++)
      cheat[k-1] = cheat[k];
         
   --number_of_cheats;
}

