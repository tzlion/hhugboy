/*
   hhugboy Game Boy emulator
   copyright 2013-2016 taizou

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
#include "../mainloop.h"
#include <time.h>

#include "../GB.h"
#include "GB_MBC.h"

#include "../debug.h"
#include "zlib/zconf.h"
#include <stdio.h>

#include "../main.h"

byte vfmultimode=0;
byte vfmultibank=0;
byte vfmultimem=0;
byte vfmultifinal=0;

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

gb_mbc::gb_mbc(gb_system* inGB):
        HuC3_RAMvalue(0),
        HuC3_RAMaddress(0),
        HuC3_address(0),
        HuC3_RAMflag(0),
        HuC3_last_time(time(0)),
        HuC3_flag(HUC3_NONE),
        HuC3_time(0),
        HuC3_shift(0),

        MBC7_cs(0),
        MBC7_sk(0),
        MBC7_state(0),
        MBC7_buffer(0),
        MBC7_idle(0),
        MBC7_count(0),
        MBC7_code(0),
        MBC7_address(0),
        MBC7_writeEnable(0),
        MBC7_value(0),

        bc_select(0),
        tama_flag(0),
        tama_time(0),
        tama_val4(0),
        tama_val5(0),
        tama_val6(0),
        tama_val7(0),
        tama_count(0),
        tama_month(0),
        tama_change_clock(0)
{
    aGB = inGB;
}

byte gb_mbc::readmemory_cart(register unsigned short address) {
    switch(aGB->memory_read)
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

void gb_mbc::writememory_cart(unsigned short address, register byte data) {
    switch(aGB->memory_write)
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

void gb_mbc::setXorForBank(byte bankNo)
{
  	switch(bankNo & 0x0F) {
	case 0x00: case 0x04: case 0x08: case 0x0C:
		aGB->sintax_currentxor = aGB->sintax_xor2;
		break;
	case 0x01: case 0x05: case 0x09: case 0x0D:
		aGB->sintax_currentxor = aGB->sintax_xor3;
		break;
	case 0x02: case 0x06: case 0x0A: case 0x0E:
		aGB->sintax_currentxor = aGB->sintax_xor4;
		break;
	case 0x03: case 0x07: case 0x0B: case 0x0F:
		aGB->sintax_currentxor = aGB->sintax_xor5;
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
byte gb_mbc::readmemory_default(register unsigned short address)
{            
  /* if(address >= 0xA000 && address < 0xC000)
   {
      if((!rom->RAMsize && !rom->battery))
         return 0xFF; 
   }*/
   
    //  wchar_t wrmessage[50];
    //  wsprintf(wrmessage,L"MM %X %X",superaddroffset,mem_map[0x0]);
    //  renderer.showMessage(wrmessage,60,GB1);
        
   /* mem_map[0x0] = &cartridge[superaddroffset];
    mem_map[0x1] = &cartridge[superaddroffset+0x1000];
    mem_map[0x2] = &cartridge[superaddroffset+0x2000];
    mem_map[0x3] = &cartridge[superaddroffset+0x3000];
    
    mem_map[0x4] = &cartridge[superaddroffset+0x4000];
    mem_map[0x5] = &cartridge[superaddroffset+0x5000];
    mem_map[0x6] = &cartridge[superaddroffset+0x6000];
    mem_map[0x7] = &cartridge[superaddroffset+0x7000];    */

   return aGB->io_reg_read(address);
}

//-------------------------------------------------------------------------
// readmemory_sintax:
// for SiNTAX
//-------------------------------------------------------------------------
byte gb_mbc::readmemory_sintax(register unsigned short address)
{            

   if(address >= 0x4000 && address < 0x8000)
   {
   	byte data = aGB->io_reg_read(address);

   	
   	//char buff[100];
	//sprintf(buff,"MBCLo %X Addr %X Data %X XOR %X XOR'd data %X",MBClo,address,data,sintax_currentxor, data ^ sintax_currentxor);
	//debug_print(buff);
   	
     return  data ^ aGB->sintax_currentxor;
   }

   return aGB->io_reg_read(address);
}


//-------------------------------------------------------------------------
// readmemory_MBC3:
// for MBC3
//-------------------------------------------------------------------------
byte gb_mbc::readmemory_MBC3(register unsigned short address)
{ 
   if(address >= 0xA000 && address < 0xC000)
   {
      if(aGB->rom->RTC && aGB->RTCIO)
      {
         switch(aGB->rtc.cur_register)
         {
            case 0x08:
               return aGB->rtc_latch.s;
            case 0x09:
               return aGB->rtc_latch.m;
            case 0x0A:
               return aGB->rtc_latch.h;
            case 0x0B:
               return (aGB->rtc_latch.d&0xFF);
            case 0x0C:
               return ((aGB->rtc_latch.d&0x100)>>8)|aGB->rtc_latch.control;
         }
      } 
      //if(!rom->RAMsize)
      //   return 0xFF; 
   }

   return aGB->io_reg_read(address);
}

//-------------------------------------------------------------------------
// readmemory_Camera:
// for GB Pocket Camera
//-------------------------------------------------------------------------
byte gb_mbc::readmemory_Camera(register unsigned short address)
{   
   if(address >= 0xA000 && address < 0xC000)
   {
      if(aGB->cameraIO) // Camera I/O register in cart RAM area
      {
         if(address == 0xA000)
            return 0x00; // Hardware is ready 
         else
            return 0xFF; // others write only
      }
   }
   
   return aGB->io_reg_read(address);
}
      
//-------------------------------------------------------------------------
// writememory_default:
// for ROM only and for undocumented/unknown MBCs
//-------------------------------------------------------------------------
void gb_mbc::writememory_default(register unsigned short address,register byte data)
{
   if(address < 0x2000)// Is it a RAM bank enable/disable?
   {
      aGB->RAMenable = ( (data&0x0A) == 0x0A ? 1 : 0);
      return;  
   } 
   
   if(address < 0x4000) // Is it a ROM bank switch?
   {  
      if(aGB->rom->bankType == ROM)
         return;
      else
      {
         if(data == 0)
            data = 1;
         
         aGB->rom_bank = data;
         
         int cadr = (data<<14)+((aGB->MBChi<<1)<<14);
         aGB->mem_map[0x4] = &(aGB->cartridge)[cadr];
         aGB->mem_map[0x5] = &(aGB->cartridge)[cadr+0x1000];
         aGB->mem_map[0x6] = &(aGB->cartridge)[cadr+0x2000];
         aGB->mem_map[0x7] = &(aGB->cartridge)[cadr+0x3000];
         return; 
      }
   }

   if(address < 0x6000) // Is it a RAM bank switch?
   {          
      data &= 0x03;
              
      aGB->ram_bank = data;
      
      int madr = data<<13;
      aGB->mem_map[0xA] = &(aGB->cartRAM)[madr];
      aGB->mem_map[0xB] = &(aGB->cartRAM)[madr+0x1000];
      return;  
   } 
   
   if(address < 0x8000) // BHGOS multicart
   {           
      if(++bc_select == 2 && aGB->rom->ROMsize>1)
      {
         aGB->MBChi = (data&0xFF);

         cart_address = (aGB->MBChi<<1)<<14;
         
         aGB->mem_map[0x0] = &(aGB->cartridge)[cart_address];
         aGB->mem_map[0x1] = &(aGB->cartridge)[cart_address+0x1000];
         aGB->mem_map[0x2] = &(aGB->cartridge)[cart_address+0x2000];
         aGB->mem_map[0x3] = &(aGB->cartridge)[cart_address+0x3000];
         
         aGB->mem_map[0x4] = &(aGB->cartridge)[cart_address+0x4000];
         aGB->mem_map[0x5] = &(aGB->cartridge)[cart_address+0x5000];
         aGB->mem_map[0x6] = &(aGB->cartridge)[cart_address+0x6000];
         aGB->mem_map[0x7] = &(aGB->cartridge)[cart_address+0x7000];
      }
      return;      
   }

   // Always allow RAM writes.
   
   if(aGB->io_reg_write(address,data)) return;

   aGB->mem_map[address>>12][address&0x0FFF] = data;
}

//-------------------------------------------------------------------------
// writememory_8in1:
// for Sachen 8in1
//-------------------------------------------------------------------------
void gb_mbc::writememory_8in1(register unsigned short address,register byte data)
{
   if(address < 0x2000)
      return;  
   
   if(address < 0x4000) // Is it a ROM bank switch?
   { 
      data &= 0x1f;
      if(data == 0)
         data = 1;
         
      aGB->rom_bank = data;
         
      int cadr = (data<<14)+(aGB->MBChi<<14);
      cadr &= rom_size_mask[aGB->rom->ROMsize];
      aGB->mem_map[0x4] = &(aGB->cartridge)[cadr];
      aGB->mem_map[0x5] = &(aGB->cartridge)[cadr+0x1000];
      aGB->mem_map[0x6] = &(aGB->cartridge)[cadr+0x2000];
      aGB->mem_map[0x7] = &(aGB->cartridge)[cadr+0x3000];
      return; 
   }
      
   if(address < 0x6000) // Is it a RAM bank switch?
   {      
      if(address == 0x4000 && bc_select < 3) // game select
      {
         ++bc_select;
         
         aGB->MBClo = 0;
         
         aGB->MBChi = (data&0x1f);

         cart_address = aGB->MBChi<<14;
         aGB->mem_map[0x0] = &(aGB->cartridge)[cart_address];
         aGB->mem_map[0x1] = &(aGB->cartridge)[cart_address+0x1000];
         aGB->mem_map[0x2] = &(aGB->cartridge)[cart_address+0x2000];
         aGB->mem_map[0x3] = &(aGB->cartridge)[cart_address+0x3000];
         
         aGB->mem_map[0x4] = &(aGB->cartridge)[cart_address+0x4000];
         aGB->mem_map[0x5] = &(aGB->cartridge)[cart_address+0x5000];
         aGB->mem_map[0x6] = &(aGB->cartridge)[cart_address+0x6000];
         aGB->mem_map[0x7] = &(aGB->cartridge)[cart_address+0x7000];
         return;      
      }    

      data &= 0x03;
              
      aGB->ram_bank = data;
      
      int madr = data<<13;
      aGB->mem_map[0xA] = &(aGB->cartRAM)[madr];
      aGB->mem_map[0xB] = &(aGB->cartRAM)[madr+0x1000];
      return;  
   } 
   
   if(address < 0x8000)       
      return;      

   // Always allow RAM writes.
   
   if(aGB->io_reg_write(address,data)) return;

   aGB->mem_map[address>>12][address&0x0FFF] = data;
}

//-------------------------------------------------------------------------
// writememory_MBC1:
// for MBC1 and HuC1
//-------------------------------------------------------------------------
void gb_mbc::writememory_MBC1(register unsigned short address,register byte data)
{
   if(address < 0x2000)// Is it a RAM bank enable/disable?
   {
       aGB->RAMenable = ( (data&0x0A) == 0x0A ? 1 : 0);
      return;  
   }
   
   if(address < 0x4000) // Is it a ROM bank switch?
   {      
      if(aGB->MBC1memorymodel == 0)
      {
         if(data == 0)
            data = 1;

         aGB->MBClo = data;
         
         aGB->rom_bank = aGB->MBClo|(aGB->MBChi<<5);
          
         cart_address = aGB->MBClo<<14;
         cart_address |= (aGB->MBChi<<19);
         
         cart_address &= rom_size_mask[aGB->rom->ROMsize];

         aGB->mem_map[0x4] = &(aGB->cartridge)[cart_address];
         aGB->mem_map[0x5] = &(aGB->cartridge)[cart_address+0x1000];
         aGB->mem_map[0x6] = &(aGB->cartridge)[cart_address+0x2000];
         aGB->mem_map[0x7] = &(aGB->cartridge)[cart_address+0x3000];
      } else 
      {
         if(data == 0)
            data = 1;
      
         aGB->rom_bank = data;
         
         int cadr = aGB->rom_bank<<14;
         
         cadr &= rom_size_mask[aGB->rom->ROMsize];
         
         aGB->mem_map[0x4] = &(aGB->cartridge)[cadr];
         aGB->mem_map[0x5] = &(aGB->cartridge)[cadr+0x1000];
         aGB->mem_map[0x6] = &(aGB->cartridge)[cadr+0x2000];
         aGB->mem_map[0x7] = &(aGB->cartridge)[cadr+0x3000];
      }
      return;
   }
   
   if(address < 0x6000) // Is it a RAM bank switch?
   {          
      if(aGB->MBC1memorymodel == 0)
      {  
         if((((data&0x03)<<5)|aGB->MBClo) > maxROMbank[aGB->rom->ROMsize])
            return;
         
         aGB->MBChi = (data&0x03);
         
         aGB->rom_bank = aGB->MBClo|(aGB->MBChi<<5);
         
         cart_address = aGB->MBClo<<14;
         cart_address |= (aGB->MBChi<<19);
         
         cart_address &= rom_size_mask[aGB->rom->ROMsize];

         aGB->mem_map[0x4] = &(aGB->cartridge)[cart_address];
         aGB->mem_map[0x5] = &(aGB->cartridge)[cart_address+0x1000];
         aGB->mem_map[0x6] = &(aGB->cartridge)[cart_address+0x2000];
         aGB->mem_map[0x7] = &(aGB->cartridge)[cart_address+0x3000];
         return;
      }

      if(aGB->rom->RAMsize <= 2) // no need to change it if there isn't over 8KB ram
         return;

      data &= 0x03;
         
      if(data > maxRAMbank[aGB->rom->RAMsize])
         data = maxRAMbank[aGB->rom->RAMsize];
      
      aGB->ram_bank = data;
      
      int madr = data<<13;
      aGB->mem_map[0xA] = &(aGB->cartRAM)[madr];
      aGB->mem_map[0xB] = &(aGB->cartRAM)[madr+0x1000];
      return;  
   }
   
   if(address < 0x8000) // Is it a MBC1 max memory model change?
   {
       aGB->MBC1memorymodel = (data&0x01);
      return;
   }
   
/*   if(address >= 0xA000 && address < 0xC000)
   {
      if((!RAMenable || !rom->RAMsize) && !RAM_always_enable)
         return;
   }*/
   
   if(aGB->io_reg_write(address,data)) return;
   
   aGB->mem_map[address>>12][address&0x0FFF] = data;
}

//-------------------------------------------------------------------------
// writememory_MMM01: 
// for MMM01 
// Only game with MBC set as MMM01 in header I've seen is
// Momotarou Collection 2, which is propably a bad dump?
//-------------------------------------------------------------------------
void gb_mbc::writememory_MMM01(register unsigned short address,register byte data)
{
   if(address < 0x2000)
      return;  
   
   if(address < 0x4000) // Is it a ROM bank switch?
   {            
      if(data == 0)
         data = 1;
        
      aGB->rom_bank = data;
         
      int cadr = (aGB->rom_bank<<14)+(aGB->MBChi<<14);
         
      cadr &= rom_size_mask[aGB->rom->ROMsize];
         
      aGB->mem_map[0x4] = &(aGB->cartridge)[cadr];
      aGB->mem_map[0x5] = &(aGB->cartridge)[cadr+0x1000];
      aGB->mem_map[0x6] = &(aGB->cartridge)[cadr+0x2000];
      aGB->mem_map[0x7] = &(aGB->cartridge)[cadr+0x3000];
      return;
   }
   
   if(address < 0x6000) // Is it a RAM bank switch?
   {                 
      if(address == 0x5fff && !bc_select)
      {
         bc_select = 1;
         
         data &= 0x2f;
         aGB->MBClo = 0;
      
         aGB->MBChi = data;
         cart_address = aGB->MBChi<<14;
         
         aGB->mem_map[0x0] = &(aGB->cartridge)[cart_address];
         aGB->mem_map[0x1] = &(aGB->cartridge)[cart_address+0x1000];
         aGB->mem_map[0x2] = &(aGB->cartridge)[cart_address+0x2000];
         aGB->mem_map[0x3] = &(aGB->cartridge)[cart_address+0x3000];
         
         aGB->mem_map[0x4] = &(aGB->cartridge)[cart_address+0x4000];
         aGB->mem_map[0x5] = &(aGB->cartridge)[cart_address+0x5000];
         aGB->mem_map[0x6] = &(aGB->cartridge)[cart_address+0x6000];
         aGB->mem_map[0x7] = &(aGB->cartridge)[cart_address+0x7000];
         return;      
      }
         
      if(aGB->rom->RAMsize <= 2) // no need to change it if there isn't over 8KB ram
         return;

      data &= 0x03;
         
      if(data > maxRAMbank[aGB->rom->RAMsize])
         data = maxRAMbank[aGB->rom->RAMsize];
      
      aGB->ram_bank = data;
      
      int madr = data<<13;
      aGB->mem_map[0xA] = &(aGB->cartRAM)[madr];
      aGB->mem_map[0xB] = &(aGB->cartRAM)[madr+0x1000];
      return;  
   }
   
   if(address < 0x8000) 
      return;
   
   // Always allow RAM writes.
   
   if(aGB->io_reg_write(address,data)) return;
   
   aGB->mem_map[address>>12][address&0x0FFF] = data;
}

//-------------------------------------------------------------------------
// writememory_BC:
// for Collection Carts
//-------------------------------------------------------------------------
void gb_mbc::writememory_BC(register unsigned short address,register byte data)
{
   if(address < 0x2000)// Is it a RAM bank enable/disable?
   {
      aGB->RAMenable = ( (data&0x0A) == 0x0A ? 1 : 0);
      return;  
   }
   
   if(address < 0x4000) // Is it a ROM bank switch?
   { 
      data &= 0x0F;
            
      if(data == 0)
         data = 1;
        
      aGB->rom_bank = data|(aGB->MBChi<<4);
       
      int cadr = aGB->rom_bank<<14;
         
      cadr &= rom_size_mask[aGB->rom->ROMsize];
         
      aGB->mem_map[0x4] = &(aGB->cartridge)[cadr];
      aGB->mem_map[0x5] = &(aGB->cartridge)[cadr+0x1000];
      aGB->mem_map[0x6] = &(aGB->cartridge)[cadr+0x2000];
      aGB->mem_map[0x7] = &(aGB->cartridge)[cadr+0x3000];
      return;
   }
 
   if(address < 0x6000) 
   {                   
      if(address == 0x4000 || address == 0x5fff) // game select
      { 
         aGB->MBClo = 0;
         aGB->MBChi = (data&0x03);
                  
         cart_address = (aGB->MBChi<<4)<<14;
         
         aGB->mem_map[0x0] = &(aGB->cartridge)[cart_address];
         aGB->mem_map[0x1] = &(aGB->cartridge)[cart_address+0x1000];
         aGB->mem_map[0x2] = &(aGB->cartridge)[cart_address+0x2000];
         aGB->mem_map[0x3] = &(aGB->cartridge)[cart_address+0x3000];
         
         aGB->mem_map[0x4] = &(aGB->cartridge)[cart_address+0x4000];
         aGB->mem_map[0x5] = &(aGB->cartridge)[cart_address+0x5000];
         aGB->mem_map[0x6] = &(aGB->cartridge)[cart_address+0x6000];
         aGB->mem_map[0x7] = &(aGB->cartridge)[cart_address+0x7000];
         return;      
      }
      return;  
   }
   
   if(address < 0x8000) 
   {
      aGB->MBC1memorymodel = (data&0x01);
      return;
   }
   
  /* if(address >= 0xA000 && address < 0xC000)
   {
      if((!RAMenable || !rom->RAMsize) && !RAM_always_enable)
         return;
   }*/
   
   if(aGB->io_reg_write(address,data)) return;
   
   aGB->mem_map[address>>12][address&0x0FFF] = data;
}

//-------------------------------------------------------------------------
// writememory_MK12:
// for Mortal Kombat 1&2
//-------------------------------------------------------------------------
void gb_mbc::writememory_MK12(register unsigned short address,register byte data)
{
   if(address < 0x2000)// Is it a RAM bank enable/disable?
   {
      aGB->RAMenable = ( (data&0x0A) == 0x0A ? 1 : 0);
      return;  
   }
   
   if(address < 0x4000) // Is it a ROM bank switch?
   { 
      data &= 0x0F;
            
      if(data == 0)
         data = 1;
        
      aGB->rom_bank = data+aGB->MBChi;
       
      int cadr = aGB->rom_bank<<14;
        
      cadr &= rom_size_mask[aGB->rom->ROMsize];
         
      aGB->mem_map[0x4] = &(aGB->cartridge)[cadr];
      aGB->mem_map[0x5] = &(aGB->cartridge)[cadr+0x1000];
      aGB->mem_map[0x6] = &(aGB->cartridge)[cadr+0x2000];
      aGB->mem_map[0x7] = &(aGB->cartridge)[cadr+0x3000];
      return;
   }
 
   if(address < 0x6000) 
   {                   
      if(address == 0x5000) // game select
      {         
         aGB->MBClo = 0;
         aGB->MBChi = (data&0x03);
         if(aGB->MBChi==2) aGB->MBChi = 17;
                  
         cart_address = aGB->MBChi<<14;

         aGB->mem_map[0x0] = &(aGB->cartridge)[cart_address];
         aGB->mem_map[0x1] = &(aGB->cartridge)[cart_address+0x1000];
         aGB->mem_map[0x2] = &(aGB->cartridge)[cart_address+0x2000];
         aGB->mem_map[0x3] = &(aGB->cartridge)[cart_address+0x3000];
         
         aGB->mem_map[0x4] = &(aGB->cartridge)[cart_address+0x4000];
         aGB->mem_map[0x5] = &(aGB->cartridge)[cart_address+0x5000];
         aGB->mem_map[0x6] = &(aGB->cartridge)[cart_address+0x6000];
         aGB->mem_map[0x7] = &(aGB->cartridge)[cart_address+0x7000];
         return;      
      }
      return;  
   }
   
   if(address < 0x8000) 
   {
      aGB->MBC1memorymodel = (data&0x01);
      return;
   }
   
  /* if(address >= 0xA000 && address < 0xC000)
   {
      if((!RAMenable || !rom->RAMsize) && !RAM_always_enable)
         return;
   }*/
   
   if(aGB->io_reg_write(address,data)) return;
   
   aGB->mem_map[address>>12][address&0x0FFF] = data;
}
//-------------------------------------------------------------------------
// writememory_Rockman8:
// for Rockman8
// only game that requires memory echo emulation ?
//-------------------------------------------------------------------------
void gb_mbc::writememory_Rockman8(register unsigned short address,register byte data)
{
   if(address < 0x2000)
      return;  
   
   if(address < 0x4000) // Is it a ROM bank switch?
   { 
      data &= 0x1F;
         
      if(data == 0)
         data = 1;

      if(data > maxROMbank[aGB->rom->ROMsize])
         data -= 8; // <--- MAKE IT WORK!!!
         
      aGB->rom_bank = data;
          
      cart_address = data<<14;
         
      cart_address &= rom_size_mask[aGB->rom->ROMsize];

      aGB->mem_map[0x4] = &(aGB->cartridge)[cart_address];
      aGB->mem_map[0x5] = &(aGB->cartridge)[cart_address+0x1000];
      aGB->mem_map[0x6] = &(aGB->cartridge)[cart_address+0x2000];
      aGB->mem_map[0x7] = &(aGB->cartridge)[cart_address+0x3000];
              
      return;
   }
     
   if(address < 0x8000) 
      return;
   
   // Always allow RAM writes.
         
   if(aGB->io_reg_write(address,data)) return;
   
   aGB->mem_map[address>>12][address&0x0FFF] = data;
}

//-------------------------------------------------------------------------
// writememory_MBC2:
// for MBC2
//-------------------------------------------------------------------------
void gb_mbc::writememory_MBC2(register unsigned short address,register byte data)
{
   if(address < 0x2000)// Is it a RAM bank enable/disable?
   {
      if(!(address&0x0100))
         aGB->RAMenable =  (data&0x0F) == 0x0A;
      return;  
   }
   
   if(address < 0x4000) // Is it a ROM bank switch?
   { 
      //if(address&0x0100)
      {
         data &= 0x0F;
         if(data==0) 
            data=1;
         if(data > maxROMbank[aGB->rom->ROMsize])
            data = maxROMbank[aGB->rom->ROMsize];
         
         aGB->rom_bank = data;
         
         int cadr = data<<14;
         aGB->mem_map[0x4] = &(aGB->cartridge)[cadr];
         aGB->mem_map[0x5] = &(aGB->cartridge)[cadr+0x1000];
         aGB->mem_map[0x6] = &(aGB->cartridge)[cadr+0x2000];
         aGB->mem_map[0x7] = &(aGB->cartridge)[cadr+0x3000];
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
   
   if(aGB->io_reg_write(address,data)) return;
   
   aGB->mem_map[address>>12][address&0x0FFF] = data;
}

void gb_mbc::rtc_update()
{
   if(aGB->rtc.control&0x40)
   {
       aGB->rtc.last_time = time(0);
      return;
   }
   
   time_t now = time(0);
   time_t diff = now-aGB->rtc.last_time;
   if(diff > 0)
   {
    aGB->rtc.s += diff % 60;
    if(aGB->rtc.s > 59)
    {
      aGB->rtc.s -= 60;
      aGB->rtc.m++;
    }

    diff /= 60;

    aGB->rtc.m += diff % 60;
    if(aGB->rtc.m > 59)
    {
      aGB->rtc.m -= 60;
      aGB->rtc.h++;
    }

    diff /= 60;

    aGB->rtc.h += diff % 24;
    if(aGB->rtc.h > 24)
    {
      aGB->rtc.h -= 24;
      aGB->rtc.d++;
    }
    diff /= 24;

    aGB->rtc.d += diff;
    if(aGB->rtc.d > 255)
    {
      if(aGB->rtc.d > 511)
      {
         aGB->rtc.d %= 512;
         aGB->rtc.control |= 0x80;
      }
      aGB->rtc.control = (aGB->rtc.control & 0xfe) | (aGB->rtc.d>255 ? 1 : 0);
    }
  }
  aGB->rtc.last_time = now;
}

//-------------------------------------------------------------------------
// writememory_poke:
// for Pokemon Red & Blue 2-in-1
//-------------------------------------------------------------------------
int bank0_change = 0;

void gb_mbc::writememory_poke(register unsigned short address,register byte data)
{
   if(address < 0x2000)// Is it a RAM bank enable/disable?
   {
      aGB->RAMenable = ( (data&0x0A) == 0x0A ? 1 : 0);
      bank0_change = ( (data&0xC0) == 0xC0 ? 1 : 0);
      return;  
   }
   
   if(address < 0x4000) // Is it a ROM bank switch?
   {      
      data &= 0x7F;  
      if(data==0) 
         data=1;
      data += aGB->MBChi;
         
      aGB->rom_bank = data;
         
      int cadr = data<<14;
      cadr &= rom_size_mask[aGB->rom->ROMsize];
      aGB->mem_map[0x4] = &(aGB->cartridge)[cadr];
      aGB->mem_map[0x5] = &(aGB->cartridge)[cadr+0x1000];
      aGB->mem_map[0x6] = &(aGB->cartridge)[cadr+0x2000];
      aGB->mem_map[0x7] = &(aGB->cartridge)[cadr+0x3000];
      return;
   }
   
   if(address < 0x6000) // Is it a RAM bank switch?
   {                 
      if(aGB->rom->RAMsize <= 2) // no need to change it if there isn't over 8KB ram
         return;

      data &= 0x03;
         
      if(data > maxRAMbank[aGB->rom->RAMsize])
         data = maxRAMbank[aGB->rom->RAMsize];
      
      aGB->ram_bank = data;
      
      int madr = data<<13;
      aGB->mem_map[0xA] = &(aGB->cartRAM)[madr];
      aGB->mem_map[0xB] = &(aGB->cartRAM)[madr+0x1000];
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
         aGB->MBClo = 0;
         if(data==1)
            aGB->MBChi = 2;
         else 
         if(data!=0xc0)
            aGB->MBChi = 66;
         else
            bc_select = 1;
                  
         cart_address = aGB->MBChi<<14;

         aGB->mem_map[0x0] = &(aGB->cartridge)[cart_address];
         aGB->mem_map[0x1] = &(aGB->cartridge)[cart_address+0x1000];
         aGB->mem_map[0x2] = &(aGB->cartridge)[cart_address+0x2000];
         aGB->mem_map[0x3] = &(aGB->cartridge)[cart_address+0x3000];
         
         aGB->mem_map[0x4] = &(aGB->cartridge)[cart_address+0x4000];
         aGB->mem_map[0x5] = &(aGB->cartridge)[cart_address+0x5000];
         aGB->mem_map[0x6] = &(aGB->cartridge)[cart_address+0x6000];
         aGB->mem_map[0x7] = &(aGB->cartridge)[cart_address+0x7000];
         return;
      }
      //if(!RAMenable || !rom->RAMsize)
      //   return;
   }
   
   if(aGB->io_reg_write(address,data)) return;
   
   aGB->mem_map[address>>12][address&0x0FFF] = data;
}

//-------------------------------------------------------------------------
// writememory_MBC3:
// for MBC3
//-------------------------------------------------------------------------
void gb_mbc::writememory_MBC3(register unsigned short address,register byte data)
{
   if(address < 0x2000)// Is it a RAM bank enable/disable?
   {
      aGB->RAMenable = ( (data&0x0A) == 0x0A ? 1 : 0);
      return;  
   }
   
   if(address < 0x4000) // Is it a ROM bank switch?
   {  
      data &= 0x7F;  
      if(data==0) 
         data=1;
         
      aGB->rom_bank = data;
         
      int cadr = data<<14;
      cadr &= rom_size_mask[aGB->rom->ROMsize];
      aGB->mem_map[0x4] = &(aGB->cartridge)[cadr];
      aGB->mem_map[0x5] = &(aGB->cartridge)[cadr+0x1000];
      aGB->mem_map[0x6] = &(aGB->cartridge)[cadr+0x2000];
      aGB->mem_map[0x7] = &(aGB->cartridge)[cadr+0x3000];
      return;
   }
   
   if(address < 0x6000) // Is it a RAM bank switch?
   {                
      if(aGB->rom->RTC && data>8)
      {
         aGB->RTCIO = 1;
         aGB->rtc.cur_register = data;

         return;
      } else aGB->RTCIO = 0;
      
      if(aGB->rom->RAMsize <= 2) // no need to change it if there isn't over 8KB ram
         return;

      data &= 0x03;
         
      if(data > maxRAMbank[aGB->rom->RAMsize])
         data = maxRAMbank[aGB->rom->RAMsize];
      
      aGB->ram_bank = data;
      
      int madr = data<<13;
      aGB->mem_map[0xA] = &(aGB->cartRAM)[madr];
      aGB->mem_map[0xB] = &(aGB->cartRAM)[madr+0x1000];
      return;  
   }
   
   if(address < 0x8000)
   {   
      if(data == 1)
      {
         rtc_update();
         aGB->RTC_latched = !aGB->RTC_latched;
         aGB->rtc_latch = aGB->rtc;
      }
      return;
   }
   
   if(address >= 0xA000 && address < 0xC000)
   {  
      if(aGB->RAMenable && aGB->rom->RTC && aGB->RTCIO)
      {
         time(&(aGB->rtc).last_time);
         switch(aGB->rtc.cur_register)
         {
            case 0x08:
               aGB->rtc.s = data;
            break;
            case 0x09:
               aGB->rtc.m = data;
            break;            
            case 0x0A:
               aGB->rtc.h = data;
            break;            
            case 0x0B:
               aGB->rtc.d = data;
            break;            
            case 0x0C:
               aGB->rtc.control = data;
               aGB->rtc.d |= (data&1)<<8;
            break;            
         }
      }
      //if(!RAMenable || !rom->RAMsize)
      //   return;
   }
   
   if(aGB->io_reg_write(address,data)) return;
   
   aGB->mem_map[address>>12][address&0x0FFF] = data;
}

//-------------------------------------------------------------------------
// writememory_MBC5:
// for MBC5 and MBC5 rumble
//-------------------------------------------------------------------------
void gb_mbc::writememory_MBC5(register unsigned short address,register byte data,bool isNiutoude,bool isSintax)
{
    bool vfmulti = true;
    if ( vfmulti && !bc_select ) {
            
        if ( /*address & 0xF000 == 0x5000*/ address >= 0x5000 && address <= 0x5FFF ) {
            vfmultimode = data;
        }
        if ( /*address & 0xF000 == 0x7000*/ address >= 0x7000 && address <= 0x7FFF ) {
            bool effectChange = false;
            switch( vfmultimode ) {
                case 0xAA:
                    if ( vfmultibank == 0 ) {
                        vfmultibank = data;
                    } else {
                        effectChange = true;
                    }
                    break;
                case 0xBB:
                    vfmultimem = data;
                    break;
                case 0x55:
                    vfmultifinal = data;
                    effectChange = true;
                    break;
            }
            if ( effectChange ) {
                    
                byte size = vfmultifinal & 0x07;
                byte eightMegBankNo = ( vfmultifinal & 0x08 ) >> 3; // 0 or 1
                byte doReset = ( vfmultifinal & 0x80 ) >> 7; // 0 or 1 again
                
                int addroffset = vfmultibank << 15;
                addroffset += (eightMegBankNo << 0x17);
                aGB->superaddroffset = addroffset;
                
                wchar_t wrmessage[50];
                wsprintf(wrmessage,L"MM %X %X",aGB->superaddroffset,vfmultibank);
                renderer.showMessage(wrmessage,60,GB1);
                
                aGB->mem_map[0x0] = &(aGB->cartridge)[addroffset];
                aGB->mem_map[0x1] = &(aGB->cartridge)[addroffset+0x1000];
                aGB->mem_map[0x2] = &(aGB->cartridge)[addroffset+0x2000];
                aGB->mem_map[0x3] = &(aGB->cartridge)[addroffset+0x3000];
                
                aGB->mem_map[0x4] = &(aGB->cartridge)[addroffset+0x4000];
                aGB->mem_map[0x5] = &(aGB->cartridge)[addroffset+0x5000];
                aGB->mem_map[0x6] = &(aGB->cartridge)[addroffset+0x6000];
                aGB->mem_map[0x7] = &(aGB->cartridge)[addroffset+0x7000];
                
                // todo: Do the bank switch, changes the effective ROM..
                // todo: Do the memory switch
                if ( doReset ) {
                      GB1->reset(true,true);
                  /*  GB1->load_rom(L"dodgy-hardcoded-path",superaddroffset); // reload the ROM from a new offset
                      superaddroffset = 0;
                      GB1->reset();
                    */
                  // IT SEEMS that if we do a reset here it just doesnt fuck work properly anyway? 
                  // Need to re-reset thru the menu (assuming superaddroffset not changed) & then it works
                }
                
               if ( vfmultifinal>0) bc_select = 1;
                
                vfmultimode=0; vfmultibank=0; vfmultimem=0; vfmultifinal = 0;
            
                return;
            }
        }
    }
    
   if(address < 0x2000)// Is it a RAM bank enable/disable?
   {
      aGB->RAMenable = ( (data&0x0A) == 0x0A ? 1 : 0);
      return;  
   }
   
   if(address < 0x3000)
   {    
	   	// 2100 needs to be not-ignored (for Cannon Fodder's sound)
	   	// but 2180 DOES need to be ignored (for FF DX3)
	   	// Determined to find the right number here
      if (isNiutoude && address > 0x2100) {
      	return;
      }
      byte origData = data;
      
      if (isSintax) {
      	switch(aGB->sintax_mode & 0x0f) {
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

      		case 0x00: { // 0x10=lion 0x00 hmmmmm // 1 and 0 unconfirmed
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
      		
      		case 0x07: {
      			byte flips[] = {5,7,4,6,2,3,0,1}; // 5 and 7 unconfirmed
      			data = switchOrder( data, flips );
      			break;
      		}
      		
      		case 0x0B: {
      			byte flips[] = {5,4,7,6,1,0,3,2}; // 5 and 6 unconfirmed
      			data = switchOrder( data, flips );
      			break;
      		}

      	}
      	
      	setXorForBank(origData);
    
      }
    
	
      // Set current xor so we dont have to figure it out on every read
      
      aGB->rom_bank = data|(aGB->MBChi<<8);
      cart_address = aGB->rom_bank<<14;
      
      cart_address &= rom_size_mask[aGB->rom->ROMsize];
     
      cart_address += aGB->superaddroffset;
      
      aGB->MBClo = data;

      aGB->mem_map[0x4] = &(aGB->cartridge)[cart_address];
      aGB->mem_map[0x5] = &(aGB->cartridge)[cart_address+0x1000];
      aGB->mem_map[0x6] = &(aGB->cartridge)[cart_address+0x2000];
      aGB->mem_map[0x7] = &(aGB->cartridge)[cart_address+0x3000];
      
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
                 
      aGB->rom_bank = aGB->MBClo|(data<<8);
         
      cart_address = aGB->rom_bank<<14;
    
      cart_address &= rom_size_mask[aGB->rom->ROMsize];
      
      cart_address += aGB->superaddroffset;
      
      aGB->MBChi = data;

      aGB->mem_map[0x4] = &(aGB->cartridge)[cart_address];
      aGB->mem_map[0x5] = &(aGB->cartridge)[cart_address+0x1000];
      aGB->mem_map[0x6] = &(aGB->cartridge)[cart_address+0x2000];
      aGB->mem_map[0x7] = &(aGB->cartridge)[cart_address+0x3000];
      
      return;
   }
   
   if(address < 0x6000) // Is it a RAM bank switch?
   {        

   		// sintaxs not entirely understood addressing thing hi  
   		
   		// check sintax_mode was not already set; if it was, ignore it (otherwise Metal Max breaks)
   		if (isSintax && aGB->sintax_mode ==0 && address >= 0x5000 ) {
   					
   		 switch(0x0F & data) {
   		 	case 0x0D: // old
   		 	case 0x09: // ???
   		 	case 0x00:// case 0x10: // LiON, GoldenSun
   		 	case 0x01: // LANGRISSER
   		 	case 0x05: // Maple, PK Platinum
   		 	case 0x07: // Bynasty5
   		 	case 0x0B: // Shaolin
   		 		// These are all supported
   		 	break;

   		 	default:
	 			char buff[100];
   		 		sprintf(buff,"Unknown Sintax Mode %X Addr %X - probably won't work!",data,address);
   		 		debug_print(buff);
   			break;
   		 }
   		 aGB->sintax_mode=data;
   		 
   		 writememory_MBC5(0x2000,01,false,true); // force a fake bank switch
   		 
   		 return;

   		}
   
      if(aGB->rom->rumble)
      {
         if(data&0x08)
            aGB->rumble_counter = 4;
         data &= 0x07;
      }
      
      if(aGB->rom->RAMsize <= 2) // no need to change it if there isn't over 8KB ram
         return;
         
      data &= 0x0F;
         
      if(data > maxRAMbank[aGB->rom->RAMsize])
         data = maxRAMbank[aGB->rom->RAMsize];
      
      aGB->ram_bank = data;
      
      int madr = data<<13;
      aGB->mem_map[0xA] = &(aGB->cartRAM)[madr];
      aGB->mem_map[0xB] = &(aGB->cartRAM)[madr+0x1000];
      return;  
   }
   
   if(address<0x8000)
   {
   		if ( isSintax && address >= 0x7000 ) {

   			int xorNo = ( address & 0x00F0 ) >> 4;
   			switch (xorNo) {
   				case 2:
   					aGB->sintax_xor2 = data;
   				break;
   				case 3:
   					aGB->sintax_xor3 = data;
   				break;
   				case 4:
   					aGB->sintax_xor4 = data;
   				break;
   				case 5:
					aGB->sintax_xor5 = data;
   				break;
   			}
   			
   			if (aGB->sintax_currentxor == 0 ) {
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
   
   if(aGB->io_reg_write(address,data)) return;
   
   aGB->mem_map[address>>12][address&0x0FFF] = data;
}

//-------------------------------------------------------------------------
// writememory_Camera:
// for the Gameboy Pocket Camera
//-------------------------------------------------------------------------
void gb_mbc::writememory_Camera(register unsigned short address,register byte data)
{
   if(address < 0x2000)// Is it a RAM bank enable/disable?
   {
      aGB->RAMenable = ( (data&0x0A) == 0x0A ? 1 : 0);
      return;  
   }
      
   if(address < 0x4000) // Is it a ROM bank switch?
   { 
      if(data == 0)
         data = 1;
      if(data > maxROMbank[aGB->rom->ROMsize])
         data = maxROMbank[aGB->rom->ROMsize];
         
      aGB->rom_bank = data;
         
      int cadr = data<<14;
      aGB->mem_map[0x4] = &(aGB->cartridge)[cadr];
      aGB->mem_map[0x5] = &(aGB->cartridge)[cadr+0x1000];
      aGB->mem_map[0x6] = &(aGB->cartridge)[cadr+0x2000];
      aGB->mem_map[0x7] = &(aGB->cartridge)[cadr+0x3000];
      return; 
   }
   
   if(address < 0x6000) // Is it a RAM bank switch?
   {               
      if(data == 0x10)
      {
         aGB->cameraIO = 1;
         return;
      }
      else
         aGB->cameraIO = 0;
         
      data &= 0x0F;
         
      if(data > maxRAMbank[aGB->rom->RAMsize])
         data = maxRAMbank[aGB->rom->RAMsize];
      
      aGB->ram_bank = data;
      
      int madr = data<<13;
      aGB->mem_map[0xA] = &(aGB->cartRAM)[madr];
      aGB->mem_map[0xB] = &(aGB->cartRAM)[madr+0x1000];
      return;  

   }
   
   if(address<0x8000)
      return;
   
 /*  if(address >= 0xA000 && address < 0xC000)
   {
      if(!RAMenable)
         return;
   }*/
   
   if(aGB->io_reg_write(address,data)) return;
   
   aGB->mem_map[address>>12][address&0x0FFF] = data;
}

void gb_mbc::update_HuC3time()
{  
   time_t now = time(0);
   time_t diff = now-HuC3_last_time;
   if(diff > 0)
   {
      aGB->rtc.s += diff % 60; // use rtc.s to store seconds
      if(aGB->rtc.s > 59)
      {
          aGB->rtc.s -= 60;
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
byte gb_mbc::readmemory_HuC3(register unsigned short address)
{            
   if(address >= 0xA000 && address < 0xC000)
   {
      if(HuC3_RAMflag >= 0x0b && HuC3_RAMflag < 0x0e)
      {        
         if(HuC3_RAMflag == 0x0D)
            return 1;            
         return HuC3_RAMvalue;
      }
      if(!aGB->rom->RAMsize)
         return 0xFF;
   }
   
   return aGB->io_reg_read(address);
}

//-------------------------------------------------------------------------
// writememory_HuC3:
// for HuC-3
//-------------------------------------------------------------------------
void gb_mbc::writememory_HuC3(register unsigned short address,register byte data)
{
   if(address < 0x2000)// Is it a RAM bank enable/disable?
   {
      aGB->RAMenable = ( (data&0x0A) == 0x0A ? 1 : 0);
      HuC3_RAMflag = data;
      return;  
   }
      
   if(address < 0x4000) // Is it a ROM bank switch?
   { 
      if(data == 0)
         data = 1;
      if(data > maxROMbank[aGB->rom->ROMsize])
         data = maxROMbank[aGB->rom->ROMsize];
         
      aGB->rom_bank = data;
         
      int cadr = data<<14;
      aGB->mem_map[0x4] = &(aGB->cartridge)[cadr];
      aGB->mem_map[0x5] = &(aGB->cartridge)[cadr+0x1000];
      aGB->mem_map[0x6] = &(aGB->cartridge)[cadr+0x2000];
      aGB->mem_map[0x7] = &(aGB->cartridge)[cadr+0x3000];
      return; 
   }
   
   if(address < 0x6000) // Is it a RAM bank switch?
   {                        
      data &= 0x0F;
         
      if(data > maxRAMbank[aGB->rom->RAMsize])
         data = maxRAMbank[aGB->rom->RAMsize];
      
      aGB->ram_bank = data;
      
      int madr = data<<13;
      aGB->mem_map[0xA] = &(aGB->cartRAM)[madr];
      aGB->mem_map[0xB] = &(aGB->cartRAM)[madr+0x1000];
      return;  
   }
   
   if(address < 0x8000) // programs will write 1 here
      return;
   
   if(address >= 0xA000 && address < 0xC000)
   { 
      if(HuC3_RAMflag < 0x0b || HuC3_RAMflag > 0x0e) // write to RAM
      {
         if(!aGB->RAMenable || !aGB->rom->RAMsize)
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
   
   if(aGB->io_reg_write(address,data)) return;
   
   aGB->mem_map[address>>12][address&0x0FFF] = data;
}

//-------------------------------------------------------------------------
// writememory_MBC7:
// for MBC7
//-------------------------------------------------------------------------
void gb_mbc::writememory_MBC7(register unsigned short address,register byte data)
{
   if(address < 0x2000)
      return;  
   
   if(address < 0x4000) // Is it a ROM bank switch?
   { 
      data = data&0x7F;

      if(data==0)
         data=1;


       aGB->rom_bank = data;
         
      cart_address = data<<14;

      cart_address &= rom_size_mask[aGB->rom->ROMsize];

       aGB->mem_map[0x4] = &(aGB->cartridge)[cart_address];
       aGB->mem_map[0x5] = &(aGB->cartridge)[cart_address+0x1000];
       aGB->mem_map[0x6] = &(aGB->cartridge)[cart_address+0x2000];
       aGB->mem_map[0x7] = &(aGB->cartridge)[cart_address+0x3000];
      return;
   }
   
   if(address < 0x6000) // Is it a RAM bank switch?
   {               
      if(data<8)
      {
          aGB->RAMenable = 0;
         
         if(aGB->rom->RAMsize <= 2) // no need to change it if there isn't over 8KB ram
            return;
         
         data &= 0x03;
         
         if(data > maxRAMbank[aGB->rom->RAMsize])
            data = maxRAMbank[aGB->rom->RAMsize];

          aGB->ram_bank = data;
      
         int madr = data<<13;
          aGB->mem_map[0xA] = &(aGB->cartRAM)[madr];
          aGB->mem_map[0xB] = &(aGB->cartRAM)[madr+0x1000];
         return;  
      } else
      {
          aGB->RAMenable=0;
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
                   aGB->memory[0xa000+MBC7_address*2] = MBC7_buffer>>8;
                   aGB->memory[0xa000+MBC7_address*2+1] = MBC7_buffer&0xff;
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
                                 aGB->memory[0xa000+i*2] = MBC7_buffer >> 8;
                                 aGB->memory[0xa000+i*2+1] = MBC7_buffer & 0xff;
                             }
                          }
                          MBC7_state = 5;
                       } else if((MBC7_address>>6) == 2)
                       {
                          if(MBC7_writeEnable)
                          {
                             for(int i=0;i<256;i++)
                                *((unsigned short *)&(aGB->memory)[0xa000+i*2]) = 0xffff;
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
                       MBC7_buffer = (aGB->memory[0xa000+MBC7_address*2]<<8)|(aGB->memory[0xa000+MBC7_address*2+1]);
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
   
   if(aGB->io_reg_write(address,data)) return;

    aGB->mem_map[address>>12][address&0x0FFF] = data;
}

//-------------------------------------------------------------------------
// readmemory_MBC7:
// for MBC7
//-------------------------------------------------------------------------
byte gb_mbc::readmemory_MBC7(register unsigned short address)
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
   
   return aGB->io_reg_read(address);
}

//-------------------------------------------------------------------------
// readmemory_TAMA5:
// for Bandai TAMA5 (Tamagotchi3)
//-------------------------------------------------------------------------

void gb_mbc::update_tama_RTC()
{
   time_t now = time(0);
   time_t diff = now-aGB->rtc.last_time;
   if(diff > 0)
   {
    aGB->rtc.s += diff % 60;
    if(aGB->rtc.s > 59)
    {
      aGB->rtc.s -= 60;
      aGB->rtc.m++;
    }

    diff /= 60;

    aGB->rtc.m += diff % 60;
    if(aGB->rtc.m > 59)
    {
      aGB->rtc.m -= 60;
      aGB->rtc.h++;
    }

    diff /= 60;

    aGB->rtc.h += diff % 24;
    if(aGB->rtc.h > 24)
    {
      aGB->rtc.h -= 24;
      aGB->rtc.d++;
    }
    diff /= 24;

    aGB->rtc.d += diff;
    if(aGB->rtc.d > 31)
    {
      aGB->rtc.d -= 31;
      tama_month++;
      if(tama_month > 12)
         tama_month -= 12;
    }
  }
  aGB->rtc.last_time = now;
}

byte gb_mbc::readmemory_TAMA5(register unsigned short address)
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
                     read = aGB->rtc.m;
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
                     read = aGB->rtc.h;
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
               read = aGB->memory[0xA000|(tama_val6<<4)|tama_val7];
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
               if(aGB->rtc.s == 0)
                  read = 0;
               else
                  read = aGB->rtc.s - (tama_time+1); // return 0 if second has passed
                tama_time = aGB->rtc.s;
            break;
            case 1:   
               if(tama_val6 == 0x04)
               {              
                  if(tama_val7 == 0x06) // get minutes lower nibble
                  {
                     read = aGB->rtc.m;
                     
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
                     read = aGB->rtc.h;
                     
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
               read = aGB->rtc.d&0xF;
            break;
            case 8: // day high
               read = ((aGB->rtc.d&0xf0)>>4);
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
   
   return aGB->io_reg_read(address);
}

//-------------------------------------------------------------------------
// writememory_TAMA5:
// for Bandai TAMA5 (Tamagotchi3)
//-------------------------------------------------------------------------
void gb_mbc::writememory_TAMA5(register unsigned short address,register byte data)
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

             aGB->MBClo = data;
             aGB->rom_bank = aGB->MBClo|(aGB->MBChi<<4);
       
            int cadr = aGB->rom_bank<<14;
             aGB->mem_map[0x4] = &(aGB->cartridge)[cadr];
             aGB->mem_map[0x5] = &(aGB->cartridge)[cadr+0x1000];
             aGB->mem_map[0x6] = &(aGB->cartridge)[cadr+0x2000];
             aGB->mem_map[0x7] = &(aGB->cartridge)[cadr+0x3000];
         }
         return;
         case 1: // rom bank high bit
         {
            data &= 0x01;
            
            aGB->MBChi = data;
            
            aGB->rom_bank = aGB->MBClo|(aGB->MBChi<<4);
       
            int cadr = aGB->rom_bank<<14;
            aGB->mem_map[0x4] = &(aGB->cartridge)[cadr];
            aGB->mem_map[0x5] = &(aGB->cartridge)[cadr+0x1000];
            aGB->mem_map[0x6] = &(aGB->cartridge)[cadr+0x2000];
            aGB->mem_map[0x7] = &(aGB->cartridge)[cadr+0x3000];
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
            if(tama_change_clock == 3) aGB->rtc.last_time = time(0);

             aGB->memory[0xA000+(tama_val6<<4)+tama_val7] = tama_val4|(data<<4);
            
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
               aGB->rtc.d = data;
            } else
            if(tama_count==9 && tama_change_clock==3)
            {
                aGB->rtc.d += data*10;
            } else
            if(tama_count==10 && tama_change_clock==3)
            {
                aGB->rtc.m = data*10;
            } else
            if(tama_count==11 && tama_change_clock==3)
            {
                aGB->rtc.h = data*10;
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

   if(aGB->io_reg_write(address,data)) return;

    aGB->mem_map[address>>12][address&0x0FFF] = data;
}
