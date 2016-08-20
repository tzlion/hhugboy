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

#include "cpu.h"
#include "debug.h"
#include "sound.h"
#include "SGB.h"
#include "GB_gfx.h"
#include "GB.h"
#include "memory/GB_MBC.h"
#include "mainloop.h"

#include "main.h"

#define WIN32_LEAN_AND_MEAN
#define UNICODE 

#include <windows.h>
#include <winbase.h>
#include <stdlib.h>
#include <stdio.h>

int GB1_state_slot = 0;
int GB2_state_slot = 0;

extern int ramsize[6];
extern int sensorX;
extern int sensorY;

#include "strings.h"
#include "rom.h"
#include "config.h"
#include "memory/GB_MBC.h"

/*
byte A;
byte F; 
word BC;
word DE;
word HL;
word SP;
word PC; 

unsigned short flags;
*/
int emulating = 1;

int sgb_mode = 0;

byte opcode = 0x00;

int cycles_SGB = 0;

int cur_cycle = 0;
/*
int CFLAG = 1;
int HFLAG = 1;
int ZFLAG = 1;
int NFLAG = 0;
*/
int multiple_gb = 0;
int gb_speed_another = 0;

/*void init_gb1()
{
   if(current_gb == 1)
      switch_gb();
      
   stored_readmemory = NULL;
   stored_writememory = NULL;
   
   switch_gb();  
}*/

bool init_gb2()
{
   if(sgb_mode) 
   { // don't allow SGB mode
       multiple_gb = 1;
       GB1->reset();
       multiple_gb = 0;
   }
          
   if(GB2 != NULL)
   {
      multiple_gb = 1;
      
      return true;
   }
            
   GB2 = new gb_system;
   if(!GB2 || !GB2->init() || !GB2->init_gfx())
   { 
      debug_print(str_table[ERROR_MEMORY]); 

      return false;  
   }

   GB1->memory_another = GB2->memory;
   GB2->memory_another = GB1->memory;   

   multiple_gb = 1;

   return true;
}

/*void switch_gb()
{
   if(!GB2 || GB2->cartridge == NULL)
      return;

   if(GB == GB2)
   {
      GB = GB1;
   } else
   {
      GB = GB2;
   }
}*/

bool gb_system::save_state()
{
   
   wchar_t old_directory[PROGRAM_PATH_SIZE]; // this needs unicode too
   GetCurrentDirectory(PROGRAM_PATH_SIZE,old_directory);
   if (!SetCurrentDirectory(options->state_directory.c_str())) {
   		CreateDirectory(options->state_directory.c_str(),NULL);
   		SetCurrentDirectory(options->state_directory.c_str());
   }
   
   wchar_t save_filename[275]; 
   wchar_t file_ext[5];
   wcscpy(save_filename,rom_filename);

   int save_state_slot = 0;

   if(this == GB1)
   {
      save_state_slot =  GB1_state_slot;
      swprintf(file_ext,L".st%d",GB1_state_slot);
      wcscat(save_filename,file_ext);
   } else
   {
      save_state_slot = GB2_state_slot;
      swprintf(file_ext,L".s2%d",GB2_state_slot);
      wcscat(save_filename,file_ext);
   }

   FILE* statefile = _wfopen(save_filename,L"wb");
   if(!statefile) 
   {       
      wchar_t dx_message[100];
      wsprintf(dx_message,L"%s %d %s",str_table[SAVE_TO_SLOT],save_state_slot,str_table[SAVE_FAILED]);
      renderer.showMessage(dx_message,60,this);
      SetCurrentDirectory(old_directory);
      return false; 
   }
   
   //CFLAG = (flags&1)>>8;
   //HFLAG = ((flags>>12)&1);
   //ZFLAG = ((flags>>14)&1);
   
   fputc((byte)gbc_mode,statefile);
   
   fputc(A,statefile);
   fputc(F,statefile);
   fputc(BC.B.l,statefile);
   fputc(BC.B.h,statefile);
   fputc(DE.B.l,statefile);
   fputc(DE.B.h,statefile);
   fputc(HL.B.l,statefile);
   fputc(HL.B.h,statefile);
   fputc(SP.B.l,statefile);
   fputc(SP.B.h,statefile);
   fputc(PC.B.l,statefile);
   fputc(PC.B.h,statefile);
   fputc((byte)IME,statefile);
   fputc((byte)CPUHalt,statefile);
   fputc((byte)EI_count,statefile);
   fputc((byte)CFLAG,statefile);
   fputc((byte)HFLAG,statefile);
   fputc((byte)ZFLAG,statefile);
   fputc((byte)NFLAG,statefile);
   fputc((byte)gb_speed,statefile);
   fwrite(&cycles_DIV, sizeof(int),1,statefile);
   fwrite(&cycles_LCD, sizeof(int),1,statefile);
   fwrite(&cycles_timer, sizeof(int),1,statefile);   
   fwrite(&timer_freq, sizeof(int),1,statefile);   
   fwrite(&cycles_serial, sizeof(int),1,statefile);
   fwrite(&cycles_sound, sizeof(int),1,statefile);
   fwrite(&timeron, sizeof(int),1,statefile);
   fwrite(&LCDon, sizeof(int),1,statefile);   
   fwrite(&hdma_on, sizeof(int),1,statefile);   
   fwrite(&hdma_source, sizeof(int),1,statefile);  
   fwrite(&hdma_destination, sizeof(int),1,statefile);  
   fwrite(&hdma_bytes, sizeof(int),1,statefile);
   mbc->writeMbcBanksToStateFile(statefile);
    fwrite(&wram_bank, sizeof(int),1,statefile);
   fwrite(&vram_bank, sizeof(int),1,statefile);
   mbc->writeMbcOtherStuffToStateFile(statefile);
    fwrite(&sound_on, sizeof(int),1,statefile);
   fwrite(&sound_index, sizeof(int),1,statefile);           
   fwrite(&sound_buffer_index, sizeof(int),1,statefile);           
   fwrite(&sound_balance, sizeof(int),1,statefile);           
   fwrite(&sound_level1, sizeof(int),1,statefile);           
   fwrite(&sound_level2, sizeof(int),1,statefile);           
   fwrite(&sound_digital, sizeof(int),1,statefile);           
   fwrite(&channel1_on, sizeof(int),1,statefile);           
   fwrite(&channel1_ATL, sizeof(int),1,statefile);           
   fwrite(&channel1_skip, sizeof(int),1,statefile);              
   fwrite(&channel1_continue, sizeof(int),1,statefile);              
   fwrite(&channel1_sweepATL, sizeof(int),1,statefile);              
   fwrite(&channel1_sweepATLreload, sizeof(int),1,statefile);              
   fwrite(&channel1_sweepdir, sizeof(int),1,statefile);              
   fwrite(&channel1_sweepsteps, sizeof(int),1,statefile);              
   fwrite(&channel1_envelopevolume, sizeof(int),1,statefile);           
   fwrite(&channel1_envelopeATL, sizeof(int),1,statefile);              
   fwrite(&channel1_envelopeATLreload, sizeof(int),1,statefile);              
   fwrite(&channel1_envelopedir, sizeof(int),1,statefile);              
   fwrite(&channel1_index, sizeof(int),1,statefile);              
   fwrite(&channel2_on, sizeof(int),1,statefile);           
   fwrite(&channel2_ATL, sizeof(int),1,statefile);
   fwrite(&channel2_skip, sizeof(int),1,statefile);
   fwrite(&channel2_continue, sizeof(int),1,statefile);   
   fwrite(&channel2_envelopevolume, sizeof(int),1,statefile);   
   fwrite(&channel2_envelopeATL, sizeof(int),1,statefile);   
   fwrite(&channel2_envelopeATLreload, sizeof(int),1,statefile);   
   fwrite(&channel2_envelopedir, sizeof(int),1,statefile);   
   fwrite(&channel2_index, sizeof(int),1,statefile);   
   fwrite(&channel3_on, sizeof(int),1,statefile);
   fwrite(&channel3_continue, sizeof(int),1,statefile);   
   fwrite(&channel3_ATL, sizeof(int),1,statefile);   
   fwrite(&channel3_skip, sizeof(int),1,statefile);   
   fwrite(&channel3_index, sizeof(int),1,statefile);   
   fwrite(&channel3_last, sizeof(int),1,statefile);   
   fwrite(&channel3_outputlevel, sizeof(int),1,statefile);   
   fwrite(&channel4_on, sizeof(int),1,statefile);
   fwrite(&channel4_clock, sizeof(int),1,statefile);   
   fwrite(&channel4_ATL, sizeof(int),1,statefile);   
   fwrite(&channel4_skip, sizeof(int),1,statefile);   
   fwrite(&channel4_index, sizeof(int),1,statefile);   
   fwrite(&channel4_shiftright, sizeof(int),1,statefile);   
   fwrite(&channel4_shiftskip, sizeof(int),1,statefile);   
   fwrite(&channel4_shiftindex, sizeof(int),1,statefile);   
   fwrite(&channel4_Nsteps, sizeof(int),1,statefile);   
   fwrite(&channel4_countdown, sizeof(int),1,statefile);   
   fwrite(&channel4_continue, sizeof(int),1,statefile);   
   fwrite(&channel4_envelopevolume, sizeof(int),1,statefile);   
   fwrite(&channel4_envelopeATL, sizeof(int),1,statefile);   
   fwrite(&channel4_envelopedir, sizeof(int),1,statefile);   
   fwrite(&channel4_envelopeATLreload, sizeof(int),1,statefile);   
   fwrite(button_pressed, sizeof(int),8,statefile);   
   fwrite(&serialbits, sizeof(int),1,statefile);   
   fwrite(&IWait, sizeof(int),1,statefile);
   fwrite(&windowline,sizeof(int),1,statefile);
   fwrite(BGP, sizeof(byte),4,statefile);   
   fwrite(OBP0, sizeof(byte),4,statefile);   
   fwrite(OBP1, sizeof(byte),4,statefile);         
   fwrite(memory+0x8000,sizeof(byte),0x8000,statefile);
   fwrite(sound_buffer, sizeof(byte),4*735,statefile);
   //fwrite(final_wave, sizeof(signed short),2*735,statefile);
      
   if(gbc_mode && rom->CGB)
   {
      fwrite(VRAM,sizeof(byte),0x4000,statefile);
      fwrite(WRAM,sizeof(byte),0x8000,statefile);  
      fwrite(GBC_BGP, sizeof(unsigned short),32,statefile);       
      fwrite(GBC_OBP, sizeof(unsigned short),32,statefile);      
   }
   
   if(rom->RAMsize > 2)
   {
      fwrite(cartRAM,sizeof(byte),ramsize[rom->RAMsize]*1024,statefile);
   }

    mbc->writeCartSpecificVarsToStateFile(statefile);

    if(sgb_mode)
   {
      fputc((byte)sgb_mode,statefile);
      fwrite(&bit_received,sizeof(int),1,statefile);
      fwrite(&bits_received,sizeof(int),1,statefile);
      fwrite(&packets_received,sizeof(int),1,statefile);
      fwrite(&sgb_state,sizeof(int),1,statefile);
      fwrite(&sgb_index,sizeof(int),1,statefile);
      fwrite(&sgb_multiplayer,sizeof(int),1,statefile);
      fwrite(&sgb_fourplayers,sizeof(int),1,statefile);
      fwrite(&sgb_nextcontrol,sizeof(int),1,statefile);
      fwrite(&sgb_readingcontrol,sizeof(int),1,statefile);
      fwrite(&sgb_mask,sizeof(int),1,statefile);
      
      fwrite(sgb_palette,sizeof(unsigned short),8*16,statefile);
      fwrite(sgb_palette_memory,sizeof(unsigned short),512*4,statefile);
      fwrite(sgb_buffer,sizeof(byte),7*16,statefile);
      fwrite(sgb_ATF,sizeof(byte),18*20,statefile);
      fwrite(sgb_ATF_list,sizeof(byte),45*20*18,statefile);
      
      fwrite(sgb_border,sizeof(byte),2048,statefile);
      fwrite(sgb_borderchar,sizeof(byte),32*256,statefile);
      fwrite(sgb_border_buffer,sizeof(unsigned short),256*224,statefile);
      
   }

    mbc->writeNewerCartSpecificVarsToStateFile(statefile);

    fclose(statefile);
   
   wchar_t dx_message[50];
   wsprintf(dx_message,L"%s %d %s",str_table[SAVE_TO_SLOT],save_state_slot,str_table[SAVE_OK]);
   renderer.showMessage(dx_message,60,this);
   
   SetCurrentDirectory(old_directory);
   
   return true;
}

bool gb_system::load_state()
{
   wchar_t old_directory[PROGRAM_PATH_SIZE];
   GetCurrentDirectory(PROGRAM_PATH_SIZE,old_directory);
   SetCurrentDirectory(options->state_directory.c_str());
   
   wchar_t save_filename[275]; 
   wchar_t file_ext[5];
   wcscpy(save_filename,rom_filename);

   int save_state_slot = 0;

   if(this == GB1)
   {
      save_state_slot =  GB1_state_slot;
      swprintf(file_ext,L".st%d",GB1_state_slot);
      wcscat(save_filename,file_ext);
   } else
   {
      save_state_slot = GB2_state_slot;
      swprintf(file_ext,L".s2%d",GB2_state_slot);
      wcscat(save_filename,file_ext);
   }
   
   FILE* statefile = _wfopen(save_filename,L"rb");
   if(!statefile) 
   { 
       wchar_t dx_message[50];
      wsprintf(dx_message,L"%s %d %s",str_table[LOAD_FROM_SLOT],save_state_slot,str_table[SAVE_FAILED]);
      renderer.showMessage(dx_message,60,this);
   
      SetCurrentDirectory(old_directory);
      return false; 
   }
   
   gbc_mode = fgetc(statefile);
   
   A = fgetc(statefile);
   F = fgetc(statefile);
   BC.B.l = fgetc(statefile);
   BC.B.h = fgetc(statefile);
   DE.B.l = fgetc(statefile);
   DE.B.h = fgetc(statefile);
   HL.B.l = fgetc(statefile);
   HL.B.h = fgetc(statefile);
   SP.B.l = fgetc(statefile);
   SP.B.h = fgetc(statefile);
   PC.B.l = fgetc(statefile);
   PC.B.h = fgetc(statefile);
   IME = fgetc(statefile);
   CPUHalt = fgetc(statefile);
   EI_count = fgetc(statefile);
   CFLAG = fgetc(statefile);
   HFLAG = fgetc(statefile);
   ZFLAG = fgetc(statefile);
   NFLAG = fgetc(statefile); 
   gb_speed = fgetc(statefile);     
   fread(&cycles_DIV, sizeof(int),1,statefile);
   fread(&cycles_LCD, sizeof(int),1,statefile);
   fread(&cycles_timer, sizeof(int),1,statefile);   
   fread(&timer_freq, sizeof(int),1,statefile);   
   fread(&cycles_serial, sizeof(int),1,statefile);
   fread(&cycles_sound, sizeof(int),1,statefile);
   fread(&timeron, sizeof(int),1,statefile);
   fread(&LCDon, sizeof(int),1,statefile);   
   fread(&hdma_on, sizeof(int),1,statefile);   
   fread(&hdma_source, sizeof(int),1,statefile);  
   fread(&hdma_destination, sizeof(int),1,statefile);  
   fread(&hdma_bytes, sizeof(int),1,statefile);
   mbc->readMbcBanksFromStateFile(statefile);
    fread(&wram_bank, sizeof(int),1,statefile);
   fread(&vram_bank, sizeof(int),1,statefile);
   mbc->readMbcMoreCrapFromStateFile(statefile);
    fread(&sound_on, sizeof(int),1,statefile);
   fread(&sound_index, sizeof(int),1,statefile);           
   fread(&sound_buffer_index, sizeof(int),1,statefile);           
   fread(&sound_balance, sizeof(int),1,statefile);           
   fread(&sound_level1, sizeof(int),1,statefile);           
   fread(&sound_level2, sizeof(int),1,statefile);           
   fread(&sound_digital, sizeof(int),1,statefile);           
   fread(&channel1_on, sizeof(int),1,statefile);           
   fread(&channel1_ATL, sizeof(int),1,statefile);           
   fread(&channel1_skip, sizeof(int),1,statefile);              
   fread(&channel1_continue, sizeof(int),1,statefile);              
   fread(&channel1_sweepATL, sizeof(int),1,statefile);              
   fread(&channel1_sweepATLreload, sizeof(int),1,statefile);              
   fread(&channel1_sweepdir, sizeof(int),1,statefile);              
   fread(&channel1_sweepsteps, sizeof(int),1,statefile);              
   fread(&channel1_envelopevolume, sizeof(int),1,statefile);           
   fread(&channel1_envelopeATL, sizeof(int),1,statefile);              
   fread(&channel1_envelopeATLreload, sizeof(int),1,statefile);              
   fread(&channel1_envelopedir, sizeof(int),1,statefile);              
   fread(&channel1_index, sizeof(int),1,statefile);              
   fread(&channel2_on, sizeof(int),1,statefile);           
   fread(&channel2_ATL, sizeof(int),1,statefile);
   fread(&channel2_skip, sizeof(int),1,statefile);
   fread(&channel2_continue, sizeof(int),1,statefile);   
   fread(&channel2_envelopevolume, sizeof(int),1,statefile);   
   fread(&channel2_envelopeATL, sizeof(int),1,statefile);   
   fread(&channel2_envelopeATLreload, sizeof(int),1,statefile);   
   fread(&channel2_envelopedir, sizeof(int),1,statefile);   
   fread(&channel2_index, sizeof(int),1,statefile);   
   fread(&channel3_on, sizeof(int),1,statefile);
   fread(&channel3_continue, sizeof(int),1,statefile);   
   fread(&channel3_ATL, sizeof(int),1,statefile);   
   fread(&channel3_skip, sizeof(int),1,statefile);   
   fread(&channel3_index, sizeof(int),1,statefile);   
   fread(&channel3_last, sizeof(int),1,statefile);   
   fread(&channel3_outputlevel, sizeof(int),1,statefile);   
   fread(&channel4_on, sizeof(int),1,statefile);
   fread(&channel4_clock, sizeof(int),1,statefile);   
   fread(&channel4_ATL, sizeof(int),1,statefile);   
   fread(&channel4_skip, sizeof(int),1,statefile);   
   fread(&channel4_index, sizeof(int),1,statefile);   
   fread(&channel4_shiftright, sizeof(int),1,statefile);   
   fread(&channel4_shiftskip, sizeof(int),1,statefile);   
   fread(&channel4_shiftindex, sizeof(int),1,statefile);   
   fread(&channel4_Nsteps, sizeof(int),1,statefile);   
   fread(&channel4_countdown, sizeof(int),1,statefile);   
   fread(&channel4_continue, sizeof(int),1,statefile);   
   fread(&channel4_envelopevolume, sizeof(int),1,statefile);   
   fread(&channel4_envelopeATL, sizeof(int),1,statefile);   
   fread(&channel4_envelopedir, sizeof(int),1,statefile);   
   fread(&channel4_envelopeATLreload, sizeof(int),1,statefile);   
   fread(button_pressed, sizeof(int),8,statefile);   
   fread(&serialbits, sizeof(int),1,statefile);   
   fread(&IWait, sizeof(int),1,statefile);
   fread(&windowline,sizeof(int),1,statefile);   
   fread(BGP, sizeof(byte),4,statefile);   
   fread(OBP0, sizeof(byte),4,statefile);   
   fread(OBP1, sizeof(byte),4,statefile);    
   fread(memory+0x8000,sizeof(byte),0x8000,statefile);
   fread(sound_buffer, sizeof(byte),4*735,statefile);
   //fread(final_wave, sizeof(signed short),2*735,statefile);
      
   if(gbc_mode && rom->CGB)
   {
      fread(VRAM,sizeof(byte),0x4000,statefile);
      fread(WRAM,sizeof(byte),0x8000,statefile);  
      fread(GBC_BGP, sizeof(unsigned short),32,statefile);       
      fread(GBC_OBP, sizeof(unsigned short),32,statefile);           
   }        
   
   if(rom->RAMsize > 2)
   {
      fread(cartRAM,sizeof(byte),ramsize[rom->RAMsize]*1024,statefile);
   }

    mbc->readCartSpecificVarsFromStateFile(statefile);

    char c = fgetc(statefile);
   
   if(c != EOF)
   {
      sgb_mode = c;
      fread(&bit_received,sizeof(int),1,statefile);
      fread(&bits_received,sizeof(int),1,statefile);
      fread(&packets_received,sizeof(int),1,statefile);
      fread(&sgb_state,sizeof(int),1,statefile);
      fread(&sgb_index,sizeof(int),1,statefile);
      fread(&sgb_multiplayer,sizeof(int),1,statefile);
      fread(&sgb_fourplayers,sizeof(int),1,statefile);
      fread(&sgb_nextcontrol,sizeof(int),1,statefile);
      fread(&sgb_readingcontrol,sizeof(int),1,statefile);
      fread(&sgb_mask,sizeof(int),1,statefile);
      
      fread(sgb_palette,sizeof(unsigned short),8*16,statefile);
      fread(sgb_palette_memory,sizeof(unsigned short),512*4,statefile);
      fread(sgb_buffer,sizeof(byte),7*16,statefile);
      fread(sgb_ATF,sizeof(byte),18*20,statefile);
      fread(sgb_ATF_list,sizeof(byte),45*20*18,statefile);
      
      fread(sgb_border,sizeof(byte),2048,statefile);
      fread(sgb_borderchar,sizeof(byte),32*256,statefile);
      fread(sgb_border_buffer,sizeof(unsigned short),256*224,statefile);     
      
      (renderer.*renderer.drawBorder)();   
   }

    mbc->readNewerCartSpecificVarsFromStateFile(statefile);

    int cadr = (mbc->getRomBank())<<14;
   mem_map[0x4] = &cartridge[cadr];
   mem_map[0x5] = &cartridge[cadr+0x1000];
   mem_map[0x6] = &cartridge[cadr+0x2000];
   mem_map[0x7] = &cartridge[cadr+0x3000];
   
   if(rom->RAMsize > 2)
   {
      int madr = (mbc->getRamBank())<<13;
      mem_map[0xA] = &cartRAM[madr];
      mem_map[0xB] = &cartRAM[madr+0x1000];          
   }
   
   if(gbc_mode)
   {
      int vramadr = vram_bank*0x2000;
      mem_map[0x08] = &VRAM[vramadr];
      mem_map[0x09] = &VRAM[vramadr + 0x1000];
   
      mem_map[0x0D] = &WRAM[wram_bank*0x1000];
   }
   
   sound_buffer_index = sound_index * 2;
   channel1_wave = sound_wave_pattern[memory[NR11] >> 6];
   channel2_wave = sound_wave_pattern[memory[NR21] >> 6];
   
   tile_map = ((memory[0xFF40] & 0x08)?0x1c00:0x1800);
   win_tile_map = ((memory[0xFF40] & 0x40)?0x1c00:0x1800);
   tile_pattern = ((memory[0xFF40] & 0x10)?0x0000:0x0800);
   
  /* flags = 0;
   flags |= CFLAG<<8;
   flags |= HFLAG<<12;
   flags |= ZFLAG<<14;*/
   
   wchar_t dx_message[50];
   wsprintf(dx_message,L"%s %d %s",str_table[LOAD_FROM_SLOT],save_state_slot,str_table[SAVE_OK]);
   renderer.showMessage(dx_message,60,this);
   fclose(statefile);
   
   SetCurrentDirectory(old_directory);
   
   return true;
}
