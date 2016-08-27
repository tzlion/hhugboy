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

#ifndef HHUGBOY_MBCLICHUC3_H
#define HHUGBOY_MBCLICHUC3_H


#include "BasicMbc.h"
//-------------------------------------------------------------------------
// for HuC-3
//-------------------------------------------------------------------------
class MbcLicHuc3 : public BasicMbc {

public:
    virtual void readMbcSpecificVarsFromStateFile(FILE *statefile) override;
    virtual void writeMbcSpecificVarsToStateFile(FILE *statefile) override;
    virtual void writeMemory(unsigned short address, register byte data) override;
    virtual byte readMemory(register unsigned short address);
    MbcLicHuc3();

    virtual void resetVars(bool preserveMulticartState) override;

    virtual void readMbcSpecificVarsFromSaveFile(FILE *savefile) override;

    virtual void writeMbcSpecificVarsToSaveFile(FILE *savefile) override;

private:
    void updateHuc3Time();

    int HuC3_register[8];
    int HuC3_RAMvalue;
    int HuC3_RAMaddress;
    int HuC3_address;
    int HuC3_RAMflag;

    int HuC3_flag;
    int HuC3_shift;

    time_t HuC3_last_time;
    unsigned int HuC3_time;

};


#endif //HHUGBOY_MBCLICHUC3_H
