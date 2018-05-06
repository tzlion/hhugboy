
#ifndef HHUGBOY_LINKER_H
#define HHUGBOY_LINKER_H

/* data types */
typedef unsigned char		U8;
typedef unsigned short		U16;

//data		register(baseaddress + 0)  	0x378  	0x278
#define LPTREG_DATA					0x378
//status	register (baseaddress + 1) 	0x379 	0x279
#define LPTREG_STATUS				(LPTREG_DATA + 1)
//control	register (baseaddress + 2) 	0x37a 	0x27a
#define LPTREG_CONTROL				(LPTREG_DATA + 2)

enum {
    STATUS_BUSY				= 0x80,
    STATUS_ACK				= 0x40,
    STATUS_PAPER			= 0x20,
    STATUS_SELECTIN			= 0x10,
    STATUS_ERROR			= 0x08,
    STATUS_NIRQ				= 0x04,
};

enum {
    CTL_MODE_DATAIN			= 0x20,
    CTL_MODE_IRQACK			= 0x10,
    CTL_SELECT				= 0x08,
    CTL_INIT				= 0x04,
    CTL_LINEFEED			= 0x02,
    CTL_STROBE				= 0x01,
};

enum {
    // .... ..cd
    //	c:	clock
    //	d:	data (serial bit)
            D_CLOCK_HIGH	= 0x02,
};

class linker {
public:
    static U8 bank0[0x4000];
    static int initlinker();
    static int deinitlinker();
    static void gb_sendwrite(U16 addr, U8 val);
    static void gb_sendblockread(U16 addr, U16 length);
    static void gb_readbank(U8* dest, U16 addr, int len);
    static U8 gb_readbyte();
};

#endif //HHUGBOY_LINKER_H
