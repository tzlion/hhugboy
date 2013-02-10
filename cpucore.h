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
#include "config.h"

// cpu emulation code #included to mainloop.c

   byte temp = 0x00;
   word tempreg; 
   tempreg.W = 0x0000;
   signed char offset = 0x00;
   int carry = 0;
      
   switch(opcode)
   { 
      case 0x00: //NOP
      case 0x7f: //LD A,A
      case 0x40: //LD B,B
      case 0x49: //LD C,C
      case 0x52: //LD D,D
      case 0x5b: //LD E,E
      case 0x64: //LD H,H
      case 0x6d: //LD L,L
      break;
      
      case 0x01: //LD BC,xxxx
       BC.W = readword(PC.W);
       PC.W+=2;      
      break;
      case 0x02: //LD (BC),A
        writememory(BC.W,A);
      break;
      case 0x03: //INC BC
        ++BC.W;
      break;
      case 0x04: //INC B
        ++BC.B.h;
        HFLAG = ((BC.B.h&0x0F)?0:1);
        ZFLAG = zero_table[BC.B.h]; 
        NFLAG = 0;
      break;
      case 0x05: //DEC B
        --BC.B.h;
        HFLAG = ((BC.B.h&0x0F)==0x0F);
        ZFLAG = zero_table[BC.B.h];
        NFLAG = 1; 
      break;
      case 0x06: //LD B,xx
        BC.B.h = readopcode(PC.W++);
      break;
      case 0x07: //RLCA
        CFLAG = (A&0x80?1:0);
        A = ((A<<1)|(A>>7));
        ZFLAG = NFLAG = HFLAG = 0;         
      break;
      case 0x08: //LD (xxxx),SP 
        writeword(readword(PC.W),SP.W);
        PC.W+=2;              
      break;
      case 0x09: //ADD HL,BC
       CFLAG = (((long)HL.W + (long)BC.W) > 0xFFFF); 
       HFLAG = (((HL.W & 0x0FFF) + (BC.W & 0x0FFF)) > 0x0FFF);
       HL.W = HL.W + BC.W;
       NFLAG = 0;
      break;
      case 0x0A: //LD A,(BC)
        A = readmemory(BC.W);
      break;
      case 0x0B: //DEC BC
        --BC.W;
      break;
      case 0x0C: //INC C
        ++BC.B.l;
        HFLAG = ((BC.B.l&0x0F)?0:1);
        ZFLAG = zero_table[BC.B.l];
        NFLAG = 0;
      break;
      case 0x0D: //DEC C
        --BC.B.l;
        HFLAG = ((BC.B.l&0x0F)==0x0F);
        ZFLAG = zero_table[BC.B.l];
        NFLAG = 1;
      break;    
      case 0x0E: //LD C,xx
        BC.B.l = readopcode(PC.W++);
      break;
      case 0x0F: //RRCA
        CFLAG = (A&0x01);
        A = ((A>>1)|(CFLAG?0x80:0));
        ZFLAG = NFLAG = HFLAG = 0;    
      break;
      case 0x10: //STOP
        PC.W++; 

        if(gbc_mode && (memory[0xFF4D] & 0x01)) // GBC speed switch
        {
           gb_speed = !gb_speed;
           
           memory[0xFF4D] = (gb_speed?0x80:0x00);
        }
      break;
      case 0x11: //LD DE,xxxx
        DE.W = readword(PC.W);
        PC.W+=2;           
      break;
      case 0x12: //LD (DE),A
        writememory(DE.W,A);
      break;
      case 0x13: //INC DE
        ++DE.W;
      break;
      case 0x14: //INC D
        ++DE.B.h;
        HFLAG = ((DE.B.h&0x0F)?0:1);
        ZFLAG = zero_table[DE.B.h];
        NFLAG = 0; 
      break;
      case 0x15: //DEC D
        --DE.B.h;
        HFLAG = ((DE.B.h&0x0F)==0x0F);
        ZFLAG = zero_table[DE.B.h];
        NFLAG = 1;
      break;
      case 0x16: //LD D,xx
        DE.B.h = readopcode(PC.W++);
      break;
      case 0x17: //RLA
        carry = (A&0x80?1:0);
        A = ((A<<1)|CFLAG);
        CFLAG = carry;
        ZFLAG = NFLAG = HFLAG = 0;   
      break;
      case 0x18: //JR xx
        PC.W += (signed char)readopcode(PC.W)+1;
      break;
      case 0x19: //ADD HL,DE
        CFLAG = (((long)HL.W + (long)DE.W) > 0xFFFF); 
        HFLAG = (((HL.W & 0x0FFF) + (DE.W & 0x0FFF)) > 0x0FFF);
        HL.W = HL.W + DE.W;
        NFLAG = 0; 
      break;
      case 0x1A: //LD A,(DE)
        A = readmemory(DE.W);
      break;
      case 0x1B: //DEC DE
        --DE.W;
      break;
      case 0x1C: //INC E
        ++DE.B.l;
        HFLAG = ((DE.B.l&0x0F)?0:1);
        ZFLAG = zero_table[DE.B.l];
        NFLAG = 0; 
      break;
      case 0x1D: //DEC E
        --DE.B.l;
        HFLAG = ((DE.B.l&0x0F)==0x0F);
        ZFLAG = zero_table[DE.B.l];
        NFLAG = 1; 
      break;
      case 0x1E: //LD E,xx
        DE.B.l = readopcode(PC.W++);
      break;
      case 0x1F: //RRA
        carry = (A&0x01);
        A = ((A>>1)|(CFLAG?0x80:0));
        CFLAG = carry;
        ZFLAG = NFLAG = HFLAG = 0;  
      break;
      case 0x20: //JR NZ,xx
        if(!ZFLAG) 
        { 
           cur_cycle = 12; 
           PC.W += (signed char)readopcode(PC.W)+1; 
        } else 
           ++PC.W;
      break;
      case 0x21: //LD HL,xxxx
        HL.W = readword(PC.W);
        PC.W+=2;           
      break;
      case 0x22: //"LD (HL+),A" "LD (HLI),A" LDI (HL),A
        writememory(HL.W,A); 
        ++HL.W;
      break;
      case 0x23: //INC HL
        ++HL.W;
      break;
      case 0x24: //INC H
        ++HL.B.h;
        HFLAG = ((HL.B.h&0x0F)?0:1);
        ZFLAG = zero_table[HL.B.h];
        NFLAG = 0; 
      break;   
      case 0x25: //DEC H
        --HL.B.h;
        HFLAG = ((HL.B.h&0x0F)==0x0F);
        ZFLAG = zero_table[HL.B.h];
        NFLAG = 1;
      break;
      case 0x26: //LD H,xx
        HL.B.h = readopcode(PC.W++);
      break;
      case 0x27: //DAA // Decimal Adjust Accumulator
        if(!NFLAG)
        {
           if((A&0x0F)>=0x0A || HFLAG)
              A+=0x06;
           if((A&0xF0)>=0xA0 || CFLAG)
           {
              A+=0x60;
              CFLAG=1;
           }
        } else if(NFLAG)
        {
           if((A&0x0F)>=0x0A || HFLAG)
              A-=0x06;
           if((A&0xF0)>=0xA0 || CFLAG)
           {
              A-=0x60;
              CFLAG=1;
           }
        }
        ZFLAG=zero_table[A];
        HFLAG=0;
      break;
      case 0x28: //JR Z,xx
        if(ZFLAG) 
        { 
           cur_cycle = 12; 
           PC.W += (signed char)readopcode(PC.W)+1; 
        } else
           ++PC.W;
      break;
      case 0x29: //ADD HL,HL
        CFLAG = (((long)HL.W + (long)HL.W) > 0xFFFF); 
        HFLAG = (((HL.W & 0x0FFF) + (HL.W & 0x0FFF)) > 0x0FFF);
        HL.W = HL.W + HL.W;
        NFLAG = 0;
      break;
      case 0x2A: //LDI A,(HL) "LD A,(HL+)" "LD A,(HLI)"
        A = readmemory(HL.W); 
        ++HL.W;
      break;
      case 0x2B: //DEC HL
        --HL.W;
      break;
      case 0x2C: //INC L
        ++HL.B.l;
        HFLAG = ((HL.B.l&0x0F)?0:1);
        ZFLAG = zero_table[HL.B.l];
        NFLAG = 0;
      break;
      case 0x2D: //DEC L
        --HL.B.l;
        HFLAG = ((HL.B.l&0x0F)==0x0F);
        ZFLAG = zero_table[HL.B.l];
        NFLAG = 1; 
      break;
      case 0x2E: //LD L,xx
        HL.B.l = readopcode(PC.W++);
      break;
      case 0x2F: //CPL
        A = ~A; 
        HFLAG = NFLAG = 1; 
      break;
      case 0x30: //JR NC,xx
        if(!CFLAG) 
        { 
           cur_cycle = 12; 
           PC.W += (signed char)readopcode(PC.W)+1; 
        } else 
           ++PC.W;
      break;
      case 0x31: //LD SP,xxxx
        SP.W = readword(PC.W);
        PC.W+=2;           
      break;
      case 0x32: //LDD (HL),A "LD (HL-),A" "LD (HLD),A"
        writememory(HL.W,A);
        --HL.W;
      break;
      case 0x33: //INC SP
        ++SP.W;
      break;
      case 0x34: //INC (HL)
        temp = readmemory(HL.W)+1;
        writememory(HL.W,temp);
        HFLAG = ((temp&0x0F)?0:1);
        ZFLAG = zero_table[temp];
        NFLAG = 0; 
      break;
      case 0x35: //DEC (HL)
        temp = readmemory(HL.W)-1;
        writememory(HL.W,temp);
        HFLAG = ((temp&0x0F)==0x0F);
        ZFLAG = zero_table[temp];
        NFLAG = 1;
      break;   
      case 0x36: //LD (HL),xx
        writememory(HL.W,readopcode(PC.W++));
      break;
      case 0x37: //SCF Set Carry Flag
        CFLAG = 1; 
        NFLAG = HFLAG = 0;  
      break;
      case 0x38: //JR C,xx
        if(CFLAG) 
        { 
           cur_cycle = 12; 
           PC.W += (signed char)readopcode(PC.W)+1; 
        } else 
           ++PC.W;
      break;
      case 0x39: //ADD HL,SP
        CFLAG = (((long)HL.W + (long)SP.W) > 0xFFFF); 
        HFLAG = (((HL.W & 0x0FFF) + (SP.W & 0x0FFF)) > 0x0FFF);
        HL.W = HL.W + SP.W;
        NFLAG = 0; 
      break;
      case 0x3A: //LDD A,(HL) "LD A,(HL-)" "LD A,(HLD)"
        A = readmemory(HL.W);
        --HL.W;
      break;
      case 0x3B: //DEC SP
        --SP.W;
      break;
      case 0x3C: //INC A
        ++A;
        HFLAG = ((A&0x0F)?0:1);
        ZFLAG = zero_table[A];
        NFLAG = 0; 
      break;
      case 0x3D: //DEC A
        --A;
        HFLAG = ((A&0x0F)==0x0F);
        ZFLAG = zero_table[A];
        NFLAG = 1;
      break;
      case 0x3E: //LD A,xx
        A = readopcode(PC.W++);
      break;
      case 0x3F: //CCF
        CFLAG = !CFLAG; 
        NFLAG = HFLAG = 0;
      break;
      case 0x41: //LD B,C
        BC.B.h = BC.B.l;
      break;
      case 0x42: //LD B,D
        BC.B.h = DE.B.h;
      break;  
      case 0x43: //LD B,E    
        BC.B.h = DE.B.l;
      break;
      case 0x44: //LD B,H
        BC.B.h = HL.B.h;
      break;
      case 0x45: //LD B,L
        BC.B.h = HL.B.l;
      break;
      case 0x46: //LD B,(HL)
        BC.B.h = readmemory(HL.W);
      break;
      case 0x47: //LD B,A
        BC.B.h = A;
      break;
      case 0x48: //LD C,B
        BC.B.l = BC.B.h;
      break;
      case 0x4A: //LD C,D
        BC.B.l = DE.B.h;
      break;
      case 0x4B: //LD C,E
        BC.B.l = DE.B.l;
      break;
      case 0x4C: //LD C,H
        BC.B.l = HL.B.h;
      break;  
      case 0x4D: //LD C,L
        BC.B.l = HL.B.l;
      break;   
      case 0x4E: //LD C,(HL)
        BC.B.l = readmemory(HL.W);
      break;
      case 0x4F: //LD C,A
        BC.B.l = A;
      break;
      case 0x50: //LD D,B
        DE.B.h = BC.B.h;
      break;
      case 0x51: //LD D,C
        DE.B.h = BC.B.l;
      break;
      case 0x53: //LD D,E
        DE.B.h = DE.B.l;
      break;
      case 0x54: //LD D,H
        DE.B.h = HL.B.h;
      break;
      case 0x55: //LD D,L
        DE.B.h = HL.B.l;
      break;
      case 0x56: //LD D,(HL)
        DE.B.h = readmemory(HL.W);
      break;
      case 0x57: //LD D,A
        DE.B.h = A;
      break;
      case 0x58: //LD E,B
        DE.B.l = BC.B.h;
      break;
      case 0x59: //LD E,C
        DE.B.l = BC.B.l;
      break;
      case 0x5A: //LD E,D
        DE.B.l = DE.B.h;
      break;
      case 0x5C: //LD E,H
        DE.B.l = HL.B.h;
      break;
      case 0x5D: //LD E,L
        DE.B.l = HL.B.l;
      break;
      case 0x5E: //LD E,(HL)
        DE.B.l = readmemory(HL.W);
      break;
      case 0x5F: //LD E,A
        DE.B.l = A;
      break;
      case 0x60: //LD H,B
        HL.B.h = BC.B.h;
      break;
      case 0x61: //LD H,C
        HL.B.h = BC.B.l;
      break;
      case 0x62: //LD H,D
        HL.B.h = DE.B.h;
      break;
      case 0x63: //LD H,E
        HL.B.h = DE.B.l;
      break;
      case 0x65: //LD H,L
        HL.B.h = HL.B.l;
      break;
      case 0x66: //LD H,(HL)
        HL.B.h = readmemory(HL.W);
      break;
      case 0x67: //LD H,A
        HL.B.h = A;
      break;
      case 0x68: //LD L,B
        HL.B.l = BC.B.h;
      break;
      case 0x69: //LD L,C
        HL.B.l = BC.B.l;
      break;
      case 0x6A: //LD L,D
        HL.B.l = DE.B.h;
      break;
      case 0x6B: //LD L,E
        HL.B.l = DE.B.l;
      break;
      case 0x6C: //LD L,H
        HL.B.l = HL.B.h;
      break;
      case 0x6E: //LD L,(HL)
        HL.B.l = readmemory(HL.W);
      break;
      case 0x6F: //LD L,A
        HL.B.l = A;
      break;
      case 0x70: //LD (HL),B
        writememory(HL.W,BC.B.h);
      break;
      case 0x71: //LD (HL),C
        writememory(HL.W,BC.B.l);
      break;
      case 0x72: //LD (HL),D
        writememory(HL.W,DE.B.h);
      break;
      case 0x73: //LD (HL),E
        writememory(HL.W,DE.B.l);
      break;
      case 0x74: //LD (HL),H
        writememory(HL.W,HL.B.h);
      break;
      case 0x75: //LD (HL),L
        writememory(HL.W,HL.B.l);
      break;
      case 0x76: //HALT //  Halt CPU until interrupt        
        if(IME || EI_count == 1) // IME = 1 or last instruction was EI
        {
           CPUHalt = HALT;
           EI_count = 0;
           IME = 1;
        }
        else if(memory[0xFF0F]&memory[0xFFFF])
           CPUHalt = HALT2;     
        else if(memory[0xFFFF]==0)
           CPUHalt = HALT;  // freeze
      break;
      case 0x77: //LD (HL),A
        writememory(HL.W,A);
      break;
      case 0x78: //LD A,B
        A = BC.B.h;
      break;
      case 0x79: //LD A,C
        A = BC.B.l;
      break;
      case 0x7A: //LD A,D
        A = DE.B.h;
      break;
      case 0x7B: //LD A,E
        A = DE.B.l;
      break;
      case 0x7C: //LD A,H
        A = HL.B.h;
      break;
      case 0x7D: //LD A,L
        A = HL.B.l;
      break;
      case 0x7E: //LD A,(HL)
        A = readmemory(HL.W);
      break;
      case 0x80: //ADD B
        CFLAG = ((A + BC.B.h) > 0xFF); 
        HFLAG = (((A&0x0F) + (BC.B.h&0x0F)) > 0x0F);
        A = A + BC.B.h;
        ZFLAG = zero_table[A];
        NFLAG = 0; 
      break;
      case 0x81: //ADD C
        CFLAG = ((A + BC.B.l) > 0xFF); 
        HFLAG = (((A&0x0F) + (BC.B.l&0x0F)) > 0x0F);
        A = A + BC.B.l;
        ZFLAG = zero_table[A];
        NFLAG = 0; 
      break;
      case 0x82: //ADD D
        CFLAG = ((A + DE.B.h) > 0xFF); 
        HFLAG = (((A&0x0F) + (DE.B.h&0x0F)) > 0x0F);
        A = A + DE.B.h;
        ZFLAG = zero_table[A];
        NFLAG = 0;  
      break;
      case 0x83: //ADD E
        CFLAG = ((A + DE.B.l) > 0xFF); 
        HFLAG = (((A&0x0F) + (DE.B.l&0x0F)) > 0x0F);
        A = A + DE.B.l;
        ZFLAG = zero_table[A];
        NFLAG = 0;
      break;
      case 0x84: //ADD H
        CFLAG = ((A + HL.B.h) > 0xFF);
        HFLAG = (((A&0x0F) + (HL.B.h&0x0F)) > 0x0F);
        A = A + HL.B.h;
        ZFLAG = zero_table[A];
        NFLAG = 0; 
      break;
      case 0x85: //ADD L
        CFLAG = ((A + HL.B.l) > 0xFF); 
        HFLAG = (((A&0x0F) + (HL.B.l&0x0F)) > 0x0F);
        A = A + HL.B.l;
        ZFLAG = zero_table[A];
        NFLAG = 0;
      break;
      case 0x86: //ADD (HL)
        temp = readmemory(HL.W);
        CFLAG = ((A + temp) > 0xFF); 
        HFLAG = (((A&0x0F) + (temp&0x0F)) > 0x0F);
        A = A + temp;
        ZFLAG = zero_table[A];
        NFLAG = 0;
      break;
      case 0x87: //ADD A
        CFLAG = ((A + A) > 0xFF);
        HFLAG = (((A&0x0F) + (A&0x0F)) > 0x0F);
        A = A + A;
        ZFLAG = zero_table[A];
        NFLAG = 0;  
      break;
      case 0x88: //ADC B
        tempreg.W = A + BC.B.h + CFLAG;
        CFLAG = (tempreg.B.h?1:0);
        HFLAG = (((A^BC.B.h^tempreg.B.l)&0x10)?1:0);
        A = tempreg.B.l;
        ZFLAG = zero_table[A];
        NFLAG = 0;  
      break;
      case 0x89: //ADC C
        tempreg.W = A + BC.B.l + CFLAG;
        CFLAG = (tempreg.B.h?1:0);
        HFLAG = (((A^BC.B.l^tempreg.B.l)&0x10)?1:0);
        A = tempreg.B.l;
        ZFLAG = zero_table[A];
        NFLAG = 0;  
      break;
      case 0x8A: //ADC D
        tempreg.W = A + DE.B.h + CFLAG;
        CFLAG = (tempreg.B.h?1:0);
        HFLAG = (((A^DE.B.h^tempreg.B.l)&0x10)?1:0);
        A = tempreg.B.l;
        ZFLAG = zero_table[A];
        NFLAG = 0;  
      break;
      case 0x8B: //ADC E
        tempreg.W = A + DE.B.l + CFLAG;
        CFLAG = (tempreg.B.h?1:0);
        HFLAG = (((A^DE.B.l^tempreg.B.l)&0x10)?1:0);
        A = tempreg.B.l;
        ZFLAG = zero_table[A];
        NFLAG = 0;  
      break;
      case 0x8C: //ADC H
        tempreg.W = A + HL.B.h + CFLAG;
        CFLAG = (tempreg.B.h?1:0);
        HFLAG = (((A^HL.B.h^tempreg.B.l)&0x10)?1:0);
        A = tempreg.B.l;
        ZFLAG = zero_table[A];
        NFLAG = 0;  
      break;
      case 0x8D: //ADC L
        tempreg.W = A + HL.B.l + CFLAG;
        CFLAG = (tempreg.B.h?1:0);
        HFLAG = (((A^HL.B.l^tempreg.B.l)&0x10)?1:0);
        A = tempreg.B.l;
        ZFLAG = zero_table[A];
        NFLAG = 0;   
      break;
      case 0x8E: //ADC (HL)
        temp = readmemory(HL.W);
        tempreg.W = A + temp + CFLAG;
        CFLAG = (tempreg.B.h?1:0);
        HFLAG = (((A^temp^tempreg.B.l)&0x10)?1:0);
        A = tempreg.B.l;
        ZFLAG = zero_table[A];
        NFLAG = 0;  
      break;
      case 0x8F: //ADC A
        tempreg.W = A + A + CFLAG;
        CFLAG = (tempreg.B.h?1:0);
        HFLAG = (((A^A^tempreg.B.l)&0x10)?1:0);
        A = tempreg.B.l;
        ZFLAG = zero_table[A];
        NFLAG = 0;  
      break;
      case 0x90: //SUB B
        CFLAG = (A < BC.B.h);
        HFLAG = ((A&0x0F) < (BC.B.h&0x0F));
        A = A - BC.B.h;
        ZFLAG = zero_table[A];
        NFLAG = 1; 
      break;
      case 0x91: //SUB C
        CFLAG = (A < BC.B.l);
        HFLAG = ((A&0x0F) < (BC.B.l&0x0F));
        A = A - BC.B.l;
        ZFLAG = zero_table[A];
        NFLAG = 1;
      break;
      case 0x92: //SUB D
        CFLAG = (A < DE.B.h);
        HFLAG = ((A&0x0F) < (DE.B.h&0x0F));
        A = A - DE.B.h;
        ZFLAG = zero_table[A];
        NFLAG = 1; 
      break;
      case 0x93: //SUB E
        CFLAG = (A < DE.B.l);
        HFLAG = ((A&0x0F) < (DE.B.l&0x0F));
        A = A - DE.B.l;
        ZFLAG = zero_table[A];
        NFLAG = 1; 
      break;
      case 0x94: //SUB H
        CFLAG = (A < HL.B.h);
        HFLAG = ((A&0x0F) < (HL.B.h&0x0F));
        A = A - HL.B.h;
        ZFLAG = zero_table[A];
        NFLAG = 1;
      break;
      case 0x95: //SUB L
        CFLAG = (A < HL.B.l);
        HFLAG = ((A&0x0F) < (HL.B.l&0x0F));
        A = A - HL.B.l;
        ZFLAG = zero_table[A];
        NFLAG = 1;  
      break;
      case 0x96: //SUB (HL)
        temp = readmemory(HL.W);
        CFLAG = (A < temp);
        HFLAG = ((A&0x0F) < (temp&0x0F));
        A = A - temp;
        ZFLAG = zero_table[A];
        NFLAG = 1;
      break;
      case 0x97: //SUB A
        A = 0;
        CFLAG = HFLAG = 0;
        ZFLAG = NFLAG = 1;  
      break;
      case 0x98: //SBC B
        tempreg.W = A - BC.B.h - CFLAG;
        CFLAG = (tempreg.B.h?1:0);
        HFLAG = ((A^BC.B.h^tempreg.B.l)&0x10?1:0);
        A = tempreg.B.l;
        ZFLAG = zero_table[A];
        NFLAG = 1;   
      break;
      case 0x99: //SBC C
        tempreg.W = A - BC.B.l - CFLAG;
        CFLAG = (tempreg.B.h?1:0);
        HFLAG = ((A^BC.B.l^tempreg.B.l)&0x10?1:0);
        A = tempreg.B.l;
        ZFLAG = zero_table[A];
        NFLAG = 1;
      break;
      case 0x9A: //SBC D
        tempreg.W = A - DE.B.h - CFLAG;
        CFLAG = (tempreg.B.h?1:0);
        HFLAG = ((A^DE.B.h^tempreg.B.l)&0x10?1:0);
        A = tempreg.B.l;
        ZFLAG = zero_table[A];
        NFLAG = 1;    
      break;
      case 0x9B: //SBC E
        tempreg.W = A - DE.B.l - CFLAG;
        CFLAG = (tempreg.B.h?1:0);
        HFLAG = ((A^DE.B.l^tempreg.B.l)&0x10?1:0);
        A = tempreg.B.l;
        ZFLAG = zero_table[A];
        NFLAG = 1;  
      break;
      case 0x9C: //SBC H
        tempreg.W = A - HL.B.h - CFLAG;
        CFLAG = (tempreg.B.h?1:0);
        HFLAG = ((A^HL.B.h^tempreg.B.l)&0x10?1:0);
        A = tempreg.B.l;
        ZFLAG = zero_table[A];
        NFLAG = 1;  
      break;
      case 0x9D: //SBC L
        tempreg.W = A - HL.B.l - CFLAG;
        CFLAG = (tempreg.B.h?1:0);
        HFLAG = ((A^HL.B.l^tempreg.B.l)&0x10?1:0);
        A = tempreg.B.l;
        ZFLAG = zero_table[A];
        NFLAG = 1;  
      break;
      case 0x9E: //SBC (HL)
        temp = readmemory(HL.W);
        tempreg.W = A - temp - CFLAG;
        CFLAG = (tempreg.B.h?1:0);
        HFLAG = ((A^temp^tempreg.B.l)&0x10?1:0);
        A = tempreg.B.l;
        ZFLAG = zero_table[A];
        NFLAG = 1;  
      break;            
      case 0x9F: //SBC A
        tempreg.W = A - A - CFLAG;
        CFLAG = (tempreg.B.h?1:0);
        HFLAG = ((A^A^tempreg.B.l)&0x10?1:0);
        A = tempreg.B.l;
        ZFLAG = zero_table[A];
        NFLAG = 1;  
      break;
      case 0xA0: //AND B
        A &= BC.B.h;
        ZFLAG = zero_table[A];
        NFLAG = CFLAG = 0;
        HFLAG = 1; 
      break;
      case 0xA1: //AND C
        A &= BC.B.l;
        ZFLAG = zero_table[A];
        NFLAG = CFLAG = 0;
        HFLAG = 1;
      break;
      case 0xA2: //AND D
        A &= DE.B.h;
        ZFLAG = zero_table[A];
        NFLAG = CFLAG = 0;
        HFLAG = 1;
      break;
      case 0xA3: //AND E
        A &= DE.B.l;
        ZFLAG = zero_table[A];
        NFLAG = CFLAG = 0;
        HFLAG = 1;
      break;
      case 0xA4: //AND H
        A &= HL.B.h;
        ZFLAG = zero_table[A];
        NFLAG = CFLAG = 0;
        HFLAG = 1;
      break;
      case 0xA5: //AND L
        A &= HL.B.l;
        ZFLAG = zero_table[A];
        NFLAG = CFLAG = 0;
        HFLAG = 1;
      break;
      case 0xA6: //AND (HL)
        A &= readmemory(HL.W);
        ZFLAG = zero_table[A];
        NFLAG = CFLAG = 0;
        HFLAG = 1;
      break;
      case 0xA7: //AND A
        //A &= A;
        ZFLAG = zero_table[A];
        NFLAG = CFLAG = 0;
        HFLAG = 1; 
      break;
      case 0xA8: //XOR B
        A ^= BC.B.h;
        ZFLAG = zero_table[A];
        NFLAG = HFLAG = CFLAG = 0; 
      break;
      case 0xA9: //XOR C
        A ^= BC.B.l;
        ZFLAG = zero_table[A];
        NFLAG = HFLAG = CFLAG = 0;
      break;
      case 0xAA: //XOR D
        A ^= DE.B.h;
        ZFLAG = zero_table[A];
        NFLAG = HFLAG = CFLAG = 0;
      break;
      case 0xAB: //XOR E
        A ^= DE.B.l;
        ZFLAG = zero_table[A];
        NFLAG = HFLAG = CFLAG = 0;
      break;
      case 0xAC: //XOR H
        A ^= HL.B.h;
        ZFLAG = zero_table[A];
        NFLAG = HFLAG = CFLAG = 0;
      break;
      case 0xAD: //XOR L
        A ^= HL.B.l;
        ZFLAG = zero_table[A];
        NFLAG = HFLAG = CFLAG = 0;
      break;
      case 0xAE: //XOR (HL)
        A ^= readmemory(HL.W);
        ZFLAG = zero_table[A];
        NFLAG = HFLAG = CFLAG = 0;
      break;
      case 0xAF: //XOR A
        A = 0;
        ZFLAG = 1;
        NFLAG = HFLAG = CFLAG = 0;
      break;
      case 0xB0: //OR B
        A |= BC.B.h;
        ZFLAG = zero_table[A];
        NFLAG = HFLAG = CFLAG = 0;
      break;
      case 0xB1: //OR C
        A |= BC.B.l;
        ZFLAG = zero_table[A];
        NFLAG = HFLAG = CFLAG = 0;
      break;
      case 0xB2: //OR D
        A |= DE.B.h;
        ZFLAG = zero_table[A];
        NFLAG = HFLAG = CFLAG = 0;
      break;
      case 0xB3: //OR E
        A |= DE.B.l;
        ZFLAG = zero_table[A];
        NFLAG = HFLAG = CFLAG = 0;
      break;
      case 0xB4: //OR H
        A |= HL.B.h;
        ZFLAG = zero_table[A];
        NFLAG = HFLAG = CFLAG = 0;
      break;
      case 0xB5: //OR L
        A |= HL.B.l;
        ZFLAG = zero_table[A];
        NFLAG = HFLAG = CFLAG = 0;
      break;
      case 0xB6: //OR (HL)
        A |= readmemory(HL.W);
        ZFLAG = zero_table[A];
        NFLAG = HFLAG = CFLAG = 0;
      break;
      case 0xB7: //OR A
        //A |= A;
        ZFLAG = zero_table[A];
        NFLAG = HFLAG = CFLAG = 0;
      break;
      case 0xB8: //CP B
        CFLAG = (A < BC.B.h);
        HFLAG = ((A&0x0F) < (BC.B.h&0x0F));
        ZFLAG = (A == BC.B.h);
        NFLAG = 1;
      break;
      case 0xB9: //CP C
        CFLAG = (A < BC.B.l);
        HFLAG = ((A&0x0F) < (BC.B.l&0x0F));
        ZFLAG = (A == BC.B.l);
        NFLAG = 1;
      break;   
      case 0xBA: //CP D
        CFLAG = (A < DE.B.h);
        HFLAG = ((A&0x0F) < (DE.B.h&0x0F));
        ZFLAG = (A == DE.B.h);
        NFLAG = 1;
      break;   
      case 0xBB: //CP E
        CFLAG = (A < DE.B.l);
        HFLAG = ((A&0x0F) < (DE.B.l&0x0F));
        ZFLAG = (A == DE.B.l);
        NFLAG = 1;
      break;
      case 0xBC: //CP H
        CFLAG = (A < HL.B.h);
        HFLAG = ((A&0x0F) < (HL.B.h&0x0F));
        ZFLAG = (A == HL.B.h);
        NFLAG = 1;
      break;
      case 0xBD: //CP L
        CFLAG = (A < HL.B.l);
        HFLAG = ((A&0x0F) < (HL.B.l&0x0F));
        ZFLAG = (A == HL.B.l);
        NFLAG = 1;
      break;
      case 0xBE: //CP (HL)
        temp = readmemory(HL.W);
        CFLAG = (A < temp);
        HFLAG = ((A&0x0F) < (temp&0x0F));
        ZFLAG = (A == temp);
        NFLAG = 1;
      break;
      case 0xBF: //CP A
        CFLAG = HFLAG = 0;
        ZFLAG = NFLAG = 1;
      break;
      case 0xC0: //RET NZ
        if(!ZFLAG) 
        { 
           cur_cycle = 20; 
           POP(PC.B.l); 
           POP(PC.B.h); 
        }
      break;
      case 0xC1: //POP BC
        POP(BC.B.l); 
        POP(BC.B.h);
      break;
      case 0xC2: //JP NZ,xxxx
        if(!ZFLAG) 
        { 
           cur_cycle = 16; 

           PC.W = readword(PC.W);  
        } else 
           PC.W+=2;
      break;
      case 0xC3: //JP xxxx
         PC.W = readword(PC.W);
      break;
      case 0xC4: //CALL NZ,xxxx
        if(!ZFLAG) 
        { 
           cur_cycle = 24; 

           tempreg.W = readword(PC.W);
           PC.W+=2;   
           PUSH(PC.B.h); 
           PUSH(PC.B.l); 

           PC.W = tempreg.W;
        } else 
           PC.W+=2;
      break;
      case 0xC5: //PUSH BC
        PUSH(BC.B.h); 
        PUSH(BC.B.l); 
      break;
      case 0xC6: //ADD xx
        temp = readopcode(PC.W++);
        CFLAG = ((A + temp) > 0xFF);
        HFLAG = (((A&0x0F) + (temp&0x0F)) > 0x0F);
        A = A + temp;
        ZFLAG = zero_table[A];
        NFLAG = 0; 
      break;
      case 0xC7: //RST 0
        PUSH(PC.B.h); 
        PUSH(PC.B.l); 
        PC.W=0x0000; 
      break;
      case 0xC8: //RET Z
        if(ZFLAG) 
        { 
           cur_cycle = 20; 
           POP(PC.B.l); 
           POP(PC.B.h); 
        }
      break;
      case 0xC9: //RET
        POP(PC.B.l); 
        POP(PC.B.h);
      break;
      case 0xCA: //JP Z,xxxx
        if(ZFLAG) 
        { 
           cur_cycle = 16; 

           PC.W = readword(PC.W);
        } else 
           PC.W+=2;
      break;
      case 0xCB: //EXTENDED
        opcode = readopcode(PC.W++);
        cur_cycle =  CB_cycles[opcode];
        switch(opcode)
        {
        case 0x00: //RLC B
          CFLAG = (BC.B.h&0x80?1:0);
          BC.B.h = ((BC.B.h<<1)|CFLAG);
          ZFLAG = zero_table[BC.B.h];
          NFLAG = HFLAG = 0;  
        break;
        case 0x01: //RLC C
          CFLAG = (BC.B.l&0x80?1:0);
          BC.B.l = ((BC.B.l<<1)|CFLAG);
          ZFLAG = zero_table[BC.B.l];
          NFLAG = HFLAG = 0;
        break;
        case 0x02: //RLC D
          CFLAG = (DE.B.h&0x80?1:0);
          DE.B.h = ((DE.B.h<<1)|CFLAG);
          ZFLAG = zero_table[DE.B.h];
          NFLAG = HFLAG = 0;
        break;
        case 0x03: //RLC E
          CFLAG = (DE.B.l&0x80?1:0);
          DE.B.l = ((DE.B.l<<1)|CFLAG);
          ZFLAG = zero_table[DE.B.l];
          NFLAG = HFLAG = 0;
        break;
        case 0x04: //RLC H
          CFLAG = (HL.B.h&0x80?1:0);
          HL.B.h = ((HL.B.h<<1)|CFLAG);
          ZFLAG = zero_table[HL.B.h];
          NFLAG = HFLAG = 0;
        break;
        case 0x05: //RLC L
          CFLAG = (HL.B.l&0x80?1:0);
          HL.B.l = ((HL.B.l<<1)|CFLAG);
          ZFLAG = zero_table[HL.B.l];
          NFLAG = HFLAG = 0;
        break;
        case 0x06: //RLC (HL)
          temp = readmemory(HL.W);
          CFLAG = (temp&0x80?1:0);
          temp = ((temp<<1)|CFLAG);
          writememory(HL.W,temp);
          ZFLAG = zero_table[temp];
          NFLAG = HFLAG = 0;
        break;
        case 0x07: //RLC A
          CFLAG = (A&0x80?1:0);
          A = ((A<<1)|CFLAG);
          ZFLAG = zero_table[A];
          NFLAG = HFLAG = 0;
        break;
        case 0x08: //RRC B
          CFLAG = (BC.B.h&0x01);
          BC.B.h = ((BC.B.h>>1)|(BC.B.h<<7));
          ZFLAG = zero_table[BC.B.h];
          NFLAG = HFLAG = 0;
        break;
        case 0x09: //RRC C
          CFLAG = (BC.B.l&0x01);
          BC.B.l = ((BC.B.l>>1)|(BC.B.l<<7));
          ZFLAG = zero_table[BC.B.l];
          NFLAG = HFLAG = 0;
        break;
        case 0x0A: //RRC D
          CFLAG = (DE.B.h&0x01);
          DE.B.h = ((DE.B.h>>1)|(DE.B.h<<7));
          ZFLAG = zero_table[DE.B.h];
          NFLAG = HFLAG = 0;
        break;
        case 0x0B: //RRC E
          CFLAG = (DE.B.l&0x01);
          DE.B.l = ((DE.B.l>>1)|(DE.B.l<<7));
          ZFLAG = zero_table[DE.B.l];
          NFLAG = HFLAG = 0;
        break;
        case 0x0C: //RRC H
          CFLAG = (HL.B.h&0x01);
          HL.B.h = ((HL.B.h>>1)|(HL.B.h<<7));
          ZFLAG = zero_table[HL.B.h];
          NFLAG = HFLAG = 0;
        break;
        case 0x0D: //RRC L
          CFLAG = (HL.B.l&0x01);
          HL.B.l = ((HL.B.l>>1)|(HL.B.l<<7));
          ZFLAG = zero_table[HL.B.l];
          NFLAG = HFLAG = 0;
        break;
        case 0x0E: //RRC (HL)
          temp = readmemory(HL.W);
          CFLAG = (temp&0x01);
          temp = ((temp>>1)|(temp<<7));
          writememory(HL.W,temp);
          ZFLAG = zero_table[temp];
          NFLAG = HFLAG = 0;   
        break;
        case 0x0F: //RRC A
          CFLAG = (A&0x01);
          A = ((A>>1)|(A<<7));
          ZFLAG = zero_table[A];
          NFLAG = HFLAG = 0;
        break;
        case 0x10: //RL B
          carry = (BC.B.h&0x80?1:0);
          BC.B.h = ((BC.B.h<<1)|CFLAG);
          ZFLAG = zero_table[BC.B.h];
          CFLAG = carry;
          NFLAG = HFLAG = 0; 
        break;
        case 0x11: //RL C
          carry = (BC.B.l&0x80?1:0);
          BC.B.l = ((BC.B.l<<1)|CFLAG);
          ZFLAG = zero_table[BC.B.l];
          CFLAG = carry;
          NFLAG = HFLAG = 0; 
        break;
        case 0x12: //RL D
          carry = (DE.B.h&0x80?1:0);
          DE.B.h = ((DE.B.h<<1)|CFLAG);
          ZFLAG = zero_table[DE.B.h];
          CFLAG = carry;
          NFLAG = HFLAG = 0; 
        break;
        case 0x13: //RL E
          carry = (DE.B.l&0x80?1:0);
          DE.B.l = ((DE.B.l<<1)|CFLAG);
          ZFLAG = zero_table[DE.B.l];
          CFLAG = carry;
          NFLAG = HFLAG = 0;
        break;
        case 0x14: //RL H
          carry = (HL.B.h&0x80?1:0);
          HL.B.h = ((HL.B.h<<1)|CFLAG);
          ZFLAG = zero_table[HL.B.h];
          CFLAG = carry;
          NFLAG = HFLAG = 0;
        break;
        case 0x15: //RL L
          carry = (HL.B.l&0x80?1:0);
          HL.B.l = ((HL.B.l<<1)|CFLAG);
          ZFLAG = zero_table[HL.B.l];
          CFLAG = carry;
          NFLAG = HFLAG = 0;
        break;
        case 0x16: //RL (HL)
          temp = readmemory(HL.W);
          carry = (temp&0x80?1:0);
          temp = ((temp<<1)|CFLAG);
          writememory(HL.W,temp);
          ZFLAG = zero_table[temp];
          CFLAG = carry;
          NFLAG = HFLAG = 0;
        break;
        case 0x17: //RL A
          carry = (A&0x80?1:0);
          A = ((A<<1)|CFLAG);
          ZFLAG = zero_table[A];
          CFLAG = carry;
          NFLAG = HFLAG = 0;
        break;
        case 0x18: //RR B
          carry = (BC.B.h&0x01);
          BC.B.h = ((BC.B.h>>1)|(CFLAG?0x80:0));
          ZFLAG = zero_table[BC.B.h];
          CFLAG = carry;
          NFLAG = HFLAG = 0;
        break;
        case 0x19: //RR C
          carry = (BC.B.l&0x01);
          BC.B.l = ((BC.B.l>>1)|(CFLAG?0x80:0));
          ZFLAG = zero_table[BC.B.l];
          CFLAG = carry;
          NFLAG = HFLAG = 0;
        break;
        case 0x1A: //RR D
          carry = (DE.B.h&0x01);
          DE.B.h = ((DE.B.h>>1)|(CFLAG?0x80:0));
          ZFLAG = zero_table[DE.B.h];
          CFLAG = carry;
          NFLAG = HFLAG = 0;
        break;
        case 0x1B: //RR E
          carry = (DE.B.l&0x01);
          DE.B.l = ((DE.B.l>>1)|(CFLAG?0x80:0));
          ZFLAG = zero_table[DE.B.l];
          CFLAG = carry;
          NFLAG = HFLAG = 0;
        break;
        case 0x1C: //RR H
          carry = (HL.B.h&0x01);
          HL.B.h = ((HL.B.h>>1)|(CFLAG?0x80:0));
          ZFLAG = zero_table[HL.B.h];
          CFLAG = carry;
          NFLAG = HFLAG = 0;
        break;
        case 0x1D: //RR L
          carry = (HL.B.l&0x01);
          HL.B.l = ((HL.B.l>>1)|(CFLAG?0x80:0));
          ZFLAG = zero_table[HL.B.l];
          CFLAG = carry;
          NFLAG = HFLAG = 0;
        break;
        case 0x1E: //RR (HL)
          temp = readmemory(HL.W);
          carry = (temp&0x01);
          temp = ((temp>>1)|(CFLAG?0x80:0));
          writememory(HL.W,temp);
          ZFLAG = zero_table[temp];
          CFLAG = carry;
          NFLAG = HFLAG = 0;
        break;
        case 0x1F: //RR A
          carry = (A&0x01);
          A = ((A>>1)|(CFLAG?0x80:0));
          ZFLAG = zero_table[A];
          CFLAG = carry;
          NFLAG = HFLAG = 0;
        break;
        case 0x20: //SLA B
          CFLAG = (BC.B.h&0x80?1:0);
          BC.B.h <<= 1;
          ZFLAG = zero_table[BC.B.h];
          NFLAG = HFLAG = 0;
        break;
        case 0x21: //SLA C
          CFLAG = (BC.B.l&0x80?1:0);
          BC.B.l <<= 1;
          ZFLAG = zero_table[BC.B.l];
          NFLAG = HFLAG = 0;
        break;
        case 0x22: //SLA D
          CFLAG = (DE.B.h&0x80?1:0);
          DE.B.h <<= 1;
          ZFLAG = zero_table[DE.B.h];
          NFLAG = HFLAG = 0;
        break;
        case 0x23: //SLA E
          CFLAG = (DE.B.l&0x80?1:0);
          DE.B.l <<= 1;
          ZFLAG = zero_table[DE.B.l];
          NFLAG = HFLAG = 0;
        break;
        case 0x24: //SLA H
          CFLAG = (HL.B.h&0x80?1:0);
          HL.B.h <<= 1;
          ZFLAG = zero_table[HL.B.h];
          NFLAG = HFLAG = 0;
        break;
        case 0x25: //SLA L
          CFLAG = (HL.B.l&0x80?1:0);
          HL.B.l <<= 1;
          ZFLAG = zero_table[HL.B.l];
          NFLAG = HFLAG = 0;
        break;
        case 0x26: //SLA (HL)
          temp = readmemory(HL.W);
          CFLAG = (temp&0x80?1:0);
          temp <<= 1;
          writememory(HL.W,temp);
          ZFLAG = zero_table[temp];
          NFLAG = HFLAG = 0;
        break;
        case 0x27: //SLA A
          CFLAG = (A&0x80?1:0);
          A <<= 1;
          ZFLAG = zero_table[A];
          NFLAG = HFLAG = 0;
        break;
        case 0x28: //SRA B
          CFLAG = (BC.B.h&0x01);
          BC.B.h = ((BC.B.h>>1)|(BC.B.h&0x80));
          ZFLAG = zero_table[BC.B.h];
          NFLAG = HFLAG = 0;
        break;  
        case 0x29: //SRA C
          CFLAG = (BC.B.l&0x01);
          BC.B.l = ((BC.B.l>>1)|(BC.B.l&0x80));
          ZFLAG = zero_table[BC.B.l];
          NFLAG = HFLAG = 0;
        break;  
        case 0x2A: //SRA D
          CFLAG = (DE.B.h&0x01);
          DE.B.h = ((DE.B.h>>1)|(DE.B.h&0x80));
          ZFLAG = zero_table[DE.B.h];
          NFLAG = HFLAG = 0;
        break;     
        case 0x2B: //SRA E   
          CFLAG = (DE.B.l&0x01);
          DE.B.l = ((DE.B.l>>1)|(DE.B.l&0x80));
          ZFLAG = zero_table[DE.B.l];
          NFLAG = HFLAG = 0;
        break;
        case 0x2C: //SRA H
          CFLAG = (HL.B.h&0x01);
          HL.B.h = ((HL.B.h>>1)|(HL.B.h&0x80));
          ZFLAG = zero_table[HL.B.h];
          NFLAG = HFLAG = 0;
        break;   
        case 0x2D: //SRA L
          CFLAG = (HL.B.l&0x01);
          HL.B.l = ((HL.B.l>>1)|(HL.B.l&0x80));
          ZFLAG = zero_table[HL.B.l];
          NFLAG = HFLAG = 0;
        break;   
        case 0x2E: //SRA (HL)
          temp = readmemory(HL.W);
          CFLAG = (temp&0x01);
          temp = ((temp>>1)|(temp&0x80));
          writememory(HL.W,temp);
          ZFLAG = zero_table[temp];
          NFLAG = HFLAG = 0;
        break;   
        case 0x2F: //SRA A
          CFLAG = (A&0x01);
          A = ((A>>1)|(A&0x80));
          ZFLAG = zero_table[A];
          NFLAG = HFLAG = 0;
        break;   
        case 0x30: //SWAP B
          BC.B.h = ((BC.B.h>>4)|(BC.B.h<<4));       
          ZFLAG = zero_table[BC.B.h];
          NFLAG = HFLAG = CFLAG = 0;
        break;   
        case 0x31: //SWAP C
          BC.B.l = ((BC.B.l>>4)|(BC.B.l<<4));       
          ZFLAG = zero_table[BC.B.l];
          NFLAG = HFLAG = CFLAG = 0;
        break;   
        case 0x32: //SWAP D
          DE.B.h = ((DE.B.h>>4)|(DE.B.h<<4));       
          ZFLAG = zero_table[DE.B.h];
          NFLAG = HFLAG = CFLAG = 0;
        break;   
        case 0x33: //SWAP E
          DE.B.l = ((DE.B.l>>4)|(DE.B.l<<4));       
          ZFLAG = zero_table[DE.B.l];
          NFLAG = HFLAG = CFLAG = 0;
        break;   
        case 0x34: //SWAP H
          HL.B.h = ((HL.B.h>>4)|(HL.B.h<<4));       
          ZFLAG = zero_table[HL.B.h];
          NFLAG = HFLAG = CFLAG = 0;
        break;   
        case 0x35: //SWAP L
          HL.B.l = ((HL.B.l>>4)|(HL.B.l<<4));       
          ZFLAG = zero_table[HL.B.l];
          NFLAG = HFLAG = CFLAG = 0;
        break;   
        case 0x36: //SWAP (HL)
          temp = readmemory(HL.W);
          temp = ((temp>>4)|(temp<<4));
          writememory(HL.W,temp);       
          ZFLAG = zero_table[temp];
          NFLAG = HFLAG = CFLAG = 0;
        break;   
        case 0x37: //SWAP A
          A = ((A>>4)|(A<<4));       
          ZFLAG = zero_table[A];
          NFLAG = HFLAG = CFLAG = 0;
        break;  
        case 0x38: //SRL B
          CFLAG = (BC.B.h&0x01);
          BC.B.h >>= 1;
          ZFLAG = zero_table[BC.B.h];
          NFLAG = HFLAG = 0;
        break; 
        case 0x39: //SRL C
          CFLAG = (BC.B.l&0x01);
          BC.B.l >>= 1;
          ZFLAG = zero_table[BC.B.l];
          NFLAG = HFLAG = 0;
        break; 
        case 0x3A: //SRL D
          CFLAG = (DE.B.h&0x01);
          DE.B.h >>= 1;
          ZFLAG = zero_table[DE.B.h];
          NFLAG = HFLAG = 0;
        break; 
        case 0x3B: //SRL E
          CFLAG = (DE.B.l&0x01);
          DE.B.l >>= 1;
          ZFLAG = zero_table[DE.B.l];
          NFLAG = HFLAG = 0;
        break; 
        case 0x3C: //SRL H
          CFLAG = (HL.B.h&0x01);
          HL.B.h >>= 1;
          ZFLAG = zero_table[HL.B.h];
          NFLAG = HFLAG = 0;
        break; 
        case 0x3D: //SRL L
          CFLAG = (HL.B.l&0x01);
          HL.B.l >>= 1;
          ZFLAG = zero_table[HL.B.l];
          NFLAG = HFLAG = 0;
        break; 
        case 0x3E: //SRL (HL)
          temp = readmemory(HL.W);
          CFLAG = (temp&0x01);
          temp >>= 1;
          writememory(HL.W,temp);
          ZFLAG = zero_table[temp];
          NFLAG = HFLAG = 0;
        break; 
        case 0x3F: //SRL A
          CFLAG = (A&0x01);
          A >>= 1;
          ZFLAG = zero_table[A];
          NFLAG = HFLAG = 0;
        break; 
        case 0x40: //BIT 0,B
          ZFLAG = (BC.B.h&0x01?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x41: //BIT 0,C
          ZFLAG = (BC.B.l&0x01?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x42: //BIT 0,D
          ZFLAG = (DE.B.h&0x01?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x43: //BIT 0,E
          ZFLAG = (DE.B.l&0x01?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x44: //BIT 0,H
          ZFLAG = (HL.B.h&0x01?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x45: //BIT 0,L
          ZFLAG = (HL.B.l&0x01?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x46: //BIT 0,(HL)
          temp = readmemory(HL.W);
          ZFLAG = (temp&0x01?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x47: //BIT 0,A
          ZFLAG = (A&0x01?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x48: //BIT 1,B
          ZFLAG = (BC.B.h&0x02?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x49: //BIT 1,C
          ZFLAG = (BC.B.l&0x02?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x4A: //BIT 1,D
          ZFLAG = (DE.B.h&0x02?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x4B: //BIT 1,E
          ZFLAG = (DE.B.l&0x02?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x4C: //BIT 1,H
          ZFLAG = (HL.B.h&0x02?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x4D: //BIT 1,L
          ZFLAG = (HL.B.l&0x02?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x4E: //BIT 1,(HL)
          temp = readmemory(HL.W);
          ZFLAG = (temp&0x02?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x4F: //BIT 1,A
          ZFLAG = (A&0x02?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x50: //BIT 2,B
          ZFLAG = (BC.B.h&0x04?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x51: //BIT 2,C
          ZFLAG = (BC.B.l&0x04?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x52: //BIT 2,D
          ZFLAG = (DE.B.h&0x04?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x53: //BIT 2,E
          ZFLAG = (DE.B.l&0x04?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x54: //BIT 2,H
          ZFLAG = (HL.B.h&0x04?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x55: //BIT 2,L
          ZFLAG = (HL.B.l&0x04?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x56: //BIT 2,(HL)
          temp = readmemory(HL.W);
          ZFLAG = (temp&0x04?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x57: //BIT 2,A
          ZFLAG = (A&0x04?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x58: //BIT 3,B
          ZFLAG = (BC.B.h&0x08?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x59: //BIT 3,C
          ZFLAG = (BC.B.l&0x08?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x5A: //BIT 3,D
          ZFLAG = (DE.B.h&0x08?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x5B: //BIT 3,E
          ZFLAG = (DE.B.l&0x08?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x5C: //BIT 3,H
          ZFLAG = (HL.B.h&0x08?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x5D: //BIT 3,L
          ZFLAG = (HL.B.l&0x08?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x5E: //BIT 3,(HL)
          temp = readmemory(HL.W);
          ZFLAG = (temp&0x08?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x5F: //BIT 3,A
          ZFLAG = (A&0x08?0:1);
          NFLAG = 0; HFLAG = 1;
        break;
        case 0x60: //BIT 4,B
          ZFLAG = (BC.B.h&0x10?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x61: //BIT 4,C
          ZFLAG = (BC.B.l&0x10?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x62: //BIT 4,D
          ZFLAG = (DE.B.h&0x10?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x63: //BIT 4,E
          ZFLAG = (DE.B.l&0x10?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x64: //BIT 4,H
          ZFLAG = (HL.B.h&0x10?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x65: //BIT 4,L
          ZFLAG = (HL.B.l&0x10?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x66: //BIT 4,(HL)
          temp = readmemory(HL.W);
          ZFLAG = (temp&0x10?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x67: //BIT 4,A
          ZFLAG = (A&0x10?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x68: //BIT 5,B
          ZFLAG = (BC.B.h&0x20?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x69: //BIT 5,C
          ZFLAG = (BC.B.l&0x20?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x6A: //BIT 5,D
          ZFLAG = (DE.B.h&0x20?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x6B: //BIT 5,E
          ZFLAG = (DE.B.l&0x20?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x6C: //BIT 5,H
          ZFLAG = (HL.B.h&0x20?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x6D: //BIT 5,L
          ZFLAG = (HL.B.l&0x20?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x6E: //BIT 5,(HL)
          temp = readmemory(HL.W);
          ZFLAG = (temp&0x20?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x6F: //BIT 5,A
          ZFLAG = (A&0x20?0:1);
          NFLAG = 0; HFLAG = 1;
        break;       
        case 0x70: //BIT 6,B
          ZFLAG = (BC.B.h&0x40?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x71: //BIT 6,C
          ZFLAG = (BC.B.l&0x40?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x72: //BIT 6,D
          ZFLAG = (DE.B.h&0x40?0:1);
          NFLAG = 0; HFLAG = 1;
        break;
        case 0x73: //BIT 6,E
          ZFLAG = (DE.B.l&0x40?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x74: //BIT 6,H
          ZFLAG = (HL.B.h&0x40?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x75: //BIT 6,L
          ZFLAG = (HL.B.l&0x40?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x76: //BIT 6,(HL)
          temp = readmemory(HL.W);
          ZFLAG = (temp&0x40?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x77: //BIT 6,A
          ZFLAG = (A&0x40?0:1);
          NFLAG = 0; HFLAG = 1;
        break;
        case 0x78: //BIT 7,B
          ZFLAG = (BC.B.h&0x80?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x79: //BIT 7,C
          ZFLAG = (BC.B.l&0x80?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x7A: //BIT 7,D
          ZFLAG = (DE.B.h&0x80?0:1);
          NFLAG = 0; HFLAG = 1;
        break;
        case 0x7B: //BIT 7,E
          ZFLAG = (DE.B.l&0x80?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x7C: //BIT 7,H
          ZFLAG = (HL.B.h&0x80?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x7D: //BIT 7,L
          ZFLAG = (HL.B.l&0x80?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x7E: //BIT 7,(HL)
          temp = readmemory(HL.W);
          ZFLAG = (temp&0x80?0:1);
          NFLAG = 0; HFLAG = 1;
        break; 
        case 0x7F: //BIT 7,A
          ZFLAG = (A&0x80?0:1);
          NFLAG = 0; HFLAG = 1;
        break;   
        case 0x80: //RES 0,B
          BC.B.h &= 0xFE;
        break;     
        case 0x81: //RES 0,C
          BC.B.l &= 0xFE;
        break; 
        case 0x82: //RES 0,D
          DE.B.h &= 0xFE;
        break; 
        case 0x83: //RES 0,E
          DE.B.l &= 0xFE;
        break; 
        case 0x84: //RES 0,H
          HL.B.h &= 0xFE;
        break; 
        case 0x85: //RES 0,L
          HL.B.l &= 0xFE;
        break; 
        case 0x86: //RES 0,(HL)
          writememory(HL.W,readmemory(HL.W)&0xFE);
        break; 
        case 0x87: //RES 0,A
          A &= 0xFE;
        break; 
        case 0x88: //RES 1,B
          BC.B.h &= 0xFD;
        break;     
        case 0x89: //RES 1,C
          BC.B.l &= 0xFD;
        break; 
        case 0x8A: //RES 1,D
          DE.B.h &= 0xFD;
        break; 
        case 0x8B: //RES 1,E
          DE.B.l &= 0xFD;
        break; 
        case 0x8C: //RES 1,H
          HL.B.h &= 0xFD;
        break; 
        case 0x8D: //RES 1,L
          HL.B.l &= 0xFD;
        break; 
        case 0x8E: //RES 1,(HL)
          writememory(HL.W,readmemory(HL.W)&0xFD);
        break; 
        case 0x8F: //RES 1,A
          A &= 0xFD;
        break;
        case 0x90: //RES 2,B
          BC.B.h &= 0xFB;
        break;     
        case 0x91: //RES 2,C
          BC.B.l &= 0xFB;
        break; 
        case 0x92: //RES 2,D
          DE.B.h &= 0xFB;
        break; 
        case 0x93: //RES 2,E
          DE.B.l &= 0xFB;
        break; 
        case 0x94: //RES 2,H
          HL.B.h &= 0xFB;
        break; 
        case 0x95: //RES 2,L
          HL.B.l &= 0xFB;
        break; 
        case 0x96: //RES 2,(HL)
          writememory(HL.W,readmemory(HL.W)&0xFB);
        break; 
        case 0x97: //RES 2,A
          A &= 0xFB;
        break;
        case 0x98: //RES 3,B
          BC.B.h &= 0xF7;
        break;     
        case 0x99: //RES 3,C
          BC.B.l &= 0xF7;
        break; 
        case 0x9A: //RES 3,D
          DE.B.h &= 0xF7;
        break; 
        case 0x9B: //RES 3,E
          DE.B.l &= 0xF7;
        break; 
        case 0x9C: //RES 3,H
          HL.B.h &= 0xF7;
        break; 
        case 0x9D: //RES 3,L
          HL.B.l &= 0xF7;
        break; 
        case 0x9E: //RES 3,(HL)
          writememory(HL.W,readmemory(HL.W)&0xF7);
        break; 
        case 0x9F: //RES 3,A
          A &= 0xF7;
        break;
        case 0xA0: //RES 4,B
          BC.B.h &= 0xEF;
        break;     
        case 0xA1: //RES 4,C
          BC.B.l &= 0xEF;
        break; 
        case 0xA2: //RES 4,D
          DE.B.h &= 0xEF;
        break; 
        case 0xA3: //RES 4,E
          DE.B.l &= 0xEF;
        break; 
        case 0xA4: //RES 4,H
          HL.B.h &= 0xEF;
        break; 
        case 0xA5: //RES 4,L
          HL.B.l &= 0xEF;
        break; 
        case 0xA6: //RES 4,(HL)
          writememory(HL.W,readmemory(HL.W)&0xEF);
        break; 
        case 0xA7: //RES 4,A
          A &= 0xEF;
        break;
        case 0xA8: //RES 5,B
          BC.B.h &= 0xDF;
        break;     
        case 0xA9: //RES 5,C
          BC.B.l &= 0xDF;
        break; 
        case 0xAA: //RES 5,D
          DE.B.h &= 0xDF;
        break; 
        case 0xAB: //RES 5,E
          DE.B.l &= 0xDF;
        break; 
        case 0xAC: //RES 5,H
          HL.B.h &= 0xDF;
        break; 
        case 0xAD: //RES 5,L
          HL.B.l &= 0xDF;
        break; 
        case 0xAE: //RES 5,(HL)
          writememory(HL.W,readmemory(HL.W)&0xDF);
        break; 
        case 0xAF: //RES 5,A
          A &= 0xDF;
        break; 
        case 0xB0: //RES 6,B
          BC.B.h &= 0xBF;
        break;     
        case 0xB1: //RES 6,C
          BC.B.l &= 0xBF;
        break; 
        case 0xB2: //RES 6,D
          DE.B.h &= 0xBF;
        break; 
        case 0xB3: //RES 6,E
          DE.B.l &= 0xBF;
        break; 
        case 0xB4: //RES 6,H
          HL.B.h &= 0xBF;
        break; 
        case 0xB5: //RES 6,L
          HL.B.l &= 0xBF;
        break; 
        case 0xB6: //RES 6,(HL)
          writememory(HL.W,readmemory(HL.W)&0xBF);
        break; 
        case 0xB7: //RES 6,A
          A &= 0xBF;
        break;  
        case 0xB8: //RES 7,B
          BC.B.h &= 0x7F;
        break;     
        case 0xB9: //RES 7,C
          BC.B.l &= 0x7F;
        break; 
        case 0xBA: //RES 7,D
          DE.B.h &= 0x7F;
        break; 
        case 0xBB: //RES 7,E
          DE.B.l &= 0x7F;
        break; 
        case 0xBC: //RES 7,H
          HL.B.h &= 0x7F;
        break; 
        case 0xBD: //RES 7,L
          HL.B.l &= 0x7F;
        break; 
        case 0xBE: //RES 7,(HL)
          writememory(HL.W,readmemory(HL.W)&0x7F);
        break; 
        case 0xBF: //RES 7,A
          A &= 0x7F;
        break;   
        case 0xC0: //SET 0,B
          BC.B.h |= 0x01;
        break;  
        case 0xC1: //SET 0,C
          BC.B.l |= 0x01;
        break;
        case 0xC2: //SET 0,D
          DE.B.h |= 0x01;
        break;
        case 0xC3: //SET 0,E
          DE.B.l |= 0x01;
        break;
        case 0xC4: //SET 0,H
          HL.B.h |= 0x01;
        break;
        case 0xC5: //SET 0,L
          HL.B.l |= 0x01;
        break;
        case 0xC6: //SET 0,(HL)
          writememory(HL.W,readmemory(HL.W)|0x01);
        break;
        case 0xC7: //SET 0,A
          A |= 0x01;
        break;
        case 0xC8: //SET 1,B
          BC.B.h |= 0x02;
        break;  
        case 0xC9: //SET 1,C
          BC.B.l |= 0x02;
        break;
        case 0xCA: //SET 1,D
          DE.B.h |= 0x02;
        break;
        case 0xCB: //SET 1,E
          DE.B.l |= 0x02;
        break;
        case 0xCC: //SET 1,H
          HL.B.h |= 0x02;
        break;
        case 0xCD: //SET 1,L
          HL.B.l |= 0x02;
        break;
        case 0xCE: //SET 1,(HL)
          writememory(HL.W,readmemory(HL.W)|0x02);
        break;
        case 0xCF: //SET 1,A
          A |= 0x02;
        break;
        case 0xD0: //SET 2,B
          BC.B.h |= 0x04;
        break;  
        case 0xD1: //SET 2,C
          BC.B.l |= 0x04;
        break;
        case 0xD2: //SET 2,D
          DE.B.h |= 0x04;
        break;
        case 0xD3: //SET 2,E
          DE.B.l |= 0x04;
        break;
        case 0xD4: //SET 2,H
          HL.B.h |= 0x04;
        break;
        case 0xD5: //SET 2,L
          HL.B.l |= 0x04;
        break;
        case 0xD6: //SET 2,(HL)
          writememory(HL.W,readmemory(HL.W)|0x04);
        break;
        case 0xD7: //SET 2,A
          A |= 0x04;
        break;
        case 0xD8: //SET 3,B
          BC.B.h |= 0x08;
        break;  
        case 0xD9: //SET 3,C
          BC.B.l |= 0x08;
        break;
        case 0xDA: //SET 3,D
          DE.B.h |= 0x08;
        break;
        case 0xDB: //SET 3,E
          DE.B.l |= 0x08;
        break;
        case 0xDC: //SET 3,H
          HL.B.h |= 0x08;
        break;
        case 0xDD: //SET 3,L
          HL.B.l |= 0x08;
        break;
        case 0xDE: //SET 3,(HL)
          writememory(HL.W,readmemory(HL.W)|0x08);
        break;
        case 0xDF: //SET 3,A
          A |= 0x08;
        break;
        case 0xE0: //SET 4,B
          BC.B.h |= 0x10;
        break;  
        case 0xE1: //SET 4,C
          BC.B.l |= 0x10;
        break;
        case 0xE2: //SET 4,D
          DE.B.h |= 0x10;
        break;
        case 0xE3: //SET 4,E
          DE.B.l |= 0x10;
        break;
        case 0xE4: //SET 4,H
          HL.B.h |= 0x10;
        break;
        case 0xE5: //SET 4,L
          HL.B.l |= 0x10;
        break;
        case 0xE6: //SET 4,(HL)
          writememory(HL.W,readmemory(HL.W)|0x10);
        break;
        case 0xE7: //SET 4,A
          A |= 0x10;
        break;
        case 0xE8: //SET 5,B
          BC.B.h |= 0x20;
        break;  
        case 0xE9: //SET 5,C
          BC.B.l |= 0x20;
        break;
        case 0xEA: //SET 5,D
          DE.B.h |= 0x20;
        break;
        case 0xEB: //SET 5,E
          DE.B.l |= 0x20;
        break;
        case 0xEC: //SET 5,H
          HL.B.h |= 0x20;
        break;
        case 0xED: //SET 5,L
          HL.B.l |= 0x20;
        break;
        case 0xEE: //SET 5,(HL)
          writememory(HL.W,readmemory(HL.W)|0x20);
        break;
        case 0xEF: //SET 5,A
          A |= 0x20;
        break;
        case 0xF0: //SET 6,B
          BC.B.h |= 0x40;
        break;  
        case 0xF1: //SET 6,C
          BC.B.l |= 0x40;
        break;
        case 0xF2: //SET 6,D
          DE.B.h |= 0x40;
        break;
        case 0xF3: //SET 6,E
          DE.B.l |= 0x40;
        break;
        case 0xF4: //SET 6,H
          HL.B.h |= 0x40;
        break;
        case 0xF5: //SET 6,L
          HL.B.l |= 0x40;
        break;
        case 0xF6: //SET 6,(HL)
          writememory(HL.W,readmemory(HL.W)|0x40);
        break;
        case 0xF7: //SET 6,A
          A |= 0x40;
        break;
        case 0xF8: //SET 7,B
          BC.B.h |= 0x80;
        break;  
        case 0xF9: //SET 7,C
          BC.B.l |= 0x80;
        break;
        case 0xFA: //SET 7,D
          DE.B.h |= 0x80;
        break;
        case 0xFB: //SET 7,E
          DE.B.l |= 0x80;
        break;
        case 0xFC: //SET 7,H
          HL.B.h |= 0x80;
        break;
        case 0xFD: //SET 7,L
          HL.B.l |= 0x80;
        break;
        case 0xFE: //SET 7,(HL)
          writememory(HL.W,readmemory(HL.W)|0x80);
        break;
        case 0xFF: //SET 7,A
          A |= 0x80;
        break;
        //default:
        // shouldn't ever get here...
        //break;
        }
      break;
      case 0xCC: //CALL Z,xxxx
        if(ZFLAG) 
        { 
           cur_cycle = 24; 

           tempreg.W = readword(PC.W);
           PC.W+=2;   
           PUSH(PC.B.h); 
           PUSH(PC.B.l); 

           PC.W = tempreg.W;  
        } else 
           PC.W+=2;
      break;
      case 0xCD: //CALL xxxx
        tempreg.W = readword(PC.W);
        PC.W+=2;   
        PUSH(PC.B.h); 
        PUSH(PC.B.l); 

        PC.W = tempreg.W;  
      break;
      case 0xCE: //ADC xx
        temp = readopcode(PC.W++);
        tempreg.W = A + temp + CFLAG;
        CFLAG = (tempreg.B.h?1:0);
        HFLAG = (((A^temp^tempreg.B.l)&0x10)?1:0);
        A = tempreg.B.l;
        ZFLAG = zero_table[A];
        NFLAG = 0;  
      break;
      case 0xCF: //RST 8
        PUSH(PC.B.h); 
        PUSH(PC.B.l); 

        PC.W=0x0008;
      break;
      case 0xD0: //RET NC
        if(!CFLAG) 
        { 
           cur_cycle = 20; 
           POP(PC.B.l); 
           POP(PC.B.h); 
        }
      break;
      case 0xD1: //POP DE
        POP(DE.B.l); 
        POP(DE.B.h);
      break;
      case 0xD2: //JP NC,xxxx
        if(!CFLAG) 
        { 
           cur_cycle = 16; 

           PC.W = readword(PC.W); 
        } else
           PC.W+=2; 
      break;
      case 0xD4: //CALL NC,xxxx
        if(!CFLAG) 
        { 
           cur_cycle = 24; 

           tempreg.W = readword(PC.W);
           PC.W+=2;   
           PUSH(PC.B.h); 
           PUSH(PC.B.l); 

           PC.W = tempreg.W;   
        } else 
           PC.W+=2;
      break;
      case 0xD5: //PUSH DE
       PUSH(DE.B.h); 
       PUSH(DE.B.l);   
      break;
      case 0xD6: //SUB xx
        temp = readopcode(PC.W++);
        CFLAG = (A < temp);
        HFLAG = ((A&0x0F) < (temp&0x0F));
        A = A - temp;
        ZFLAG = zero_table[A];
        NFLAG = 1; 
      break;
      case 0xD7: //RST 10
        PUSH(PC.B.h); 
        PUSH(PC.B.l); 

        PC.W=0x0010; 
      break;
      case 0xD8: //RET C
        if(CFLAG) 
        { 
           cur_cycle = 20; 
           POP(PC.B.l); 
           POP(PC.B.h); 
         
        }
      break;
      case 0xD9: //RETI // Return, enable interrupts
        POP(PC.B.l); 
        POP(PC.B.h); 
  
        IME = 1;
      break;
      case 0xDA: //JP C,xxxx
        if(CFLAG) 
        { 
           cur_cycle = 16; 

           PC.W = readword(PC.W);
        } else 
           PC.W+=2;
      break;
      case 0xDC: //CALL C,xxxx
        if(CFLAG) 
        { 
           cur_cycle = 24; 

           tempreg.W = readword(PC.W);
           PC.W+=2;   
           PUSH(PC.B.h); 
           PUSH(PC.B.l); 
        
           PC.W = tempreg.W;  
        } else 
           PC.W+=2;
      break;
      case 0xDE: //SBC xx
        temp = readopcode(PC.W++);
        tempreg.W = A - temp - CFLAG;
        CFLAG = (tempreg.B.h?1:0);
        HFLAG = ((A^temp^tempreg.B.l)&0x10?1:0);
        A = tempreg.B.l;
        ZFLAG = zero_table[A];
        NFLAG = 1;  
      break;
      case 0xDF: //RST 18
        PUSH(PC.B.h); 
        PUSH(PC.B.l); 
      
        PC.W=0x0018; 
      break;
      case 0xE0: //LD ($FF00+xx),A "LDH (x),A"
        writememory(0xFF00+readopcode(PC.W++),A);
      break;
      case 0xE1: //POP HL
       POP(HL.B.l); 
       POP(HL.B.h); 
      break;
      case 0xE2: //LD ($FF00+C),A
        writememory(0xFF00+BC.B.l,A);
      break;
      case 0xE5: //PUSH HL
       PUSH(HL.B.h); 
       PUSH(HL.B.l);
      break;
      case 0xE6: //AND xx
        A &= readopcode(PC.W++);
        ZFLAG = zero_table[A];
        HFLAG = 1; 
        NFLAG = CFLAG = 0; 
      break;
      case 0xE7: //RST 20
        PUSH(PC.B.h); 
        PUSH(PC.B.l); 
      
        PC.W=0x0020; 
      break;
      case 0xE8: //ADD SP,xx
        offset = (signed char)readopcode(PC.W++);
        tempreg.W = SP.W + offset;
         
        if(offset>=0)
        {
          CFLAG = (SP.W > tempreg.W); 
          HFLAG = ((SP.W^offset^tempreg.W)&0x1000?1:0);
          SP.W = tempreg.W;
        } else
        {
          CFLAG = (SP.W < tempreg.W);
          HFLAG = ((SP.W^offset^tempreg.W)&0x1000?1:0);
          SP.W = tempreg.W;
        }  
        ZFLAG = NFLAG = 0;   
      break;
      case 0xE9: //JP HL "LD PC,HL"
        PC.W = HL.W;
      break;
      case 0xEA: //LD (xxxx),A
        writememory(readword(PC.W),A);
        PC.W += 2;
      break;
      case 0xEE: //XOR xx
        A ^= readopcode(PC.W++);
        ZFLAG = zero_table[A];
        NFLAG = HFLAG = CFLAG = 0;
      break;
      case 0xEF: //RST 28
        PUSH(PC.B.h); 
        PUSH(PC.B.l); 
       
        PC.W=0x0028; 
      break;
      case 0xF0: //LD A,($FF00+xx) "LDH A,(x)"
        A = readmemory(0xFF00+readopcode(PC.W++));
      break;
      case 0xF1: //POP AF
       POP(F);
       POP(A); 
       CFLAG = (F&0x10?1:0);
       HFLAG = (F&0x20?1:0);
       NFLAG = (F&0x40?1:0);
       ZFLAG = (F&0x80?1:0);
      break;
      case 0xF2: //LD A,($FF00+C)
        A = readmemory(0xFF00+BC.B.l);
      break;
      case 0xF3: //DI disable interrupts
        IME = EI_count = 0;
      break;
      case 0xF5: //PUSH AF
       PUSH(A); 
       F = ((CFLAG<<4)|(HFLAG<<5)|(NFLAG<<6)|(ZFLAG<<7)); 
       PUSH(F);
      break;
      case 0xF6: //OR xx
        A |= readopcode(PC.W++);
        ZFLAG = zero_table[A];
        NFLAG = HFLAG = CFLAG = 0;
      break;
      case 0xF7: //RST 30
        PUSH(PC.B.h); 
        PUSH(PC.B.l);
       
        PC.W=0x0030; 
      break;
      case 0xF8: //LD HL,SP+xx
        offset = (signed char)readopcode(PC.W++);
        tempreg.W = SP.W + offset;
        
        if(offset >= 0) 
        {
           CFLAG = (SP.W > tempreg.W);
           HFLAG = ((SP.W^offset^tempreg.W)&0x1000?1:0);
           HL.W = tempreg.W;
        } else
        {
           CFLAG = (SP.W > tempreg.W);
           HFLAG = ((SP.W^offset^tempreg.W)&0x1000?1:0);
           HL.W = tempreg.W;
        }
        ZFLAG = NFLAG = 0; 
      break;
      case 0xF9: //LD SP,HL
        SP.W = HL.W;
      break;
      case 0xFA: //LD A,(xxxx)
        A = readmemory(readword(PC.W));
        PC.W+=2;
      break;
      case 0xFB: //EI enable interrupts after next instruction
         EI_count = 1;
      break;
      case 0xFE: //CP xx
        temp = readopcode(PC.W++);         
        CFLAG = (A < temp);
        HFLAG = ((A&0x0F) < (temp&0x0F));
        ZFLAG = (A == temp);
        NFLAG = 1; 
      break;
      case 0xFF: //RST 38
        PUSH(PC.B.h); 
        PUSH(PC.B.l); 
       
        PC.W=0x0038; 
      break;
      default:
        if(options->halt_on_unknown_opcode)
        {
           char buffer[100];

           sprintf(buffer,"%s PC:%x, %s:%x bank %x",str_table[ERROR_UNK_OPCODE],PC.W,str_table[ERROR_OPCODE],opcode,gb_system::MBClo);
           debug_print(buffer);  

           emulating = false;
           ++frames;
        }
      break;
   }

