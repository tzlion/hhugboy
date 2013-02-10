/*
   unGEST Game Boy emulator
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

#define DIRECTINPUT_VERSION 0x0700
#include <dinput.h>

#include "directinput.h"
#include "cpu.h"
#include "debug.h"
#include "mainloop.h"
#include "sound.h"
#include "SGB.h"
#include "GB.h"
#include "config.h"

#include "keynames.h"

extern int speedup;
extern int sizen_w,sizen_h; 

int old_sound_on = 0;
int soft_reset = 0;
int joystick_device_index = -1;

IDirectInput7* DI = NULL; 
IDirectInputDevice7*  di_keyboard = NULL; 
#define MAX_INPUT_DEVICES 10
DIDEVICEINSTANCE di_devices[MAX_INPUT_DEVICES+1];
int num_devices = 0;
IDirectInputDevice7*  di_joystick = NULL; 

IDirectInput7* DI_c = NULL; 
IDirectInputDevice7*  di_keyboard_c = NULL; 
IDirectInputDevice7*  di_joystick_c = NULL; 

void Kill_DI()
{
   SafeRelease(di_keyboard);
   SafeRelease(di_joystick);
   
   SafeRelease(DI);
}

#define KEYDOWN(name,key) (name[key] & 0x80) 

int autofire_delay[4][4];

void check_joystick_input()
{
   HRESULT                 hRes; 
   DIJOYSTATE              js; 
 
   // poll the joystick to read the current state
   hRes = IDirectInputDevice7_Poll(di_joystick);
 
   // get data from the joystick 
   hRes = IDirectInputDevice7_GetDeviceState(di_joystick, sizeof(DIJOYSTATE), &js); 

   if(hRes != DI_OK) 
   { 
      //if(hRes == DIERR_INPUTLOST) 
      di_joystick->Acquire();
      return;
   } 

   // Now study the position of the stick and the buttons. 
 
   if(js.lX < 0) 
   { 
      GB->button_pressed[B_RIGHT] = 1;                       
      GB->button_pressed[B_LEFT] = 0;  
   } else if (js.lX > 0) 
   { 
      GB->button_pressed[B_RIGHT] = 0;                       
      GB->button_pressed[B_LEFT] = 1;   
   }
 
   if(js.lY < 0) 
   { 
      GB->button_pressed[B_UP] = 0;                       
      GB->button_pressed[B_DOWN] = 1;  
   } else if (js.lY > 0) 
   { 
      GB->button_pressed[B_DOWN] = 0;                       
      GB->button_pressed[B_UP] = 1;   
   }
 
   if(js.rgbButtons[options->joystick_config[0]] & 0x80)
   { 
      GB->button_pressed[B_A] = 0;   
   }   
 
   if(js.rgbButtons[options->joystick_config[1]] & 0x80)
   { 
      GB->button_pressed[B_B] = 0;  
   }
 
   if(js.rgbButtons[options->joystick_config[2]] & 0x80)
   { 
      GB->button_pressed[B_START] = 0;
   }

   if(js.rgbButtons[options->joystick_config[3]] & 0x80)
   { 
      GB->button_pressed[B_SELECT] = 0;
   }    

   if(js.rgbButtons[options->joystick_config[4]] & 0x80)
   { 
      if(--autofire_delay[options->use_joystick_input][0] <= 0)
      {
         autofire_delay[options->use_joystick_input][0] = options->autofire_speed;
         GB->button_pressed[B_A] = !GB->button_pressed[B_A];
      }
   } else
       autofire_delay[options->use_joystick_input][0] = 0;

   if(js.rgbButtons[options->joystick_config[5]] & 0x80)
   { 
      if(--autofire_delay[options->use_joystick_input][1] <= 0)
      {
         autofire_delay[options->use_joystick_input][1] = options->autofire_speed;
         GB->button_pressed[B_B] = !GB->button_pressed[B_B];
      }
   } else
       autofire_delay[options->use_joystick_input][1] = 0;
}
      
void Check_KBInput(int i) 
{ 
   char     buffer[256]; 
   HRESULT  hr; 
   if(GB == GB2)
      i = 1;
   hr = di_keyboard->GetDeviceState(sizeof(buffer),(LPVOID)&buffer);

   if(FAILED(hr))
   { 
      di_keyboard->Acquire();
      return; 
   } 
   
   if(soft_reset)
   {
      if(soft_reset == 1)
      {
         GB1->button_pressed[B_START] = GB1->button_pressed[B_SELECT] = GB1->button_pressed[B_A] = GB1->button_pressed[B_B] = 0;
      } else
      {
         GB2->button_pressed[B_START] = GB2->button_pressed[B_SELECT] = GB2->button_pressed[B_A] = GB2->button_pressed[B_B] = 0;
      }

      soft_reset = 0;
      return;
   }         
      
   if(KEYDOWN(buffer,options->multi_key_config[i][BUTTON_TURBO_A]))
   {
      if(--autofire_delay[i][0] <= 0)
      {
         autofire_delay[i][0] = options->autofire_speed;
         GB->button_pressed[B_A] = !GB->button_pressed[B_A];
      }
   }
   else
   if(KEYDOWN(buffer,options->multi_key_config[i][BUTTON_A]))
      GB->button_pressed[B_A] = 0;           
   else
   {
      GB->button_pressed[B_A] = 1;
      autofire_delay[i][0] = 0;
   }
      
   if(KEYDOWN(buffer,options->multi_key_config[i][BUTTON_TURBO_B]))
   {
      if(--autofire_delay[i][1] <= 0)
      {
         autofire_delay[i][1] = options->autofire_speed;
         GB->button_pressed[B_B] = !GB->button_pressed[B_B];
      }
   }
   else                 
   if(KEYDOWN(buffer,options->multi_key_config[i][BUTTON_B]))
      GB->button_pressed[B_B] = 0;           
   else
   {
      GB->button_pressed[B_B] = 1;
      autofire_delay[i][1] = 0;
   }
         
   if(KEYDOWN(buffer,options->multi_key_config[i][BUTTON_LEFT]))
   {                       
      GB->button_pressed[B_LEFT] = 0;
      if(!options->opposite_directions_allowed) GB->button_pressed[B_RIGHT] = 1;
   }
   else
      GB->button_pressed[B_LEFT] = 1; 
         
   if(KEYDOWN(buffer,options->multi_key_config[i][BUTTON_RIGHT]))
   {
      GB->button_pressed[B_RIGHT] = 0;                       
      if(!options->opposite_directions_allowed) GB->button_pressed[B_LEFT] = 1;
   }
   else  
      GB->button_pressed[B_RIGHT] = 1; 
          
   if(KEYDOWN(buffer,options->multi_key_config[i][BUTTON_UP]))
   {
      GB->button_pressed[B_UP] = 0;                       
      if(!options->opposite_directions_allowed) GB->button_pressed[B_DOWN] = 1;
   }
   else    
      GB->button_pressed[B_UP] = 1;   
        
   if(KEYDOWN(buffer,options->multi_key_config[i][BUTTON_DOWN]))
   {
      GB->button_pressed[B_DOWN] = 0;                       
      if(!options->opposite_directions_allowed) GB->button_pressed[B_UP] = 1;
   }
   else    
      GB->button_pressed[B_DOWN] = 1;     

   if(KEYDOWN(buffer,options->multi_key_config[i][BUTTON_TURBO_START]))
   {
      if(--autofire_delay[i][2] <= 0)
      {
         autofire_delay[i][2] = options->autofire_speed;
         GB->button_pressed[B_START] = !GB->button_pressed[B_START];
      }
   }
   else
   if(KEYDOWN(buffer,options->multi_key_config[i][BUTTON_START]))
      GB->button_pressed[B_START] = 0;           
   else
   {
      GB->button_pressed[B_START] = 1;
      autofire_delay[i][2] = 0;
   }

   if(KEYDOWN(buffer,options->multi_key_config[i][BUTTON_TURBO_SELECT]))
   {
      if(--autofire_delay[i][3] <= 0)
      {
         autofire_delay[i][3] = options->autofire_speed;
         GB->button_pressed[B_SELECT] = !GB->button_pressed[B_SELECT];
      }
   }
   else
   if(KEYDOWN(buffer,options->multi_key_config[i][BUTTON_SELECT]))
      GB->button_pressed[B_SELECT] = 0;           
   else
   {
      GB->button_pressed[B_SELECT] = 1;
      autofire_delay[i][3] = 0;
   }
         
   if(KEYDOWN(buffer,options->special_keys[BUTTON_SENSOR_UP]))
      sensor_dir[SENSOR_UP] = 1;             
   else     
      sensor_dir[SENSOR_UP] = 0;   
      
   if(KEYDOWN(buffer,options->special_keys[BUTTON_SENSOR_DOWN]))
      sensor_dir[SENSOR_DOWN] = 1;             
   else               
      sensor_dir[SENSOR_DOWN] = 0;  
                   
   if(KEYDOWN(buffer,options->special_keys[BUTTON_SENSOR_LEFT]))
      sensor_dir[SENSOR_LEFT] = 1;             
   else             
      sensor_dir[SENSOR_LEFT] = 0;     
                
   if(KEYDOWN(buffer,options->special_keys[BUTTON_SENSOR_RIGHT]))
      sensor_dir[SENSOR_RIGHT] = 1;             
   else                                                                                  
      sensor_dir[SENSOR_RIGHT] = 0;   
   
   if(di_joystick != NULL && i == options->use_joystick_input)
      check_joystick_input();
      
   if(KEYDOWN(buffer,options->special_keys[BUTTON_SPEEDUP]))
   {
      if(speedup == 0 && options->speedup_sound_off)
      {
         old_sound_on = options->sound_on;
         options->sound_on = 0;
         if(FSOUND_IsPlaying(channel_n) == TRUE)
            FSOUND_StopSound(channel_n);
      }
      speedup = 1;
   }
   else if(speedup)
   {      
      if(speedup && options->speedup_sound_off)
      {
         options->sound_on = old_sound_on;
         if(options->sound_on)
            if(FSOUND_IsPlaying(channel_n) == FALSE)
               channel_n = FSOUND_PlaySound(FSOUND_FREE,FSbuffer);
      }
      speedup = 0;
   }
            
   if(KEYDOWN(buffer,options->special_keys[BUTTON_L]) && !multiple_gb)
   {
      RECT winRect;
      GetWindowRect(hwnd,&winRect); 
      if(GB1->system_type == SYS_GBA) 
         MoveWindow(hwnd,winRect.left,winRect.top,sizen_w+options->video_size*240,options->video_size*144+sizen_h,TRUE);
   } else
   if(KEYDOWN(buffer,options->special_keys[BUTTON_R]) && !multiple_gb)
   {
      RECT winRect;
      GetWindowRect(hwnd,&winRect); 
      if(GB1->system_type == SYS_GBA) 
         MoveWindow(hwnd,winRect.left,winRect.top,sizen_w+options->video_size*160,options->video_size*144+sizen_h,TRUE);
   }                                                                              
}

void check_system_keys()
{
   char     buffer[256]; 
   HRESULT  hr; 
 
   hr = di_keyboard->GetDeviceState(sizeof(buffer),(LPVOID)&buffer); 
   if(FAILED(hr))
   { 
      di_keyboard->Acquire();
      return; 
   } 
                   
   if(KEYDOWN(buffer,options->special_keys[BUTTON_SPEEDUP]))
   {
      if(speedup == 0 && options->speedup_sound_off)
      {
         old_sound_on = options->sound_on;
         options->sound_on = 0;
         if(FSOUND_IsPlaying(channel_n) == TRUE)
            FSOUND_StopSound(channel_n);
      }
      speedup = 1;
   }
   else
   {      
      if(speedup && options->speedup_sound_off)
      {
         options->sound_on = old_sound_on;
         if(options->sound_on)
            if(FSOUND_IsPlaying(channel_n) == FALSE)
               channel_n = FSOUND_PlaySound(FSOUND_FREE,FSbuffer);
      }
      speedup = 0;
   }
            
   if(KEYDOWN(buffer,options->special_keys[BUTTON_L]) && !multiple_gb)
   {
      RECT winRect;
      GetWindowRect(hwnd,&winRect); 
      if(GB1->system_type == SYS_GBA) 
         MoveWindow(hwnd,winRect.left,winRect.top,sizen_w+options->video_size*240,options->video_size*144+sizen_h,TRUE);
   } else
   if(KEYDOWN(buffer,options->special_keys[BUTTON_R]) && !multiple_gb)
   {
      RECT winRect;
      GetWindowRect(hwnd,&winRect); 
      if(GB1->system_type == SYS_GBA) 
         MoveWindow(hwnd,winRect.left,winRect.top,sizen_w+options->video_size*160,options->video_size*144+sizen_h,TRUE);
   }                  

}

BOOL CALLBACK EnumDeviceProc(DIDEVICEINSTANCE* pdidi, VOID* )
{
	if(num_devices >= MAX_INPUT_DEVICES)
		return DIENUM_STOP;

	memcpy(&di_devices[num_devices++], pdidi, sizeof(DIDEVICEINSTANCE));

	return DIENUM_CONTINUE;
}

IDirectInputDevice7* InitJoystickInput(DIDEVICEINSTANCE* pdinst,HWND hwndx) 
{ 
   IDirectInputDevice7* pdev; 

   // Create the DirectInput joystick device. 
   if(DI->CreateDeviceEx(pdinst->guidInstance, IID_IDirectInputDevice7, (void**)&pdev, NULL) != DI_OK) 
   { 
      debug_print("IDirectInput7::CreateDeviceEx FAILED\n"); 
      return NULL; 
   }

   if(pdev->SetDataFormat(&c_dfDIJoystick) != DI_OK) 
   { 
      debug_print("IDirectInputDevice7::SetDataFormat FAILED\n"); 
      pdev->Release(); 
      return NULL; 
   }
   
   if(pdev->SetCooperativeLevel(hwndx, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND) != DI_OK) 
   { 
      debug_print("IDirectInputDevice7::SetCooperativeLevel FAILED\n"); 
      pdev->Release(); 
      return NULL; 
   }

   DIPROPRANGE diprg; 
 
   diprg.diph.dwSize       = sizeof(diprg); 
   diprg.diph.dwHeaderSize = sizeof(diprg.diph); 
   diprg.diph.dwObj        = DIJOFS_X; 
   diprg.diph.dwHow        = DIPH_BYOFFSET; 
   diprg.lMin              = -1000; 
   diprg.lMax              = 1000; 
 
   if(FAILED(pdev->SetProperty(DIPROP_RANGE, &diprg.diph)))
   { 
      debug_print("IDirectInputDevice7::SetProperty(DIPH_RANGE) FAILED\n"); 
      pdev->Release(); 
      return NULL; 
   }

   diprg.diph.dwObj = DIJOFS_Y; 
   if(FAILED(pdev->SetProperty(DIPROP_RANGE, &diprg.diph))) 
   { 
      debug_print("IDirectInputDevice7::SetProperty(DIPH_RANGE) FAILED\n"); 
      pdev->Release(); 
      return NULL; 
   }   
   
   DIPROPDWORD dipdw;
   HRESULT hr;
   
   // Set the deadzone for the device
	dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(dipdw.diph);
	dipdw.diph.dwHow        = DIPH_BYOFFSET;
	dipdw.dwData            = 2500;

    // Set the x-axis deadzone property
    dipdw.diph.dwObj         = DIJOFS_X;
    hr = pdev->SetProperty( DIPROP_DEADZONE, &dipdw.diph );
    if( FAILED(hr) )
    {
       pdev->Release(); 
       return NULL;
    }

    // Set the y-axis deadzone property
    dipdw.diph.dwObj = DIJOFS_Y;
    hr = pdev->SetProperty( DIPROP_DEADZONE, &dipdw.diph );
    if(FAILED(hr))
    {
       pdev->Release();     
       return NULL;
    }

    // Acquire the device
    pdev->Acquire();
    
    return pdev;
}

bool Init_DI()
{
   for(int player_n = 0; player_n < 4; player_n++)
   {
       for(int autofire_key = 0; autofire_key < 4; autofire_key++)
       {
           autofire_delay[player_n][autofire_key] = options->autofire_speed;
       }
   }

   HRESULT hr;
   hr = DirectInputCreateEx(hinst, DIRECTINPUT_VERSION, IID_IDirectInput7, (void**)&DI, NULL); 
   if(FAILED(hr)) 
   { 
      return false;// DirectInput not available; take appropriate action 
   }
   
   hr = DI->CreateDeviceEx(GUID_SysKeyboard, IID_IDirectInputDevice7,(void**)&di_keyboard, NULL); 
   if(FAILED(hr)) 
   { 
      Kill_DI(); 
      return false; 
   } 
   
   hr = di_keyboard->SetDataFormat(&c_dfDIKeyboard); 
   if(FAILED(hr)) 
   { 
      Kill_DI(); 
      return false; 
   } 

   hr = di_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE); 
   if(FAILED(hr)) 
   { 
      Kill_DI(); 
      return false; 
   } 
   
   if(di_keyboard) 
      hr = di_keyboard->Acquire();
   
   DI->EnumDevices(DIDEVTYPE_JOYSTICK, (LPDIENUMDEVICESCALLBACK)EnumDeviceProc, DI, DIEDFL_ATTACHEDONLY);
   for(int i=0;i<num_devices;++i)
   {
      DWORD dewtype = di_devices[i].dwDevType & 0xff;
      if(dewtype == DIDEVTYPE_JOYSTICK)
      {
         di_joystick = InitJoystickInput(&di_devices[i],hwnd);
         joystick_device_index = i;
         break;
      }  
   }
         
   return true;
}

// Next functions for the change controls dialog box

void Kill_DI_change()
{
   SafeRelease(di_keyboard_c);
   SafeRelease(di_joystick_c);
   SafeRelease(DI_c);
}

bool Init_DI_change(HWND winHwnd)
{
   HRESULT hr;
   hr = DirectInputCreateEx(hinst, DIRECTINPUT_VERSION, IID_IDirectInput7, (void**)&DI_c, NULL); 
   if(FAILED(hr)) 
   { 
      return false;// DirectInput not available; take appropriate action 
   }
   
   hr = DI->CreateDeviceEx(GUID_SysKeyboard, IID_IDirectInputDevice7,(void**)&di_keyboard_c, NULL); 
   if(FAILED(hr)) 
   { 
      Kill_DI(); 
      return false; 
   } 
   
   hr = di_keyboard_c->SetDataFormat(&c_dfDIKeyboard); 
   if(FAILED(hr)) 
   { 
      Kill_DI(); 
      return false; 
   } 

   hr = di_keyboard_c->SetCooperativeLevel(winHwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE); 
   if(FAILED(hr)) 
   { 
      Kill_DI(); 
      return false; 
   } 
   
   if(di_keyboard_c) 
      hr = di_keyboard_c->Acquire();
      
   return true;
}

bool Init_DI_change_joy(HWND winHwnd)
{
   HRESULT hr;
   hr = DirectInputCreateEx(hinst, DIRECTINPUT_VERSION, IID_IDirectInput7, (void**)&DI_c, NULL); 
   if(FAILED(hr)) 
   { 
      return false;
   }
   
   if(joystick_device_index != -1)
      di_joystick_c = InitJoystickInput(&di_devices[joystick_device_index],winHwnd);
      
   return true;
}

//Check if a key has been pressed in the controls dialog
int check_change_keys(int index, int controller_nr)
{ 
   if(DI_c == NULL || di_keyboard_c == NULL)
      return 0;
       
   char buffer[256]; 
   HRESULT  hr; 
 
   hr = di_keyboard_c->GetDeviceState(sizeof(buffer),(LPVOID)&buffer); 
   if(FAILED(hr))
   { 
      di_keyboard_c->Acquire();
      return 0; 
   } 
    
   for(int i = 0; i < 0xee; ++i)
      if(KEYDOWN(buffer,i))
      {
         if(index < 12)
            options->multi_key_config[controller_nr][index] = i;
         else
            options->special_keys[index-12] = i;
         return 1;
      }          
   
   return 0;                   
}

int check_change_joypad(int index) 
{ 
   if(di_joystick_c == NULL)
      return 0;

   HRESULT hRes; 
   DIJOYSTATE js; 
 
   hRes = IDirectInputDevice7_Poll(di_joystick_c);

   hRes = IDirectInputDevice7_GetDeviceState(di_joystick_c, sizeof(DIJOYSTATE), &js); 

   if(hRes != DI_OK) 
   { 
      di_joystick_c->Acquire();
      return 0;
   } 

   for(int i=0;i<10;++i)
   {
      if(js.rgbButtons[i] & 0x80) 
      { 
         options->joystick_config[index] = i;
         return 1;
      }    
   }    
   
   return 0;                   
}
