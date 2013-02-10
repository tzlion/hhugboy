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

#ifndef GBDEVICES_H
#define GBDEVICES_H

#include "types.h"

#define DEVICE_NONE    0
#define DEVICE_BARCODE 1 

extern int connected_device;
extern int external_clock;

extern unsigned short (*serial_function)(int);

void reset_devices();
unsigned short serial_device_none(int);
unsigned short serial_device_barcode(int);

#endif
