/*
   unGEST Game Boy emulator
   copyright 2013 taizou

   Based on GEST
   Copyright (C) 2003-2010 TM
   This file incorporates code from VisualBoyAdvance
   Copyright (C) 1999-2004 by Forgotten

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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "strings.h"

#include "config.h"
#include "rom.h"
#include "debug.h"
#include "cpu.h"
#include "mainloop.h"
#include ".\zlib\unzip.h"

#include "GB.h"

void gb_system::crdtype(byte value,byte romsize,byte ramsize)
{
   if((romsize > 8 && romsize < 0x52) || romsize > 0x54) 
      romsize = 0;
   rom->ROMsize = romsize;
   rom->RAMsize = ramsize;
   if(rom->RAMsize >= 8)
      rom->RAMsize = 1;
   rom->RTC = false;
   rom->rumble = false;
   rom->carttype = value;

   memory_read = MEMORY_DEFAULT;
   memory_write = MEMORY_DEFAULT;
   
   switch(value) 
   {
   case 0x00: //"ROM"
       rom->battery = false;
       rom->bankType = ROM;
   break;
   
   case 0x01: //"MBC1"
       rom->battery = false;
       rom->bankType = MBC1;
       memory_write = MEMORY_MBC1;
   break;
   
   case 0x02: //"MBC1+RAM"
       rom->battery = false;
       rom->bankType = MBC1;
       memory_write = MEMORY_MBC1;
   break;

   case 0x03: //"MBC1+RAM+BATTERY"
       rom->battery = true;
       rom->bankType = MBC1;
       memory_write = MEMORY_MBC1;
   break;

   case 0x05: //"MBC2"
       rom->battery = false;
       rom->bankType = MBC2;
       memory_write = MEMORY_MBC2;
   break;

   case 0x06: //"MBC2+BATTERY"
       rom->battery = true;
       rom->bankType = MBC2;
       memory_write = MEMORY_MBC2;
   break;

   case 0x08: //"ROM+RAM";
       rom->battery = false;
       rom->bankType = ROM;
   break;

   case 0x09: //"ROM+RAM+BATTERY"
       rom->battery = true;
       rom->bankType = ROM;
   break;

   case 0x0B: //"MMM01"
       rom->battery = false;
       rom->bankType = MMM01;
       memory_write = MEMORY_MMM01;
   break;

   case 0x0C: //"MMM01+RAM"
       rom->battery = false;
       rom->bankType = MMM01;
       memory_write = MEMORY_MMM01;
   break;

   case 0x0D: //"MMM01+RAM+BATTERY"
       rom->battery = true;
       rom->bankType = MMM01;
       memory_write = MEMORY_MMM01;
   break;

   case 0x0F: //"MBC3+TIMER+BATTERY"
       rom->battery = true;
       rom->bankType = MBC3;
       rom->RTC = true;
       memory_write = MEMORY_MBC3;
       memory_read = MEMORY_MBC3;
   break;

   case 0x10: //"MBC3+TIMER+RAM+BATTERY"
       rom->battery = true;
       rom->bankType = MBC3;
       rom->RTC = true;
       memory_write = MEMORY_MBC3;
       memory_read = MEMORY_MBC3;
   break;

   case 0x11: //"MBC3"
       rom->battery = false;
       rom->bankType = MBC3;
       memory_write = MEMORY_MBC3;
       memory_read = MEMORY_MBC3;
   break;

   case 0x12: //"MBC3+RAM"
       rom->battery = false;
       rom->bankType = MBC3;
       memory_write = MEMORY_MBC3;
       memory_read = MEMORY_MBC3;
   break;

   case 0x13: //"MBC3+RAM+BATTERY"
       rom->battery = true;
       rom->bankType = MBC3;
       memory_write = MEMORY_MBC3;
       memory_read = MEMORY_MBC3;
   break;

   case 0x15: //"MBC4"
       rom->battery = false;
       rom->bankType = MBC4;
   break;

   case 0x16: //"MBC4+RAM"
       rom->battery = false;
       rom->bankType = MBC4;
   break;

   case 0x17: //"MBC4+RAM+BATTERY"
       rom->battery = true;
       rom->bankType = MBC4;
   break;

   case 0x19: //"MBC5"
       rom->battery = false;
       rom->bankType = MBC5;
       memory_write = MEMORY_MBC5;
   break;

   case 0x1A: //"MBC5+RAM"
       rom->battery = false;
       rom->bankType = MBC5;
       memory_write = MEMORY_MBC5;
   break;

   case 0x1B: //"MBC5+RAM+BATTERY"
       rom->battery = true;
       rom->bankType = MBC5;
       memory_write = MEMORY_MBC5;
   break;

   case 0x1C: //"MBC5+RUMBLE"
       rom->battery = false;
       rom->bankType = MBC5;
       rom->rumble = true;
       memory_write = MEMORY_MBC5;
   break;

   case 0x1D: //"MBC5+RUMBLE+RAM"
       rom->battery = false;
       rom->bankType = MBC5;
       rom->rumble = true;
       memory_write = MEMORY_MBC5;
   break;

   case 0x1E: //"MBC5+RUMBLE+RAM+BATTERY"
       rom->battery = true;
       rom->bankType = MBC5;
       rom->rumble = true;
       memory_write = MEMORY_MBC5;
   break;

   case 0x22: //Kirby's Tilt'n'Tumble
       rom->battery = true;
       rom->bankType = MBC7;
       memory_write = MEMORY_MBC7;
       memory_read = MEMORY_MBC7;
   break;

   case 0x59: //Game Boy Smart Card
       rom->battery = false;
       rom->bankType = MBC1;
       memory_write = MEMORY_MBC1;
   break;

   case 0xBE: //Pocket Voice Recorder
       rom->battery = false;
       rom->ROMsize++;
       rom->bankType = MBC5;
       memory_write = MEMORY_MBC5;
   break;

   case 0xEA: //SONIC5
       rom->battery = false;
       rom->bankType = MBC1;
       memory_write = MEMORY_MBC1;
   break;

   case 0xFC: //"POCKET CAMERA"
       rom->battery = true;
       rom->bankType = Camera;
       memory_write = MEMORY_CAMERA;
       memory_read = MEMORY_CAMERA;
   break;

   case 0xFD: //"Bandai TAMA5"
       rom->battery = true;
       rom->bankType = TAMA5;
       memory_write = MEMORY_TAMA5;
       memory_read = MEMORY_TAMA5;
   break;

   case 0xFE: //"Hudson HuC-3"
       rom->battery = true;
       rom->bankType = HuC3;
       memory_write = MEMORY_HUC3;
       memory_read = MEMORY_HUC3;
   break;

   case 0xFF: //"Hudson HuC-1+RAM+BATTERY"
       rom->battery = true;
       rom->bankType = HuC1;
       memory_write = MEMORY_MBC1;
   break;

   default: //"Unknown"
       rom->battery = false;
       rom->bankType = UnknownCart;
       debug_print(str_table[ERROR_ROM_TYPE]);
   break;
   }
}

int gb_system::process_rom_info(byte *rominfo,byte *logo1, byte *logo2)
{

   int addr = 0;
   byte data;
   for(;addr<=14; ++addr)
      rom->name[addr] = rominfo[addr];
      
   int thisromsize = 0;

   if(rominfo[addr] == 0x80) 
      rom->CGB = 1; 
   else if(rominfo[addr] == 0xC0)
      rom->CGB = 2; // gbc only
   else
      rom->CGB = 0;
   // modes changed when reset
          
   ++addr; rom->newlic = (rominfo[addr]<<8);
   ++addr; rom->newlic |= rominfo[addr];
   ++addr; 
   if(rominfo[addr] == 0x03) 
      rom->SGB = 1; 
   else 
      rom->SGB = 0;
      
   ++addr; crdtype(rominfo[addr],data=rominfo[addr+1],rominfo[addr+2]);
   
   bool useNiutoude = false;
   bool useSintax = false;
   
   sintax_currentxor = 0; // reset this, otherwise it leaks over into other roms
   sintax_mode = 0;
   sintax_xor2 = sintax_xor3 = sintax_xor4 = sintax_xor5 = 0;
   
   switch(options->unl_compat_mode) {
   	
   		case UNL_AUTO: {
   			//int ts1= 0;
			//for(int lb=0;lb<0x30;++lb) {
			//	ts1+=logo1[lb];
			//}
   			int ts2= 0;
			for(int lb=0;lb<0x30;++lb) {
				ts2+=logo2[lb];
			}
			//char buff[1000];
			//sprintf(buff,"%d %d",ts1,ts2);
			//debug_print(buff);
   		   	// 4876 = "niutoude"
   		   	// 4125 = Sintax "Kwichvu" (corrupted Nintendo)
   		   	// 4138 = Slight variation on Sintax, seen in Sintax Harry.
		   	// 5152= odd logo from Digimon Fight
		  	// 3746 = not a logo at all; data from Cap vs SNK (its logo is at 0x0904 instead)
	      	if ( ts2 == 4876 ) {
	      		useNiutoude = true;
	      	}
	      	
	      	switch ( ts2 ) {
	      		case 4876:
	      			useNiutoude = true;
	      		break;
	      		case 4138:
	      		case 4125:
	      			useSintax = true;
	      		break;
	      	}
   			break;
   		}

   		break;
   		case UNL_NIUTOUDE:
   			useNiutoude = true;
   		break;
   		case UNL_SINTAX:
   			useSintax = true;
   		break;
   		case UNL_NONE: default:
   			
   		break;
   	
   }
   

	if (useNiutoude) {

   	    //debug_print("Niutoude enabled");
		rom->battery = true;
		rom->bankType = MBC5;
		memory_write = MEMORY_NIUTOUDE;
		rom->ROMsize=07; // assumption for now
		rom->RAMsize=03; // assumption for now; Sango5 doesnt work with smaller
		rom->carttype=0x1B;
   } else if (useSintax) {
   		rom->battery = true;
		rom->bankType = MBC5;
		memory_read = MEMORY_SINTAX;
   		memory_write = MEMORY_SINTAX;
   		rom->ROMsize=07; // assumption for now
   		rom->RAMsize=03; // assumption for now
   		rom->carttype=0x1B; // same
   }
      
   if(!strcmp(rom->name,"GB SMART CARD"))
   {
      data = 0;
      rom->ROMsize = 0;    
   }     
   
   // BHGOS MultiCart
   if(!strcmp(rom->name,"MultiCart"))
   {
      memory_read = MEMORY_DEFAULT;
      memory_write = MEMORY_DEFAULT;

      rom->RAMsize = 3;
      rom->ROMsize = 2;
   }
   
   switch(data)
   {
      case 0: thisromsize=2; break;
      case 1: thisromsize=4; break;
      case 2: thisromsize=8; break;  
      case 3: thisromsize=16; break; 
      case 4: thisromsize=32; break;  
      case 5: thisromsize=64; break; 
      case 6: thisromsize=128; break;
      case 7: thisromsize=256; break;
      case 8: thisromsize=512; break;
      case 0x52: thisromsize=72; break;
      case 0x53: thisromsize=80; break;
      case 0x54: thisromsize=96; break;
      default: thisromsize=2; debug_print(str_table[ERROR_ROM_SIZE]); break;
   }

   char ball_name[16] = { 0x42,0x61,0x6C,0x6C,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x80 };    
   // Ball (Bung)(PD)[C] and Capman (Lik-Sang)(PD)[C] and Fix & Foxi [C][t1]
   if(!strcmp(rom->name,ball_name) || strstr(rom->name,"CAPMAN") || !strcmp(rom->name,"LUPO +3HI"))
   {
      memory_write = MEMORY_DEFAULT;
   }
   else    
   //Bugs Bunny - Crazy Castle 3 (J)[C][t2]
   if(!strcmp(rom->name,"BUGS CC3 CRACK"))
   {
      memory_write = MEMORY_DEFAULT;
      rom->RAMsize=1;
   }
   else
   //Pulsar (Freedom GB Contest 2001)(PD)[C]
   if(strstr(rom->name,"PULSAR"))
   {
      rom->bankType=MBC5;
   }
   else
   //Pokemon Red-Blue 2-in-1 (Unl)[S][h1]
   if(!strcmp(rom->name,"POKEMON RED") && rom->ROMsize == 6)
   {
      rom->ROMsize = 7;
      memory_write = MEMORY_POKE;
   }
   else   
   // SGB Pack
   if(!strcmp(rom->name,"SGBPACK"))
   {
      thisromsize = 2;
      rom->ROMsize = 0;
   }
   else        
   // Dragon Ball Z Goku (Chinese)
   if(!strcmp(rom->name,"GB DBZ GOKOU"))  
   {
      rom->carttype = 3;
      rom->battery = true; 
      rom->bankType = MBC1; 
      memory_write = MEMORY_MBC1;
   }
   else       
   // Bokujou Monogatari 3 Chinese
   if(!strcmp(rom->name,"BOKUMONOGB3BWAJ") || !strcmp(rom->name,"BOYGIRLD640BWAJ"))
   {
      thisromsize = 128;
      rom->ROMsize = 6;
   }
   else      
   // Monsters GO!GO!GO!!
   if(!strcmp(rom->name,"POCKET MONSTER"))
   {
      thisromsize = 32;
      rom->ROMsize = 4;
   }
   else      
   // Sonic 3D Blast 5
   if(strstr(rom->name,"SONIC5"))
   {
      thisromsize = 16;
      rom->ROMsize = 3;
      rom->RAMsize = 0;
   }
   else   
   // Rockman 8
   if(!strcmp(rom->name,"ROCKMAN 99"))
      memory_write = MEMORY_ROCKMAN8;
   else
   // Collection Carts
   if(!strcmp(rom->name,"BOMCOL") || !strcmp(rom->name,"GENCOL") || strstr(rom->name,"MOMOCOL") || strstr(rom->name,"SUPERCHINESE 12"))
   {
      memory_write = MEMORY_BC;
      if(strstr(rom->name,"MOMOCOL2"))
         memory_write = MEMORY_MMM01;
   }
   else
   if(!strcmp(rom->name,"MORTALKOMBATI&I"))
   {
      memory_write = MEMORY_MK12;
   }
   else
   // Gameboy Camera
   if(!strcmp(rom->name,"GAMEBOYCAMERA"))
   {
      rom->bankType = Camera;
      thisromsize = 64;
      rom->ROMsize = 5;
      rom->RAMsize = 4;
      memory_write = MEMORY_CAMERA;
      memory_read = MEMORY_CAMERA;
   }
      
   addr+=3; rom->destcode = rominfo[addr];
   ++addr; rom->lic = rominfo[addr];
   ++addr; rom->version = rominfo[addr];
   ++addr; rom->complement = rominfo[addr];
   ++addr; rom->checksum=(rominfo[addr]<<8);
   ++addr; rom->checksum|=rominfo[addr];

   // Digimon 3 saving
   if(!strcmp(rom->name,"DIGIMON") && rom->checksum == 0xE11B)
      rom->battery = true;
   else   
   // Joust & Defender (U)[C][t1]
   if(!strcmp(rom->name,"DEFENDER/JOUST") && rom->checksum == 0xB110)
      rom->RAMsize = 1;
           
   // check complement
   byte cmpl=0;
   for(addr=0;addr<=0x19;++addr) 
      cmpl+=rominfo[addr];
   cmpl+=25; rom->complementok = !cmpl;
   // done processing info...

   return thisromsize;
}

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
	
   memset(rom_filename,0,ROM_FILENAME_SIZE);  

   wchar_t temp2[ROM_FILENAME_SIZE];

   wchar_t* temp = wcsrchr(filename,'\\'); // find last '\'
   if(temp == NULL)
   {
      wcscpy(temp2,filename);
      
      temp = temp2;
   }
   else
      temp += 1;
      
   wcsncpy(rom_filename,temp,wcslen(temp)-4);

   byte rominfo[30];  
   
   romloaded = false;
   
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
  
   int size = info.uncompressed_size;
    
   r = unzOpenCurrentFile(unz);

   if(r != UNZ_OK) 
   {
      unzClose(unz);
      debug_print(str_table[ERROR_OPEN_ZIP_FILE]);
      return false;
   }
   
   if(cartridge != NULL) 
   { 
      delete [] cartridge; 
      cartridge = NULL; 
   }
      
   cartridge = new byte[get_size(size)];
   if(cartridge == NULL) 
   { 
      unzCloseCurrentFile(unz);
      unzClose(unz);         
      debug_print(str_table[ERROR_MEMORY]); 
      return false; 
   }

   if(size != unzReadCurrentFile(unz,cartridge,size)) 
   {
      debug_print(str_table[ERROR_READ_FILE]);
      delete [] cartridge;
      cartridge = NULL;
      return false;
   }
   memcpy(rominfo,cartridge+0x0134,0x1C);
   
   byte logo1[0x30];
   byte logo2[0x30];
   memcpy(logo1,cartridge+0x0104,0x30); // Real logo
   memcpy(logo2,cartridge+0x0184,0x30); // Unlicensed game's logo. Sometimes.
   
   unzCloseCurrentFile(unz);
   unzClose(unz);
  
   process_rom_info(rominfo,logo1,logo2);
   
   
   
   if(!strcmp(rom->name,"TETRIS") && size > 32768 && rom->ROMsize==0)
   {
      memory_write = MEMORY_MBC1;
      rom->ROMsize = 2;
   } else
   if(!strcmp(rom->name,"\0") && size > 32768 && rom->ROMsize==0)
   {
      memory_write = MEMORY_8IN1;
      rom->ROMsize = 4;
   } else
   if(strstr(rom->name,"TUWAMONO") && size == 524288) // Joryu Janshi Ni Chousen (J)[C] (bad)
   {
      debug_print("Bad dump!");
      rom->ROMsize--;
   }  else      
   if(!strcmp(rom->name,"SGBPACK") && size > 32768)
   {
      rom->ROMsize = 6;
      memory_write = MEMORY_POKE;
   } else     
   if(size == 262144 && rom->ROMsize == 4)
      rom->ROMsize--;
      
   romloaded = true;

   return true;
}

bool gb_system::load_rom(const wchar_t* filename)
{    
   // todo: Reinstate later... 
   if(wcsstr(filename,L".zip") || wcsstr(filename,L".ZIP"))
      return loadrom_zip(filename);
        
   struct stat file_stat;

   memset(rom_filename,0,ROM_FILENAME_SIZE);
   byte rominfo[30];

   romloaded = false;
   FILE* romfile = _wfopen(filename,L"rb");
   if(!romfile) 
   { 
      debug_print(str_table[ERROR_OPEN_FILE]); 
      return false; 
   }
  
  
   // Go to start of ROM info area
   if(fseek(romfile,0x0134,SEEK_SET))
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
   
   byte logo1[0x30];
   byte logo2[0x30];
   fseek(romfile,0x0104,SEEK_SET);
   fread(logo1,1,0x30,romfile);
   fseek(romfile,0x0184,SEEK_SET);
   fread(logo2,1,0x30,romfile);
   
   // ...process it...
   int thisromsize = process_rom_info(rominfo,logo1,logo2);
   
   int file_size = 0;
   
   if(wstat(filename,&file_stat) == 0)
   {
      file_size = file_stat.st_size;
      
      // Captain Knick Knack
      if(!strcmp(rom->name,"TETRIS") && file_size > 32768 && rom->ROMsize == 0)
      {
         memory_write = MEMORY_MBC1;
         rom->ROMsize = 2;
      } else
      if(!strcmp(rom->name,"\0") && file_size > 32768 && rom->ROMsize == 0)
      {
         memory_write = MEMORY_8IN1;
         rom->ROMsize = 4;
      } else
      if(strstr(rom->name,"TUWAMONO") && file_size == 524288) // Joryu Janshi Ni Chousen (J)[C]
      {
         debug_print("Bad dump!");
         rom->ROMsize--;
      } else      
      if(!strcmp(rom->name,"SGBPACK") && file_size > 32768)
      {
         rom->ROMsize = 6;
         memory_write = MEMORY_POKE;
      }
      if(file_size == 262144 && rom->ROMsize == 4)
         rom->ROMsize--;
   }
   else // failed ?
      file_size = 16384 * thisromsize;
   
   // ...now load the rom into memory
   rewind(romfile);  // go to beginning
   
   if(cartridge != NULL) 
   { 
      delete [] cartridge; 
      cartridge=NULL; 
   }
   cartridge = new byte[get_size(file_size)];
   if(cartridge == NULL) 
   { 
      debug_print(str_table[ERROR_MEMORY]); 
      fclose(romfile);
      return false; 
   }
   if((int)fread(cartridge,1,file_size,romfile) < file_size)
   { 
      if(rom->ROMsize)
         debug_print(str_table[ERROR_READ_ROM_TO_MEMORY]); 
      //don't care...
   }    

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

   romloaded = true;
   fclose(romfile);
     
   return true;
}


