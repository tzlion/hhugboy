/*
   unGEST Game Boy emulator
   copyright 2013 taizou

   Based on GEST
   Copyright (C) 2003-2010 TM
   This file incorporates code from VisualBoyAdvance
   Copyright (C) 1999-2004 by Forgotten

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

#ifndef MAINLOOP_H
#define MAINLOOP_H

#define B_UP 0
#define B_DOWN 1
#define B_LEFT 2
#define B_RIGHT 3
#define B_A 4
#define B_B 5
#define B_SELECT 6
#define B_START 7

extern int speedup;

/*#define KEY_A 0
#define KEY_B 1
#define KEY_START 2
#define KEY_SELECT 3
#define KEY_L 4
#define KEY_R 5
#define KEY_TURBO_A 6
#define KEY_TURBO_B 7
#define KEY_SPEEDUP 8
#define KEY_UP 9
#define KEY_DOWN 10
#define KEY_LEFT 11
#define KEY_RIGHT 12
#define KEY_SENSOR_L 13
#define KEY_SENSOR_R 14
#define KEY_SENSOR_U 15
#define KEY_SENSOR_D 16*/

#define SENSOR_LEFT 0
#define SENSOR_RIGHT 1
#define SENSOR_UP 2
#define SENSOR_DOWN 3

extern int sensor_dir[4];
extern int sensorX;
extern int sensorY;

//extern int frames;

void check_sensor();

#endif
