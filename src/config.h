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
#ifndef CONFIG_H
#define CONFIG_H

#include "types.h"

const int PROGRAM_PATH_SIZE = 1400;

bool read_config_file();

bool write_config_file();

void init_menu_options();

#include "options.h"

#include <string>
#include <fstream>

using namespace std;

void getlinew(ifstream&, wstring&);
void putlinew(ostream&, const wstring&);

class program_configuration
{
public:
    program_configuration();
    ~program_configuration();

    std::wstring program_directory;
    std::wstring save_directory;
    std::wstring state_directory;
    std::wstring rom_directory;
    
    std::wstring recent_rom_names[10];

    bool halt_on_unknown_opcode;
    bool reduce_cpu_usage;

    bool speedup_skip_9frames;
    bool speedup_sound_off;
    bool speedup_filter_off;

    SGBborderoption GBC_SGB_border;

    int video_size;
    mixframestype video_mix_frames;
    GBpaletteoption video_GB_color;
    bool video_GBCBGA_real_colors;
    bool video_auto_frameskip;
    int video_frameskip;
    bool video_sprite_limit;
    bool video_LCDoff_clear_screen;
    videofiltertype video_filter;
    videofiltertype video_SGBborder_filter;
    bool video_filter_use_mmx;
    bool video_visual_rumble;

    int sound_on;
    bool sound_reverse_stereo;
    int sound_lowpass_filter;
    soundvolumetype sound_volume;

    bool opposite_directions_allowed;
    int use_joystick_input;

    int multi_key_config[4][12];
    int special_keys[7];
    int joystick_config[6];

    int autofire_speed;
    
        
    unlCompatMode unl_compat_mode;

};

extern program_configuration* options;


#endif
