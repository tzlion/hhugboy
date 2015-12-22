/*
   hhugboy Game Boy emulator
   copyright 2013 taizou

   Based on GEST
   Copyright (C) 2003-2010 TM

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
#ifndef SOUND_H
#define SOUND_H

#include "types.h"
#include "debug.h"
#include "fmod/fmod.h"

#define NR10 0xff10
#define NR11 0xff11
#define NR12 0xff12
#define NR13 0xff13
#define NR14 0xff14
#define NR21 0xff16
#define NR22 0xff17
#define NR23 0xff18
#define NR24 0xff19
#define NR30 0xff1a
#define NR31 0xff1b
#define NR32 0xff1c
#define NR33 0xff1d
#define NR34 0xff1e
#define NR41 0xff20
#define NR42 0xff21
#define NR43 0xff22
#define NR44 0xff23
#define NR50 0xff24
#define NR51 0xff25
#define NR52 0xff26

#define SND_EN_CH1 1
#define SND_EN_CH2 2
#define SND_EN_CH3 4
#define SND_EN_CH4 8

extern int sound_enable;

const int LOWPASS_LEVEL1 = 1;
const int LOWPASS_LEVEL2 = 2;

extern int CYCLES_SOUND;
//extern int cycles_sound;
extern FSOUND_SAMPLE* FSbuffer;
extern int sound_buffer_total_len;
extern int sound_quality;
extern int channel_n;

extern byte sound_wave_pattern[4][32];

#endif
