#include "palette.h"
#include "GB.h"
#include "strings.h"
#include "config.h"

Palette::~Palette()
{
    if(this->gfxPal32 != NULL) { 
        delete [] this->gfxPal32; 
        this->gfxPal32 = NULL; 
    }
    if(this->gfxPal16 != NULL) { 
        delete [] this->gfxPal16; 
        this->gfxPal16 = NULL; 
    }
}

bool Palette::initPalettes(int bitCount)
{
    this->bitCount = bitCount;
    
    if(bitCount  == 16) {
        this->gfxPal16 = new WORD[0x10000];
    } else {
        this->gfxPal32 = new DWORD[0x10000];
    }
    
    this->mixGbcColours();
  
    if(!this->gfxPal32 && !this->gfxPal16) {
        debug_print(str_table[ERROR_MEMORY]); 
        return false;
    }
    
    return true;
}

void Palette::setPaletteShifts(int rs, int gs, int bs)
{
    this->rs = rs;
    this->gs = gs;
    this->bs = bs;
}

void Palette::mixGbcColours()
{
  if(GB1->gbc_mode && options->video_GBCBGA_real_colors) // << this
  {
     if(GB1->system_type == SYS_GBA)
     {
        for(int i=0;i<0x10000;++i)
        {
           int red_init = (i & 0x1F);
           int green_init = ((i & 0x3E0) >> 5);
           int blue_init = ((i & 0x7C00) >> 10);
         
           if(red_init < 0x19) red_init -= 4; else red_init -= 3; 
           if(green_init < 0x19) green_init -= 4; else green_init -= 3;
           if(blue_init < 0x19) blue_init -= 4; else blue_init -= 3;
           if(red_init < 0) red_init = 0;       
           if(green_init < 0) green_init = 0;
           if(blue_init < 0) blue_init = 0;
        
           int red = ((red_init*12+green_init+blue_init)/14);
           int green = ((green_init*12+blue_init+red_init)/14);
           int blue = ((blue_init*12+red_init+green_init)/14);
           if(this->bitCount == 16)
              this->gfxPal16[i] = (red<<this->rs) | (green<<this->gs) | (blue<<this->bs);              
           else
              this->gfxPal32[i] = (red<<this->rs) | (green<<this->gs) | (blue<<this->bs);
        }
     }     
     else
     {
        for(int i=0;i<0x10000;++i)
        {
           int red_init = (i & 0x1F);
           int green_init = ((i & 0x3E0) >> 5);
           int blue_init = ((i & 0x7C00) >> 10);
         
           if(red_init && red_init < 0x10) red_init += 2; else if(red_init) red_init += 3; 
           if(green_init && green_init < 0x10) green_init += 2; else if(green_init) green_init += 3;
           if(blue_init && blue_init < 0x10) blue_init += 2; else if(blue_init) blue_init += 3;
           if(red_init >= 0x1F) red_init = 0x1E;       
           if(green_init >= 0x1F) green_init = 0x1E;
           if(blue_init >= 0x1F) blue_init = 0x1E;
        
           int red = ((red_init*10+green_init*3+blue_init)/14);
           int green = ((green_init*10+blue_init*2+red_init*2)/14);
           int blue = ((blue_init*10+red_init*2+green_init*2)/14);
           if(this->bitCount == 16)
              this->gfxPal16[i] = (red<<this->rs) | (green<<this->gs) | (blue<<this->bs);
           else        
              this->gfxPal32[i] = (red<<this->rs) | (green<<this->gs) | (blue<<this->bs);
        }
     }
  } else
  {
     if(this->bitCount == 16)
     {
        for(int i=0;i<0x10000;++i)
           this->gfxPal16[i] = ((i & 0x1F) << this->rs) | (((i & 0x3E0) >> 5) << this->gs) | (((i & 0x7C00) >> 10) << this->bs);
     } else
     {
        for(int i=0;i<0x10000;++i)
            this->gfxPal32[i] = ((i & 0x1F) << this->rs) | (((i & 0x3E0) >> 5) << this->gs) | (((i & 0x7C00) >> 10) << this->bs);
           //gfx_pal32[i] = (((i & 0x1F) << rs) | (((i & 0x3E0) >> 5) << gs) | (((i & 0x7C00) >> 10) << bs)) ^ 0xFFFFFFFF; = negative
           //gfx_pal32[i] = ((i & 0x1F) << rs) | (((i & 0x3E0) >> 5) << gs) | (((i & 0x7C00) >> 10) << bs) ^ 0xFFFFFFFF; = super yellow ridiculousness. i actually quite enjoy this
     }  
  }
}

