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

#include <windows.h>
#include <stdio.h>

#include <string>
#include <iostream>

using namespace std;

extern wstring gb1_loaded_file_name;

#include "dialogs.h"
#include "strings.h"
#include "../main.h"
#include "../rom.h"
#include "../config.h"
#include "../cheats.h"
#include "../directinput.h"
#include "../GB.h"
#include "window.h"

int change_index = 0; // currently changing which button? 

char* stpncpy(char* to,char* from,int count)
{
   char *to_ptr = to;
   for(int i=0;i<count;++i,++to_ptr)
      *to_ptr = from[i];
   return to_ptr;
}

bool DoFileOpen(HWND hwnd,int gb_number)
{
   OPENFILENAMEW ofn;
   wchar_t szFileName[MAX_PATH];

   ZeroMemory(&ofn, sizeof(ofn));
   szFileName[0] = 0;

   ofn.lStructSize = sizeof(ofn);
   ofn.hwndOwner = hwnd;
   
   /*char ctrl_str[150];
   char* cptr = ctrl_str;

   cptr = stpncpy(cptr,str_table[GB_ROMS],strlen(str_table[GB_ROMS]));
   cptr = stpncpy(cptr," (*.gb,*.gbc,*.sgb,*.zip)\0*.gb;*.gbc;*.sgb;*.zip\0",strlen(" (*.gb,*.gbc,*.sgb,*.zip)*.gb;*.gbc;*.sgb;*.zip")+2);
   cptr = stpncpy(cptr,str_table[ALL_FILES],strlen(str_table[ALL_FILES]));
   cptr = stpncpy(cptr," (*.*)\0*.*\0\0",strlen(" (*.*)*.*")+3);
   
   wchar_t w_ctrl_str[150];
   mbstowcs(w_ctrl_str,ctrl_str,150);
   //ofn.lpstrFilter =  w_ctrl_str; */ //removed for now, just to get it working..
   ofn.lpstrFilter =  L"GB roms (*.gb,*.gbc,*.gbx,*.sgb,*.zip)\0*.gb;*.gbc;*.gbx;*.sgb;*.zip\0All files (*.*)\0*.*\0\0";
   ofn.lpstrFile = szFileName;
   ofn.nMaxFile = MAX_PATH;
   ofn.lpstrDefExt = L"gb";

   ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
   
   SetCurrentDirectory(options->rom_directory.c_str());

   if(GetOpenFileNameW(&ofn))
   {
      wchar_t temp[PROGRAM_PATH_SIZE];

      GetCurrentDirectory(PROGRAM_PATH_SIZE,temp);

      options->rom_directory = temp;
      
      if(gb_number == 1)
      {
         gb1_loaded_file_name = ofn.lpstrFile;
         return GB1->load_rom(ofn.lpstrFile);
      }
      else
         return GB2->load_rom(ofn.lpstrFile);        
   }

   if(gb_number == 1)
      GB1->romloaded = false;  
   else
      GB2->romloaded = false;   

   return true;
}

HWND debugDialog;

void addDebugLogMessage(const wchar_t* message)
{
    if (debugDialog) {
        if (SendDlgItemMessage(debugDialog, ID_DEBUG_LOG_CHECKBOX, BM_GETCHECK, 0, 0)) {
            FILE* logfile;
            logfile = fopen("debuglog.txt","a");
            fputws(message, logfile);
            fputws(L"\r\n", logfile);
            fclose(logfile);
        }
        HWND hwndbox = GetDlgItem(debugDialog, ID_DEBUG_LOG);
        SendMessage(hwndbox, LB_ADDSTRING, 0, (LPARAM)message );
        SendMessage(hwndbox, LB_SETCARETINDEX, SendMessage(hwndbox,LB_GETCOUNT,0,0)-1, true );
    }
}

BOOL CALLBACK DebugLogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_INITDIALOG:
            debugDialog = hwndDlg;
            addDebugLogMessage(L"SECRET DEBUG LOG ACTIVATE");
            break;
        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case IDOK:
                case IDCANCEL:
                    EndDialog(hwndDlg, wParam);
                    return TRUE;
            }
            break;
        case WM_VKEYTOITEM:
            return -2;
    }
    return FALSE;
}

BOOL CALLBACK FolderProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{
    switch (message)
    {
    case WM_INITDIALOG:
        SetDlgItemText(hwndDlg, ID_F_SAVE, options->save_directory.c_str());
        SetDlgItemText(hwndDlg, ID_F_STATE, options->state_directory.c_str());
    break;
    case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case IDOK:
                    wchar_t temp[PROGRAM_PATH_SIZE];

                    GetDlgItemText(hwndDlg, ID_F_SAVE, temp,PROGRAM_PATH_SIZE);
                    options->save_directory = temp;
                    GetDlgItemText(hwndDlg, ID_F_STATE, temp,PROGRAM_PATH_SIZE);
                    options->state_directory = temp;

                    // Fall through.

                case IDCANCEL:
                    EndDialog(hwndDlg, wParam);
                return TRUE;
            }
    }
    return FALSE;
} 

gg_cheats old_cheats[MAXGGCHEATS];
int old_num = 0;

// Cheat dialog box program
BOOL CALLBACK ChtProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{ 
    HWND hwndbox = GetDlgItem(hwndDlg, ID_CHT_CHEATS);

    switch (message) 
    { 
    case WM_INITDIALOG:
       for(int i=0;i<MAXGGCHEATS;++i)
          old_cheats[i] = cheat[i];
       old_num = number_of_cheats;
       for(int i=0;i<number_of_cheats;++i)
          SendMessage(hwndbox, LB_ADDSTRING, 0, (LPARAM)cheat[i].str );
    break;
    case WM_COMMAND: 
            switch (LOWORD(wParam)) 
            { 
                case ID_CHT_ADDBTN:
                   if(number_of_cheats < MAXGGCHEATS)
                   {
                      wchar_t cheat_str[20];
                      GetDlgItemText(hwndDlg, ID_CHT_ADD, cheat_str,20); 

                      if(add_cheat(cheat_str))
                         SendMessage(hwndbox, LB_ADDSTRING, 0, (LPARAM)cheat[number_of_cheats-1].str );
                      else
                         if(number_of_cheats != MAXGGCHEATS)
                            debug_print(str_table[GG_CHT_FORMAT]);
                   }   
                break;
                case ID_CHT_RMVBTN:
                {
                   int selected_cheat = SendMessage(hwndbox, LB_GETCURSEL, 0, 0L); 
                   if(selected_cheat != LB_ERR)
                   {
                      SendMessage(hwndbox,LB_DELETESTRING,(WPARAM)selected_cheat,0L);                      
                      remove_cheat(selected_cheat);
                   }
                }
                break;
                case IDOK: 
                   EndDialog(hwndDlg, wParam); 
                break;

 
                case IDCANCEL: 
                   number_of_cheats = old_num;
                   
                   for(int i=0;i<MAXGGCHEATS;++i)
                      cheat[i] = old_cheats[i];
                   EndDialog(hwndDlg, wParam); 
                return TRUE; 
            } 
    } 
    return FALSE; 
} 

// Change controls timer program
VOID CALLBACK TProc(HWND hwnd,UINT uMsg,UINT idEvent,DWORD dwTime)
{
  // options->key_config = options->multi_key_config[current_controller];
   
   int changed = check_change_keys(change_index,current_controller);

   //options->key_config = options->multi_key_config[0];
   
   if(!changed)
      return;
   
   if(change_index < 12)
      SetDlgItemText(hwnd, ID_CTRL_A + change_index, key_names[options->multi_key_config[current_controller][change_index]]);
   else 
      SetDlgItemText(hwnd, ID_CTRL_A + change_index, key_names[options->special_keys[change_index-12]]);

/*   if(current_controller != 0) //Other than player 1
   {
      SetDlgItemText(hwnd, ID_CTRL_A + change_index, key_names[options->multi_key_config[current_controller][change_index]]);
   } else
   {
      if(change_index < 12)
         SetDlgItemText(hwnd, ID_CTRL_A + change_index, key_names[options->key_config[change_index]]);
      else 
         SetDlgItemText(hwnd, ID_CTRL_A + change_index, key_names[options->special_keys[change_index-12]]);
   }*/
}  

// Change controls dialog box program   
BOOL CALLBACK CtrlProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{    
    switch(message) 
    { 
    case WM_INITDIALOG:
       for(int i = 0;i < 12; ++i)
       {
          SetDlgItemText(hwndDlg, ID_CTRL_A+i, key_names[options->multi_key_config[PLAYER1][i]]);
          
          HWND hwndx = GetDlgItem(hwndDlg, ID_CTRL_A+i);
          SendMessage(hwndx, EM_SETREADONLY, TRUE,0L);
       }
       for(int i = 0;i < 3; ++i)
       {
          SetDlgItemText(hwndDlg, ID_CTRL_L+i, key_names[options->special_keys[i]]);

          HWND hwndx = GetDlgItem(hwndDlg, ID_CTRL_L+i);
          SendMessage(hwndx, EM_SETREADONLY, TRUE,0L);
       }

       change_index = 0;
       
       timer_id = SetTimer(hwndDlg,20,100,TProc);
    break;
    case WM_COMMAND: 
            if(HIWORD(wParam) == EN_SETFOCUS && LOWORD(wParam) >= ID_CTRL_A && LOWORD(wParam) <= ID_CTRL_SPEEDUP)
            {
               change_index = LOWORD(wParam) - ID_CTRL_A;
               break;
            }
               
            switch (LOWORD(wParam)) 
            { 
                case IDOK: 
                    Kill_DI_change();     
                    KillTimer(hwndDlg,timer_id);           
                    EndDialog(hwndDlg, wParam); 
                return TRUE; 
                
                case IDCANCEL: 
                    Kill_DI_change();                
                    KillTimer(hwndDlg,timer_id);                    
                    DestroyWindow(hwndDlg); 
                    hwndCtrl = NULL; 
                return TRUE; 

            } 
    break;
    } 
    return FALSE; 
} 

// Change multi-controls dialog box program   
BOOL CALLBACK MultiCtrlProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{    
    switch(message) 
    { 
    case WM_INITDIALOG:
       for(int i = 0; i < 12;++i)
       {
          SetDlgItemText(hwndDlg, ID_CTRL_A+i, key_names[options->multi_key_config[current_controller][i]]);
          
          HWND hwndx = GetDlgItem(hwndDlg, ID_CTRL_A+i);
          SendMessage(hwndx, EM_SETREADONLY, TRUE,0L);
       }  
       change_index = 0;
       
       timer_id = SetTimer(hwndDlg,20,100,TProc);
    break;
    case WM_COMMAND: 
            if(HIWORD(wParam) == EN_SETFOCUS && LOWORD(wParam) >= ID_CTRL_A && LOWORD(wParam) <= ID_CTRL_RIGHT)
            {
               change_index = LOWORD(wParam)-ID_CTRL_A;
               break;
            }
               
            switch (LOWORD(wParam)) 
            { 
                case ID_CTRL_CLEAR:
                   for(int i = 0;i < 12; ++i)
                   {
                      options->multi_key_config[current_controller][i] = 0;
                      SetDlgItemText(hwndDlg, ID_CTRL_A+i, key_names[options->multi_key_config[current_controller][i]]);
                   }                          
                break;
                case IDOK: 
                    Kill_DI_change();     
                    KillTimer(hwndDlg,timer_id);           
                    EndDialog(hwndDlg, wParam); 
                return TRUE; 
                
                case IDCANCEL: 
                    Kill_DI_change();                
                    KillTimer(hwndDlg,timer_id);                    
                    DestroyWindow(hwndDlg); 
                    hwndCtrl = NULL; 
                return TRUE; 

            } 
    break;
    } 
    return FALSE; 
} 

// Change sensor controls dialog box program   
BOOL CALLBACK SensorProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{    
    switch(message) 
    { 
    case WM_INITDIALOG:
       for(int i = 3;i < 7;++i)
       {
          SetDlgItemText(hwndDlg, ID_SENSOR_LEFT+(i-3), key_names[options->special_keys[i]]);
          
          HWND hwndx = GetDlgItem(hwndDlg, ID_SENSOR_LEFT+(i-3));
          SendMessage(hwndx, EM_SETREADONLY, TRUE,0L);
       }
       change_index = 15;
       
       timer_id = SetTimer(hwndDlg,20,100,TProc);
    break;
    case WM_COMMAND: 
            if(HIWORD(wParam) == EN_SETFOCUS && LOWORD(wParam) >= ID_SENSOR_LEFT && LOWORD(wParam) <= ID_SENSOR_DOWN)
            {
               change_index = LOWORD(wParam)-ID_CTRL_A;
               break;
            }
               
            switch (LOWORD(wParam)) 
            { 
                case IDOK: 
                    Kill_DI_change();     
                    KillTimer(hwndDlg,timer_id);                                 
                    EndDialog(hwndDlg, wParam); 
                return TRUE; 
                
                case IDCANCEL: 
                    Kill_DI_change();             
                    KillTimer(hwndDlg,timer_id);                         
                    DestroyWindow(hwndDlg); 
                    hwndCtrl = NULL; 
                return TRUE; 

            } 
    break;
    } 
    return FALSE; 
} 


VOID CALLBACK TJoyProc(HWND hwnd,UINT uMsg,UINT idEvent,DWORD dwTime)
{  
   int changed = check_change_joypad(change_index);
   
   if(!changed)
      return;

   wchar_t buffer[10];
   wsprintf(buffer,L"%d",options->joystick_config[change_index]);
   SetDlgItemText(hwnd, ID_CTRLJOY_A+change_index, buffer); 
} 

// Change joystick controls dialog box program
BOOL CALLBACK JoyCtrlProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{    
    switch(message) 
    { 
    case WM_INITDIALOG:
       for(int i=0;i<6;++i)
       {         
          HWND hwndx = GetDlgItem(hwndDlg, ID_CTRLJOY_A+i);
          SendMessage(hwndx, EM_SETREADONLY, TRUE,0L);
          wchar_t buffer[10];
          wsprintf(buffer,L"%d",options->joystick_config[i]);
          SetDlgItemText(hwndDlg, ID_CTRLJOY_A+i, buffer); 
       }  
       change_index = 0;
       
       timer_id = SetTimer(hwndDlg,20,100,TJoyProc);
    break;
    case WM_COMMAND: 
            if(HIWORD(wParam) == EN_SETFOCUS && LOWORD(wParam) >= ID_CTRLJOY_A && LOWORD(wParam) <= ID_CTRLJOY_AUTO_B)
            {
               change_index = LOWORD(wParam)-ID_CTRLJOY_A;
               break;
            }
               
            switch (LOWORD(wParam)) 
            { 
                case IDOK: 
                    KillTimer(hwndDlg,timer_id);           
                    EndDialog(hwndDlg, wParam); 
                return TRUE; 
                
                case IDCANCEL:              
                    KillTimer(hwndDlg,timer_id);                    
                    EndDialog(hwndDlg, wParam); 
                return TRUE; 
            } 
    break;
    } 
    return FALSE; 
} 


