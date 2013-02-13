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

const int PROGRAM_PATH_SIZE = 1400;

bool read_config_file();

bool write_config_file();

enum SGBborderoption
{
    OFF = 0,
    GBC_WITH_SGB_BORDER = 1,
    GBC_WITH_INITIAL_SGB_BORDER = 2
};

enum GBpaletteoption
{
    BLACK_WHITE = 0,
    GRAY = 1,
    LCD_BROWN = 2,
    LCD_GREEN = 3
};

enum videofiltertype
{
    VIDEO_FILTER_NONE = 0,
    VIDEO_FILTER_SOFT2X = 1,
    VIDEO_FILTER_SCALE2X = 2,
    VIDEO_FILTER_BLUR = 3,
    VIDEO_FILTER_SCALE3X = 4,
    VIDEO_FILTER_SOFTXX = 5
};

enum soundvolumetype
{
    VOLUME_1X = 1,
    VOLUME_2X = 2,
    VOLUME_3X = 3,
    VOLUME_4X = 4
};

enum _controllernumber
{
    PLAYER1 = 0,
    PLAYER2 = 1,
    PLAYER3 = 2,
    PLAYER4 = 3
};

enum _keys
{
    BUTTON_A = 0,
    BUTTON_B,
    BUTTON_START,
    BUTTON_SELECT,
    BUTTON_TURBO_A,
    BUTTON_TURBO_B,
    BUTTON_TURBO_START,
    BUTTON_TURBO_SELECT,
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_LEFT,
    BUTTON_RIGHT
};

enum _specialkeys
{
    BUTTON_L = 0,
    BUTTON_R,
    BUTTON_SPEEDUP,
    BUTTON_SENSOR_LEFT,
    BUTTON_SENSOR_RIGHT,
    BUTTON_SENSOR_UP,
    BUTTON_SENSOR_DOWN
};

enum mixframestype
{
    MIX_FRAMES_OFF = 0,
    MIX_FRAMES_ON = 1,
    MIX_FRAMES_MORE = 2
};

enum unlCompatMode
{
	UNL_AUTO,
	UNL_NONE,
	UNL_NIUTOUDE,
	UNL_SINTAX
};

#include <string>
#include <fstream>

class program_configuration
{
public:
    program_configuration();
    ~program_configuration();

    std::string program_directory;
    std::string save_directory;
    std::string state_directory;
    std::string rom_directory;

    bool halt_on_unknown_opcode;
    bool reduce_cpu_usage;

    bool speedup_skip_9frames;
    bool speedup_sound_off;

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
