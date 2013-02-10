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

#include <windows.h>
#include <stdio.h>

#define DIRECTINPUT_VERSION 0x0700
#include <dinput.h>

#include <iostream>
#include <fstream>

using namespace std;

#include "rom.h"
#include "debug.h"
#include "types.h"
#include "config.h"
#include "main.h"
#include "mainloop.h"
#include "directdraw.h"
#include "sound.h"

#include "strings.h"
// Directories ------------------------------------------
/*char program_directory[ROM_PATH_SIZE];
char save_directory[SAVE_PATH_SIZE];
char state_directory[STATE_PATH_SIZE];
char rom_directory[ROM_PATH_SIZE];*/

//extern int joy_config[6];
//extern char player_name[100];

program_configuration::program_configuration():
        halt_on_unknown_opcode(true),
        reduce_cpu_usage(false),
        speedup_skip_9frames(false),
        speedup_sound_off(false),
        GBC_SGB_border(OFF),
        video_size(2),
        video_mix_frames(MIX_FRAMES_ON),
        video_GB_color(BLACK_WHITE),
        video_GBCBGA_real_colors(true),
        video_auto_frameskip(false),
        video_frameskip(0),
        video_sprite_limit(true),
        video_LCDoff_clear_screen(true),
        video_filter(VIDEO_FILTER_NONE),
        video_SGBborder_filter(VIDEO_FILTER_NONE),
        video_filter_use_mmx(true),
        video_visual_rumble(true),
        sound_on(1),
        sound_reverse_stereo(false),
        sound_lowpass_filter(0),
        sound_volume(VOLUME_1X),
        opposite_directions_allowed(false),
        use_joystick_input(-1),
        autofire_speed(0),
        unl_compat_mode(UNL_AUTO)
{
    multi_key_config[PLAYER1][BUTTON_A] = DIK_Z;
    multi_key_config[PLAYER1][BUTTON_B] = DIK_X;
    multi_key_config[PLAYER1][BUTTON_START] = DIK_RETURN;
    multi_key_config[PLAYER1][BUTTON_SELECT] = DIK_BACK;
    multi_key_config[PLAYER1][BUTTON_TURBO_A] = DIK_A;
    multi_key_config[PLAYER1][BUTTON_TURBO_B] = DIK_S;
    multi_key_config[PLAYER1][BUTTON_TURBO_START] = 0;
    multi_key_config[PLAYER1][BUTTON_TURBO_SELECT] = 0;
    multi_key_config[PLAYER1][BUTTON_UP] = DIK_UP;
    multi_key_config[PLAYER1][BUTTON_DOWN] = DIK_DOWN;
    multi_key_config[PLAYER1][BUTTON_LEFT] = DIK_LEFT;
    multi_key_config[PLAYER1][BUTTON_RIGHT] = DIK_RIGHT;

    special_keys[BUTTON_L] = DIK_Q;
    special_keys[BUTTON_R] = DIK_W;
    special_keys[BUTTON_SPEEDUP] = DIK_SPACE;
    special_keys[BUTTON_SENSOR_LEFT] = DIK_NUMPAD4;
    special_keys[BUTTON_SENSOR_RIGHT] = DIK_NUMPAD6;
    special_keys[BUTTON_SENSOR_UP] = DIK_NUMPAD8;
    special_keys[BUTTON_SENSOR_DOWN] = DIK_NUMPAD2;

    for(int i = 0; i < 12; i++)
    {
        multi_key_config[PLAYER2][i] = 0;
        multi_key_config[PLAYER3][i] = 0;
        multi_key_config[PLAYER4][i] = 0;
    }

    joystick_config[BUTTON_A] = 0;
    joystick_config[BUTTON_B] = 1;
    joystick_config[BUTTON_START] = 2;
    joystick_config[BUTTON_SELECT] = 3;
    joystick_config[BUTTON_TURBO_A] = 4;
    joystick_config[BUTTON_TURBO_B] = 5;
}

program_configuration::~program_configuration()
{
}

void init_menu_options()
{
    switch(options->unl_compat_mode)
   {
      case UNL_AUTO:
         CheckMenuItem(menu,IDM_UNLAUTO,MF_CHECKED);       
      break;
      case UNL_NONE:
         CheckMenuItem(menu,IDM_UNLNONE,MF_CHECKED);      
      break;
      case UNL_NIUTOUDE:
         CheckMenuItem(menu,IDM_UNLNIUTOUDE,MF_CHECKED);       
      break;
      case UNL_SINTAX:
      	CheckMenuItem(menu,IDM_UNLSINTAX,MF_CHECKED);
      break;
      default:
         CheckMenuItem(menu,IDM_UNLAUTO,MF_CHECKED);    
      break;
   } 

	
	
   if(options->GBC_SGB_border == GBC_WITH_SGB_BORDER)
   {
      CheckMenuItem(menu,IDM_CPUGBCSGB,MF_CHECKED);
   } else if(options->GBC_SGB_border == GBC_WITH_INITIAL_SGB_BORDER)
   {
      CheckMenuItem(menu,IDM_CPUGBCSGBI,MF_CHECKED);
   }
   
   if(options->video_auto_frameskip)
      CheckMenuItem(menu,IDM_OPTIONVIDEOFSAUTO,MF_CHECKED);  
      
   if(options->video_GBCBGA_real_colors)
      CheckMenuItem(menu,IDM_OPTIONVIDEOGBCCOLORS,MF_CHECKED);  
      
   switch(options->sound_volume)
   {
      case VOLUME_2X:
         CheckMenuItem(menu,IDM_OPTIONSOUNDVOL2,MF_CHECKED);       
      break;
      case VOLUME_3X:
         CheckMenuItem(menu,IDM_OPTIONSOUNDVOL3,MF_CHECKED);       
      break;
      case VOLUME_4X:
         CheckMenuItem(menu,IDM_OPTIONSOUNDVOL4,MF_CHECKED);       
      break;
      case VOLUME_1X:
      default:
         CheckMenuItem(menu,IDM_OPTIONSOUNDVOL1,MF_CHECKED);      
      break;
   }

   if(options->sound_reverse_stereo)
      CheckMenuItem(menu,IDM_OPTIONSOUNDRSTEREO,MF_CHECKED);   
   
   if(options->sound_lowpass_filter)
      CheckMenuItem(menu,IDM_OPTIONSOUNDLOWPASSNONE,MF_UNCHECKED);   
   if(options->sound_lowpass_filter == LOWPASS_LEVEL1)
      CheckMenuItem(menu,IDM_OPTIONSOUNDLOWPASS1,MF_CHECKED);
   else
   if(options->sound_lowpass_filter == LOWPASS_LEVEL2)
      CheckMenuItem(menu,IDM_OPTIONSOUNDLOWPASS2,MF_CHECKED);

   switch(options->video_SGBborder_filter)
   {
      case VIDEO_FILTER_SOFT2X:
         CheckMenuItem(menu,IDM_VIDEOFILTERBORDERSOFT2X,MF_CHECKED);  
         
         border_filter_width = 2;
         border_filter_height = 2;
      break;   
      case VIDEO_FILTER_SCALE2X:
         CheckMenuItem(menu,IDM_VIDEOFILTERBORDERSCALE2X,MF_CHECKED);  
         
         border_filter_width = 2;
         border_filter_height = 2;
      break;    
      case VIDEO_FILTER_SCALE3X:
         CheckMenuItem(menu,IDM_VIDEOFILTERBORDERSCALE3X,MF_CHECKED);  
         
         border_filter_width = 3;
         border_filter_height = 3;
      break;          
 /*     case VIDEO_FILTER_BLUR:
         CheckMenuItem(menu,IDM_VIDEOFILTERBORDERBLUR,MF_CHECKED);  
         
         border_filter_width = 2;
         border_filter_height = 2;
      break;     */
      case VIDEO_FILTER_NONE:
      default:
         options->video_SGBborder_filter = VIDEO_FILTER_NONE;
         
         CheckMenuItem(menu,IDM_VIDEOFILTERBORDERNONE,MF_CHECKED);
         border_filter_width = 1;
         border_filter_height = 1;         
      break;
   }
 
   switch(options->video_filter)
   {
      case VIDEO_FILTER_SOFT2X:
         CheckMenuItem(menu,IDM_VIDEOFILTERSOFT2X,MF_CHECKED);  
         
         filter_width = 2;
         filter_height = 2;
                   
         change_filter(); 
      break;   
      case VIDEO_FILTER_SCALE2X:
         CheckMenuItem(menu,IDM_VIDEOFILTERSCALE2X,MF_CHECKED);  
         
         filter_width = 2;
         filter_height = 2;
                   
         change_filter(); 
      break;    
      case VIDEO_FILTER_SCALE3X:
         CheckMenuItem(menu,IDM_VIDEOFILTERSCALE3X,MF_CHECKED);  
         
         filter_width = 3;
         filter_height = 3;
                   
         change_filter(); 
      break;          
   /*   case VIDEO_FILTER_BLUR:
         CheckMenuItem(menu,IDM_VIDEOFILTERBLUR,MF_CHECKED);  
         
         filter_width = 2;
         filter_height = 2;
                   
         change_filter(); 
      break;      */
      case VIDEO_FILTER_NONE:
      default:
         options->video_filter = VIDEO_FILTER_NONE;
         
         CheckMenuItem(menu,IDM_VIDEOFILTERNONE,MF_CHECKED);

         filter_width = 1;
         filter_height = 1;        
         
         change_filter(); 
      break;
   }

   if(!options->video_visual_rumble)
      CheckMenuItem(menu,IDM_OPTIONVIDEOVISUALRUMBLE,MF_UNCHECKED);

   if(options->video_filter_use_mmx)
      CheckMenuItem(menu,IDM_VIDEOFILTERUSEMMX,MF_CHECKED);
   
   if(!options->halt_on_unknown_opcode)
      CheckMenuItem(menu,IDM_CPUOPCODE,MF_UNCHECKED);
   
   if(!options->reduce_cpu_usage)
      CheckMenuItem(menu,IDM_OPTIONCPUUSAGE,MF_UNCHECKED);  
  
   if(options->opposite_directions_allowed)
      CheckMenuItem(menu,IDM_OPTIONOPPOSITEDIRECTIONS,MF_CHECKED);
   
   switch(options->use_joystick_input)
   {
     case 0: CheckMenuItem(menu,IDM_OPTIONCONTROLJOY1,MF_CHECKED); break;
     case 1: CheckMenuItem(menu,IDM_OPTIONCONTROLJOY2,MF_CHECKED); break;
     case 2: CheckMenuItem(menu,IDM_OPTIONCONTROLJOY3,MF_CHECKED); break;
     case 3: CheckMenuItem(menu,IDM_OPTIONCONTROLJOY4,MF_CHECKED); break;
   }
   
   CheckMenuItem(menu,IDM_OPTIONVIDEOFS0,MF_UNCHECKED); 
   CheckMenuItem(menu,IDM_OPTIONVIDEOFS0 + options->video_frameskip,MF_CHECKED);
   
   if(!options->video_sprite_limit)
      CheckMenuItem(menu,IDM_OPTIONVIDEOSPRLIM,MF_UNCHECKED);   
      
   if(!options->video_LCDoff_clear_screen)
      CheckMenuItem(menu,IDM_OPTIONVIDEOLCDOFF,MF_UNCHECKED);
      
   if(options->speedup_skip_9frames)
      CheckMenuItem(menu,IDM_CPUSPFS9,MF_CHECKED);

   if(options->speedup_sound_off)
      CheckMenuItem(menu,IDM_CPUSPSNDOFF,MF_CHECKED);
   
   if(!options->sound_on)
      CheckMenuItem(menu,IDM_OPTIONSOUND,MF_UNCHECKED);   
    
   switch(options->video_GB_color)
   {
      case LCD_BROWN:
         CheckMenuItem(menu,IDM_OPTIONVIDEOLCDBROWN,MF_CHECKED);  
      break; 
      case LCD_GREEN:
         CheckMenuItem(menu,IDM_OPTIONVIDEOLCDGREEN,MF_CHECKED);  
      break;
      case GRAY:
         CheckMenuItem(menu,IDM_OPTIONVIDEOGRAY,MF_CHECKED);  
      break;            
      case BLACK_WHITE:
      default:
         CheckMenuItem(menu,IDM_OPTIONVIDEOBW,MF_CHECKED);
      break;	 
   }
   if(options->video_mix_frames == MIX_FRAMES_ON)
   {
      CheckMenuItem(menu,IDM_OPTIONVIDEOMIXON,MF_CHECKED);

      if(dx_bitcount == 16)
         draw_screen = draw_screen_mix16;
      else
         draw_screen = draw_screen_mix32;             	              
   } else if(options->video_mix_frames == MIX_FRAMES_MORE)
   {
      CheckMenuItem(menu,IDM_OPTIONVIDEOMIXMORE,MF_CHECKED);
      CheckMenuItem(menu,IDM_OPTIONVIDEOMIXON,MF_UNCHECKED);

      if(dx_bitcount == 16)
         draw_screen = draw_screen_mix16;
      else
         draw_screen = draw_screen_mix32; 
   } else
   {
      CheckMenuItem(menu,IDM_OPTIONVIDEOMIXOFF,MF_CHECKED);
      CheckMenuItem(menu,IDM_OPTIONVIDEOMIXON,MF_UNCHECKED);

      if(dx_bitcount == 16)
         draw_screen = draw_screen16;
      else
         draw_screen = draw_screen32;             	                           
   }
   
   RECT winRect;
   GetWindowRect(hwnd,&winRect);
   if(options->video_size <= 0 || options->video_size > 4)
      options->video_size = 2;
   MoveWindow(hwnd,winRect.left,winRect.top,options->video_size * 160 + sizen_w,options->video_size * 144 + sizen_h,TRUE);
}

void read_comment_line(ifstream& in)
{
    string commentline;
    char c;

    in.get(c);

    getline(in, commentline); // empty line

    getline(in, commentline); // comment line
}


ifstream& operator>>(ifstream& in, program_configuration& config)
{
    string commentline;

    getline(in, commentline);

    if(commentline == "#Rom directory:")
    {
        cout << "Old config file format, using defaults.";
        return in;
    }

    getline(in,config.rom_directory);

    getline(in, commentline); // empty line

    getline(in, commentline); // comment line

    getline(in,config.save_directory);

    getline(in, commentline); // empty line

    getline(in, commentline); // comment line

    getline(in,config.state_directory);  

    getline(in, commentline); // empty line

    getline(in, commentline); // comment line

    in >> config.halt_on_unknown_opcode;

    read_comment_line(in);
    
    in >> config.reduce_cpu_usage;

    read_comment_line(in);

    in >> config.speedup_skip_9frames;

    read_comment_line(in);

    in >> config.speedup_sound_off;

    read_comment_line(in);

    int temp = 0;
    in >> temp;
    config.GBC_SGB_border = (SGBborderoption)temp;

    read_comment_line(in);

    in >> config.video_size;

    read_comment_line(in);

    in >> temp;
    config.video_mix_frames = (mixframestype)temp;

    read_comment_line(in);

    in >> temp;
    config.video_GB_color = (GBpaletteoption)temp;

    read_comment_line(in);

    in >> config.video_GBCBGA_real_colors;

    read_comment_line(in);

    in >> config.video_auto_frameskip;

    read_comment_line(in);

    in >> config.video_frameskip;

    read_comment_line(in);

    in >> config.video_sprite_limit;

    read_comment_line(in);

    in >> config.video_LCDoff_clear_screen;

    read_comment_line(in);

    in >> temp;
    config.video_filter = (videofiltertype)temp;

    read_comment_line(in);

    in >> temp;
    config.video_SGBborder_filter = (videofiltertype)temp;

    read_comment_line(in);

    in >> config.video_filter_use_mmx;

    read_comment_line(in);

    in >> config.video_visual_rumble;

    read_comment_line(in);

    in >> config.sound_on;

    read_comment_line(in);

    in >> config.sound_reverse_stereo;

    read_comment_line(in);

    in >> config.sound_lowpass_filter;

    read_comment_line(in);

    in >> temp;
    config.sound_volume = (soundvolumetype)temp;

    read_comment_line(in);

    in >> config.opposite_directions_allowed;

    read_comment_line(in);

    in >> config.use_joystick_input;

    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 12; j++)
        {
             read_comment_line(in);

             in >> config.multi_key_config[i][j];
        }
    }

   for(int i = 0; i < 7; i++)
   {
        read_comment_line(in);

        in >> config.special_keys[i];
   }

   for(int i = 0; i < 6; i++)
   {
        read_comment_line(in);

        in >> config.joystick_config[i];
   }

   return in;
}

ostream& operator<<(ostream& out, const program_configuration& config)
{
    out << "#Rom Directory:\n";
    out << config.rom_directory << "\n\n";

    out << "#Save Directory:\n";
    out << config.save_directory << "\n\n";

    out << "#Save State Directory:\n";
    out << config.state_directory << "\n\n";

    out << "#Halt on unknown opcode:\n";
    out << config.halt_on_unknown_opcode << "\n\n";

    out << "#Reduce cpu usage:\n";
    out << config.reduce_cpu_usage << "\n\n";

    out << "#On speedup skip 9 frames:\n";
    out << config.speedup_skip_9frames << "\n\n";

    out << "#On speedup sound off:\n";
    out << config.speedup_sound_off << "\n\n";

    out << "#GBC SGB border:\n";
    out << config.GBC_SGB_border << "\n\n";

    out << "#Video size:\n";
    out << config.video_size << "\n\n";

    out << "#Video mix frames:\n";
    out << config.video_mix_frames << "\n\n";

    out << "#Video GB color palette:\n";
    out << config.video_GB_color << "\n\n";

    out << "#Video GBC/GBA real colors:\n";
    out << config.video_GBCBGA_real_colors << "\n\n";

    out << "#Video auto frameskip:\n";
    out << config.video_auto_frameskip << "\n\n";

    out << "#Video frame skip:\n";
    out << config.video_frameskip << "\n\n";

    out << "#Video sprite limit:\n";
    out << config.video_sprite_limit << "\n\n";

    out << "#Video LCD off clear screen:\n";
    out << config.video_LCDoff_clear_screen << "\n\n";

    out << "#Video filter:\n";
    out << config.video_filter << "\n\n";

    out << "#Video SGB border filter:\n";
    out << config.video_SGBborder_filter << "\n\n";

    out << "#Video filter use MMX:\n";
    out << config.video_filter_use_mmx << "\n\n";

    out << "#Video visual rumble:\n";
    out << config.video_visual_rumble << "\n\n";

    out << "#Sound on:\n";
    out << config.sound_on << "\n\n";

    out << "#Sound reverse stereo:\n";
    out << config.sound_reverse_stereo << "\n\n";

    out << "#Sound lowpass filter:\n";
    out << config.sound_lowpass_filter << "\n\n";

    out << "#Sound volume:\n";
    out << config.sound_volume << "\n\n";

    out << "#Controls opposite directions allowed:\n";
    out << config.opposite_directions_allowed << "\n\n";

    out << "#Controls use joystick input for:\n";
    out << config.use_joystick_input << "\n\n";

    for(int i = 0; i < 4; i++)
    {
        out << "#Player " << i+1 << " A button:\n";
        out << config.multi_key_config[i][BUTTON_A] << "\n\n";

        out << "#Player " << i+1 << " B button:\n";
        out << config.multi_key_config[i][BUTTON_B] << "\n\n";

        out << "#Player " << i+1 << " START button:\n";
        out << config.multi_key_config[i][BUTTON_START] << "\n\n";

        out << "#Player " << i+1 << " SELECT button:\n";
        out << config.multi_key_config[i][BUTTON_SELECT] << "\n\n";

        out << "#Player " << i+1 << " Autofire A button:\n";
        out << config.multi_key_config[i][BUTTON_TURBO_A] << "\n\n";

        out << "#Player " << i+1 << " Autofire B button:\n";
        out << config.multi_key_config[i][BUTTON_TURBO_B] << "\n\n";

        out << "#Player " << i+1 << " Autofire START button:\n";
        out << config.multi_key_config[i][BUTTON_TURBO_START] << "\n\n";

        out << "#Player " << i+1 << " Autofire SELECT button:\n";
        out << config.multi_key_config[i][BUTTON_TURBO_SELECT] << "\n\n";

        out << "#Player " << i+1 << " UP button:\n";
        out << config.multi_key_config[i][BUTTON_UP] << "\n\n";

        out << "#Player " << i+1 << " DOWN button:\n";
        out << config.multi_key_config[i][BUTTON_DOWN] << "\n\n";

        out << "#Player " << i+1 << " LEFT button:\n";
        out << config.multi_key_config[i][BUTTON_LEFT] << "\n\n";

        out << "#Player " << i+1 << " RIGHT button:\n";
        out << config.multi_key_config[i][BUTTON_RIGHT] << "\n\n";
    }

    out << "#L button:\n";
    out << config.special_keys[BUTTON_L] << "\n\n";

    out << "#R button:\n";
    out << config.special_keys[BUTTON_R] << "\n\n";

    out << "#Speedup button:\n";
    out << config.special_keys[BUTTON_SPEEDUP] << "\n\n";

    out << "#Sensor LEFT button:\n";
    out << config.special_keys[BUTTON_SENSOR_LEFT] << "\n\n";

    out << "#Sensor RIGHT button:\n";
    out << config.special_keys[BUTTON_SENSOR_RIGHT] << "\n\n";

    out << "#Sensor UP button:\n";
    out << config.special_keys[BUTTON_SENSOR_UP] << "\n\n";
    
    out << "#Sensor DOWN button:\n";
    out << config.special_keys[BUTTON_SENSOR_DOWN] << "\n\n";

    out << "#Joystick A button:\n";
    out << config.joystick_config[BUTTON_A] << "\n\n";

    out << "#Joystick B button:\n";
    out << config.joystick_config[BUTTON_B] << "\n\n";

    out << "#Joystick START button:\n";
    out << config.joystick_config[BUTTON_START] << "\n\n";

    out << "#Joystick SELECT button:\n";
    out << config.joystick_config[BUTTON_SELECT] << "\n\n";

    out << "#Joystick Autofire A button:\n";
    out << config.joystick_config[BUTTON_TURBO_A] << "\n\n";

    out << "#Joystick Autofire B button:\n";
    out << config.joystick_config[BUTTON_TURBO_B] << "\n\n";

    return out;
}

bool read_config_file()
{
   SetCurrentDirectory(options->program_directory.c_str());

   options->save_directory = options->program_directory + "\\save";

   options->state_directory = options->program_directory + "\\state";

   options->rom_directory = options->program_directory;

   ifstream configfile("gest.cfg", ifstream::in);
   if(configfile.fail())
   {
       cout << "Config file load failed, using defaults.";

       init_menu_options();

       return true;
   }

   configfile >> *options;
   
   configfile.close();

   init_menu_options();

   return true;
}

bool write_config_file()
{
    SetCurrentDirectory(options->program_directory.c_str());

    ofstream configfile("gest.cfg");
    if(configfile.fail())
    {
      debug_print(str_table[ERROR_CFG_FILE]);

      return false;
    }

    configfile << *options;
    
    configfile.close();

    return true;
}

