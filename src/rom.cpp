/*
   hhugboy Game Boy emulator
   copyright 2013 taizou

   Based on GEST
   Copyright (C) 2003-2010 TM
   This file incorporates code from VisualBoyAdvance
   Copyright (C) 1999-2004 by Forgotten

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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "ui/strings.h"

#include "config.h"
#include "rom.h"
#include "debug.h"
#include "cpu.h"
#include "mainloop.h"
#include "zlib/unzip.h"

#include "GB.h"
#include "memory/GB_MBC.h"
#include "memory/CartDetection.h"

int get_size(int real_size) // return good size to use
{
   if(real_size < 64*1024)
      return 64*1024;
   if(real_size < 128*1024)
      return 128*1024;
   else
   if(real_size < 256*1024)
      return 256*1024;
   else
   if(real_size < 512*1024)
      return 512*1024;
   else
   if(real_size < 1024*1024)
      return 1024*1024;
   else
   if(real_size < 2048*1024)
      return 2048*1024;   
   else
   if(real_size < 4096*1024)
      return 4096*1024;
   else
   if(real_size < 8192*1024)
      return 8192*1024;
   else
      return real_size;                                 
}

bool gb_system::loadrom_zip(const wchar_t* filename)
{

   char mFilename[ROM_FILENAME_SIZE];
   wcstombs(mFilename,filename,ROM_FILENAME_SIZE);
   unzFile unz = unzOpen(mFilename);
   if(unz == NULL)
   {
      debug_print(str_table[ERROR_OPEN_FILE]);
      return false;
   }
   
   int r = unzGoToFirstFile(unz);
    
   if(r != UNZ_OK) 
   {
      unzClose(unz);
      debug_print(str_table[ERROR_FIND_FILE]);
      return false;
   }
   
  bool found = false;
    
  unz_file_info info;
  char buffer_file_name[300];
  
   while(true) 
   {
      r = unzGetCurrentFileInfo(unz,&info,buffer_file_name,sizeof(buffer_file_name),NULL,0,NULL,0);
      
      if(r != UNZ_OK) 
      {
         unzClose(unz);
         debug_print(str_table[ERROR_BAD_ZIP]);
         return false;
      }

      if(strstr(buffer_file_name,".gb") || strstr(buffer_file_name,".GB") || strstr(buffer_file_name,".SGB") || strstr(buffer_file_name,".sgb")) 
      {       
         found = true;
         break;
      }
    
      r = unzGoToNextFile(unz);
      
      if(r != UNZ_OK)
         break;
   }

   if(!found) 
   {
      unzClose(unz);
      debug_print(str_table[ERROR_FIND_ROM]);
      return false;
   }

   romFileSize = info.uncompressed_size;
    
   r = unzOpenCurrentFile(unz);

   if(r != UNZ_OK) 
   {
      unzClose(unz);
      debug_print(str_table[ERROR_OPEN_ZIP_FILE]);
      return false;
   }
   
   if(cartROM != NULL)
   { 
      delete [] cartROM;
      cartROM = NULL;
   }
      
   cartROM = new byte[get_size(romFileSize)];
   if(cartROM == NULL)
   { 
      unzCloseCurrentFile(unz);
      unzClose(unz);         
      debug_print(str_table[ERROR_MEMORY]); 
      return false; 
   }

   if(romFileSize != unzReadCurrentFile(unz,cartROM,romFileSize))
   {
      debug_print(str_table[ERROR_READ_FILE]);
      delete [] cartROM;
      cartROM = NULL;
      return false;
   }
   
   unzCloseCurrentFile(unz);
   unzClose(unz);

   return true;
}

bool gb_system::loadrom_file(const wchar_t* filename,int offset)
{
        
   struct _stat file_stat;


   byte rominfo[30];

   FILE* romfile = _wfopen(filename,L"rb");
   if(!romfile) 
   { 
      debug_print(str_table[ERROR_OPEN_FILE]); 
      return false; 
   }
  
  
   // Go to start of ROM info area
   if(fseek(romfile,offset+0x0134,SEEK_SET))
   { 
      debug_print(str_table[ERROR_FSEEK]); 
      fclose(romfile);
      return false; 
   }
               
   // Read internal ROM information area...
   if(fread(rominfo,1,0x1C,romfile)<0x1C)
   { 
      debug_print(str_table[ERROR_READ_ROM_INFO]); 
      fclose(romfile);
      return false; 
   }  

   romFileSize = 0;

   if(_wstat(filename,&file_stat) == 0) {
       romFileSize = file_stat.st_size;
   } else {
       debug_print("Couldn't read file info");
       fclose(romfile);
       return false;
       // If _wstat failed this used to try to fake the file size based on the ROM header
       // e.g. file_size = 16384 * thisromsize;
       // But honestly if we can't read the file size at this point we are probably pretty fucked
   }

   // ...now load the rom into memory
   rewind(romfile);  // go to beginning
   fseek(romfile,offset,SEEK_SET);
   
   if(cartROM != NULL)
   { 
      delete [] cartROM;
      cartROM=NULL;
   }
   cartROM = new byte[get_size(romFileSize)];
   if(cartROM == NULL)
   { 
      debug_print(str_table[ERROR_MEMORY]); 
      fclose(romfile);
      return false; 
   }
   
   if((int)fread(cartROM,1,romFileSize-offset,romfile) < romFileSize-offset)
   { 
      //if(rom->ROMsize)
         debug_print(str_table[ERROR_READ_ROM_TO_MEMORY]); 
      //don't care...
   }    


   fclose(romfile);

    return true;
}

bool gb_system::load_rom(const wchar_t* filename,int offset)
{
    memset(rom_filename,0,ROM_FILENAME_SIZE);

    romloaded = false;

    if (wcsstr(filename, L".zip") || wcsstr(filename, L".ZIP"))
        romloaded = loadrom_zip(filename);
    else
    romloaded = loadrom_file(filename,offset);

    if ( !romloaded ) return false;

    (new CartDetection())->processRomInfo(cartROM, cartridge, romFileSize);
    mbc->setMemoryReadWrite(cartridge->mbcType);

    wchar_t temp2[ROM_FILENAME_SIZE];

    // Get filename
    wchar_t* temp = wcsrchr(filename,L'\\'); // find last '\'
    if(temp == NULL)
    {
        wcscpy(temp2,filename);

        temp = temp2;
    }
    else
        temp += 1;

    wchar_t* temp3 = wcsrchr(temp,L'.'); // find last '.'
    if(temp3 == NULL) // no extension
        wcsncpy(rom_filename,temp,wcslen(temp));
    else
        wcsncpy(rom_filename,temp,wcslen(temp)-wcslen(temp3));



    // this really is a bad place for this
    if ( options->recent_rom_names[0] == filename ) {
        // well thats just fine
    } else {
        int filepos=66;
        for (int x=9;x>0;x--){
            if ( options->recent_rom_names[x] == filename ) {
                filepos = x;
                break;
            }
        }
        for (int x=9;x>0;x--){
            if (x > filepos)
                options->recent_rom_names[x] = options->recent_rom_names[x];
            else
                options->recent_rom_names[x] = options->recent_rom_names[x-1];
        }
        options->recent_rom_names[0] = filename;
        init_menu_options();
    }



    return true;
}


