/*
   hhugboy Game Boy emulator
   copyright 2013-17 taizou

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

#include <fmod/fmod.h>
#include <afxres.h>
#include "window.h"
#include "../main.h"
#include "strings.h"
#include "menuactions.h"
#include "keyactions.h"
#include "../SGB.h"
#include "../state.h"
#include "../directinput.h"

HWND hwnd = NULL;
HWND hwndCtrl = NULL;
HINSTANCE hinst;

wchar_t w_title_text[ROM_FILENAME_SIZE + 16];

int sizen_w,sizen_h; // Size needed for menu,borders,etc.

wchar_t w_szClassName[] = L"hhugboyclass";

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

bool initWindow(HINSTANCE hThisInstance)
{
    WNDCLASSEX wincl;

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
        return false;

    emuMenu.init(hThisInstance);

    hwnd = CreateWindowEx(0,w_szClassName,w_emu_title,WS_SIZEBOX|WS_OVERLAPPEDWINDOW,150,150,2*160,2*144,HWND_DESKTOP,emuMenu.getMenu(),hThisInstance,NULL);

    RECT adjrect;
    GetClientRect(hwnd,&adjrect);

    sizen_w = 2*160-(adjrect.right-adjrect.left);
    sizen_h = 2*144-(adjrect.bottom-adjrect.top);

    return true;
}

void showWindow()
{
    ShowWindow(hwnd, SW_SHOW);
    DragAcceptFiles(hwnd,TRUE);
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_SYSKEYDOWN:
            if (wParam == VK_MENU) {
                return 0;
            }
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
            if(!wcsstr(drag_filename,L".zip") && !wcsstr(drag_filename,L".ZIP") && !wcsstr(drag_filename,L".sgb") && !wcsstr(drag_filename,L".SGB") && !wcsstr(drag_filename,L".GB") && !wcsstr(drag_filename,L".gb") && !wcsstr(drag_filename,L".GBX") && !wcsstr(drag_filename,L".gbx"))
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

                wsprintfW(w_title_text,L"%s - %s",w_emu_title,GB1->rom_filename);
                SetWindowText(hwnd,w_title_text);
                if(!paused)
                    FSOUND_SetMute(FSOUND_ALL,FALSE);
            } else
            if(romwasloaded && GB1->cartROM != NULL)
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
