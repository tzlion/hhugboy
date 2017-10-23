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
#include <commdlg.h>
#include <mmsystem.h>
#include <stdio.h>

#include <string>
#include <iostream>

using namespace std;

#include <math.h>
#include <time.h>

#define DIRECTINPUT_VERSION 0x0700
#include <dinput.h>

#include "ui/strings.h"
#include "cheats.h"
#include "config.h"
#include "ui/dialogs.h"

#include "main.h"
#include "debug.h"
#include "types.h"
#include "cpu.h"
#include "mainloop.h"
#include "sound.h"
#include "GB_gfx.h"
#include "SGB.h"
#include "devices.h"
#include "rendering/render.h"

#include "rendering/directdraw.h"
#include "directinput.h"


#include "ui/menu.h"

#include "GB.h"

gb_system* GB = NULL;
gb_system* GB1 = NULL;
gb_system* GB2 = NULL;

const wchar_t* prg_version = L"1.2.1";

// Windows stuff ----------------------------------------
HWND hwnd = NULL;           
HWND hwndCtrl = NULL;    

HINSTANCE hinst;

LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

wchar_t w_szClassName[] = L"GESTclass";
wchar_t w_emu_title[] = L"hhugboy";

char title_text[ROM_FILENAME_SIZE + 8];
wchar_t w_title_text[ROM_FILENAME_SIZE + 16];

int timer_id = 0;
int sizen_w,sizen_h; // Size needed for menu,borders,etc.

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

void cleanup()
{
   if(GB1 != NULL) 
   {
      delete GB1;
      GB1 = NULL;
   }
   
   if(GB2 != NULL) 
   {
      delete GB2;
      GB2 = NULL;
   }

   if(options != NULL)
   {
       delete options;
       options = NULL;
   }
                  
   sgb_end();
   
   Kill_DI();  
   
   FSOUND_Close();
}

int WINAPI WinMain(HINSTANCE hThisInstance,HINSTANCE hPrevInstance, LPSTR  lpszArgument,int nFunsterStil)
{
	
   WNDCLASSEX wincl;        
   MSG msg;    
   
   /* The Window structure */
   hinst = hThisInstance;
   wincl.hInstance = hThisInstance;
   wincl.lpszClassName = w_szClassName;
   wincl.lpfnWndProc = WindowProcedure;      
   wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
   wincl.cbSize = sizeof(WNDCLASSEX);

   wincl.hIcon = LoadIcon(hThisInstance, MAKEINTRESOURCE(ID_ICON));
   wincl.hIconSm = LoadIcon(hThisInstance, MAKEINTRESOURCE(ID_ICON));
   wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
   wincl.lpszMenuName = NULL;                 /* No menu */
   wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
   wincl.cbWndExtra = 0;                      /* structure or the window instance */
   wincl.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
   
   if(!RegisterClassEx(&wincl))
      return 0;
   
   emuMenu.init(hThisInstance);
   


   hwnd = CreateWindowEx(0,w_szClassName,w_emu_title,WS_SIZEBOX|WS_OVERLAPPEDWINDOW,150,150,2*160,2*144,HWND_DESKTOP,emuMenu.getMenu(),hThisInstance,NULL);
   
   RECT adjrect;
   GetClientRect(hwnd,&adjrect);
   
   sizen_w = 2*160-(adjrect.right-adjrect.left);
   sizen_h = 2*144-(adjrect.bottom-adjrect.top);

   options = new program_configuration;
   GB1 = new gb_system;

   if(!options || !GB1 || !GB1->init() || !sgb_init())
   { 
      debug_print(str_table[ERROR_MEMORY]); 
      PostMessage(hwnd, WM_QUIT, 0, 0L);  
   }

   GB = GB1;

   if(!renderer.init(&palette)) 
   { 
      debug_print(str_table[ERROR_DDRAW]); 
      PostMessage(hwnd, WM_QUIT, 0, 0L);  
   }
   gb_system::gfx_bit_count = renderer.getBitCount();
   GB->init_gfx();
        
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

   ShowWindow(hwnd, SW_SHOW);    

   if(!Init_DI())
   {
      debug_print(str_table[ERROR_DINPUT]);
      PostMessage(hwnd, WM_QUIT, 0, 0L);        
   }

   FSOUND_SetHWND(hwnd);
   if(FSOUND_Init(44100,2,0) == FALSE)
   {
       debug_print(str_table[ERROR_FMOD]);
       options->sound_on = -1;
   } else
      FSbuffer = FSOUND_Sample_Alloc(FSOUND_FREE,sound_buffer_total_len>>2,FSOUND_LOOP_NORMAL|FSOUND_16BITS|FSOUND_STEREO|FSOUND_SIGNED,44100,200,FSOUND_STEREOPAN,1);
   
   if(options->sound_on)
      channel_n = FSOUND_PlaySound(FSOUND_FREE,FSbuffer);
  
   DragAcceptFiles(hwnd,TRUE);

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
      

         swprintf(w_title_text,L"%s - %s",w_emu_title,GB1->rom_filename);
         SetWindowText(hwnd,w_title_text);
      }
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
                   swprintf(w_title_text,L"%s - %s --- %s, frameskip: %i",w_emu_title,GB1->rom_filename,GB2->rom_filename,options->video_frameskip);
                else
                   swprintf(w_title_text,L"%s - %s, frameskip: %i",w_emu_title,GB1->rom_filename,options->video_frameskip);
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

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)           
    {
        case WM_COMMAND:
            menuAction(wParam);
    	//char txt[100];
    	//sprintf(txt,"%d",wParam);
    	//debug_print(txt);

        break;
        case WM_INITMENU:
           FSOUND_SetMute(FSOUND_ALL,TRUE);
        break;
        case WM_EXITMENULOOP:
           if(GB1->romloaded && !paused)
              FSOUND_SetMute(FSOUND_ALL,FALSE);  
           if(sgb_mode || (options->GBC_SGB_border != OFF && border_uploaded))
               (renderer.*renderer.drawBorder)();
        break;
        case WM_ENTERSIZEMOVE:
           FSOUND_SetMute(FSOUND_ALL,TRUE); 
        break;
        case WM_EXITSIZEMOVE:
        {
           if(GB1->romloaded && !paused)   
              FSOUND_SetMute(FSOUND_ALL,FALSE); 

           renderer.handleWindowResize(); 
           
           if(sgb_mode || (options->GBC_SGB_border != OFF && border_uploaded)) {
               (renderer.*renderer.drawBorder)();
              if(sgb_mask == 1) (renderer.*renderer.drawScreen)();
           }      
           
           if(!GB1->romloaded || paused)
             (renderer.*renderer.drawScreen)(); // Totally not a fan of this syntax
        }
        break;
        case WM_SIZE:
        {
            
           renderer.handleWindowResize();  
           
           if(sgb_mode || (options->GBC_SGB_border != OFF && border_uploaded)) {
              (renderer.*renderer.drawBorder)();   
              if(sgb_mask == 1) (renderer.*renderer.drawScreen)();
           }    
           
           if(!GB1->romloaded || paused)
              (renderer.*renderer.drawScreen)();
        }
        break;
        case WM_ACTIVATE:  
           if(GB1->romloaded && (sgb_mode || (options->GBC_SGB_border != OFF && border_uploaded)))
              (renderer.*renderer.drawBorder)();
           
           if(!GB1->romloaded || paused)
              (renderer.*renderer.drawScreen)();
           
           control_pressed = 0;

           if((wParam == WA_ACTIVE) || (wParam == WA_CLICKACTIVE))
           {
              if(!menupause)
              {
                 paused = false;
                 if(GB1->romloaded)
                    FSOUND_SetMute(FSOUND_ALL,FALSE);
              }      
           } else
           {
              paused = true;
              speedup = 0;
              FSOUND_SetMute(FSOUND_ALL,TRUE);
           }
        break;
        case WM_KEYDOWN:
            keyAction(wParam);
        break;
        case WM_KEYUP:
            if(wParam == VK_CONTROL)
               control_pressed = 0;
        break;
        case WM_DESTROY:            
            if(GB1->romloaded && !GB1->write_save())
               debug_print(str_table[ERROR_SAVE_FILE_WRITE]);

            if(GB2 && GB2->romloaded && !GB2->write_save())
               debug_print(str_table[ERROR_SAVE_FILE_WRITE]);            
            
            write_config_file();
            cleanup();
            PostQuitMessage(0);     
        break;
        case WM_DROPFILES:
        {
           bool romwasloaded = GB1->romloaded;
                  
           if(GB1->romloaded && !GB1->write_save())
              debug_print(str_table[ERROR_SAVE_FILE_WRITE]);
           HDROP d_handle = (HDROP)wParam;      
           wchar_t drag_filename[500];
           DragQueryFileW(d_handle,0,drag_filename,500);
           if(!wcsstr(drag_filename,L".zip") && !wcsstr(drag_filename,L".ZIP") && !wcsstr(drag_filename,L".sgb") && !wcsstr(drag_filename,L".SGB") && !wcsstr(drag_filename,L".GB") && !wcsstr(drag_filename,L".gb"))
           {
              debug_print(str_table[NOT_A_ROM]);
              GB1->romloaded = false;
           } else {
		      gb1_loaded_file_name = drag_filename;
              GB1->load_rom(drag_filename);
           }
           
              
                  
           if(GB1->romloaded)
           {                    
              GB1->reset();
              
              int reswidth,resheight;
              
              if(sgb_mode)
              {
              	setWinSize(256,224);
              } else
              {
				setWinSize(160,144);
			  }
                     
              if(!GB1->load_save())
                 debug_print(str_table[ERROR_SAVE_FILE_READ]);
                  
              swprintf(w_title_text,L"%s - %s",w_emu_title,GB1->rom_filename);
              SetWindowText(hwnd,w_title_text);
              if(!paused)
                 FSOUND_SetMute(FSOUND_ALL,FALSE);   
           } else 
              if(romwasloaded && GB1->cartridge != NULL) 
                 GB1->romloaded = true;    
                 
           DragFinish(d_handle);  
           SetForegroundWindow(hwnd);  
        }
        break;
        default:                  
            return DefWindowProc(hwnd, message, wParam, lParam);
    }

    return 0;
}

void keyAction(int key)
{
    switch(key)
    { 
        case VK_CONTROL:
           control_pressed = 1;            
        break;
        case VK_ESCAPE: // QUIT
           SendMessage(hwnd, WM_CLOSE, 0, 0L);
        break;
        case VK_F2:
           if(GB1->romloaded)
              GB1->save_state();
        break;
        case VK_F3:

           if(++GB1_state_slot > 9)
              GB1_state_slot = 0;
              
           wchar_t dx_message[50];
           wsprintf(dx_message,L"%s %d",str_table[STATE_SLOT],GB1_state_slot);
           renderer.showMessage(dx_message,60,GB1);
           
           emuMenu.checkOption(IDM_CPUSTATESLOT0 + GB1_state_slot);
        break;
        case VK_F4:
           if(GB1->romloaded)
              GB1->load_state();
        break;
        case VK_F5:
            if(video_enable&VID_EN_BG)
            {
               emuMenu.uncheckOption(IDM_OPTIONVIDEOENBG);
               video_enable &= ~VID_EN_BG;
               wchar_t dx_message[50];
               wsprintf(dx_message,L"%s",L"BG off");
               renderer.showMessage(dx_message,40,GB1);
            } else
            {
               emuMenu.checkOption(IDM_OPTIONVIDEOENBG);
               video_enable |= VID_EN_BG;             
               wchar_t dx_message[50];   
               wsprintf(dx_message,L"%s",L"BG on");
               renderer.showMessage(dx_message,40,GB1);
            }              
        break;
        case VK_F6:
            if(video_enable&VID_EN_WIN)
            {
               emuMenu.uncheckOption(IDM_OPTIONVIDEOENWIN);
               video_enable &= ~VID_EN_WIN;
               wchar_t dx_message[50];
               wsprintf(dx_message,L"%s",L"WIN off");
               renderer.showMessage(dx_message,40,GB1);
            } else
            {
               emuMenu.checkOption(IDM_OPTIONVIDEOENWIN);
               video_enable |= VID_EN_WIN;       
               wchar_t dx_message[50];         
               wsprintf(dx_message,L"%s",L"WIN on");
               renderer.showMessage(dx_message,40,GB1);
            }              
        break;
        case VK_F7:
            if(video_enable&VID_EN_SPRITE)
            {
               emuMenu.uncheckOption(IDM_OPTIONVIDEOENSPRITE);
               video_enable &= ~VID_EN_SPRITE;
               wchar_t dx_message[50];
               wsprintf(dx_message,L"%s",L"Sprites off");
               renderer.showMessage(dx_message,40,GB1);
            } else
            {
               emuMenu.checkOption(IDM_OPTIONVIDEOENSPRITE);
               video_enable |= VID_EN_SPRITE;    
               wchar_t dx_message[50];            
               wsprintf(dx_message,L"%s",L"Sprites on");
               renderer.showMessage(dx_message,40,GB1);
            }              
        break;
        case VK_F12: {
            
			gb_system* screenshotGb = GB1;
			if (control_pressed && GB2) {
			    screenshotGb = GB2;
			}
			
			if (!screenshotGb->romloaded) {
			    break;
			}
        	
        	wchar_t img_filename[275];
			wcscpy(img_filename,screenshotGb->rom_filename);  // get the unicode filename
			char img_filename_mb[275]; 

			size_t res = wcstombs(img_filename_mb,img_filename,275); // but then oh well, look what happened
			// due to a limitation of png++ i cba to work around right now
		
			// now figure out the first available filename
			char final_filename[275];
			int fileno = 1;
			bool fileok = false;
			while ( fileok == false ) {
				char tmp_filename[275];

				if (res == -1) { // -1 = could not convert, so take the internal name instead
    			    sprintf(tmp_filename,"[%s]_%04d.png",GB1->rom->name,fileno);
    			} else {
    			    sprintf(tmp_filename,"%s_%04d.png",img_filename_mb,fileno);
    			}

		        ifstream ifile(tmp_filename);
				if ((bool)ifile) {
					fileno++;
				} else {
					fileok = true;
					strcpy(final_filename,tmp_filename);
				}
			}
			
			// show a msg
			wchar_t dx_message[50];
            wsprintf(dx_message,L"%s",L"Screenshot");
            renderer.showMessage(dx_message,40,screenshotGb);

			// and then screenshot this thing
        	screenshotPng(final_filename,screenshotGb);
        	break;
        	
        }
        	
        #ifdef ALLOW_DEBUG
        case 'D': // DEBUG
           if(!control_pressed)
              break;
           debug = !debug;
        break;
        case 'F': // STEP
           if(GB->romloaded && paused && !debug)
           {
              GB1->frames = 0;
              GB1->mainloop();
           }
           if(debug && GB->romloaded)
           {
              GB1->frames = 1;
              GB1->mainloop();
              GB1->frames = 0;
              (renderer.*renderer.drawDebugScreen)();
           }
        break;
        #endif
        case 'P': // PAUSE
           if(!control_pressed)
              break;
           
           if(menupause)
           {
              emuMenu.uncheckOption(IDM_CPUPAUSE);
              if(GB1->romloaded)
                 FSOUND_SetMute(FSOUND_ALL,FALSE);
           }
           else
           {
              FSOUND_SetMute(FSOUND_ALL,TRUE);
              emuMenu.checkOption(IDM_CPUPAUSE); 
           }
           menupause = !menupause;
           paused = !paused;
        break;
        #ifndef ALLOW_DEBUG
        case 'F': // RESET
           if(!control_pressed)
              break;

           soft_reset = 1;     
        break;          
        #endif  
        case 'R': // RESET
           if(!control_pressed)
              break;       
                   
           if(GB1->romloaded && !GB1->write_save())
              debug_print(str_table[ERROR_SAVE_FILE_WRITE]);
                 
           GB1->reset();
           
           if(GB1->romloaded && !GB1->load_save())
              debug_print(str_table[ERROR_SAVE_FILE_READ]);
        break;
        
        // change GBC palette for mono games
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '0':
        case 0xDC:
        case 0xBB: {
            int paletteNo;
            if(key >= '1' && key <= '9') {
                paletteNo = key - '1';
            } else if (key == '0') {
                paletteNo = 9;
            } else if (key == 0xDC) {
                paletteNo = 10;
            } else if (key == 0xBB) {
                paletteNo = 11;
            }
            if(GB1->gbc_mode && !GB1->rom->CGB) {
                memcpy(GB1->GBC_BGP,GBC_DMGBG_palettes[paletteNo],sizeof(unsigned int)*4);
                memcpy(GB1->GBC_OBP,GBC_DMGOBJ0_palettes[paletteNo],sizeof(unsigned int)*4);
                memcpy(GB1->GBC_OBP+4,GBC_DMGOBJ1_palettes[paletteNo],sizeof(unsigned int)*4);
            }
            if(GB2 && GB2->gbc_mode && !GB2->rom->CGB) {
                memcpy(GB2->GBC_BGP,GBC_DMGBG_palettes[paletteNo],sizeof(unsigned int)*4);
                memcpy(GB2->GBC_OBP,GBC_DMGOBJ0_palettes[paletteNo],sizeof(unsigned int)*4);
                memcpy(GB2->GBC_OBP+4,GBC_DMGOBJ1_palettes[paletteNo],sizeof(unsigned int)*4);
            }
            break; 
        }
                                                                                           
    }
}

void setWinSize(int width,int height) 
{
	// width, height are the base width and height for the screen size
	
   RECT winRect;
   GetWindowRect(hwnd,&winRect);
   MoveWindow(hwnd,winRect.left,winRect.top,options->video_size * width + sizen_w,options->video_size * height + sizen_h,TRUE);
   
   RECT adjrect;
   GetClientRect(hwnd,&adjrect);
   int sizen_w2 = options->video_size * width-(adjrect.right-adjrect.left);
   int sizen_h2 = options->video_size * height-(adjrect.bottom-adjrect.top);
   MoveWindow(hwnd,winRect.left,winRect.top,width*options->video_size+sizen_w+sizen_w2,height*options->video_size+sizen_h+sizen_h2,TRUE);
}
