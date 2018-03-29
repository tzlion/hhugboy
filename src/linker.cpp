
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
#ifdef __linux__
#include <sys/io.h>
#elif defined(__FreeBSD__)
#include <sys/types.h>
#include <machine/cpufunc.h>
#include <machine/sysarch.h>
#elif defined(_WIN32)
#include "windows.h"
#include "debug.h"

#else
#error Unsupported platform
#endif
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
#ifdef _WIN32
    return gfpInp32(port);
#else
    return inb(port);
#endif
}
/******************************************************************************/
void outportb(unsigned short port, unsigned char value)
{
#ifdef _WIN32
    gfpOut32(port,value);
#elif defined(__FreeBSD__)
    outb(port,value);
#elif defined(__linux__)
   outb(value,port);
#endif
}
/******************************************************************************/
char *printbin(U8 v)
{
    char *s = szt;
    for(int i=7;i>=0;i--) {
        *s++ = (v>>i)&1?'1':'.';
        if(i==4) *s++ = '-';
    }
    *s = '\0';
    return szt;
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
int doKindaCrappyScriptedWrites(char* scriptName)
{
    FILE *script = fopen(scriptName, "r");
    if (!script) {
        sprintf(msg,"Unable to open file: %s for reading!\n", scriptName);
        debug_win(msg);
        return 2;
    }

    char line[10];
    bool oddline = false;
    int lastval = 0;
    while (!feof(script)) {
        fgets(line, 10, script);
        int i = strtol(line, NULL, 16);
        if (!oddline) {
            oddline = true;
        }
        else {
            linker::gb_sendwrite(lastval, i);
            oddline = false;
        }
        lastval = i;
    }
    return 0;
}
/******************************************************************************/
int doDump(char* filename,bool overrideMode,U8 carttype, int bankscount)
{
    FILE *f = fopen(filename,"wb");
    if(!f) {
        sprintf(msg,"Unable to open file: %s for writing!\n",filename);
        debug_win(msg);
        return 2;
    }

    sprintf(msg,"\nWriting to file: %s\n",filename);
    debug_win(msg);

    if (overrideMode) {
        debug_win("CUSTOM MODE\n");
        bankscount = 0x100;
        carttype = 0xFF;
    }

    // dump the data
    switch(carttype) {
        case 0: // 0 - ROM ONLY
            fwrite(linker::bank0,0x4000,1,f);
            gb_readblock(f, 0x4000,0x4000);
            break;
        case 1: // ROM+MBC1
        case 2: // ROM+MBC1+RAM
        case 3: // ROM+MBC1+RAM+BATT
            fwrite(linker::bank0,0x4000,1,f);

            if(memcmp(hdr.gamename,"SUPER MARIO 3",13)==0) {
                debug_win("Detected Super Mario 3 Special. Doing RAW Dump\n");
                const int remap[13] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x08,0x0B,0x0C,0x0D,0x0F,0x13};
                linker::gb_sendwrite(0x6000,5);
                linker::gb_sendwrite(0x5000,0);
                for(int bank = 1; bank<13; bank++) {
                    linker::gb_sendwrite(0x2000,remap[bank]);
                    gb_readblock(f, 0x4000,0x4000);
                    sprintf(msg,"bank %02x/%02x transferred.\n",bank,13);
                    debug_win(msg);
                }
                for(int i=0;i<3*0x4000;i++)
                    fputc(0xFF,f);
            } else {
                // normal MBC1
                for(int bank = 1; bank<hdr.totalbanks; bank++) {
                    linker::gb_sendwrite(0x2000,bank);
                    gb_readblock(f, 0x4000,0x4000);
                    sprintf(msg,"bank %02x/%02x transferred.\n",bank,hdr.totalbanks-1);
                    debug_win(msg);
                }
            }
            break;
        default:
            sprintf(msg,"Cartridge type (real): %02X\n", hdr.carttype);
            debug_win(msg);
            debug_win("Trying as regular ass MBC(5)\n");
            gb_readblock(f, 0x0000, 0x4000);
            for(int bank = 1; bank<bankscount; bank++) {
                linker::gb_sendwrite(0x2000,bank);
                gb_readblock(f, 0x4000,0x4000);
                sprintf(msg,"bank %02x/%02x transferred.\n",bank,bankscount-1);
                debug_win(msg);
            }
            break;
    }
    fclose(f);

    debug_win("\ndone.\n");

    return 0;

}
/******************************************************************************/
// http://stackoverflow.com/questions/236129/split-a-string-in-c
void split(const string &s, char delim, vector<string> &elems) {
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
}
vector<string> split(const string &s, char delim) {
    vector<string> elems;
    split(s, delim, elems);
    return elems;
}
/******************************************************************************/
int interactive()
{
    debug_win("\n* Interactive mode *\n\n");
    debug_win("Usage:\nr xxxx yy to read yy bytes from xxxx\nw xxxx yy to write yy to xxxx\nd to dump 4mb as standard mbc\na to attempt auto-detect and dump\nt to reread title\nx to exit\n"
                   "All numbers should be in hexadecimal\nMultiple commands can be separated by semicolons\nDon't put a space after the semicolon tho\n");

    while(1) {
        std::string inpstring = "";
        std::string mode = "";
        debug_win("\nEnter command: ");
        getline(std::cin,inpstring);

        debug_win("\n");

        vector<string> commands = split(inpstring,';');

        for(int s=0;s<commands.size();s++) {

            string command = commands[s];

            mode = command.substr(0,1);

            if ( mode == "x" ) {
                return -1;
            }
            if ( mode == "d" ) {
                return 1;
            }
            if ( mode == "a" ) {
                return 2;
            }
            if ( mode == "t" ) {
                linker::gb_sendblockread(0x134,16);
                char newtitle[17];
                gb_readstring(newtitle,16);
                newtitle[16] = '\0';
                sprintf(msg,"Title: %s\n",newtitle);
                debug_win(msg);
            }
            if ( ( mode == "r" || mode == "w" ) )  {

                vector<string> parts = split(command,' ');

                if ( parts.size() != 3 ) {
                    debug_win("Unrecognised format\n");
                    continue;
                }
                if ( mode == "r" ) {
                    int addr = strtol(parts[1].c_str(), NULL, 16);
                    int len= strtol(parts[2].c_str(), NULL, 16);

                    linker::gb_sendblockread(addr,len);
                    for(int i=0;i<len;i++) {
                        if (i>0 && i % 16 == 0) {
                            debug_win("\n");
                        }
                        sprintf(msg,"%02x ",linker::gb_readbyte());
                        debug_win(msg);
                    }

                    debug_win("\n");
                }
                if ( mode == "w" ) {
                    int addr = strtol(parts[1].c_str(), NULL, 16);
                    int val = strtol(parts[2].c_str(), NULL, 16);
                    linker::gb_sendwrite(addr,val);
                }
            }


        }

    }
    return 0;
}

#ifdef _WIN32
HINSTANCE hInpOutDll;
#endif

/******************************************************************************/
int linker::initlinker()
{
    int argc=3;
    char* argv[3];
    argv[1] = "fuc.gb";
    argv[2] = "-i";


#ifdef _WIN32
    hInpOutDll = LoadLibrary("inpout32.dll");
    if (hInpOutDll != NULL) {
        gfpOut32 = (lpOut32)GetProcAddress(hInpOutDll, "Out32");
        gfpInp32 = (lpInp32)GetProcAddress(hInpOutDll, "Inp32");
    } else {
        debug_win("Unable to load inpout32.dll\n");
        return -1;
    }
#endif
    debug_win(
            "GBlinkDX PC Client v0.2\n"
                    "Original GBlinkdl by Brian Provinciano November 2nd, 2005 http://www.bripro.com\n"
                    "Modified by taizou 2016-2017\n\n");

    if(argc < 2) {
        debug_win("Usage: gblinkdx \"output filename\" [option]\n"
                       " Option can be: -i for interactive mode\n"
                       "                -o to override auto-detection and dump max size as standard MBC\n"
                       "                Any other value will be treated as a pre-dump script filename\n"
                       "                (Scripted mode implies -o)\n\n");
        return 3;
    }

    debug_win("Setting up ports...\n");

#ifdef __linux__
    ioperm(LPTREG_DATA, 3 , true);
#elif defined(__FreeBSD__)
    i386_set_ioperm(LPTREG_DATA, 3, true);
#endif
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

    hdr.totalbanks = (1<<(hdr.romsize&0xF)) * 2;
    if(hdr.romsize&0xF0)
        hdr.totalbanks += (1<<((hdr.romsize>>4)&0xF)) * 2;

    sprintf(msg,
            "size: %d KB\n",
            hdr.totalbanks*16
    );
    debug_win(msg);

    debug_win("\nReceiving...\n");

    U8 carttype = hdr.carttype;
    int bankscount = hdr.totalbanks;

    readBankZero();

    char* filename=argv[1];

    bool interactiveMode = false;
    bool overrideMode = false;
    bool scriptedMode = false;
    char* scriptName;
    if (argc >= 3) {
        if ( memcmp(argv[2],"-o",8) == 0 )
            overrideMode = true;
        else if ( memcmp(argv[2],"-i",2) == 0 )
            interactiveMode = true;
        else {
            scriptedMode = true;
            overrideMode = true;
            scriptName = argv[2];
        }
    }


    return 0;

    while(1) {

        bool dump = false;

        if (interactiveMode) {
            int intret = interactive();
            if ( intret >= 1 )  {
                dump = true;
                if (intret == 2) overrideMode = false;
                else overrideMode = true;
            } else if ( intret == -1 ) {
                break;
            }
        } else {
            dump = true;
        }

        if ( dump ) {

            if (scriptedMode){
                int scretval = doKindaCrappyScriptedWrites(scriptName);
                if (scretval > 0) return scretval;
            }

            int retval= doDump(filename,overrideMode,carttype, bankscount);
            if ( retval != 0 ) return retval;
        }

        if ( interactiveMode ) {
            debug_win("\nReturn to interactive mode y/n?\n");
        } else {
            debug_win("\nDump again y/n?\n");
        }
        std::string z;
        getline(std::cin,z);
        if ( z != "y" ) {
            break;
        }

    }

    outportb(LPTREG_DATA, D_CLOCK_HIGH);
    outportb(LPTREG_CONTROL, inportb(LPTREG_CONTROL)&(~CTL_MODE_DATAIN));
    outportb(LPTREG_DATA, 0xFF);

#ifdef _WIN32
    FreeLibrary(hInpOutDll);
#endif

    debug_win("exiting\n");

    return 0;
}

int linker::deinitlinker() {
    outportb(LPTREG_DATA, D_CLOCK_HIGH);
    outportb(LPTREG_CONTROL, inportb(LPTREG_CONTROL)&(~CTL_MODE_DATAIN));
    outportb(LPTREG_DATA, 0xFF);
#ifdef _WIN32
    FreeLibrary(hInpOutDll);
#endif
}
/******************************************************************************/