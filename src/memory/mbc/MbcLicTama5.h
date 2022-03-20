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

#ifndef HHUGBOY_MBCLICTAMA5_H
#define HHUGBOY_MBCLICTAMA5_H


#include "BasicMbc.h"
//-------------------------------------------------------------------------
// for Bandai TAMA5 (Tamagotchi3)
//-------------------------------------------------------------------------

class MbcLicTama5 : public BasicMbc {

public:
    virtual void readMbcSpecificVarsFromStateFile(FILE *statefile) override;
    virtual void writeMemory(unsigned short address, register byte data) override;
    virtual void writeMbcSpecificVarsToStateFile(FILE *statefile) override;
    virtual byte readMemory(register unsigned short address);
    MbcLicTama5();

    virtual void resetVars(bool preserveMulticartState) override;

    virtual void readMbcSpecificVarsFromSaveFile(FILE *savefile) override;

    virtual void writeMbcSpecificVarsToSaveFile(FILE *savefile) override;

private:
    void updateTamaRtc();
    int tama_flag;
    byte tama_time;
    int tama_val4;
    int tama_val5;
    int tama_val6;
    int tama_val7;
    int tama_count;
    int tama_change_clock;
    int tama_month;

};


#endif //HHUGBOY_MBCLICTAMA5_H
