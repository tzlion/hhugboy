/*
   unGEST Game Boy emulator
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
#include <stdlib.h>
#include "GB_gfx.h"
#include "render.h"
#include "cpu.h"
#include "debug.h"
#include "SGB.h"
#include "GB.h"
#include "config.h"

unsigned short line_buffer[160];

int video_enable = VID_EN_BG|VID_EN_WIN|VID_EN_SPRITE;

byte invert_table[256] = 
{
  0x00,0x80,0x40,0xc0,0x20,0xa0,0x60,0xe0,
  0x10,0x90,0x50,0xd0,0x30,0xb0,0x70,0xf0,
  0x08,0x88,0x48,0xc8,0x28,0xa8,0x68,0xe8,
  0x18,0x98,0x58,0xd8,0x38,0xb8,0x78,0xf8,
  0x04,0x84,0x44,0xc4,0x24,0xa4,0x64,0xe4,
  0x14,0x94,0x54,0xd4,0x34,0xb4,0x74,0xf4,
  0x0c,0x8c,0x4c,0xcc,0x2c,0xac,0x6c,0xec,
  0x1c,0x9c,0x5c,0xdc,0x3c,0xbc,0x7c,0xfc,
  0x02,0x82,0x42,0xc2,0x22,0xa2,0x62,0xe2,
  0x12,0x92,0x52,0xd2,0x32,0xb2,0x72,0xf2,
  0x0a,0x8a,0x4a,0xca,0x2a,0xaa,0x6a,0xea,
  0x1a,0x9a,0x5a,0xda,0x3a,0xba,0x7a,0xfa,
  0x06,0x86,0x46,0xc6,0x26,0xa6,0x66,0xe6,
  0x16,0x96,0x56,0xd6,0x36,0xb6,0x76,0xf6,
  0x0e,0x8e,0x4e,0xce,0x2e,0xae,0x6e,0xee,
  0x1e,0x9e,0x5e,0xde,0x3e,0xbe,0x7e,0xfe,
  0x01,0x81,0x41,0xc1,0x21,0xa1,0x61,0xe1,
  0x11,0x91,0x51,0xd1,0x31,0xb1,0x71,0xf1,
  0x09,0x89,0x49,0xc9,0x29,0xa9,0x69,0xe9,
  0x19,0x99,0x59,0xd9,0x39,0xb9,0x79,0xf9,
  0x05,0x85,0x45,0xc5,0x25,0xa5,0x65,0xe5,
  0x15,0x95,0x55,0xd5,0x35,0xb5,0x75,0xf5,
  0x0d,0x8d,0x4d,0xcd,0x2d,0xad,0x6d,0xed,
  0x1d,0x9d,0x5d,0xdd,0x3d,0xbd,0x7d,0xfd,
  0x03,0x83,0x43,0xc3,0x23,0xa3,0x63,0xe3,
  0x13,0x93,0x53,0xd3,0x33,0xb3,0x73,0xf3,
  0x0b,0x8b,0x4b,0xcb,0x2b,0xab,0x6b,0xeb,
  0x1b,0x9b,0x5b,0xdb,0x3b,0xbb,0x7b,0xfb,
  0x07,0x87,0x47,0xc7,0x27,0xa7,0x67,0xe7,
  0x17,0x97,0x57,0xd7,0x37,0xb7,0x77,0xf7,
  0x0f,0x8f,0x4f,0xcf,0x2f,0xaf,0x6f,0xef,
  0x1f,0x9f,0x5f,0xdf,0x3f,0xbf,0x7f,0xff
};

byte tile_signed_trans[256] =
{
  0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,
  0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,
  0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,
  0xb0,0xb1,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xbb,0xbc,0xbd,0xbe,0xbf,
  0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,
  0xd0,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0xdb,0xdc,0xdd,0xde,0xdf,
  0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0xea,0xeb,0xec,0xed,0xee,0xef,
  0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfd,0xfe,0xff,
  0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
  0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
  0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
  0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
  0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
  0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,
  0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,
  0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f
};

// for GB and SGB
void gb_system::draw_line_tile_DMG()
{
   byte * bank0 = &memory[0x8000];

   register int x = 0;
   int y = memory[0xFF44];

   int sx = memory[0xFF43];
   int sy = memory[0xFF42];
     
   sy += y;

   sy &= 255;
    
   int tx = sx >> 3;
   int ty = sy >> 3;

   int bx = 1 << (7 - (sx & 7));
   int by = sy & 7;

   int tile_map_line_y = tile_map + (ty<<5); 
  
   int tile_map_address = tile_map_line_y + tx;

   byte tile = bank0[tile_map_address];
     
   ++tile_map_address;
  
   if(tile_pattern) 
      tile = tile_signed_trans[tile];
   //{ 
   //   if(tile < 128) 
   //      tile += 128;
   //   else 
   //      tile -= 128;
   //}

   int tile_pattern_address = tile_pattern + (tile<<4) + (by<<1); 
  
   if((memory[0xFF40] & 0x01) && (video_enable&VID_EN_BG)) 
   {
      while(x < 160) 
      { 
         byte tile_a = bank0[tile_pattern_address++];
         byte tile_b = bank0[tile_pattern_address];
        
         while(bx > 0) 
         {
            unsigned short c = (tile_a & bx) ? 1 : 0;
            c += ((tile_b & bx) ? 2 : 0);
          
            line_buffer[x] = c;

            c = BGP[c];   
                       
            if(sgb_mode) 
            {
               int dx = x >> 3;
               int dy = y >> 3;
              
               int palette = sgb_ATF[dy * 20 + dx]; 
               
               if(c == 0)
                  palette = 0;
              
               c = c + 4*palette;
               c = sgb_palette[c];
            } else
               c = GBC_BGP[c]; // get GB palette B&W or LCD           

            if(gfx_bit_count == 16)
               ((WORD*)gfx_buffer)[x+((y<<7)+(y<<5))] = gfx_pal16[c];
            else
               ((DWORD*)gfx_buffer)[x+((y<<7)+(y<<5))] = gfx_pal32[c];
  
            ++x;
            if(x >= 160)
               break;
            bx >>= 1;
         }
         ++tx;
         if(tx == 32)
            tx = 0;
         bx = 128; 
        
         tile = bank0[tile_map_line_y + tx];
        
         if(tile_pattern) 
            tile = tile_signed_trans[tile];
            
         tile_pattern_address = tile_pattern + (tile<<4) + (by<<1); 
      }
   } else 
   {
      fill_line16(line_buffer,0UL,160);

      unsigned short c = (sgb_mode?sgb_palette[BGP[0]]:GBC_BGP[BGP[0]]);
      if(gfx_bit_count == 16)
         fill_line16(((WORD*)gfx_buffer)+((y<<7)+(y<<5)),gfx_pal16[c]|(gfx_pal16[c]<<16),160);
      else
         fill_line32(((DWORD*)gfx_buffer)+((y<<7)+(y<<5)),gfx_pal32[c],160);
                    
      if(video_enable&VID_EN_BG) 
         return;   
   }
    
   // do the window display
   if((memory[0xFF40] & 0x20) && (video_enable&VID_EN_WIN)) 
   {
      int wy = memory[0xFF4A];

      if(y >= wy) 
      {
         int wx = memory[0xFF4B];
         wx -= 7;
        
         if(wx <= 159 && windowline <= 143) 
         {          
            if(windowline == -1) 
               windowline = 0;
          
            tx = 0;
            ty = windowline >> 3;
          
            bx = 128;
            by = windowline & 7;
          
            if(wx < 0) 
            {
               bx >>= (-wx);
               wx = 0;
            }
          
            tile_map_line_y = win_tile_map + (ty<<5); 
          
            tile_map_address = tile_map_line_y + tx;
          
            x = wx;
          
            tile = bank0[tile_map_address];

            tile_map_address++;
          
            if((memory[0xFF40] & 0x10) == 0) 
               tile = tile_signed_trans[tile];
          
            tile_pattern_address = tile_pattern + (tile<<4) + (by<<1);

            while(x < 160) 
            {
               byte tile_a = bank0[tile_pattern_address++];
               byte tile_b = bank0[tile_pattern_address];

               while(bx > 0) 
               {
                  unsigned short c = (tile_a & bx) != 0 ? 1 : 0;
                  c += ((tile_b & bx) != 0 ? 2 : 0);
                  
                  line_buffer[x] = (0x100 + c);

                  c = BGP[c];         
                 
                  if(sgb_mode) 
                  {
                     int dx = x >> 3;
                     int dy = y >> 3;
                  
                     int palette = sgb_ATF[dy * 20 + dx];
                  
                     if(c == 0)
                         palette = 0;
                  
                     c = c + 4*palette;    
                     c = sgb_palette[c];        
                  } else
                     c = GBC_BGP[c]; // get GB palette B&W or LCD 
                                 
                  if(gfx_bit_count == 16)
                     ((WORD*)gfx_buffer)[x+((y<<7)+(y<<5))] = gfx_pal16[c];
                  else
                     ((DWORD*)gfx_buffer)[x+((y<<7)+(y<<5))] = gfx_pal32[c];
                
                  ++x;
                  if(x >= 160)
                     break;
                  bx >>= 1;
               }
               ++tx;
               if(tx == 32)
                  tx = 0;
               bx = 128;
               tile = bank0[tile_map_line_y + tx];
            
               if(tile_pattern) 
                  tile = tile_signed_trans[tile];
                  
               tile_pattern_address = tile_pattern + (tile<<4) + (by<<1);
            }
            windowline++;
         }
      }
   }
}

void gb_system::draw_line_tile_GBC()
{
   byte * bank0 = &VRAM[0x0000];
   byte * bank1 = &VRAM[0x2000];

   register int x = 0;
   int y = memory[0xFF44];

   int sx = memory[0xFF43];
   int sy = memory[0xFF42];
     
   sy += y;

   sy &= 0xFF;
    
   int tx = sx >> 3;
   int ty = sy >> 3;

   int bx = 1 << (7 - (sx & 7));
   int by = sy & 7;

   int tile_map_line_y = tile_map + (ty<<5); // * 32
  
   int tile_map_address = tile_map_line_y + tx;

   byte attrs = bank1[tile_map_address];

   byte tile = bank0[tile_map_address];
     
   ++tile_map_address;
  
   if(tile_pattern) 
      tile = tile_signed_trans[tile];

   int tile_pattern_address = tile_pattern + (tile<<4) + (by<<1); 
  
   if(((memory[0xFF40] & 0x01) || rom->CGB) && (video_enable&VID_EN_BG)) 
   {
      while(x < 160) 
      { 
         if(attrs & 0x40) 
            tile_pattern_address = tile_pattern + (tile<<4) + ((7-by)<<1); 
                  
         byte tile_a = ((attrs & 0x08)?bank1[tile_pattern_address]:bank0[tile_pattern_address]);
         ++tile_pattern_address;
         byte tile_b = ((attrs & 0x08)?bank1[tile_pattern_address]:bank0[tile_pattern_address]);

         if(attrs & 0x20) 
         {
            tile_a = invert_table[tile_a];
            tile_b = invert_table[tile_b];
         }
        
         while(bx > 0) 
         {
            unsigned short c = (tile_a & bx) ? 1 : 0;
            c += ((tile_b & bx) ? 2 : 0);
          
            line_buffer[x] = c;
          
            if(attrs & 0x80)
               line_buffer[x] |= 0x300;     

            if(!rom->CGB) 
               c = BGP[c];
            else
               c = c + (attrs & 7)*4;
               
            if(gfx_bit_count == 16)
               ((WORD*)gfx_buffer)[x+((y<<7)+(y<<5))] = gfx_pal16[GBC_BGP[c]];
            else
               ((DWORD*)gfx_buffer)[x+((y<<7)+(y<<5))] = gfx_pal32[GBC_BGP[c]];

            ++x;
            if(x >= 160)
               break;
            bx >>= 1;
         }
         ++tx;
         if(tx == 32)
            tx = 0;
         bx = 128; 
        
         attrs = bank1[tile_map_line_y + tx];
        
         tile = bank0[tile_map_line_y + tx];
        
         if(tile_pattern) 
            tile = tile_signed_trans[tile];
        
         tile_pattern_address = tile_pattern + (tile<<4) + (by<<1); 
      }
   } else 
   {
      fill_line16(line_buffer,0UL,160);
  
      unsigned short c = (rom->CGB?GBC_BGP[0]:GBC_BGP[BGP[0]]);
      if(gfx_bit_count == 16)
         fill_line16(((WORD*)gfx_buffer)+((y<<7)+(y<<5)),gfx_pal16[c]|(gfx_pal16[c]<<16),160);
      else
         fill_line32(((DWORD*)gfx_buffer)+((y<<7)+(y<<5)),gfx_pal32[c],160);     
  
      if(video_enable&VID_EN_BG) 
         return;   
   }
    
   // do the window display
   if((memory[0xFF40] & 0x20) && (video_enable&VID_EN_WIN)) 
   {
      int wy = memory[0xFF4A];

      if(y >= wy) 
      {
         int wx = memory[0xFF4B];
         wx -= 7;

         if(wx <= 159 && windowline <= 143) 
         {          
            if(windowline == -1) 
               windowline = 0;

            tx = 0;
            ty = windowline >> 3;
          
            bx = 128;
            by = windowline & 7;
          
            if(wx < 0) 
            {
               bx >>= (-wx);
               wx = 0;
            }
          
            tile_map_line_y = win_tile_map + (ty<<5); //*32
          
            tile_map_address = tile_map_line_y + tx;
          
            x = wx;
          
            tile = bank0[tile_map_address];
            byte attrs = bank1[tile_map_address];
            tile_map_address++;
          
            if((memory[0xFF40] & 0x10) == 0) 
               tile = tile_signed_trans[tile];
          
            tile_pattern_address = tile_pattern + (tile<<4) + (by<<1);

            while(x < 160) 
            {
               if(attrs & 0x40) 
                  tile_pattern_address = tile_pattern + (tile<<4) + ((7-by)<<1);
                              
               byte tile_a = ((attrs & 0x08)?bank1[tile_pattern_address]:bank0[tile_pattern_address]);
               ++tile_pattern_address;
               byte tile_b = ((attrs & 0x08)?bank1[tile_pattern_address]:bank0[tile_pattern_address]);

               if(attrs & 0x20) 
               {
                  tile_a = invert_table[tile_a];
                  tile_b = invert_table[tile_b];
               }
            
               while(bx > 0) 
               {
                  unsigned short c = (tile_a & bx) != 0 ? 1 : 0;
                  c += ((tile_b & bx) != 0 ? 2 : 0);

                  if(attrs & 0x80)
                     line_buffer[x] = 0x300 + c;
                  else
                     line_buffer[x] = (0x100 + c);                      
              
                  if(!rom->CGB) 
                     c = BGP[c];
                  else
                     c = c + (attrs & 7) * 4;
              
                  if(gfx_bit_count == 16)
                     ((WORD*)gfx_buffer)[x+((y<<7)+(y<<5))] = gfx_pal16[GBC_BGP[c]];
                  else
                     ((DWORD*)gfx_buffer)[x+((y<<7)+(y<<5))] = gfx_pal32[GBC_BGP[c]];
              
                  ++x;
                  if(x >= 160)
                     break;
                  bx >>= 1;
               }
               ++tx;
               if(tx == 32)
                  tx = 0;

               bx = 128;
               tile = bank0[tile_map_line_y + tx];
               attrs = bank1[tile_map_line_y + tx];
            
               if(tile_pattern) 
                  tile = tile_signed_trans[tile];
                  
               tile_pattern_address = tile_pattern + (tile<<4) + (by<<1);
            }
            windowline++;
         }
      }
   }
}

void gb_system::draw_sprite_tile(int tile, int x,int y,int t, int flags,int size,int spriteNumber)
{
   byte* bank0;
   byte* bank1;
   if(gbc_mode) 
   {
      bank0 = &VRAM[0x0000];
      bank1 = &VRAM[0x2000];
   } else 
   {
      bank0 = &memory[0x8000];
      bank1 = NULL;
   }
  
   byte *pal = ((flags & 0x10)?OBP1:OBP0);

   int flipx = (flags & 0x20);
   int flipy = (flags & 0x40);
  
   if(flipy) 
      t = (size ? 15 : 7) - t;
  
   int prio =  flags & 0x80;
  
   int address = (tile<<4) + (t<<1);
   byte a = 0;
   byte b = 0;

   if((gbc_mode && rom->CGB) && (flags & 0x08)) 
   {
      a = bank1[address++];
      b = bank1[address++];
   } else 
   {
      a = bank0[address++];
      b = bank0[address++];
   }
  
   for(int xx = 0; xx < 8; xx++) 
   {
      byte mask = 1 << (7-xx);
      unsigned short c = 0;
      if( (a & mask))
         c++;
      if( (b & mask))
         c+=2;
    
      if(c == 0) 
         continue;

      int xxx = xx+x;
     
      if(flipx)
         xxx = (7-xx+x);

      if(xxx < 0 || xxx > 159)
         continue;

      unsigned short color = line_buffer[xxx]&0xFFF;
     
      if((rom->CGB && gbc_mode && (memory[0xFF40]&1)) || !gbc_mode || (gbc_mode && !rom->CGB))
      {
         if(prio) 
         {
            if(color < 0x100 && color != 0)
               continue;
      
            if((color > 0x100 && color < 0x200) && color != 0x100)
               continue;
         }
      
         if(color >= 0x300 && color != 0x300)
            continue;
      }
       
      if(color >= 0x200 && color < 0x300) 
      {
         int sprite = color & 0xff;

         int spriteX = memory[0xFE00 + (sprite<<2) + 1];

         if(spriteX == x) 
         {
            if(sprite < spriteNumber)
               continue;
         } else 
         {
            if(gbc_mode) 
            {        
               if(sprite < spriteNumber)
                  continue;
            } else 
            {
               if(spriteX < x+8)
                  continue;
            }
         }
      } 
    
      line_buffer[xxx] = 0x200 + spriteNumber;

      if(gbc_mode) 
      {
         if(!rom->CGB)
         {
            c = pal[c];
            if(pal == OBP1)
               c += 4;
         } 
         else
            c = c + (flags & 0x07)*4;
          
         c = GBC_OBP[c];
      } else 
      {
         c = pal[c];

         if(sgb_mode) 
         {
            int dx = xxx >> 3;
            int dy = y >> 3;
       
            int palette = sgb_ATF[dy * 20 + dx];
         
            if(c == 0)
               palette = 0;
       
            c = c + 4*palette;  
            c = sgb_palette[c];            
         } else
            c = GBC_OBP[c]; // get GB palette B&W or LCD
      }
      if(gfx_bit_count == 16)
         ((WORD*)gfx_buffer)[xxx+((y<<7)+(y<<5))] = gfx_pal16[c];
      else
         ((DWORD*)gfx_buffer)[xxx+((y<<7)+(y<<5))] = gfx_pal32[c];
   }
}

void gb_system::draw_sprites()
{    
   int x = 0;
   int y = 0;     

   int size = (memory[0xFF40] & 0x04);   

   int yc = memory[0xFF44];
      
   int address = 0xFE00;
   for(int i = 0; i < 40; i++) 
   {
      y = memory[address++];
      x = memory[address++];

      int tile = memory[address++];
      if(size)
         tile &= 254;
      int flags = memory[address++];
      
      if(x > 0 && y > 0 && x < 168 && y < 160) 
      {
         // check if sprite intersects current line
         int t = yc - y + 16;
         if(size && t >=0 && t < 16) 
         {
            draw_sprite_tile(tile,x-8,yc,t,flags,size,i);
            ++sprite_number;
         }
         else 
         if(!size && t >= 0 && t < 8)
         {
            draw_sprite_tile(tile,x-8,yc,t, flags,size,i);
            ++sprite_number;
         }
         if(options->video_sprite_limit && sprite_number >= 10)
            return;
      }
   } 
   if(sprite_number > 10)
      sprite_number = 10;
}
