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

#define UNICODE
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdio.h>

#define DIRECTINPUT_VERSION 0x0700
#include <dinput.h>

#include <iostream>
#include <fstream>

using namespace std;
#include "rendering/directdraw.h"

#include "rom.h"
#include "debug.h"
#include "types.h"
#include "config.h"

#include "main.h"


#include "mainloop.h"

#include "sound.h"

#include "strings.h"

// Directories ------------------------------------------
/*char program_directory[ROM_PATH_SIZE];
char save_directory[SAVE_PATH_SIZE];
char state_directory[STATE_PATH_SIZE];
char rom_directory[ROM_PATH_SIZE];*/

//extern int joy_config[6];
//extern char player_name[100];

#include "menu.h"
menu emuMenu;

program_configuration::program_configuration():
        halt_on_unknown_opcode(true),
        reduce_cpu_usage(false),
        speedup_skip_9frames(false),
        speedup_sound_off(false),
        speedup_filter_off(true),
        GBC_SGB_border(OFF),
        video_size(2),
        video_mix_frames(MIX_FRAMES_ON),
        video_GB_color(BLACK_WHITE),
        video_GBCBGA_real_colors(true),
        video_auto_frameskip(false),
        video_frameskip(0),
        video_sprite_limit(true),
        video_LCDoff_clear_screen(true),
        video_filter(VIDEO_FILTER_SOFTXX),
        video_SGBborder_filter(VIDEO_FILTER_SOFTXX),
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
         emuMenu.checkOption(IDM_UNLAUTO);       
      break;
      case UNL_NONE:
         emuMenu.checkOption(IDM_UNLNONE);      
      break;
      case UNL_NIUTOUDE:
         emuMenu.checkOption(IDM_UNLNIUTOUDE);       
      break;
      case UNL_SINTAX:
      	emuMenu.checkOption(IDM_UNLSINTAX);
      break;
      case UNL_BBD:
      	emuMenu.checkOption(IDM_UNLBBD);
      break;
      case UNL_HITEK:
      	emuMenu.checkOption(IDM_UNLHITEK);
      break;
      case UNL_LBMULTI:
      	emuMenu.checkOption(IDM_UNLLBMULTI);
      break;
      default:
         emuMenu.checkOption(IDM_UNLAUTO);    
      break;
   } 

    for(int x=0;x<10;x++) {
        if (options->recent_rom_names[x] != L"") 
            emuMenu.setText(IDM_RECENTROM0+x,(wchar_t*)options->recent_rom_names[x].c_str());
    }
	
	
   if(options->GBC_SGB_border == GBC_WITH_SGB_BORDER)
   {
      emuMenu.checkOption(IDM_CPUGBCSGB);
   } else if(options->GBC_SGB_border == GBC_WITH_INITIAL_SGB_BORDER)
   {
      emuMenu.checkOption(IDM_CPUGBCSGBI);
   }
   
   if(options->video_auto_frameskip)
      emuMenu.checkOption(IDM_OPTIONVIDEOFSAUTO);  
      
   if(options->video_GBCBGA_real_colors)
      emuMenu.checkOption(IDM_OPTIONVIDEOGBCCOLORS);  
      
   switch(options->sound_volume)
   {
      case VOLUME_2X:
         emuMenu.checkOption(IDM_OPTIONSOUNDVOL2);       
      break;
      case VOLUME_3X:
         emuMenu.checkOption(IDM_OPTIONSOUNDVOL3);       
      break;
      case VOLUME_4X:
         emuMenu.checkOption(IDM_OPTIONSOUNDVOL4);       
      break;
      case VOLUME_1X:
      default:
         emuMenu.checkOption(IDM_OPTIONSOUNDVOL1);      
      break;
   }

   if(options->sound_reverse_stereo)
      emuMenu.checkOption(IDM_OPTIONSOUNDRSTEREO);   
   
   if(options->sound_lowpass_filter)
      emuMenu.uncheckOption(IDM_OPTIONSOUNDLOWPASSNONE);   
   if(options->sound_lowpass_filter == LOWPASS_LEVEL1)
      emuMenu.checkOption(IDM_OPTIONSOUNDLOWPASS1);
   else
   if(options->sound_lowpass_filter == LOWPASS_LEVEL2)
      emuMenu.checkOption(IDM_OPTIONSOUNDLOWPASS2);

   switch(options->video_SGBborder_filter)
   {
      case VIDEO_FILTER_SOFT2X:
         renderer.setBorderFilter(VIDEO_FILTER_SOFT2X);
      break;   
      case VIDEO_FILTER_SOFTXX:
         emuMenu.checkOption(IDM_VIDEOFILTERBORDERSOFTXX);  
         renderer.setBorderFilter(VIDEO_FILTER_SOFTXX);
      break;  
      case VIDEO_FILTER_SCALE2X:
         emuMenu.checkOption(IDM_VIDEOFILTERBORDERSCALE2X);  
         renderer.setBorderFilter(VIDEO_FILTER_SCALE2X);
      break;    
      case VIDEO_FILTER_SCALE3X:
         emuMenu.checkOption(IDM_VIDEOFILTERBORDERSCALE3X);  
         renderer.setBorderFilter(VIDEO_FILTER_SCALE3X);
      break;          
 /*     case VIDEO_FILTER_BLUR:
         emuMenu.checkOption(IDM_VIDEOFILTERBORDERBLUR);  
         renderer.setBorderFilter(VIDEO_FILTER_BLUR);
      break;     */
      case VIDEO_FILTER_NONE:
      default:
         options->video_SGBborder_filter = VIDEO_FILTER_NONE;
         emuMenu.checkOption(IDM_VIDEOFILTERBORDERNONE);
         renderer.setBorderFilter(VIDEO_FILTER_NONE);
      break; 
   }
 
   switch(options->video_filter)
   {
      case VIDEO_FILTER_SOFT2X:
         emuMenu.checkOption(IDM_VIDEOFILTERSOFT2X);  
         renderer.setGameboyFilter(VIDEO_FILTER_SOFT2X);
      break;  
      case VIDEO_FILTER_SOFTXX:
         emuMenu.checkOption(IDM_VIDEOFILTERSOFTXX);  
         renderer.setGameboyFilter(VIDEO_FILTER_SOFTXX);
      break;  
      case VIDEO_FILTER_SCALE2X:
         emuMenu.checkOption(IDM_VIDEOFILTERSCALE2X);  
         renderer.setGameboyFilter(VIDEO_FILTER_SCALE2X);
      break;    
      case VIDEO_FILTER_SCALE3X:
         emuMenu.checkOption(IDM_VIDEOFILTERSCALE3X);  
         renderer.setGameboyFilter(VIDEO_FILTER_SCALE3X);
      break;          
   /*   case VIDEO_FILTER_BLUR:
         emuMenu.checkOption(IDM_VIDEOFILTERBLUR);  
         renderer.setGameboyFilter(VIDEO_FILTER_BLUR);
      break;      */
      case VIDEO_FILTER_NONE:
      default:
         options->video_filter = VIDEO_FILTER_NONE;
         emuMenu.checkOption(IDM_VIDEOFILTERNONE);
         renderer.setGameboyFilter(VIDEO_FILTER_NONE);
      break;  
   }

   if(!options->video_visual_rumble)
      emuMenu.uncheckOption(IDM_OPTIONVIDEOVISUALRUMBLE);

   if(options->video_filter_use_mmx)
      emuMenu.checkOption(IDM_VIDEOFILTERUSEMMX);
   
   if(!options->halt_on_unknown_opcode)
      emuMenu.uncheckOption(IDM_CPUOPCODE);
   
   if(!options->reduce_cpu_usage)
      emuMenu.uncheckOption(IDM_OPTIONCPUUSAGE);  
  
   if(options->opposite_directions_allowed)
      emuMenu.checkOption(IDM_OPTIONOPPOSITEDIRECTIONS);
   
   switch(options->use_joystick_input)
   {
     case 0: emuMenu.checkOption(IDM_OPTIONCONTROLJOY1); break;
     case 1: emuMenu.checkOption(IDM_OPTIONCONTROLJOY2); break;
     case 2: emuMenu.checkOption(IDM_OPTIONCONTROLJOY3); break;
     case 3: emuMenu.checkOption(IDM_OPTIONCONTROLJOY4); break;
   }
   
   emuMenu.checkOption(IDM_OPTIONVIDEOFS0 + options->video_frameskip);
   
   if(!options->video_sprite_limit)
      emuMenu.uncheckOption(IDM_OPTIONVIDEOSPRLIM);   
      
   if(!options->video_LCDoff_clear_screen)
      emuMenu.uncheckOption(IDM_OPTIONVIDEOLCDOFF);
      
   if(options->speedup_skip_9frames)
      emuMenu.checkOption(IDM_CPUSPFS9);

   if(options->speedup_sound_off)
      emuMenu.checkOption(IDM_CPUSPSNDOFF);
      
   if(options->speedup_filter_off)
      emuMenu.checkOption(IDM_CPUSPFOFF);
   
   if(!options->sound_on)
      emuMenu.uncheckOption(IDM_OPTIONSOUND);   
    
   switch(options->video_GB_color)
   {
      case LCD_BROWN:
         emuMenu.checkOption(IDM_OPTIONVIDEOLCDBROWN);  
      break; 
      case LCD_GREEN:
         emuMenu.checkOption(IDM_OPTIONVIDEOLCDGREEN);  
      break;
      case GRAY:
         emuMenu.checkOption(IDM_OPTIONVIDEOGRAY);  
      break;            
      case BLACK_WHITE:
      default:
         emuMenu.checkOption(IDM_OPTIONVIDEOBW);
      break;	 
   }
   if(options->video_mix_frames == MIX_FRAMES_ON)
   {
      emuMenu.checkOption(IDM_OPTIONVIDEOMIXON);

	  renderer.setDrawMode(true);
         	              
   } else if(options->video_mix_frames == MIX_FRAMES_MORE)
   {
      emuMenu.checkOption(IDM_OPTIONVIDEOMIXMORE);
	  renderer.setDrawMode(true);
   } else
   {
      emuMenu.checkOption(IDM_OPTIONVIDEOMIXOFF);

	  renderer.setDrawMode(false);           	                           
   }

   if(options->video_size <= 0 || options->video_size > 4)
      options->video_size = 2;
      
    setWinSize(160,144);
   
}

void read_comment_line(ifstream& in)
{
    string commentline;
    char c;

    in.get(c);

    getline(in, commentline); // empty line

    getline(in, commentline); // comment line
}

void getlinew(ifstream& in,wstring& dest){
    string tmpstr;
    getline(in,tmpstr);
    dest = (wchar_t*)tmpstr.c_str();
    dest.resize(tmpstr.size()/2); // sometimes dest would end up 2 chars bigger than it should be and idfk why
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

    getlinew(in,config.rom_directory);

    getline(in, commentline); // empty line

    getline(in, commentline); // comment line

    getlinew(in,config.save_directory);

    getline(in, commentline); // empty line

    getline(in, commentline); // comment line

    getlinew(in,config.state_directory);

    getline(in, commentline); // empty line

    getline(in, commentline); // comment line
    
    for(int x=0;x<10;x++) {
        getlinew(in, config.recent_rom_names[x] );
    }
    
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
   
   read_comment_line(in);

    in >> config.speedup_filter_off;

   return in;
}

ostream& operator<<(ostream& out, const program_configuration& config)
{
    out << "#ROM Directory:\n";
    //out << config.rom_directory << "\n\n";
    out.write ((char*)config.rom_directory.c_str(), config.rom_directory.size()*2);
    out << "\n\n";

    out << "#Save Directory:\n";
    //out << config.save_directory << "\n\n";
    out.write ((char*)config.save_directory.c_str(), config.save_directory.size()*2);
    out << "\n\n";

    out << "#Save State Directory:\n";
    //out << config.state_directory << "\n\n";
    out.write ((char*)config.state_directory.c_str(), config.state_directory.size()*2);
    out << "\n\n";

    out << "#Recent ROMs:\n";
    for(int x=0;x<10;x++) {
        out.write ((char*)config.recent_rom_names[x].c_str(), config.recent_rom_names[x].size()*2);
        out << "\n";
    }
    out << "\n";

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
    
    out << "#On speedup filter off:\n";
    out << config.speedup_filter_off << "\n\n";

    return out;
}

bool read_config_file()
{
   SetCurrentDirectory(options->program_directory.c_str());

   options->save_directory = options->program_directory + L"\\save";

   options->state_directory = options->program_directory + L"\\state";

   options->rom_directory = options->program_directory;

   ifstream configfile("hhugboy.cfg", ifstream::in);
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

    ofstream configfile("hhugboy.cfg", std::ios_base::binary); 

    if(configfile.fail())
    {
      debug_print(str_table[ERROR_CFG_FILE]);

      return false;
    }

    configfile << *options;

    configfile.close();

    return true;
}

