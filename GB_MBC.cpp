/*
GEST - Gameboy emulator
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
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
/*
    VisualBoyAdvance - a Gameboy and GameboyAdvance emulator
    Copyright (C) 1999-2004 by Forgotten

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#include "mainloop.h"
#include <time.h>

#include "GB.h"

#include "debug.h"
#include "zlib/zconf.h"
#include <stdio.h>
int maxROMbank[9] =
{
 1, 3, 7, 15, 31, 63, 127, 255, 511
}; 

int maxRAMbank[6] =
{
 0, 0, 0, 4, 15, 7
};

int rom_size_mask[9] = 
{ 
   0x00007fff,
   0x0000ffff,
   0x0001ffff,
   0x0003ffff,
   0x0007ffff,
   0x000fffff,
   0x001fffff,
   0x003fffff,
   0x007fffff
};

unsigned int cart_address = 0;

//int RTCIO = 0;
//int RTC_latched = 0;

byte gb_system::readmemory(unsigned short address)
{
     if(number_of_cheats)
        for(int i=0;i<number_of_cheats;++i)
           if(address == cheat[i].address && (!(cheat[i].long_code) || (cheat[i].old_value == mem_map[address>>12][address&0x0fff])))
              return cheat[i].new_value;

     switch(memory_read)
     {
         case MEMORY_MBC3:
             return readmemory_MBC3(address);
         case MEMORY_CAMERA:
             return readmemory_Camera(address);
         case MEMORY_MBC7:
             return readmemory_MBC7(address);
         case MEMORY_HUC3:
             return readmemory_HuC3(address);
         case MEMORY_TAMA5:
             return readmemory_TAMA5(address);
    	 case MEMORY_SINTAX:
    	 	return readmemory_sintax(address);
         default:
         case MEMORY_DEFAULT:
             return readmemory_default(address);
     }
}

void gb_system::writememory(unsigned short address,byte data)
{
     switch(memory_write)
     {
         case MEMORY_MBC1:
             writememory_MBC1(address,data);
         break;

         case MEMORY_MBC2:
             writememory_MBC2(address,data);
         break;

         case MEMORY_MBC3:
             writememory_MBC3(address,data);
         break;

         case MEMORY_MBC5:
             writememory_MBC5(address,data,false,false);
         break;

         case MEMORY_MBC7:
             writememory_MBC7(address,data);
         break;

         case MEMORY_CAMERA:
             writememory_Camera(address,data);
         break;

         case MEMORY_HUC3:
             writememory_HuC3(address,data);
         break;

         case MEMORY_TAMA5:
             writememory_TAMA5(address,data);
         break;

         case MEMORY_ROCKMAN8:
             writememory_Rockman8(address,data);
         break;

         case MEMORY_BC:
             writememory_BC(address,data);
         break;

         case MEMORY_MMM01:
             writememory_MMM01(address,data);
         break;

         case MEMORY_POKE:
             writememory_poke(address,data);
         break;

         case MEMORY_8IN1:
             writememory_8in1(address,data);
         break;

         case MEMORY_MK12:
             writememory_MK12(address,data);
         break;

		 case MEMORY_NIUTOUDE:
		 	 writememory_MBC5(address,data,true,false);
		 break;
		 
		 case MEMORY_SINTAX:
		 	 writememory_MBC5(address,data,false,true);
		 break;

         default:
         case MEMORY_DEFAULT:
             writememory_default(address,data);
         break;
     }
}

void gb_system::setXorForBank(byte bankNo)
{
  	switch(bankNo & 0x0F) {
	case 0x00: case 0x04: case 0x08: case 0x0C:
		sintax_currentxor = sintax_xor2;
		break;
	case 0x01: case 0x05: case 0x09: case 0x0D:
		sintax_currentxor = sintax_xor3;
		break;
	case 0x02: case 0x06: case 0x0A: case 0x0E:
		sintax_currentxor = sintax_xor4;
		break;
	case 0x03: case 0x07: case 0x0B: case 0x0F:
		sintax_currentxor = sintax_xor5;
		break;
  	}
  	
  	//char buff[200];
  //	sprintf(buff,"bank no %x abbr %x xor %x",bankNo,bankNo&0x0F,sintax_currentxor);
  //	debug_print(buff);
}

//-------------------------------------------------------------------------
// readmemory_default:
// for most MBCs
//-------------------------------------------------------------------------
byte gb_system::readmemory_default(register unsigned short address)
{            
  /* if(address >= 0xA000 && address < 0xC000)
   {
      if((!rom->RAMsize && !rom->battery))
         return 0xFF; 
   }*/

   return io_reg_read(address);
}

//-------------------------------------------------------------------------
// readmemory_sintax:
// for SiNTAX
//-------------------------------------------------------------------------
byte gb_system::readmemory_sintax(register unsigned short address)
{            

   if(address >= 0x4000 && address < 0x8000)
   {
   	byte data = io_reg_read(address);

   	
   	//char buff[100];
	//sprintf(buff,"MBCLo %X Addr %X Data %X XOR %X XOR'd data %X",MBClo,address,data,sintax_currentxor, data ^ sintax_currentxor);
	//debug_print(buff);
   	
     return  data ^ sintax_currentxor;
   }

   return io_reg_read(address);
}


//-------------------------------------------------------------------------
// readmemory_MBC3:
// for MBC3
//-------------------------------------------------------------------------
byte gb_system::readmemory_MBC3(register unsigned short address)
{ 
   if(address >= 0xA000 && address < 0xC000)
   {
      if(rom->RTC && RTCIO)
      {
         switch(rtc.cur_register)
         {
            case 0x08:
               return rtc_latch.s;
            case 0x09:
               return rtc_latch.m;                    
            case 0x0A:
               return rtc_latch.h;                       
            case 0x0B:
               return (rtc_latch.d&0xFF);                       
            case 0x0C:
               return ((rtc_latch.d&0x100)>>8)|rtc_latch.control;                     
         }
      } 
      //if(!rom->RAMsize)
      //   return 0xFF; 
   }

   return io_reg_read(address);
}

//-------------------------------------------------------------------------
// readmemory_Camera:
// for GB Pocket Camera
//-------------------------------------------------------------------------
byte gb_system::readmemory_Camera(register unsigned short address)
{   
   if(address >= 0xA000 && address < 0xC000)
   {
      if(cameraIO) // Camera I/O register in cart RAM area
      {
         if(address == 0xA000)
            return 0x00; // Hardware is ready 
         else
            return 0xFF; // others write only
      }
   }
   
   return io_reg_read(address);
}
      
//-------------------------------------------------------------------------
// writememory_default:
// for ROM only and for undocumented/unknown MBCs
//-------------------------------------------------------------------------
void gb_system::writememory_default(register unsigned short address,register byte data)
{
   if(address < 0x2000)// Is it a RAM bank enable/disable?
   {
      RAMenable = ( (data&0x0A) == 0x0A ? 1 : 0);       
      return;  
   } 
   
   if(address < 0x4000) // Is it a ROM bank switch?
   {  
      if(rom->bankType == ROM)
         return;
      else
      {
         if(data == 0)
            data = 1;
         
         rom_bank = data; 
         
         int cadr = (data<<14)+((MBChi<<1)<<14);
         mem_map[0x4] = &cartridge[cadr];
         mem_map[0x5] = &cartridge[cadr+0x1000];
         mem_map[0x6] = &cartridge[cadr+0x2000];
         mem_map[0x7] = &cartridge[cadr+0x3000];
         return; 
      }
   }

   if(address < 0x6000) // Is it a RAM bank switch?
   {          
      data &= 0x03;
              
      ram_bank = data;
      
      int madr = data<<13;
      mem_map[0xA] = &cartRAM[madr];   
      mem_map[0xB] = &cartRAM[madr+0x1000];
      return;  
   } 
   
   if(address < 0x8000) // BHGOS multicart
   {           
      if(++bc_select == 2 && rom->ROMsize>1) 
      {
         MBChi = (data&0xFF);

         cart_address = (MBChi<<1)<<14;
         
         mem_map[0x0] = &cartridge[cart_address];
         mem_map[0x1] = &cartridge[cart_address+0x1000];
         mem_map[0x2] = &cartridge[cart_address+0x2000];
         mem_map[0x3] = &cartridge[cart_address+0x3000];
         
         mem_map[0x4] = &cartridge[cart_address+0x4000];
         mem_map[0x5] = &cartridge[cart_address+0x5000];
         mem_map[0x6] = &cartridge[cart_address+0x6000];
         mem_map[0x7] = &cartridge[cart_address+0x7000];
      }
      return;      
   }

   // Always allow RAM writes.
   
   if(io_reg_write(address,data)) return;

   mem_map[address>>12][address&0x0FFF] = data;  
}

//-------------------------------------------------------------------------
// writememory_8in1:
// for Sachen 8in1
//-------------------------------------------------------------------------
void gb_system::writememory_8in1(register unsigned short address,register byte data)
{
   if(address < 0x2000)
      return;  
   
   if(address < 0x4000) // Is it a ROM bank switch?
   { 
      data &= 0x1f;
      if(data == 0)
         data = 1;
         
      rom_bank = data; 
         
      int cadr = (data<<14)+(MBChi<<14);
      cadr &= rom_size_mask[rom->ROMsize];      
      mem_map[0x4] = &cartridge[cadr];
      mem_map[0x5] = &cartridge[cadr+0x1000];
      mem_map[0x6] = &cartridge[cadr+0x2000];
      mem_map[0x7] = &cartridge[cadr+0x3000];
      return; 
   }
      
   if(address < 0x6000) // Is it a RAM bank switch?
   {      
      if(address == 0x4000 && bc_select < 3) // game select
      {
         ++bc_select;      
         
         MBClo = 0;
         
         MBChi = (data&0x1f);

         cart_address = MBChi<<14;
         mem_map[0x0] = &cartridge[cart_address];
         mem_map[0x1] = &cartridge[cart_address+0x1000];
         mem_map[0x2] = &cartridge[cart_address+0x2000];
         mem_map[0x3] = &cartridge[cart_address+0x3000];
         
         mem_map[0x4] = &cartridge[cart_address+0x4000];
         mem_map[0x5] = &cartridge[cart_address+0x5000];
         mem_map[0x6] = &cartridge[cart_address+0x6000];
         mem_map[0x7] = &cartridge[cart_address+0x7000];   
         return;      
      }    

      data &= 0x03;
              
      ram_bank = data;
      
      int madr = data<<13;
      mem_map[0xA] = &cartRAM[madr];
      mem_map[0xB] = &cartRAM[madr+0x1000];
      return;  
   } 
   
   if(address < 0x8000)       
      return;      

   // Always allow RAM writes.
   
   if(io_reg_write(address,data)) return;

   mem_map[address>>12][address&0x0FFF] = data;  
}

//-------------------------------------------------------------------------
// writememory_MBC1:
// for MBC1 and HuC1
//-------------------------------------------------------------------------
void gb_system::writememory_MBC1(register unsigned short address,register byte data)
{
   if(address < 0x2000)// Is it a RAM bank enable/disable?
   {
      RAMenable = ( (data&0x0A) == 0x0A ? 1 : 0);
      return;  
   }
   
   if(address < 0x4000) // Is it a ROM bank switch?
   {      
      if(MBC1memorymodel == 0)
      {
         if(data == 0)
            data = 1;

         MBClo = data;
         
         rom_bank = MBClo|(MBChi<<5);
          
         cart_address = MBClo<<14;
         cart_address |= (MBChi<<19);
         
         cart_address &= rom_size_mask[rom->ROMsize];

         mem_map[0x4] = &cartridge[cart_address];
         mem_map[0x5] = &cartridge[cart_address+0x1000];
         mem_map[0x6] = &cartridge[cart_address+0x2000];
         mem_map[0x7] = &cartridge[cart_address+0x3000];         
      } else 
      {
         if(data == 0)
            data = 1;
      
         rom_bank = data;
         
         int cadr = rom_bank<<14;
         
         cadr &= rom_size_mask[rom->ROMsize];
         
         mem_map[0x4] = &cartridge[cadr];
         mem_map[0x5] = &cartridge[cadr+0x1000];
         mem_map[0x6] = &cartridge[cadr+0x2000];
         mem_map[0x7] = &cartridge[cadr+0x3000];
      }
      return;
   }
   
   if(address < 0x6000) // Is it a RAM bank switch?
   {          
      if(MBC1memorymodel == 0)
      {  
         if((((data&0x03)<<5)|MBClo) > maxROMbank[rom->ROMsize]) 
            return;
         
         MBChi = (data&0x03);
         
         rom_bank = MBClo|(MBChi<<5);
         
         cart_address = MBClo<<14;
         cart_address |= (MBChi<<19);
         
         cart_address &= rom_size_mask[rom->ROMsize];

         mem_map[0x4] = &cartridge[cart_address];
         mem_map[0x5] = &cartridge[cart_address+0x1000];
         mem_map[0x6] = &cartridge[cart_address+0x2000];
         mem_map[0x7] = &cartridge[cart_address+0x3000];
         return;
      }

      if(rom->RAMsize <= 2) // no need to change it if there isn't over 8KB ram
         return;

      data &= 0x03;
         
      if(data > maxRAMbank[rom->RAMsize])
         data = maxRAMbank[rom->RAMsize];
      
      ram_bank = data;
      
      int madr = data<<13;
      mem_map[0xA] = &cartRAM[madr];
      mem_map[0xB] = &cartRAM[madr+0x1000];
      return;  
   }
   
   if(address < 0x8000) // Is it a MBC1 max memory model change?
   {  
      MBC1memorymodel = (data&0x01);
      return;
   }
   
/*   if(address >= 0xA000 && address < 0xC000)
   {
      if((!RAMenable || !rom->RAMsize) && !RAM_always_enable)
         return;
   }*/
   
   if(io_reg_write(address,data)) return;
   
   mem_map[address>>12][address&0x0FFF] = data;  
}

//-------------------------------------------------------------------------
// writememory_MMM01: 
// for MMM01 
// Only game with MBC set as MMM01 in header I've seen is
// Momotarou Collection 2, which is propably a bad dump?
//-------------------------------------------------------------------------
void gb_system::writememory_MMM01(register unsigned short address,register byte data)
{
   if(address < 0x2000)
      return;  
   
   if(address < 0x4000) // Is it a ROM bank switch?
   {            
      if(data == 0)
         data = 1;
        
      rom_bank = data;
         
      int cadr = (rom_bank<<14)+(MBChi<<14);
         
      cadr &= rom_size_mask[rom->ROMsize];
         
      mem_map[0x4] = &cartridge[cadr];
      mem_map[0x5] = &cartridge[cadr+0x1000];
      mem_map[0x6] = &cartridge[cadr+0x2000];
      mem_map[0x7] = &cartridge[cadr+0x3000];
      return;
   }
   
   if(address < 0x6000) // Is it a RAM bank switch?
   {                 
      if(address == 0x5fff && !bc_select)
      {
         bc_select = 1;
         
         data &= 0x2f;
         MBClo = 0;
      
         MBChi = data;
         cart_address = MBChi<<14;
         
         mem_map[0x0] = &cartridge[cart_address];
         mem_map[0x1] = &cartridge[cart_address+0x1000];
         mem_map[0x2] = &cartridge[cart_address+0x2000];
         mem_map[0x3] = &cartridge[cart_address+0x3000];
         
         mem_map[0x4] = &cartridge[cart_address+0x4000];
         mem_map[0x5] = &cartridge[cart_address+0x5000];
         mem_map[0x6] = &cartridge[cart_address+0x6000];
         mem_map[0x7] = &cartridge[cart_address+0x7000];   
         return;      
      }
         
      if(rom->RAMsize <= 2) // no need to change it if there isn't over 8KB ram
         return;

      data &= 0x03;
         
      if(data > maxRAMbank[rom->RAMsize])
         data = maxRAMbank[rom->RAMsize];
      
      ram_bank = data;
      
      int madr = data<<13;
      mem_map[0xA] = &cartRAM[madr];
      mem_map[0xB] = &cartRAM[madr+0x1000];
      return;  
   }
   
   if(address < 0x8000) 
      return;
   
   // Always allow RAM writes.
   
   if(io_reg_write(address,data)) return;
   
   mem_map[address>>12][address&0x0FFF] = data;  
}

//-------------------------------------------------------------------------
// writememory_BC:
// for Collection Carts
//-------------------------------------------------------------------------
void gb_system::writememory_BC(register unsigned short address,register byte data)
{
   if(address < 0x2000)// Is it a RAM bank enable/disable?
   {
      RAMenable = ( (data&0x0A) == 0x0A ? 1 : 0);
      return;  
   }
   
   if(address < 0x4000) // Is it a ROM bank switch?
   { 
      data &= 0x0F;
            
      if(data == 0)
         data = 1;
        
      rom_bank = data|(MBChi<<4);
       
      int cadr = rom_bank<<14;
         
      cadr &= rom_size_mask[rom->ROMsize];
         
      mem_map[0x4] = &cartridge[cadr];
      mem_map[0x5] = &cartridge[cadr+0x1000];
      mem_map[0x6] = &cartridge[cadr+0x2000];
      mem_map[0x7] = &cartridge[cadr+0x3000];
      return;
   }
 
   if(address < 0x6000) 
   {                   
      if(address == 0x4000 || address == 0x5fff) // game select
      { 
         MBClo = 0;
         MBChi = (data&0x03);
                  
         cart_address = (MBChi<<4)<<14;
         
         mem_map[0x0] = &cartridge[cart_address];
         mem_map[0x1] = &cartridge[cart_address+0x1000];
         mem_map[0x2] = &cartridge[cart_address+0x2000];
         mem_map[0x3] = &cartridge[cart_address+0x3000];
         
         mem_map[0x4] = &cartridge[cart_address+0x4000];
         mem_map[0x5] = &cartridge[cart_address+0x5000];
         mem_map[0x6] = &cartridge[cart_address+0x6000];
         mem_map[0x7] = &cartridge[cart_address+0x7000];   
         return;      
      }
      return;  
   }
   
   if(address < 0x8000) 
   {       
      MBC1memorymodel = (data&0x01);
      return;
   }
   
  /* if(address >= 0xA000 && address < 0xC000)
   {
      if((!RAMenable || !rom->RAMsize) && !RAM_always_enable)
         return;
   }*/
   
   if(io_reg_write(address,data)) return;
   
   mem_map[address>>12][address&0x0FFF] = data;  
}

//-------------------------------------------------------------------------
// writememory_MK12:
// for Mortal Kombat 1&2
//-------------------------------------------------------------------------
void gb_system::writememory_MK12(register unsigned short address,register byte data)
{
   if(address < 0x2000)// Is it a RAM bank enable/disable?
   {
      RAMenable = ( (data&0x0A) == 0x0A ? 1 : 0);
      return;  
   }
   
   if(address < 0x4000) // Is it a ROM bank switch?
   { 
      data &= 0x0F;
            
      if(data == 0)
         data = 1;
        
      rom_bank = data+MBChi;
       
      int cadr = rom_bank<<14;
        
      cadr &= rom_size_mask[rom->ROMsize];
         
      mem_map[0x4] = &cartridge[cadr];
      mem_map[0x5] = &cartridge[cadr+0x1000];
      mem_map[0x6] = &cartridge[cadr+0x2000];
      mem_map[0x7] = &cartridge[cadr+0x3000];
      return;
   }
 
   if(address < 0x6000) 
   {                   
      if(address == 0x5000) // game select
      {         
         MBClo = 0;
         MBChi = (data&0x03);
         if(MBChi==2) MBChi = 17;
                  
         cart_address = MBChi<<14;

         mem_map[0x0] = &cartridge[cart_address];
         mem_map[0x1] = &cartridge[cart_address+0x1000];
         mem_map[0x2] = &cartridge[cart_address+0x2000];
         mem_map[0x3] = &cartridge[cart_address+0x3000];
         
         mem_map[0x4] = &cartridge[cart_address+0x4000];
         mem_map[0x5] = &cartridge[cart_address+0x5000];
         mem_map[0x6] = &cartridge[cart_address+0x6000];
         mem_map[0x7] = &cartridge[cart_address+0x7000];   
         return;      
      }
      return;  
   }
   
   if(address < 0x8000) 
   {       
      MBC1memorymodel = (data&0x01);
      return;
   }
   
  /* if(address >= 0xA000 && address < 0xC000)
   {
      if((!RAMenable || !rom->RAMsize) && !RAM_always_enable)
         return;
   }*/
   
   if(io_reg_write(address,data)) return;
   
   mem_map[address>>12][address&0x0FFF] = data;  
}
//-------------------------------------------------------------------------
// writememory_Rockman8:
// for Rockman8
// only game that requires memory echo emulation ?
//-------------------------------------------------------------------------
void gb_system::writememory_Rockman8(register unsigned short address,register byte data)
{
   if(address < 0x2000)
      return;  
   
   if(address < 0x4000) // Is it a ROM bank switch?
   { 
      data &= 0x1F;
         
      if(data == 0)
         data = 1;

      if(data > maxROMbank[rom->ROMsize]) 
         data -= 8; // <--- MAKE IT WORK!!!
         
      rom_bank = data;
          
      cart_address = data<<14;
         
      cart_address &= rom_size_mask[rom->ROMsize];

      mem_map[0x4] = &cartridge[cart_address];
      mem_map[0x5] = &cartridge[cart_address+0x1000];
      mem_map[0x6] = &cartridge[cart_address+0x2000];
      mem_map[0x7] = &cartridge[cart_address+0x3000];    
              
      return;
   }
     
   if(address < 0x8000) 
      return;
   
   // Always allow RAM writes.
         
   if(io_reg_write(address,data)) return;
   
   mem_map[address>>12][address&0x0FFF] = data;  
}

//-------------------------------------------------------------------------
// writememory_MBC2:
// for MBC2
//-------------------------------------------------------------------------
void gb_system::writememory_MBC2(register unsigned short address,register byte data)
{
   if(address < 0x2000)// Is it a RAM bank enable/disable?
   {
      if(!(address&0x0100))
         RAMenable =  (data&0x0F) == 0x0A;
      return;  
   }
   
   if(address < 0x4000) // Is it a ROM bank switch?
   { 
      //if(address&0x0100)
      {
         data &= 0x0F;
         if(data==0) 
            data=1;
         if(data > maxROMbank[rom->ROMsize])
            data = maxROMbank[rom->ROMsize];
         
         rom_bank = data;
         
         int cadr = data<<14;
         mem_map[0x4] = &cartridge[cadr];
         mem_map[0x5] = &cartridge[cadr+0x1000];
         mem_map[0x6] = &cartridge[cadr+0x2000];
         mem_map[0x7] = &cartridge[cadr+0x3000];
      }
      return;
   }
   
   if(address < 0x6000) // Is it a RAM bank switch?         
      return;  
   
   if(address < 0x8000)
      return;
   
 /*  if(address >= 0xA000 && address < 0xC000)
   {
      if(!RAMenable || !rom->battery)
         return;
   }*/
   
   if(io_reg_write(address,data)) return;
   
   mem_map[address>>12][address&0x0FFF] = data;  
}

void gb_system::rtc_update()
{
   if(rtc.control&0x40)
   {
      rtc.last_time = time(0);
      return;
   }
   
   time_t now = time(0);
   time_t diff = now-rtc.last_time;
   if(diff > 0)
   {
    rtc.s += diff % 60;
    if(rtc.s > 59) 
    {
      rtc.s -= 60;
      rtc.m++;
    }

    diff /= 60;

    rtc.m += diff % 60;
    if(rtc.m > 59) 
    {
      rtc.m -= 60;
      rtc.h++;
    }

    diff /= 60;

    rtc.h += diff % 24;
    if(rtc.h > 24) 
    {
      rtc.h -= 24;
      rtc.d++;
    }
    diff /= 24;

    rtc.d += diff;
    if(rtc.d > 255) 
    {
      if(rtc.d > 511) 
      {
         rtc.d %= 512;
         rtc.control |= 0x80;
      }
      rtc.control = (rtc.control & 0xfe) | (rtc.d>255 ? 1 : 0);
    }
  }
  rtc.last_time = now;
}

//-------------------------------------------------------------------------
// writememory_poke:
// for Pokemon Red & Blue 2-in-1
//-------------------------------------------------------------------------
int bank0_change = 0;

void gb_system::writememory_poke(register unsigned short address,register byte data)
{
   if(address < 0x2000)// Is it a RAM bank enable/disable?
   {
      RAMenable = ( (data&0x0A) == 0x0A ? 1 : 0);
      bank0_change = ( (data&0xC0) == 0xC0 ? 1 : 0);
      return;  
   }
   
   if(address < 0x4000) // Is it a ROM bank switch?
   {      
      data &= 0x7F;  
      if(data==0) 
         data=1;
      data += MBChi; 
         
      rom_bank = data;
         
      int cadr = data<<14;
      cadr &= rom_size_mask[rom->ROMsize];
      mem_map[0x4] = &cartridge[cadr];
      mem_map[0x5] = &cartridge[cadr+0x1000];
      mem_map[0x6] = &cartridge[cadr+0x2000];
      mem_map[0x7] = &cartridge[cadr+0x3000]; 
      return;
   }
   
   if(address < 0x6000) // Is it a RAM bank switch?
   {                 
      if(rom->RAMsize <= 2) // no need to change it if there isn't over 8KB ram
         return;

      data &= 0x03;
         
      if(data > maxRAMbank[rom->RAMsize])
         data = maxRAMbank[rom->RAMsize];
      
      ram_bank = data;
      
      int madr = data<<13;
      mem_map[0xA] = &cartRAM[madr];
      mem_map[0xB] = &cartRAM[madr+0x1000];
      return;  
   }
   
   if(address < 0x8000)
   {
      return;
   }
   
   if(address >= 0xA000 && address < 0xC000)
   { 
      if(bank0_change && address==0xA100 && !bc_select)
      {
         MBClo = 0;
         if(data==1)
            MBChi = 2;
         else 
         if(data!=0xc0)
            MBChi = 66;
         else
            bc_select = 1;
                  
         cart_address = MBChi<<14;

         mem_map[0x0] = &cartridge[cart_address];
         mem_map[0x1] = &cartridge[cart_address+0x1000];
         mem_map[0x2] = &cartridge[cart_address+0x2000];
         mem_map[0x3] = &cartridge[cart_address+0x3000];
         
         mem_map[0x4] = &cartridge[cart_address+0x4000];
         mem_map[0x5] = &cartridge[cart_address+0x5000];
         mem_map[0x6] = &cartridge[cart_address+0x6000];
         mem_map[0x7] = &cartridge[cart_address+0x7000];          
         return;
      }
      //if(!RAMenable || !rom->RAMsize)
      //   return;
   }
   
   if(io_reg_write(address,data)) return;
   
   mem_map[address>>12][address&0x0FFF] = data;  
}

//-------------------------------------------------------------------------
// writememory_MBC3:
// for MBC3
//-------------------------------------------------------------------------
void gb_system::writememory_MBC3(register unsigned short address,register byte data)
{
   if(address < 0x2000)// Is it a RAM bank enable/disable?
   {
      RAMenable = ( (data&0x0A) == 0x0A ? 1 : 0);
      return;  
   }
   
   if(address < 0x4000) // Is it a ROM bank switch?
   {  
      data &= 0x7F;  
      if(data==0) 
         data=1;
         
      rom_bank = data;
         
      int cadr = data<<14;
      cadr &= rom_size_mask[rom->ROMsize];
      mem_map[0x4] = &cartridge[cadr];
      mem_map[0x5] = &cartridge[cadr+0x1000];
      mem_map[0x6] = &cartridge[cadr+0x2000];
      mem_map[0x7] = &cartridge[cadr+0x3000]; 
      return;
   }
   
   if(address < 0x6000) // Is it a RAM bank switch?
   {                
      if(rom->RTC && data>8)
      {
         RTCIO = 1;
         rtc.cur_register = data;

         return;
      } else RTCIO = 0;
      
      if(rom->RAMsize <= 2) // no need to change it if there isn't over 8KB ram
         return;

      data &= 0x03;
         
      if(data > maxRAMbank[rom->RAMsize])
         data = maxRAMbank[rom->RAMsize];
      
      ram_bank = data;
      
      int madr = data<<13;
      mem_map[0xA] = &cartRAM[madr];
      mem_map[0xB] = &cartRAM[madr+0x1000];
      return;  
   }
   
   if(address < 0x8000)
   {   
      if(data == 1)
      {
         rtc_update();
         RTC_latched = !RTC_latched;
         rtc_latch = rtc;
      }
      return;
   }
   
   if(address >= 0xA000 && address < 0xC000)
   {  
      if(RAMenable && rom->RTC && RTCIO)
      {
         time(&rtc.last_time);
         switch(rtc.cur_register)
         {
            case 0x08:
               rtc.s = data;
            break;
            case 0x09:
               rtc.m = data;          
            break;            
            case 0x0A:
               rtc.h = data;               
            break;            
            case 0x0B:
               rtc.d = data;             
            break;            
            case 0x0C:
               rtc.control = data;
               rtc.d |= (data&1)<<8;             
            break;            
         }
      }
      //if(!RAMenable || !rom->RAMsize)
      //   return;
   }
   
   if(io_reg_write(address,data)) return;
   
   mem_map[address>>12][address&0x0FFF] = data;  
}

//-------------------------------------------------------------------------
// writememory_MBC5:
// for MBC5 and MBC5 rumble
//-------------------------------------------------------------------------
void gb_system::writememory_MBC5(register unsigned short address,register byte data,bool isNiutoude,bool isSintax)
{
   if(address < 0x2000)// Is it a RAM bank enable/disable?
   {
      RAMenable = ( (data&0x0A) == 0x0A ? 1 : 0);
      return;  
   }
   
   if(address < 0x3000)
   {    
   
      if (isNiutoude && address >= 0x2100) { // Increased from 2800.. Too far?
      	return;
      }
      byte origData = data;
      
      if (isSintax) {
      	switch(sintax_mode & 0x0f) {
      		// Maybe these could go in a config file, so new ones can be added easily?
      		case 0x0D: {
      			byte flips[] = {6,7,0,1,2,3,4,5};
      			data = switchOrder( data, flips );
      			//data = ((data & 0x03) << 6 ) + ( data >> 2 );
      			break;
      		}
      		case 0x09: {
      		//	byte flips[] = {4,5,2,3,7,6,1,0}; // Monkey..no
      			byte flips[] = {3,2,5,4,7,6,1,0};
      			data = switchOrder( data, flips );
      			break;
      		}

      		case 0x00: { // 0x10=lion 0x00 hmmmmm
      			byte flips[] = {0,7,2,1,4,3,6,5};
      			data = switchOrder( data, flips );
      			break;
      		}
      		
      		case 0x01: {
      			byte flips[] = {7,6,1,0,3,2,5,4};
      			data = switchOrder( data, flips );
      			break;
      		}
      		      		
      		case 0x05: {
      			byte flips[] = {0,1,6,7,4,5,2,3}; // Not 100% on this one
      			data = switchOrder( data, flips );
      			break;
      		}

      	}
      	
      	setXorForBank(origData);
    
      }
    
	
      // Set current xor so we dont have to figure it out on every read
      
      rom_bank = data|(MBChi<<8); 
      cart_address = rom_bank<<14;
      
      cart_address &= rom_size_mask[rom->ROMsize];
      
      MBClo = data;   

      mem_map[0x4] = &cartridge[cart_address];
      mem_map[0x5] = &cartridge[cart_address+0x1000];
      mem_map[0x6] = &cartridge[cart_address+0x2000];
      mem_map[0x7] = &cartridge[cart_address+0x3000];
      
    //  if(origData == 0x69) {   
	//    	char buff[100];
	//		sprintf(buff,"%X %X %X",origData,data,cart_address);
	//		debug_print(buff);
    //  }

	
      
      return;
   }
   
   if(address < 0x4000) // Is it a ROM bank switch?
   { 
      data = data&1;
                 
      rom_bank = MBClo|(data<<8);
         
      cart_address = rom_bank<<14;

      cart_address &= rom_size_mask[rom->ROMsize];
      MBChi = data;

      mem_map[0x4] = &cartridge[cart_address];
      mem_map[0x5] = &cartridge[cart_address+0x1000];
      mem_map[0x6] = &cartridge[cart_address+0x2000];
      mem_map[0x7] = &cartridge[cart_address+0x3000];     
      
      return;
   }
   
   if(address < 0x6000) // Is it a RAM bank switch?
   {        
   		// sintaxs not entirely understood addressing thing hi  
   		if (isSintax && address >= 0x5000) {
   		 switch(0x0F & data) {
   		 	case 0x0D: // old
   		 	case 0x09: // ???
   		 	case 0x00:// case 0x10: // LiON, GoldenSun
   		 	case 0x01: // LANGRISSER
   		 	case 0x05: // Maple, PK Platinum
   		 		// These are all supported
   		 	break;
   		 	default:
	 			char buff[100];
   		 		sprintf(buff,"Unknown Sintax Mode %X - probably won't work!",data);
   		 		debug_print(buff);
   			break;
   		 }
   		 sintax_mode=data;
   		 
   		 writememory_MBC5(0x2000,01,false,true); // force a fake bank switch
   		 
   		 return;

   		}
   
      if(rom->rumble)
      {
         if(data&0x08)
            rumble_counter = 4;
         data &= 0x07;
      }
      
      if(rom->RAMsize <= 2) // no need to change it if there isn't over 8KB ram
         return;
         
      data &= 0x0F;
         
      if(data > maxRAMbank[rom->RAMsize])
         data = maxRAMbank[rom->RAMsize];
      
      ram_bank = data;
      
      int madr = data<<13;
      mem_map[0xA] = &cartRAM[madr];
      mem_map[0xB] = &cartRAM[madr+0x1000];
      return;  
   }
   
   if(address<0x8000)
   {
   		if ( isSintax && address >= 0x7000 ) {
   			int xorNo = ( address & 0x00F0 ) >> 4;
   			switch (xorNo) {
   				case 2:
   					sintax_xor2 = data;
   				break;
   				case 3:
   					sintax_xor3 = data;
   				break;
   				case 4:
   					sintax_xor4 = data;
   				break;
   				case 5:
					sintax_xor5 = data;   					
   				break;
   			}
   			
   			if (sintax_currentxor == 0 ) {
   				setXorForBank(4);
   			}
   	   	    
   			
   			return;
   			
   		}
   	
   	
    /*  if(++bc_select == 2 && rom->ROMsize>1) 
      {
         MBChi = (data&0xFF);

         cart_address = (MBChi<<1)<<14;
         
         mem_map[0x0] = &cartridge[cart_address];
         mem_map[0x1] = &cartridge[cart_address+0x1000];
         mem_map[0x2] = &cartridge[cart_address+0x2000];
         mem_map[0x3] = &cartridge[cart_address+0x3000];
         
         mem_map[0x4] = &cartridge[cart_address+0x4000];
         mem_map[0x5] = &cartridge[cart_address+0x5000];
         mem_map[0x6] = &cartridge[cart_address+0x6000];
         mem_map[0x7] = &cartridge[cart_address+0x7000];
      }*/
      return;
   }
               
 /*  if(address >= 0xA000 && address < 0xC000)
   {
      if(!RAMenable || !rom->RAMsize)     
         return;
   }*/
   
   if(io_reg_write(address,data)) return;
   
   mem_map[address>>12][address&0x0FFF] = data;  
}

//-------------------------------------------------------------------------
// writememory_Camera:
// for the Gameboy Pocket Camera
//-------------------------------------------------------------------------
void gb_system::writememory_Camera(register unsigned short address,register byte data)
{
   if(address < 0x2000)// Is it a RAM bank enable/disable?
   {
      RAMenable = ( (data&0x0A) == 0x0A ? 1 : 0);
      return;  
   }
      
   if(address < 0x4000) // Is it a ROM bank switch?
   { 
      if(data == 0)
         data = 1;
      if(data > maxROMbank[rom->ROMsize])
         data = maxROMbank[rom->ROMsize];
         
      rom_bank = data;
         
      int cadr = data<<14;
      mem_map[0x4] = &cartridge[cadr];
      mem_map[0x5] = &cartridge[cadr+0x1000];
      mem_map[0x6] = &cartridge[cadr+0x2000];
      mem_map[0x7] = &cartridge[cadr+0x3000];
      return; 
   }
   
   if(address < 0x6000) // Is it a RAM bank switch?
   {               
      if(data == 0x10)
      {
         cameraIO = 1;
         return;
      }
      else
         cameraIO = 0;
         
      data &= 0x0F;
         
      if(data > maxRAMbank[rom->RAMsize])
         data = maxRAMbank[rom->RAMsize];
      
      ram_bank = data;
      
      int madr = data<<13;
      mem_map[0xA] = &cartRAM[madr];
      mem_map[0xB] = &cartRAM[madr+0x1000];
      return;  

   }
   
   if(address<0x8000)
      return;
   
 /*  if(address >= 0xA000 && address < 0xC000)
   {
      if(!RAMenable)
         return;
   }*/
   
   if(io_reg_write(address,data)) return;
   
   mem_map[address>>12][address&0x0FFF] = data;  
}

void gb_system::update_HuC3time()
{  
   time_t now = time(0);
   time_t diff = now-HuC3_last_time;
   if(diff > 0)
   {
      rtc.s += diff % 60; // use rtc.s to store seconds
      if(rtc.s > 59) 
      {
         rtc.s -= 60;
         HuC3_time++;
      }

      diff /= 60;

      HuC3_time += diff % 60;
      if((HuC3_time&0xFFF) > 1439) 
      {
         HuC3_time = (HuC3_time&0xFFFF000)|((HuC3_time&0xFFF)-1440);
         HuC3_time += 0x1000; // day counter ?
      }
      
      diff /= 60;

      HuC3_time += (diff % 24)*60;
      if((HuC3_time&0xFFF) > 1439) 
      {
         HuC3_time = (HuC3_time&0xFFFF000)|((HuC3_time&0xFFF)-1440);
         HuC3_time += 0x1000; // day counter ?
      }

      diff /= 24;

      HuC3_time += (diff<<12);
      if(((HuC3_time&0xFFF000)>>12) > 356) 
      {
         HuC3_time = HuC3_time&0xF000FFF;
         HuC3_time += 0x1000000; // year counter ????
      }
   }
   HuC3_last_time = now;   
}

//-------------------------------------------------------------------------
// readmemory_HuC3:
// for HuC3
//-------------------------------------------------------------------------
byte gb_system::readmemory_HuC3(register unsigned short address)
{            
   if(address >= 0xA000 && address < 0xC000)
   {
      if(HuC3_RAMflag >= 0x0b && HuC3_RAMflag < 0x0e) 
      {        
         if(HuC3_RAMflag == 0x0D)
            return 1;            
         return HuC3_RAMvalue;
      }
      if(!rom->RAMsize)
         return 0xFF;
   }
   
   return io_reg_read(address);
}

//-------------------------------------------------------------------------
// writememory_HuC3:
// for HuC-3
//-------------------------------------------------------------------------
void gb_system::writememory_HuC3(register unsigned short address,register byte data)
{
   if(address < 0x2000)// Is it a RAM bank enable/disable?
   {
      RAMenable = ( (data&0x0A) == 0x0A ? 1 : 0);
      HuC3_RAMflag = data; 
      return;  
   }
      
   if(address < 0x4000) // Is it a ROM bank switch?
   { 
      if(data == 0)
         data = 1;
      if(data > maxROMbank[rom->ROMsize])
         data = maxROMbank[rom->ROMsize];
         
      rom_bank = data;
         
      int cadr = data<<14;
      mem_map[0x4] = &cartridge[cadr];
      mem_map[0x5] = &cartridge[cadr+0x1000];
      mem_map[0x6] = &cartridge[cadr+0x2000];
      mem_map[0x7] = &cartridge[cadr+0x3000];
      return; 
   }
   
   if(address < 0x6000) // Is it a RAM bank switch?
   {                        
      data &= 0x0F;
         
      if(data > maxRAMbank[rom->RAMsize])
         data = maxRAMbank[rom->RAMsize];
      
      ram_bank = data;
      
      int madr = data<<13;
      mem_map[0xA] = &cartRAM[madr];
      mem_map[0xB] = &cartRAM[madr+0x1000];
      return;  
   }
   
   if(address < 0x8000) // programs will write 1 here
      return;
   
   if(address >= 0xA000 && address < 0xC000)
   { 
      if(HuC3_RAMflag < 0x0b || HuC3_RAMflag > 0x0e) // write to RAM
      {
         if(!RAMenable || !rom->RAMsize)
            return;
      } else 
      {
         if(HuC3_RAMflag == 0x0B) // send command ? 
         {
            switch(data & 0xf0) 
            {
            case 0x10: // read time
               update_HuC3time();
               if(HuC3_flag == HUC3_READ)
               {
                  HuC3_RAMvalue = ((HuC3_time>>HuC3_shift)&0x0F);
                  HuC3_shift += 4;
                  if(HuC3_shift > 24)
                     HuC3_shift = 0;  
               }
            break;
            case 0x30: // write to registers (minute,day and year(?) counters)    
               // to write time 23:59 program will send commands
               // 3F 39 35 30 30 30 31 
               // HuC3_time will then be 59F = 1439 = 23*60+59 minutes
               if(HuC3_flag == HUC3_WRITE) 
               {
                  if(HuC3_shift == 0)
                     HuC3_time = 0;
                  if(HuC3_shift < 24)
                  {
                     HuC3_time |= ((data&0x0F)<<HuC3_shift);
                     HuC3_shift += 4;
                     if(HuC3_shift == 24)
                        HuC3_flag = HUC3_READ;
                  }
               }                          
            break;
            case 0x40: // special command ?
               switch(data&0x0F)
               {
               case 0x00: //  ?
                  //HuC3_flag = HUC3_READ; 
                  HuC3_shift = 0;                  
               break;
               case 0x03: // write time mode ?
                  HuC3_flag = HUC3_WRITE;  
                  HuC3_shift = 0;                     
               break;
               case 0x07: // read time mode ?
                  HuC3_flag = HUC3_READ; 
                  HuC3_shift = 0;               
               break;
               case 0x06: // alarm clock sound test
               break;
               case 0x08: // set alarm clock time ?
                  //HuC3_flag = HUC3_NONE;
               break;
               case 0x0F: // yobidashi (call) ?
               break;
               default:
                  //char buffer[100];
                  //sprintf(buffer,"HuC3-command:%x",data);
                  //debug_print(buffer);                 
               break;
               }                                                                           
            break;
            case 0x50: // ?   
            {
               //HuC3_register[0] = (HuC3_register[0] & 0x0f) | ((data << 4)&0x0f);                                  
            }
            break; 
            case 0x60: // ? 
            {                            
               //HuC3_RAMvalue = 1;
               HuC3_flag = HUC3_READ; 
            }              
            break; 
            }
         } else if(HuC3_RAMflag == 0x0C) // not used ?
         {
            // ?
         } else if(HuC3_RAMflag == 0x0D) // programs will write 0xFE here 
         {          
            // maybe a execute command function ?
         } 
      }
   }
   
   if(io_reg_write(address,data)) return;
   
   mem_map[address>>12][address&0x0FFF] = data;  
}

//-------------------------------------------------------------------------
// writememory_MBC7:
// for MBC7
//-------------------------------------------------------------------------
void gb_system::writememory_MBC7(register unsigned short address,register byte data)
{
   if(address < 0x2000)
      return;  
   
   if(address < 0x4000) // Is it a ROM bank switch?
   { 
      data = data&0x7F;

      if(data==0)
         data=1;
      
      
      rom_bank = data;
         
      cart_address = data<<14;

      cart_address &= rom_size_mask[rom->ROMsize];
         
      mem_map[0x4] = &cartridge[cart_address];
      mem_map[0x5] = &cartridge[cart_address+0x1000];
      mem_map[0x6] = &cartridge[cart_address+0x2000];
      mem_map[0x7] = &cartridge[cart_address+0x3000];
      return;
   }
   
   if(address < 0x6000) // Is it a RAM bank switch?
   {               
      if(data<8)
      {
         RAMenable = 0;
         
         if(rom->RAMsize <= 2) // no need to change it if there isn't over 8KB ram
            return;
         
         data &= 0x03;
         
         if(data > maxRAMbank[rom->RAMsize])
            data = maxRAMbank[rom->RAMsize];
      
         ram_bank = data;
      
         int madr = data<<13;
         mem_map[0xA] = &cartRAM[madr];
         mem_map[0xB] = &cartRAM[madr+0x1000];
         return;  
      } else
      {
         RAMenable=0;      
         return;
      }

   }
   
   if(address < 0x8000)
      return;
   
   if(address >= 0xA000 && address < 0xC000)
   {
      if(address == 0xa080) 
      {
         // special processing needed
         int oldCs = MBC7_cs,oldSk=MBC7_sk;
    
         MBC7_cs=data>>7;
         MBC7_sk=(data>>6)&1;
    
         if(!oldCs && MBC7_cs) 
         {
            if(MBC7_state == 5) 
            {
               if(MBC7_writeEnable) 
               {
                  memory[0xa000+MBC7_address*2] = MBC7_buffer>>8;
                  memory[0xa000+MBC7_address*2+1] = MBC7_buffer&0xff;
               }
               MBC7_state = 0;
               MBC7_value = 1;
            } else 
            {
               MBC7_idle = true;
               MBC7_state = 0;
            }
        }
    
        if(!oldSk && MBC7_sk) 
        {
           if(MBC7_idle) 
           {
              if(data & 0x02) 
              {
                 MBC7_idle = false;
                 MBC7_count = 0;
                 MBC7_state = 1;
              }
           } else 
           {
              switch(MBC7_state) 
              {
              case 1:
                 // receiving command
                 MBC7_buffer <<= 1;
                 MBC7_buffer |= (data & 0x02)?1:0;
                 MBC7_count++;
                 if(MBC7_count == 2) 
                 {
                    // finished receiving command
                    MBC7_state = 2;
                    MBC7_count = 0;
                    MBC7_code = MBC7_buffer & 3;
                 }
              break;
              case 2:
                 // receive address
                 MBC7_buffer <<= 1;
                 MBC7_buffer |= (data&0x02)?1:0;
                 MBC7_count++;
                 if(MBC7_count==8) 
                 {
                    // finish receiving
                    MBC7_state = 3;
                    MBC7_count = 0;
                    MBC7_address = MBC7_buffer&0xff;
                    if(MBC7_code == 0) 
                    {
                       if((MBC7_address>>6) == 0) 
                       {
                          MBC7_writeEnable = 0;
                          MBC7_state = 0;
                       } else if((MBC7_address>>6) == 3) 
                       {
                          MBC7_writeEnable = 1;
                          MBC7_state = 0;
                       }
                    }
                 }
              break;
              case 3:
                 MBC7_buffer <<= 1;
                 MBC7_buffer |= (data&0x02)?1:0;
                 MBC7_count++;
          
                 switch(MBC7_code) 
                 {
                 case 0:
                    if(MBC7_count==16) 
                    {
                       if((MBC7_address>>6)==0) 
                       {
                          MBC7_writeEnable = 0;
                          MBC7_state = 0;
                       } else if((MBC7_address>>6) == 1) 
                       {
                          if(MBC7_writeEnable) 
                          {
                             for(int i=0;i<256;i++) 
                             {
                                memory[0xa000+i*2] = MBC7_buffer >> 8;
                                memory[0xa000+i*2+1] = MBC7_buffer & 0xff;
                             }
                          }
                          MBC7_state = 5;
                       } else if((MBC7_address>>6) == 2) 
                       {
                          if(MBC7_writeEnable) 
                          {
                             for(int i=0;i<256;i++)
                                *((unsigned short *)&memory[0xa000+i*2]) = 0xffff;
                          }
                          MBC7_state = 5;
                       } else if((MBC7_address>>6) == 3) 
                       {
                          MBC7_writeEnable = 1;
                          MBC7_state = 0;
                       }
                       MBC7_count = 0;
                    }
                 break;
                 case 1:
                    if(MBC7_count == 16) 
                    {
                       MBC7_count = 0;
                       MBC7_state = 5;
                       MBC7_value = 0;
                    }
                 break;
                 case 2:
                    if(MBC7_count == 1) 
                    {
                       MBC7_state = 4;
                       MBC7_count = 0;
                       MBC7_buffer = (memory[0xa000+MBC7_address*2]<<8)|(memory[0xa000+MBC7_address*2+1]);              
                    }
                 break;
                 case 3:
                    if(MBC7_count == 16) 
                    {
                       MBC7_count = 0;
                       MBC7_state = 5;
                       MBC7_value = 0;
                       MBC7_buffer = 0xffff;
                    }
                 break;
                 }
              break;
              }
          }
       }
    
       if(oldSk && !MBC7_sk) 
       {
          if(MBC7_state == 4) 
          { 
             MBC7_value = (MBC7_buffer & 0x8000)?1:0;
             MBC7_buffer <<= 1;
             MBC7_count++;
             if(MBC7_count == 16) 
             {
                MBC7_count = 0;
                MBC7_state = 0;
             }
          }
       }
      }
      return;
   }
   
   if(io_reg_write(address,data)) return;
   
   mem_map[address>>12][address&0x0FFF] = data;  
}

//-------------------------------------------------------------------------
// readmemory_MBC7:
// for MBC7
//-------------------------------------------------------------------------
byte gb_system::readmemory_MBC7(register unsigned short address)
{ 
   if(address >= 0xA000 && address < 0xC000)
   {
      switch(address & 0xa0f0) 
      {
         case 0xa000:
         case 0xa010:
         case 0xa060:
         case 0xa070:
            return 0;
         case 0xa020:
            // sensor X low byte
            return sensorX & 255;
         case 0xa030:
            // sensor X high byte
            return sensorX >> 8;
         case 0xa040:
            // sensor Y low byte
            return sensorY & 255;
         case 0xa050:
            // sensor Y high byte
            return sensorY >> 8;
         case 0xa080:
            return MBC7_value;
      }
      return 0xff;
   }
   
   return io_reg_read(address);
}

//-------------------------------------------------------------------------
// readmemory_TAMA5:
// for Bandai TAMA5 (Tamagotchi3)
//-------------------------------------------------------------------------

void gb_system::update_tama_RTC()
{
   time_t now = time(0);
   time_t diff = now-rtc.last_time;
   if(diff > 0)
   {
    rtc.s += diff % 60;
    if(rtc.s > 59) 
    {
      rtc.s -= 60;
      rtc.m++;
    }

    diff /= 60;

    rtc.m += diff % 60;
    if(rtc.m > 59) 
    {
      rtc.m -= 60;
      rtc.h++;
    }

    diff /= 60;

    rtc.h += diff % 24;
    if(rtc.h > 24) 
    {
      rtc.h -= 24;
      rtc.d++;
    }
    diff /= 24;

    rtc.d += diff;
    if(rtc.d > 31) 
    {
      rtc.d -= 31;
      tama_month++;
      if(tama_month > 12)
         tama_month -= 12;
    }
  }
  rtc.last_time = now;
}

byte gb_system::readmemory_TAMA5(register unsigned short address)
{ 
   if(address >= 0xA000 && address < 0xC000)
   {         
      if(address == 0xA000)
      {
         switch(tama_flag)
         {
         case 0x0A: // test register ?
            return 1; 
         case 0x0D: // RTC registers 
         {       
            byte read = 0;
            if(tama_val4 == 1)
            {           
               if(tama_val6 == 0x04)
               {
                  if(tama_val7 == 0x06) // get minutes higher nibble
                  {
                     read = rtc.m;
                     if(read > 0x0f)
                        read += 6;
                     if(read > 0x1f)
                        read += 6;
                     if(read > 0x2f)
                        read += 6;
                     if(read > 0x3f)
                        read += 6;   
                     if(read > 0x4f)
                        read += 6; 
                     if(read == 0x5A)
                        read = 0;                                                             
                     read = (read&0xf0)>>4;
                  }
                  else
                  if(tama_val7 == 0x07) // get hours higher nibble
                  {
                     read = rtc.h;
                     if(read > 0x0f)
                        read += 6;
                     if(read > 0x1f)
                        read += 6;
                     if(read > 0x2f)
                        read += 6;
                     if(read > 0x3f)
                        read += 6;   
                                                        
                     read = (read&0xf0)>>4;                  
                  }
               } 
            } else
            { // read memory ?
               read = memory[0xA000|(tama_val6<<4)|tama_val7];
            } 
            return read;
         }   
         case 0x0C: // RTC registers
         {
            update_tama_RTC();
            byte read = 0;
            switch(tama_val4)
            {
            case 0:
               if(rtc.s == 0)
                  read = 0;
               else
                  read = rtc.s - (tama_time+1); // return 0 if second has passed
               tama_time = rtc.s;
            break;
            case 1:   
               if(tama_val6 == 0x04)
               {              
                  if(tama_val7 == 0x06) // get minutes lower nibble 
                  {
                     read = rtc.m;
                     
                     //change into correct format
                     if(read > 0x0f)
                        read += 6;
                     if(read > 0x1f)
                        read += 6;
                     if(read > 0x2f)
                        read += 6;
                     if(read > 0x3f)
                        read += 6;     
                     if(read > 0x4f)
                        read += 6;
                     if(read == 0x5A)
                        read = 0;
                                                                  
                     read &= 0x0f;
                  }
                  else 
                  if(tama_val7 == 0x07) // get hours lower nibble
                  {
                     read = rtc.h;
                     
                     //change into correct format
                     if(read > 0x0f)
                        read += 6;
                     if(read > 0x1f)
                        read += 6;
                     if(read > 0x2f)
                        read += 6;
                     if(read > 0x3f)
                        read += 6;     
                        
                     read &= 0x0f;                                          
                  }
               } 
            break;
            case 7: // day low
               read = rtc.d&0xF;
            break;
            case 8: // day high
               read = ((rtc.d&0xf0)>>4);
            break;
            case 9: // month
               read = tama_month;
            break;
            case 0xA: // ? (year maybe?)
               read = 0;            
            break;
            case 0xF: // ? (year maybe?)
               read = 0;            
            break;
            }
            return read;
         }  
         }
      }
   }
   
   return io_reg_read(address);
}

//-------------------------------------------------------------------------
// writememory_TAMA5:
// for Bandai TAMA5 (Tamagotchi3)
//-------------------------------------------------------------------------
void gb_system::writememory_TAMA5(register unsigned short address,register byte data)
{   
   if(address < 0x8000) // ?
   {  
      return;   
   }
   
   if(address >= 0xA000 && address < 0xC000)
   {       
      if(address == 0xA000)
      {                 
         switch(tama_flag)
         {
         case 0: // rom bank lower nibble
         {
            data &= 0x0F;
            
            MBClo = data;
            rom_bank = MBClo|(MBChi<<4);
       
            int cadr = rom_bank<<14;
            mem_map[0x4] = &cartridge[cadr];
            mem_map[0x5] = &cartridge[cadr+0x1000];
            mem_map[0x6] = &cartridge[cadr+0x2000];
            mem_map[0x7] = &cartridge[cadr+0x3000];      
         }
         return;
         case 1: // rom bank high bit
         {
            data &= 0x01;
            
            MBChi = data;
            
            rom_bank = MBClo|(MBChi<<4);
       
            int cadr = rom_bank<<14;
            mem_map[0x4] = &cartridge[cadr];
            mem_map[0x5] = &cartridge[cadr+0x1000];
            mem_map[0x6] = &cartridge[cadr+0x2000];
            mem_map[0x7] = &cartridge[cadr+0x3000];    
         }
         return;
         case 4: // RTC controls
            tama_val4 = data;         
         return;
         case 5: // write time (and write memory????)
            tama_val5 = data;
            ++tama_count;
            if(tama_count==1 && data == 0) tama_change_clock |= 2;
            if(tama_count==2 && data == 1) tama_change_clock |= 1;
            if(tama_change_clock == 3) rtc.last_time = time(0);

            memory[0xA000+(tama_val6<<4)+tama_val7] = tama_val4|(data<<4);
            
            //which time counter is changed?
            if(tama_count==6 && tama_change_clock==3)
            {
               tama_month = data;
            } else
            if(tama_count==7 && tama_change_clock==3)
            {
               tama_month += data*10;
            } else
            if(tama_count==8 && tama_change_clock==3)
            {
               rtc.d = data;
            } else
            if(tama_count==9 && tama_change_clock==3)
            {
               rtc.d += data*10;
            } else
            if(tama_count==10 && tama_change_clock==3)
            {
               rtc.m = data*10;
            } else
            if(tama_count==11 && tama_change_clock==3)
            {
               rtc.h = data*10;
            } else
            if(tama_change_clock==3 && tama_count >= 13)
            {
               tama_count = 0;
               tama_change_clock = 0;
            } else
            if(tama_change_clock!=3 && tama_count >= 2)
            {
               tama_count=0;
               tama_change_clock = 0;
            }
         return;         
         case 6: // RTC controls
            tama_val6 = data;          
         return;         
         case 7: // RTC controls
            tama_val7 = data;                  
         return;
         }
      } else
      if(address == 0xA001)
      {                         
         tama_flag = data; 
                  
         return;
      } 
   } 

   if(io_reg_write(address,data)) return;

   mem_map[address>>12][address&0x0FFF] = data;  
}
