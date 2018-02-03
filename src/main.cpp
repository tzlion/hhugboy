/*
   hhugboy Game Boy emulator
   copyright 2013-16 taizou

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

#define WIN32_LEAN_AND_MEAN
#define UNICODE
#include <windows.h>
#include <shellapi.h>
#include <mmsystem.h>

#include <string>

using namespace std;

#include <math.h>

#define DIRECTINPUT_VERSION 0x0700

#include "ui/strings.h"
#include "cheats.h"
#include "config.h"

#include "main.h"
#include "cpu.h"
#include "mainloop.h"
#include "sound.h"
#include "SGB.h"
#include "rendering/render.h"

#include "directinput.h"

#include "ui/window.h"

gb_system* GB = NULL;
gb_system* GB1 = NULL;
gb_system* GB2 = NULL;

const wchar_t* prg_version = L"1.2.1+++dev";

// Window stuff? ----------------------------------------

wchar_t w_emu_title[] = L"hhugboy";

int timer_id = 0;

DirectDraw renderer(&hwnd);

Palette palette;

// Options ----------------------------------------------
bool paused = false;
bool menupause = false;

int control_pressed = 0; // control key pressed 

int current_controller = 0; // currently changing which controller? 

int speedup = 0;

program_configuration* options = NULL;

wstring gb1_loaded_file_name;

#ifdef ALLOW_DEBUG
bool debug = false;
#endif

int ramsize[10] = { 0, 2, 8, 32, 128, 64,64,64,8, 256 }; // KBytes

int mainLoop()
{
    MSG msg;

    LARGE_INTEGER cur_time;
    LARGE_INTEGER now_time;
    DWORD time_count = 16;
    LARGE_INTEGER perf_cnt;
    LARGE_INTEGER next_time;

    int perf_flag = 0;
    int time_to_sleep = 0;

    if(QueryPerformanceFrequency(&perf_cnt))
    {
        perf_flag = 1;
        time_count = (DWORD)(perf_cnt.QuadPart/59.7275);
        QueryPerformanceCounter(&next_time);
    } else
    {
        next_time.QuadPart = timeGetTime();
        time_count = 33;
    }

    for(;;)
    {
        if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
        {
            if(msg.message == WM_QUIT)
                break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else
        if(!emulating || !GB1->romloaded || paused)
        {
            Sleep(50);
        } else
        {
#ifdef ALLOW_DEBUG
            if(debug && !paused && GB1->romloaded)
         {
            GB->frames = 1;
            GB1->mainloop();
            GB->frames = 0;

            (renderer.*renderer.drawDebugScreen)();
            continue;
         }
#endif

            if(perf_flag)
                QueryPerformanceCounter(&cur_time);
            else
                cur_time.QuadPart = timeGetTime();

            //-----Reduce CPU usage----
            if(options->reduce_cpu_usage && !speedup && cur_time.QuadPart < next_time.QuadPart)
            {
                QueryPerformanceCounter(&cur_time);

                time_to_sleep = (int)floor(((next_time.QuadPart - cur_time.QuadPart)*1000) / perf_cnt.QuadPart); // Time to sleep (in milliseconds)
                if(--time_to_sleep < 0)
                    time_to_sleep = 0;

                Sleep(time_to_sleep);

                do
                {
                    //Sleep(0);

                    QueryPerformanceCounter(&cur_time);
                } while(cur_time.QuadPart < next_time.QuadPart);
            }
            //-----Reduce CPU usage----

            if(speedup || cur_time.QuadPart >= next_time.QuadPart)
            {
                // Runs until frame
                if(!multiple_gb)
                {
                    do
                    {
                        GB1->mainloop();
                    } while(GB1->frames < 1);
                } else
                {
                    do
                    {
                        GB = GB1;

                        GB1->mainloop();

                        GB = GB2;

                        GB2->mainloop();
                    } while(GB1->frames < 1);
                }

                GB1->frames = 0;

                next_time.QuadPart = cur_time.QuadPart + time_count;

                //-----Auto frameskip----
                if(options->video_auto_frameskip)
                {
                    QueryPerformanceCounter(&cur_time);

                    if(cur_time.QuadPart > next_time.QuadPart)
                    {
                        if(options->video_frameskip < 9)
                        {
                            ++options->video_frameskip;
                        }
                    }
                    else if(options->video_frameskip > 0)
                    {
                        --options->video_frameskip;
                    }

                    if(multiple_gb)
                        wsprintfW(w_title_text,L"%s - %s --- %s, frameskip: %i",w_emu_title,GB1->rom_filename,GB2->rom_filename,options->video_frameskip);
                    else
                        wsprintfW(w_title_text,L"%s - %s, frameskip: %i",w_emu_title,GB1->rom_filename,options->video_frameskip);
                    SetWindowText(hwnd,w_title_text);
                }
                //-----Auto frameskip----

                //-----Reduce CPU usage----
                //  if(options->reduce_cpu_usage && !speedup /*&& (!multiple_gb || multiple_gb && current_gb == 2)*/)
                /*   {
                      QueryPerformanceCounter(&now_time);

                      time_to_sleep = (int)floor(((next_time.QuadPart - now_time.QuadPart)*1000) / perf_cnt.QuadPart); // Time to sleep (in milliseconds)
                      time_to_sleep -= 1; //Time to run sleep function+alpha

                      if(time_to_sleep > 0)
                         Sleep(time_to_sleep);
                   }*/
                //-----Reduce CPU usage----
            }
        }
    }

    return msg.wParam;
}

void initFromArg()
{
    int noArgs;
    LPWSTR *argList = CommandLineToArgvW(GetCommandLineW(), &noArgs);

    if(noArgs >= 2)
    {
        gb1_loaded_file_name = argList[1];
        GB1->load_rom(argList[1]);

        if(GB1->romloaded)
        {
            GB1->reset();

            if(sgb_mode)
            {
                setWinSize(256,224);
            } else
            {
                setWinSize(160,144);
            }

            if(!GB1->load_save())
                debug_print(str_table[ERROR_SAVE_FILE_READ]);


            wsprintfW(w_title_text,L"%s - %s",w_emu_title,GB1->rom_filename);
            SetWindowText(hwnd,w_title_text);
        }
    }
}

void initSound()
{
    FSOUND_SetHWND(hwnd);
    if(FSOUND_Init(44100,2,0) == FALSE)
    {
        debug_print(str_table[ERROR_FMOD]);
        options->sound_on = -1;
    } else
        FSbuffer = FSOUND_Sample_Alloc(FSOUND_FREE,sound_buffer_total_len>>2,FSOUND_LOOP_NORMAL|FSOUND_16BITS|FSOUND_STEREO|FSOUND_SIGNED,44100,200,FSOUND_STEREOPAN,1);

    if(options->sound_on)
        channel_n = FSOUND_PlaySound(FSOUND_FREE,FSbuffer);
}

void initConfigs()
{
    //Get program directory
    wchar_t buffer[PROGRAM_PATH_SIZE];
    GetModuleFileName(NULL,buffer,PROGRAM_PATH_SIZE);

    options->program_directory = buffer;
    size_t found = options->program_directory.rfind('\\'); // Cut program name
    if(found != string::npos)
    {
        options->program_directory = options->program_directory.substr(0,found);
    }

    if(!read_config_file())
        debug_print(str_table[ERROR_CFG_FILE_READ]);
}

void initGraphics()
{
    if(!renderer.init(&palette))
    {
        debug_print(str_table[ERROR_DDRAW]);
        PostMessage(hwnd, WM_QUIT, 0, 0L);
    }
    gb_system::gfx_bit_count = renderer.getBitCount();
    GB->init_gfx();
}

int WINAPI WinMain(HINSTANCE hThisInstance,HINSTANCE hPrevInstance, LPSTR  lpszArgument,int nFunsterStil)
{
    if (!initWindow(hThisInstance)) {
        return 0;
    }

   options = new program_configuration;
   GB1 = new gb_system;

   if(!options || !GB1 || !GB1->init() || !sgb_init())
   {
      debug_print(str_table[ERROR_MEMORY]);
      PostMessage(hwnd, WM_QUIT, 0, 0L);
   }

   GB = GB1;

   initGraphics();
   initConfigs();

   showWindow();

   if(!Init_DI())
   {
      debug_print(str_table[ERROR_DINPUT]);
      PostMessage(hwnd, WM_QUIT, 0, 0L);
   }

   initSound();

   initFromArg();

   return mainLoop();
}
