
#ifndef HHUGBOY_LINKER_H
#define HHUGBOY_LINKER_H

typedef unsigned char U8;
typedef unsigned short U16;

#define LPTREG_DATA 0x378
#define LPTREG_STATUS (LPTREG_DATA + 1)
#define LPTREG_CONTROL (LPTREG_DATA + 2)

#define STATUS_BUSY 0x80
#define CTL_MODE_DATAIN 0x20
#define D_CLOCK_HIGH 0x02

class GbLinker {
private:
    static unsigned char inportb(unsigned short port);
    static void outportb(unsigned short port, unsigned char value);
    static void lptdelay(int amt);
    static U8 gb_readbyte();
    static U8 gb_sendbyte(U8 value);
    static void gb_sendblockread(U16 addr, U16 length);
    static void gb_sendwrite(U16 addr, U8 val);
    static void readBankZero();
public:
    static bool linkerActive;
    static bool linkerInitialising;
    static U8 bank0[0x4000];
    static bool initLinker();
    static void deinitLinker();
    static void readBlock(U8 *dest, U16 addr, int len);
    static U8 readByte(U16 addr);
    static void writeByte(U16 addr, U8 val);
};

#endif //HHUGBOY_LINKER_H
