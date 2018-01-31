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

#include <afxres.h>
#include <fmod/fmod.h>
#include "keyactions.h"
#include "../main.h"
#include "strings.h"
#include "../GB_gfx.h"
#include "../cpu.h"
#include "../directinput.h"
#include "dialogs.h"

#define UNICODE

HWND debugDialogHandle = NULL;

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
            wsprintfW(dx_message,L"%s %d",str_table[STATE_SLOT],GB1_state_slot);
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
                wsprintfW(dx_message,L"%s",L"BG off");
                renderer.showMessage(dx_message,40,GB1);
            } else
            {
                emuMenu.checkOption(IDM_OPTIONVIDEOENBG);
                video_enable |= VID_EN_BG;
                wchar_t dx_message[50];
                wsprintfW(dx_message,L"%s",L"BG on");
                renderer.showMessage(dx_message,40,GB1);
            }
            break;
        case VK_F6:
            if(video_enable&VID_EN_WIN)
            {
                emuMenu.uncheckOption(IDM_OPTIONVIDEOENWIN);
                video_enable &= ~VID_EN_WIN;
                wchar_t dx_message[50];
                wsprintfW(dx_message,L"%s",L"WIN off");
                renderer.showMessage(dx_message,40,GB1);
            } else
            {
                emuMenu.checkOption(IDM_OPTIONVIDEOENWIN);
                video_enable |= VID_EN_WIN;
                wchar_t dx_message[50];
                wsprintfW(dx_message,L"%s",L"WIN on");
                renderer.showMessage(dx_message,40,GB1);
            }
            break;
        case VK_F7:
            if(video_enable&VID_EN_SPRITE)
            {
                emuMenu.uncheckOption(IDM_OPTIONVIDEOENSPRITE);
                video_enable &= ~VID_EN_SPRITE;
                wchar_t dx_message[50];
                wsprintfW(dx_message,L"%s",L"Sprites off");
                renderer.showMessage(dx_message,40,GB1);
            } else
            {
                emuMenu.checkOption(IDM_OPTIONVIDEOENSPRITE);
                video_enable |= VID_EN_SPRITE;
                wchar_t dx_message[50];
                wsprintfW(dx_message,L"%s",L"Sprites on");
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
            wsprintfW(dx_message,L"%s",L"Screenshot");
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
        case 'D':
            if(!control_pressed)
                break;
            debugDialogHandle = CreateDialog(hinst, MAKEINTRESOURCE(ID_DEBUG_DIALOG), hwnd, (DLGPROC)DebugLogProc);
            ShowWindow(debugDialogHandle, SW_SHOW);
            break;
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
