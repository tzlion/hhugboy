/*
   hhugboy Game Boy emulator
   copyright 2013-2016 taizou
   Based on GEST
   Copyright (C) 2003-2010 TM
   Incorporating code from VisualBoyAdvance
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

#ifndef HHUGBOY_MBCNIN7_H
#define HHUGBOY_MBCNIN7_H


#include "BasicMbc.h"
//-------------------------------------------------------------------------
// for MBC7
//-------------------------------------------------------------------------
class MbcNin7 : public BasicMbc {
public:
    virtual void readMbcSpecificVarsFromStateFile(FILE *statefile) override;
    virtual void writeMbcSpecificVarsToStateFile(FILE *statefile) override;
    virtual void writeMemory(unsigned short address, register byte data) override;
    virtual byte readMemory(register unsigned short address) override;
    MbcNin7();
private:
    int MBC7_cs; // chip select
    int MBC7_sk; // ?
    int MBC7_state; // mapper state
    int MBC7_buffer; // buffer for receiving serial data
    int MBC7_idle; // idle state
    int MBC7_count; // count of bits received
    int MBC7_code; // command received
    int MBC7_address; // address received
    int MBC7_writeEnable; // write enable
    int MBC7_value; // value to return on ram
};


#endif //HHUGBOY_MBCNIN7_H
