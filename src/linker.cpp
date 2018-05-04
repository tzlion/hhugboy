
#include "linker.h"

/******************************************************************************/
// gblinkdl.cpp
// Original by Brian Provinciano
// http://www.bripro.com
// November 2nd, 2005
// Modified by taizou 2016-2017
/******************************************************************************/
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#include "windows.h"
#include "debug.h"

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
using namespace std;
/******************************************************************************/
GBHDR hdr;
U8 linker::bank0[0x4000];
char szt[1000];

#ifdef _WIN32
typedef void(__stdcall *lpOut32)(short, short);
typedef short(__stdcall *lpInp32)(short);
lpOut32 gfpOut32;
lpInp32 gfpInp32;
#endif

char msg[420];

/******************************************************************************/
unsigned char inportb(unsigned short port)
{
    return gfpInp32(port);
}
/******************************************************************************/
void outportb(unsigned short port, unsigned char value)
{
    gfpOut32(port,value);
}

/******************************************************************************/
void lptdelay(int amt)
{
    for(int i=0;i<amt;i++)
        inportb(LPTREG_DATA);
}
/******************************************************************************/
U8 gb_sendbyte(U8 value)
{
    U8 read = 0;
    for(int i=7;i>=0;i--) {
        U8 v = (value>>i)&1;

        outportb(LPTREG_DATA, v|D_CLOCK_HIGH);
        outportb(LPTREG_DATA, v);

        U8 stat = inportb(LPTREG_STATUS);

        if(!(stat&STATUS_BUSY))
            read |= (1<<i);

        outportb(LPTREG_DATA, v|D_CLOCK_HIGH);
    }
    lptdelay(64);
    return read;
}
/******************************************************************************/
U8 linker::gb_readbyte()
{
    U8 read = 0;
    for(int i=7;i>=0;i--) {
        outportb(LPTREG_DATA, D_CLOCK_HIGH);
        outportb(LPTREG_DATA, 0);

        if(!(inportb(LPTREG_STATUS)&STATUS_BUSY))
            read |= (1<<i);
        outportb(LPTREG_DATA, D_CLOCK_HIGH);
    }
    // delay between bytes
    lptdelay(64);
    return read;
}
/******************************************************************************/
U16 gb_readword()
{
    return (linker::gb_readbyte()<<8) | (linker::gb_readbyte());
}
/******************************************************************************/
char *gb_readstring(char *out, int len)
{
    for(int i=0;i<len;i++)
        out[i] = linker::gb_readbyte();
    return out;
}
/******************************************************************************/
// can try to write to an area with the same value in case of bus conflicts
void gb_sendbankwrite(U16 start, U16 end, U8 val)
{
    // This isn't used BUT must have bank0 populated before its run
    while(start<=end) {
        if(linker::bank0[start]==val)
            break;
        start++;
    }
    if(start>end)
        start = end;
    gb_sendbyte(start>>8);
    gb_sendbyte(start&0xFF);
    gb_sendbyte(val);
    sprintf(msg,"  Wrote %02X -> %04X\n",val,start);
    debug_win(msg);

}
/******************************************************************************/
void linker::gb_sendwrite(U16 addr, U8 val)
{
    gb_sendbyte(0x49);
    gb_sendbyte(addr>>8);
    gb_sendbyte(addr&0xFF);
    gb_sendbyte(val);
    sprintf(msg,"  Wrote %02X -> %04X\n",val,addr);
    debug_win(msg);
}
/******************************************************************************/
void linker::gb_sendblockread(U16 addr, U16 length)
{
    gb_sendbyte(0x59);
    gb_sendbyte(addr>>8);
    gb_sendbyte(addr&0xFF);
    gb_sendbyte(length>>8);
    gb_sendbyte(length&0xFF);
    sprintf(msg,"  Starting Block Read: %04X (%04X in size)\n",addr, length);
    //debug_win(msg);
}
/******************************************************************************/
void gb_readblock(FILE *f, U16 addr, int len)
{
    sprintf(msg,"READ BLOCK: %04X, %04X\n",addr,len);
    debug_win(msg);
    linker::gb_sendblockread(addr,len);
    for(int i=0;i<len;i++)
        fputc(linker::gb_readbyte(),f);
}
/******************************************************************************/
void linker::gb_readbank(U8* dest, U16 addr, int len)
{
    sprintf(msg,"READ BLOCK: %04X, %04X\n",addr,len);
    debug_win(msg);
    linker::gb_sendblockread(addr,len);
    for(int i=0;i<len;i++)
        dest[i] = linker::gb_readbyte();
}
/******************************************************************************/
void readBankZero()
{
    // read the first bank of ROM
    debug_win("\nDownloading first bank...\n");
    for(int i=0;i<0x4000;i++) {
        linker::bank0[i] = linker::gb_readbyte();
    }
}

/******************************************************************************/

HINSTANCE hInpOutDll;

/******************************************************************************/
int linker::initlinker()
{

    hInpOutDll = LoadLibrary("inpout32.dll");
    if (hInpOutDll != NULL) {
        gfpOut32 = (lpOut32)GetProcAddress(hInpOutDll, "Out32");
        gfpInp32 = (lpInp32)GetProcAddress(hInpOutDll, "Inp32");
    } else {
        debug_win("Unable to load inpout32.dll\n");
        return -1;
    }

    debug_win(
            "GBlinkDX PC Client v0.2\n"
                    "Original GBlinkdl by Brian Provinciano November 2nd, 2005 http://www.bripro.com\n"
                    "Modified by taizou 2016-2017\n\n");

    debug_win("Setting up ports...\n");

    // set up the parallel port
    outportb(LPTREG_CONTROL, inportb(LPTREG_CONTROL)&(~CTL_MODE_DATAIN));
    outportb(LPTREG_DATA, 0xFF);
    outportb(LPTREG_DATA, D_CLOCK_HIGH);

    // perform communication
    debug_win("Waiting for Game Boy...\n");
    while(gb_sendbyte(0x9A)!=0xB4) {}
    lptdelay(2000);
    if(gb_sendbyte(0x9A)!=0x1D) {
        debug_win("Bad connection\n");
        return 1;
    }

    debug_win("Connected.\n\n");

    // read header info (not really needed anymore as I read the first block later)
    hdr.carttype = gb_readbyte();
    hdr.romsize = gb_readbyte();
    hdr.ramsize = gb_readbyte();
    hdr.checksum = gb_readword();

    gb_readstring(hdr.gamename,16);
    hdr.gamename[16] = '\0';

    sprintf(msg,
            "GAME:     %s\n"
                    "CARTTYPE: %02Xh\n"
                    "ROMSIZE:  %02Xh\n"
                    "RAMSIZE:  %02Xh\n"
                    "CHECKSUM: %04Xh\n\n",
            hdr.gamename,hdr.carttype,hdr.romsize,hdr.ramsize,hdr.checksum
    );
    debug_win(msg);

    debug_win("press enter to continue");
    std::string z;
    //getline(std::cin,z);

    if(gb_readbyte() != 0) {// verify we're done
        debug_win("expected 0x00 from GB, bad connection\n");
        return 1;
    }
    if(gb_readbyte() != 0xFF) {// verify we're done
        debug_win("expected 0xFF from GB, bad connection\n");
        return 1;
    }

    debug_win("\nReceiving...\n");


    readBankZero();



    return 0;

}

int linker::deinitlinker() {
    outportb(LPTREG_DATA, D_CLOCK_HIGH);
    outportb(LPTREG_CONTROL, inportb(LPTREG_CONTROL)&(~CTL_MODE_DATAIN));
    outportb(LPTREG_DATA, 0xFF);
    FreeLibrary(hInpOutDll);
}
/******************************************************************************/