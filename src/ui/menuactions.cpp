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

#include <fmod/fmod.h>
#include <stdio.h>
#include "menuactions.h"
#include "menu.h"
#include "../main.h"
#include "strings.h"
#include "../cpu.h"
#include "../directinput.h"
#include "../sound.h"
#include "../GB_gfx.h"
#include "dialogs.h"
#include "../devices.h"
#include "../SGB.h"
#include "../licnames.h"
#include "window.h"
#include "dialogs/MemorySearcher.h"

int romsize(int number)
{
    switch(number)
    {
        case 0: return 32;
        case 1: return 64;
        case 2: return 128;
        case 3: return 256;
        case 4: return 512;
        case 5: return 1024;
        case 6: return 2048;
        case 7: return 4096;
        case 8: return 8192;
        case 0x52: return 1126;
        case 0x53: return 1228;
        case 0x54: return 1536;
        default: return 32;
    }
}

void PrintROMinfo()
{
    wchar_t text[500];

    wchar_t nl[3]; // get the new licensee
    nl[0] = GB1->cartridge->header.newlic[0];
    nl[1] = GB1->cartridge->header.newlic[1];
    nl[2] = 0x0000;
    // I have no idea why but if you make nl[2] its not terminated or anything and you get garbage data after it

    wchar_t newname[16];
    mbstowcs(newname,GB1->cartridge->header.name,16);

    //sprintf(text, "Rom name: \t%s\nGBC feature: \t%s\nNew Licensee: \t%X\nSGB feature: \t%s\nCardridge Type: \t%X\nROM Size: \t%d KBytes\nRAM Size: \t%d KBytes\nCountry: \t\t%X ,%s\nLicensee: \t%X - %s\nVersion: \t\t%X\nComplement check: %X - %s\nChecksum: \t%X", rom->name, rom->CGB==1?"yes":rom->CGB==2?"GBC only":"no", rom->newlic, rom->SGB?"yes":"no", rom->carttype,romsize(rom->ROMsize),ramsize[rom->RAMsize], rom->destcode, rom->destcode==0?"Japan":"non-Japan",rom->lic,lic_names[rom->lic],rom->version,rom->complement,rom->complementok?"(ok)":"(wrong)",rom->checksum);
    wsprintf(text, str_table[ROM_INFO], newname, GB1->cartridge->header.CGB==1?str_table[STR_YES]:GB1->cartridge->header.CGB==2?str_table[GBC_ONLY]:str_table[STR_NO], nl, GB1->cartridge->header.SGB?str_table[STR_YES]:str_table[STR_NO], GB1->cartridge->header.carttype,romsize(GB1->cartridge->ROMsize),ramsize[GB1->cartridge->RAMsize], GB1->cartridge->header.destcode, GB1->cartridge->header.destcode==0?str_table[STR_JAPAN]:str_table[STR_NON_JAPAN],GB1->cartridge->header.lic,lic_names[GB1->cartridge->header.lic],GB1->cartridge->header.version,GB1->cartridge->header.complement,GB1->cartridge->header.complementok?str_table[CHECK_OK]:str_table[CHECK_WRONG],GB1->cartridge->header.checksum);
    debug_print(text);
}

void menuAction(int menuOption)
{
    switch( menuOption ) {

        case IDM_RECENTROM0:
        case IDM_RECENTROM1:
        case IDM_RECENTROM2:
        case IDM_RECENTROM3:
        case IDM_RECENTROM4:
        case IDM_RECENTROM5:
        case IDM_RECENTROM6:
        case IDM_RECENTROM7:
        case IDM_RECENTROM8:
        case IDM_RECENTROM9:
        case IDM_FILEOPEN:
        {
            bool romwasloaded = GB1->romloaded;

            if(GB1->romloaded && !GB1->write_save())
                debug_print(str_table[ERROR_SAVE_FILE_WRITE]);

            if ( menuOption == IDM_FILEOPEN ) {
                if(!DoFileOpen(hwnd,1))
                    debug_print(str_table[ERROR_ROM_LOAD]);
            } else { // its a Recent ROM
                if (options->recent_rom_names[menuOption - IDM_RECENTROM0] == L"")
                    break;

                gb1_loaded_file_name = options->recent_rom_names[menuOption - IDM_RECENTROM0];
                if (!GB1->load_rom(options->recent_rom_names[menuOption - IDM_RECENTROM0].c_str())) {
                    debug_print(str_table[ERROR_ROM_LOAD]);
                }
            }



            if(GB1->romloaded)
            {
                GB1->reset();

                if(!multiple_gb)
                {
                    if(sgb_mode)
                    {
                        setWinSize(256,224);

                        (renderer.*renderer.drawBorder)();
                    } else
                    {
                        setWinSize(160,144);
                    }
                }

                if(!GB1->load_save())
                    debug_print(str_table[ERROR_SAVE_FILE_READ]);

                if(multiple_gb)
                    swprintf(w_title_text,L"%s - %s --- %s",w_emu_title,GB1->rom_filename,GB2->rom_filename);
                else
                    swprintf(w_title_text,L"%s - %s",w_emu_title,GB1->rom_filename);

                SetWindowText(hwnd,w_title_text);

                if(!paused)
                    FSOUND_SetMute(FSOUND_ALL,FALSE);
            } else
            if(romwasloaded && GB1->cartROM != NULL)
            {
                GB1->romloaded = true;
                if(!paused)
                    FSOUND_SetMute(FSOUND_ALL,FALSE);
            }
        }
            break;

        case IDM_FILEOPEN2:
        {
            if(!GB1->romloaded)
            {
                debug_print("Load GB1 rom first.");
                break;
            }
            init_gb2();

            setWinSize(2*160,144);

            renderer.setRect(true);

            bool romwasloaded = GB2->romloaded;

            if(GB2->romloaded && !GB2->write_save())
                debug_print(str_table[ERROR_SAVE_FILE_WRITE]);

            if(!DoFileOpen(hwnd,2))
                debug_print(str_table[ERROR_ROM_LOAD]);

            if(GB2->romloaded)
            {
                GB2->reset();

                if(!GB2->load_save())
                    debug_print(str_table[ERROR_SAVE_FILE_READ]);

                swprintf(w_title_text,L"%s - %s --- %s",w_emu_title,GB1->rom_filename,GB2->rom_filename);
                SetWindowText(hwnd,w_title_text);

                if(!paused)
                    FSOUND_SetMute(FSOUND_ALL,FALSE);
            } else
            if(romwasloaded && GB2->cartROM != NULL)
            {
                GB2->romloaded = true;

                if(!paused)
                    FSOUND_SetMute(FSOUND_ALL,FALSE);
            }
        }
            break;

        case IDM_FILEOPENSAMEASGB1:
            if(GB1->romloaded)
            {
                init_gb2();

                setWinSize(2*160,144);

                renderer.setRect(true);

                if(GB2->romloaded && !GB2->write_save())
                    debug_print(str_table[ERROR_SAVE_FILE_WRITE]);

                GB2->load_rom(gb1_loaded_file_name.c_str());

                if(GB2->romloaded)
                {
                    GB2->reset();

                    if(!GB2->load_save())
                        debug_print(str_table[ERROR_SAVE_FILE_READ]);

                    swprintf(w_title_text,L"%s - %s --- %s",w_emu_title,GB1->rom_filename,GB2->rom_filename);
                    SetWindowText(hwnd,w_title_text);

                    if(!paused)
                        FSOUND_SetMute(FSOUND_ALL,FALSE);
                }
            }
            break;

        case IDM_FILECLOSE:
            FSOUND_SetMute(FSOUND_ALL,TRUE);
            SetWindowText(hwnd,w_emu_title);
            if(GB1->romloaded)
            {
                if(!GB1->write_save())
                    debug_print(str_table[ERROR_SAVE_FILE_WRITE]);
            }
            if(GB1->cartROM)
            {
                delete [] GB1->cartROM;
                GB1->cartROM = NULL;
            }
            GB1->romloaded = false;

            if(GB1->gfx_bit_count == 16)
            {
                memset(GB1->gfx_buffer,0,sizeof(WORD)*160*144);
                memset(GB1->gfx_buffer_old,0,sizeof(WORD)*160*144);
            } else
            {
                memset(GB1->gfx_buffer,0,sizeof(DWORD)*160*144);
                memset(GB1->gfx_buffer_old,0,sizeof(DWORD)*160*144);
            }
            (renderer.*renderer.drawScreen)();
            break;

        case IDM_LOADGB1SAVE:
            if(GB1 && GB1->romloaded && GB2 && GB2->romloaded)
            {
                GB2->reset();

                if(!GB2->load_save(true))
                    debug_print(str_table[ERROR_SAVE_FILE_READ]);
            }
            break;

        case IDM_FILECLOSE2:
            if(!GB2)
                break;

            if(!GB1->romloaded)
            {
                FSOUND_SetMute(FSOUND_ALL,TRUE);
                SetWindowText(hwnd,w_emu_title);
            }
            setWinSize(160,144);

            if(GB2->romloaded)
            {
                if(!GB2->write_save())
                    debug_print(str_table[ERROR_SAVE_FILE_WRITE]);

                swprintf(w_title_text,L"%s - %s",w_emu_title,GB1->rom_filename);
                SetWindowText(hwnd,w_title_text);
            }
            if(GB2->cartROM)
            {
                delete [] GB2->cartROM;
                GB2->cartROM = NULL;
            }
            GB2->romloaded = false;
            multiple_gb = 0;
            GB = GB1;

            renderer.setRect(false);

            break;

        case IDM_ROMINFO:
            if(GB1->romloaded)
                PrintROMinfo();
            break;

        case IDM_FOLDERS:
            DialogBox(hinst, MAKEINTRESOURCE(ID_F_DIALOG), hwnd, (DLGPROC)FolderProc);
            break;

        case IDM_CHEATS:
            DialogBox(hinst, MAKEINTRESOURCE(ID_CHT_DIALOG), hwnd, (DLGPROC)ChtProc);
            break;

        case IDM_MEMSEARCH:
            MemorySearcher::SpawnMemorySearcher();
            break;

        case IDM_FILEEXIT:
            SendMessage(hwnd, WM_CLOSE, 0, 0L);
            break;

        case IDM_CPUSOFTRESET:
            soft_reset = 1;
            break;

        case IDM_CPUSOFTRESET2:
            if(GB2)
                soft_reset = 2;
            break;

        case IDM_CPURESET:
            if(GB1->romloaded && !GB1->write_save())
                debug_print(str_table[ERROR_SAVE_FILE_WRITE]);

            GB1->reset();

            if(GB1->romloaded && !GB1->load_save())
                debug_print(str_table[ERROR_SAVE_FILE_READ]);
            break;

        case IDM_CPURESET2:
            if(!GB2)
                break;

            if(GB2->romloaded && !GB2->write_save())
                debug_print(str_table[ERROR_SAVE_FILE_WRITE]);

            GB2->reset();

            if(GB2->romloaded && !GB2->load_save())
                debug_print(str_table[ERROR_SAVE_FILE_READ]);
            break;

        case IDM_CPUPAUSE:
            if(menupause)
            {
                if(GB1->romloaded)
                    FSOUND_SetMute(FSOUND_ALL,FALSE);
                emuMenu.uncheckOption(IDM_CPUPAUSE);
            }
            else
            {
                FSOUND_SetMute(FSOUND_ALL,TRUE);
                emuMenu.checkOption(IDM_CPUPAUSE);
            }
            menupause=!menupause;
            paused=!paused;
            break;

        case IDM_CPUSTATELOAD:
            if(GB1->romloaded)
                GB1->load_state();
            break;

        case IDM_CPU2STATELOAD:
            if(GB2 && GB2->romloaded)
                GB2->load_state();
            break;

        case IDM_CPUSTATESAVE:
            if(GB1->romloaded)
                GB1->save_state();
            break;

        case IDM_CPU2STATESAVE:
            if(GB2 && GB2->romloaded)
                GB2->save_state();
            break;

            // Switch state slot for GB1
        case IDM_CPUSTATESLOT0:
        case IDM_CPUSTATESLOT1:
        case IDM_CPUSTATESLOT2:
        case IDM_CPUSTATESLOT3:
        case IDM_CPUSTATESLOT4:
        case IDM_CPUSTATESLOT5:
        case IDM_CPUSTATESLOT6:
        case IDM_CPUSTATESLOT7:
        case IDM_CPUSTATESLOT8:
        case IDM_CPUSTATESLOT9:
            GB1_state_slot = menuOption - IDM_CPUSTATESLOT0;
            emuMenu.checkOption(menuOption);
            break;

            // Switch state slot for GB2
        case IDM_CPU2STATESLOT0:
        case IDM_CPU2STATESLOT1:
        case IDM_CPU2STATESLOT2:
        case IDM_CPU2STATESLOT3:
        case IDM_CPU2STATESLOT4:
        case IDM_CPU2STATESLOT5:
        case IDM_CPU2STATESLOT6:
        case IDM_CPU2STATESLOT7:
        case IDM_CPU2STATESLOT8:
        case IDM_CPU2STATESLOT9:
            GB2_state_slot = menuOption - IDM_CPU2STATESLOT0;
            emuMenu.checkOption(menuOption);
            break;

            // Switch GB type for GB1
        case IDM_CPUAUTO:
        case IDM_CPUGBA:
        case IDM_CPUGB:
        case IDM_CPUGBP:
        case IDM_CPUGBC:
        case IDM_CPUSGB:
        case IDM_CPUSGB2: {
            int newSysType = menuOption - IDM_GROUP_GB1; // should be equal to the appropriate SYS_ constant
            if(GB1->system_type != newSysType) {
                GB1->system_type = newSysType;
                if(GB1->romloaded && !GB1->write_save())
                    debug_print(str_table[ERROR_SAVE_FILE_WRITE]);
                GB1->reset();
                if(GB1->romloaded && !GB1->load_save())
                    debug_print(str_table[ERROR_SAVE_FILE_READ]);
            }
            emuMenu.checkOption(menuOption);
            break;
        }

            // Switch GB type for GB2
        case IDM_CPUAUTO2:
        case IDM_CPUGBA2:
        case IDM_CPUGB2:
        case IDM_CPUGBP2:
        case IDM_CPUGBC2: {
            if(!GB2) {
                init_gb2();
                multiple_gb = 0;
            }
            int newSysType = menuOption - IDM_GROUP_GB2; // should be equal to the appropriate SYS_ constant
            if(GB2->system_type != newSysType) {
                GB2->system_type = newSysType;
                if(GB2->romloaded && !GB2->write_save())
                    debug_print(str_table[ERROR_SAVE_FILE_WRITE]);
                GB2->reset();
                if(GB2->romloaded && !GB2->load_save())
                    debug_print(str_table[ERROR_SAVE_FILE_READ]);
            }
            emuMenu.checkOption(menuOption);
            break;
        }

        case IDM_CPUGBCSGB:
            if(options->GBC_SGB_border == GBC_WITH_SGB_BORDER)
            {
                emuMenu.uncheckOption(IDM_CPUGBCSGB);
                options->GBC_SGB_border = OFF;
            } else
            {
                emuMenu.checkOption(IDM_CPUGBCSGB);
                options->GBC_SGB_border = GBC_WITH_SGB_BORDER;
            }
            break;
        case IDM_CPUGBCSGBI:
            if(options->GBC_SGB_border == GBC_WITH_INITIAL_SGB_BORDER)
            {
                emuMenu.uncheckOption(IDM_CPUGBCSGBI);
                options->GBC_SGB_border = OFF;
            } else
            {
                emuMenu.checkOption(IDM_CPUGBCSGBI);
                options->GBC_SGB_border = GBC_WITH_INITIAL_SGB_BORDER;
            }
            break;

        case IDM_UNLAUTO:
            options->unl_compat_mode = UNL_AUTO;
            emuMenu.checkOption(IDM_UNLAUTO);
            break;
        case IDM_UNLNONE:
            options->unl_compat_mode = UNL_NONE;
            emuMenu.checkOption(IDM_UNLNONE);
            break;
        case IDM_UNLNIUTOUDE:
            options->unl_compat_mode = UNL_NIUTOUDE;
            emuMenu.checkOption(IDM_UNLNIUTOUDE);
            break;
        case IDM_UNLSINTAX:
            options->unl_compat_mode = UNL_SINTAX;
            emuMenu.checkOption(IDM_UNLSINTAX);
            break;
        case IDM_UNLBBD:
            options->unl_compat_mode = UNL_BBD;
            emuMenu.checkOption(IDM_UNLBBD);
            break;
        case IDM_UNLHITEK:
            options->unl_compat_mode = UNL_HITEK;
            emuMenu.checkOption(IDM_UNLHITEK);
            break;
        case IDM_UNLLBMULTI:
            options->unl_compat_mode = UNL_LBMULTI;
            emuMenu.checkOption(IDM_UNLLBMULTI);
            break;
        case IDM_UNLNTOLD1:
            options->unl_compat_mode = UNL_NTOLD1;
            emuMenu.checkOption(IDM_UNLNTOLD1);
            break;
        case IDM_UNLNTOLD2:
            options->unl_compat_mode = UNL_NTOLD2;
            emuMenu.checkOption(IDM_UNLNTOLD2);
            break;
        case IDM_UNLNTNEW:
            options->unl_compat_mode = UNL_NTNEW;
            emuMenu.checkOption(IDM_UNLNTNEW);
            break;
        case IDM_UNLLIEBAO:
            options->unl_compat_mode = UNL_DBZTR;
            emuMenu.checkOption(IDM_UNLLIEBAO);
            break;
        case IDM_UNLPOKEJD:
            options->unl_compat_mode = UNL_POKEJD;
            emuMenu.checkOption(IDM_UNLPOKEJD);
            break;
        case IDM_UNLNEWGBHK:
            options->unl_compat_mode = UNL_NEWGBHK;
            emuMenu.checkOption(IDM_UNLNEWGBHK);
            break;
        case IDM_UNLGGB81:
            options->unl_compat_mode = UNL_GGB81;
            emuMenu.checkOption(IDM_UNLGGB81);
            break;
        case IDM_UNLVF001:
            options->unl_compat_mode = UNL_VF001;
            emuMenu.checkOption(IDM_UNLVF001);
            break;
        case IDM_UNLMBC1COMPAT:
            options->unl_compat_mode = UNL_MBC1SAVE;
            emuMenu.checkOption(IDM_UNLMBC1COMPAT);
            break;
        case IDM_UNLMBC3COMPAT:
            options->unl_compat_mode = UNL_MBC3SAVE;
            emuMenu.checkOption(IDM_UNLMBC3COMPAT);
            break;
        case IDM_UNLMBC5COMPAT:
            options->unl_compat_mode = UNL_MBC5SAVE;
            emuMenu.checkOption(IDM_UNLMBC5COMPAT);
            break;

        case IDM_CPUSPFS9:
            if(options->speedup_skip_9frames)
            {
                emuMenu.uncheckOption(IDM_CPUSPFS9);
                options->speedup_skip_9frames = false;
            } else
            {
                emuMenu.checkOption(IDM_CPUSPFS9);
                options->speedup_skip_9frames = true;
            }
            break;
        case IDM_CPUSPSNDOFF:
            if(options->speedup_sound_off)
            {
                emuMenu.uncheckOption(IDM_CPUSPSNDOFF);
                options->speedup_sound_off = false;
            } else
            {
                emuMenu.checkOption(IDM_CPUSPSNDOFF);
                options->speedup_sound_off = true;
            }
            break;
        case IDM_CPUSPFOFF:
            if(options->speedup_filter_off)
            {
                emuMenu.uncheckOption(IDM_CPUSPFOFF);
                options->speedup_filter_off = false;
            } else
            {
                emuMenu.checkOption(IDM_CPUSPFOFF);
                options->speedup_filter_off = true;
            }
            break;
        case IDM_CPUOPCODE:
            if(options->halt_on_unknown_opcode)
            {
                emuMenu.uncheckOption(IDM_CPUOPCODE);
                options->halt_on_unknown_opcode = false;
            } else
            {
                emuMenu.checkOption(IDM_CPUOPCODE);
                options->halt_on_unknown_opcode = true;
            }
            break;
        case IDM_OPTIONCPUUSAGE:
            if(options->reduce_cpu_usage)
            {
                emuMenu.uncheckOption(IDM_OPTIONCPUUSAGE);
                options->reduce_cpu_usage = false;
            } else
            {
                emuMenu.checkOption(IDM_OPTIONCPUUSAGE);
                options->reduce_cpu_usage = true;
            }
            break;
        case IDM_OPTIONBOOTSTRAP:
            if(options->use_bootstrap)
            {
                emuMenu.uncheckOption(IDM_OPTIONBOOTSTRAP);
                options->use_bootstrap = false;
            } else
            {
                emuMenu.checkOption(IDM_OPTIONBOOTSTRAP);
                options->use_bootstrap = true;
            }
            break;
        case IDM_OPTIONOPPOSITEDIRECTIONS:
            if(options->opposite_directions_allowed)
            {
                emuMenu.uncheckOption(IDM_OPTIONOPPOSITEDIRECTIONS);
                options->opposite_directions_allowed = false;
            } else
            {
                emuMenu.checkOption(IDM_OPTIONOPPOSITEDIRECTIONS);
                options->opposite_directions_allowed = true;
            }
            break;
        case IDM_DEVICENONE:
            connected_device = DEVICE_NONE;
            reset_devices();

            emuMenu.checkOption(IDM_DEVICENONE);
            break;
        case IDM_DEVICEBARCODE:
            connected_device = DEVICE_BARCODE;
            reset_devices();

            emuMenu.checkOption(IDM_DEVICEBARCODE);
            break;
        case IDM_OPTIONVIDEOSIZE1:
        {
            options->video_size = 1;
            if(!sgb_mode && !(options->GBC_SGB_border == GBC_WITH_INITIAL_SGB_BORDER && border_uploaded))
                setWinSize(160,144);
            else
            {
                setWinSize(256,224);
                (renderer.*renderer.drawBorder)();
            }

            if(multiple_gb)
                setWinSize(320,144);
        }
            break;

        case IDM_OPTIONVIDEOSIZE2:
        {
            options->video_size = 2;
            if(!sgb_mode && !(options->GBC_SGB_border == GBC_WITH_INITIAL_SGB_BORDER && border_uploaded))
                setWinSize(160,144);
            else
            {
                setWinSize(256,224);
                (renderer.*renderer.drawBorder)();
            }

            if(multiple_gb)
                setWinSize(320,144);
        }
            break;

        case IDM_OPTIONVIDEOSIZE3:
        {
            options->video_size = 3;
            if(!sgb_mode && !(options->GBC_SGB_border == GBC_WITH_INITIAL_SGB_BORDER && border_uploaded))
                setWinSize(160,144);
            else
            {
                setWinSize(256,224);
                (renderer.*renderer.drawBorder)();
            }

            if(multiple_gb)
                setWinSize(320,144);

        }
            break;

        case IDM_OPTIONVIDEOSIZE4:
        {
            options->video_size = 4;
            if(!sgb_mode && !(options->GBC_SGB_border == GBC_WITH_INITIAL_SGB_BORDER && border_uploaded))
                setWinSize(160,144);
            else
            {
                setWinSize(256,224);
                (renderer.*renderer.drawBorder)();
            }

            if(multiple_gb)
                setWinSize(320,144);

        }
            break;

        case IDM_OPTIONVIDEOBW:
            options->video_GB_color = BLACK_WHITE;
            emuMenu.checkOption(IDM_OPTIONVIDEOBW);

            if(!GB->gbc_mode || !GB->cartridge->header.CGB)
            {
                memcpy(GB->GBC_BGP,GBC_DMGBG_palettes[0],sizeof(unsigned int)*4);
                memcpy(GB->GBC_OBP,GBC_DMGOBJ0_palettes[0],sizeof(unsigned int)*4);
                memcpy(GB->GBC_OBP+4,GBC_DMGOBJ1_palettes[0],sizeof(unsigned int)*4);
            }
            break;

        case IDM_OPTIONVIDEOLCDBROWN:
            options->video_GB_color = LCD_BROWN;
            emuMenu.checkOption(IDM_OPTIONVIDEOLCDBROWN);

            if(!GB->gbc_mode || !GB->cartridge->header.CGB)
            {
                memcpy(GB->GBC_BGP,LCD_palette_brown,sizeof(unsigned int)*4);
                memcpy(GB->GBC_OBP,LCD_palette_brown,sizeof(unsigned int)*4);
                memcpy(GB->GBC_OBP+4,LCD_palette_brown,sizeof(unsigned int)*4);
            }
            break;

        case IDM_OPTIONVIDEOLCDGREEN:
            options->video_GB_color = LCD_GREEN;
            emuMenu.checkOption(IDM_OPTIONVIDEOLCDGREEN);

            if(!GB->gbc_mode || !GB->cartridge->header.CGB)
            {
                memcpy(GB->GBC_BGP,LCD_palette_green,sizeof(unsigned int)*4);
                memcpy(GB->GBC_OBP,LCD_palette_green,sizeof(unsigned int)*4);
                memcpy(GB->GBC_OBP+4,LCD_palette_green,sizeof(unsigned int)*4);
            }
            break;

        case IDM_OPTIONVIDEOGRAY:
            options->video_GB_color = GRAY;
            emuMenu.checkOption(IDM_OPTIONVIDEOGRAY);

            if(!GB->gbc_mode || !GB->cartridge->header.CGB)
            {
                memcpy(GB->GBC_BGP,GB_palette_gray,sizeof(unsigned int)*4);
                memcpy(GB->GBC_OBP,GB_palette_gray,sizeof(unsigned int)*4);
                memcpy(GB->GBC_OBP+4,GB_palette_gray,sizeof(unsigned int)*4);
            }
            break;

        case IDM_VIDEOFILTERNONE:
            if(options->video_filter != VIDEO_FILTER_NONE)
            {
                emuMenu.checkOption(IDM_VIDEOFILTERNONE);
                options->video_filter = VIDEO_FILTER_NONE;
                renderer.setGameboyFilter(VIDEO_FILTER_NONE);
            }
            break;
        case IDM_VIDEOFILTERSOFT2X:
            if(options->video_filter != VIDEO_FILTER_SOFT2X)
            {
                emuMenu.checkOption(IDM_VIDEOFILTERSOFT2X);
                options->video_filter = VIDEO_FILTER_SOFT2X;
                renderer.setGameboyFilter(VIDEO_FILTER_SOFT2X);
            }
            break;
        case IDM_VIDEOFILTERSOFTXX:
            if(options->video_filter != VIDEO_FILTER_SOFTXX)
            {
                emuMenu.checkOption(IDM_VIDEOFILTERSOFTXX);
                options->video_filter = VIDEO_FILTER_SOFTXX;
                renderer.setGameboyFilter(VIDEO_FILTER_SOFTXX);
            }
            break;
        case IDM_VIDEOFILTERSCALE2X:
            if(options->video_filter != VIDEO_FILTER_SCALE2X)
            {
                emuMenu.checkOption(IDM_VIDEOFILTERSCALE2X);
                options->video_filter = VIDEO_FILTER_SCALE2X;
                renderer.setGameboyFilter(VIDEO_FILTER_SCALE2X);
            }
            break;
        case IDM_VIDEOFILTERSCALE3X:
            if(options->video_filter != VIDEO_FILTER_SCALE3X)
            {
                emuMenu.checkOption(IDM_VIDEOFILTERSCALE3X);
                options->video_filter = VIDEO_FILTER_SCALE3X;
                renderer.setGameboyFilter(VIDEO_FILTER_SCALE3X);
            }
            break;
        case IDM_VIDEOFILTERBLUR:
            if(options->video_filter != VIDEO_FILTER_BLUR)
            {
                emuMenu.checkOption(IDM_VIDEOFILTERBLUR);
                options->video_filter = VIDEO_FILTER_BLUR;
                renderer.setGameboyFilter(VIDEO_FILTER_BLUR);
            }
            break;
        case IDM_VIDEOFILTERUSEMMX:
            if(options->video_filter_use_mmx)
            {
                emuMenu.uncheckOption(IDM_VIDEOFILTERUSEMMX);
                options->video_filter_use_mmx = false;
            } else
            {
                emuMenu.checkOption(IDM_VIDEOFILTERUSEMMX);
                options->video_filter_use_mmx = true;
            }
            break;

        case IDM_VIDEOFILTERBORDERNONE:
            if(options->video_SGBborder_filter != VIDEO_FILTER_NONE)
            {
                emuMenu.checkOption(IDM_VIDEOFILTERBORDERNONE);
                options->video_SGBborder_filter = VIDEO_FILTER_NONE;
                renderer.setBorderFilter(VIDEO_FILTER_NONE);
            }
            break;
        case IDM_VIDEOFILTERBORDERSOFT2X:
            if(options->video_SGBborder_filter != VIDEO_FILTER_SOFT2X)
            {
                emuMenu.checkOption(IDM_VIDEOFILTERBORDERSOFT2X);
                options->video_SGBborder_filter = VIDEO_FILTER_SOFT2X;
                renderer.setBorderFilter(VIDEO_FILTER_SOFT2X);
            }
            break;
        case IDM_VIDEOFILTERBORDERSOFTXX:
            if(options->video_SGBborder_filter != VIDEO_FILTER_SOFTXX)
            {
                emuMenu.checkOption(IDM_VIDEOFILTERBORDERSOFTXX);
                options->video_SGBborder_filter = VIDEO_FILTER_SOFTXX;
                renderer.setBorderFilter(VIDEO_FILTER_SOFTXX);
            }
            break;
        case IDM_VIDEOFILTERBORDERSCALE2X:
            if(options->video_SGBborder_filter != VIDEO_FILTER_SCALE2X)
            {
                emuMenu.checkOption(IDM_VIDEOFILTERBORDERSCALE2X);
                options->video_SGBborder_filter = VIDEO_FILTER_SCALE2X;
                renderer.setBorderFilter(VIDEO_FILTER_SCALE2X);
            }
            break;
        case IDM_VIDEOFILTERBORDERSCALE3X:
            if(options->video_SGBborder_filter != VIDEO_FILTER_SCALE3X)
            {
                emuMenu.checkOption(IDM_VIDEOFILTERBORDERSCALE3X);
                options->video_SGBborder_filter = VIDEO_FILTER_SCALE3X;
                renderer.setBorderFilter(VIDEO_FILTER_SCALE3X);
            }
            break;
        case IDM_VIDEOFILTERBORDERBLUR:
            if(options->video_SGBborder_filter != VIDEO_FILTER_BLUR)
            {
                emuMenu.checkOption(IDM_VIDEOFILTERBORDERBLUR);
                options->video_SGBborder_filter = VIDEO_FILTER_BLUR;
                renderer.setBorderFilter(VIDEO_FILTER_BLUR);
            }
            break;

        case IDM_OPTIONVIDEOMIXOFF:
            emuMenu.checkOption(IDM_OPTIONVIDEOMIXOFF);

            options->video_mix_frames = MIX_FRAMES_OFF;
            renderer.setDrawMode(false);
            break;

        case IDM_OPTIONVIDEOMIXON:
            emuMenu.checkOption(IDM_OPTIONVIDEOMIXON);

            options->video_mix_frames = MIX_FRAMES_ON;
            renderer.setDrawMode(true);
            break;

        case IDM_OPTIONVIDEOMIXMORE:
            emuMenu.checkOption(IDM_OPTIONVIDEOMIXMORE);

            options->video_mix_frames = MIX_FRAMES_MORE;
            renderer.setDrawMode(true);
            break;

        case IDM_OPTIONVIDEOFSAUTO:
            if(options->video_auto_frameskip)
            {
                options->video_auto_frameskip = false;
                emuMenu.uncheckOption(IDM_OPTIONVIDEOFSAUTO);
            } else
            {
                options->video_auto_frameskip = true;
                emuMenu.checkOption(IDM_OPTIONVIDEOFSAUTO);
            }
            break;
        case IDM_OPTIONVIDEOFS0:
            options->video_frameskip = 0;
            emuMenu.checkOption(IDM_OPTIONVIDEOFS0+options->video_frameskip);
            break;
        case IDM_OPTIONVIDEOFS1:
            options->video_frameskip = 1;
            emuMenu.checkOption(IDM_OPTIONVIDEOFS0+options->video_frameskip);
            break;
        case IDM_OPTIONVIDEOFS2:
            options->video_frameskip = 2;
            emuMenu.checkOption(IDM_OPTIONVIDEOFS0+options->video_frameskip);
            break;
        case IDM_OPTIONVIDEOFS3:
            options->video_frameskip = 3;
            emuMenu.checkOption(IDM_OPTIONVIDEOFS0+options->video_frameskip);
            break;
        case IDM_OPTIONVIDEOFS4:
            options->video_frameskip = 4;
            emuMenu.checkOption(IDM_OPTIONVIDEOFS0+options->video_frameskip);
            break;
        case IDM_OPTIONVIDEOFS5:
            options->video_frameskip = 5;
            emuMenu.checkOption(IDM_OPTIONVIDEOFS0+options->video_frameskip);
            break;
        case IDM_OPTIONVIDEOFS6:
            options->video_frameskip = 6;
            emuMenu.checkOption(IDM_OPTIONVIDEOFS0+options->video_frameskip);
            break;
        case IDM_OPTIONVIDEOFS7:
            options->video_frameskip = 7;
            emuMenu.checkOption(IDM_OPTIONVIDEOFS0+options->video_frameskip);
            break;
        case IDM_OPTIONVIDEOFS8:
            options->video_frameskip = 8;
            emuMenu.checkOption(IDM_OPTIONVIDEOFS0+options->video_frameskip);
            break;
        case IDM_OPTIONVIDEOFS9:
            options->video_frameskip = 9;
            emuMenu.checkOption(IDM_OPTIONVIDEOFS0+options->video_frameskip);
            break;
        case IDM_OPTIONVIDEOENBG:
            if(video_enable&VID_EN_BG)
            {
                emuMenu.uncheckOption(IDM_OPTIONVIDEOENBG);
                video_enable &= ~VID_EN_BG;
            } else
            {
                emuMenu.checkOption(IDM_OPTIONVIDEOENBG);
                video_enable |= VID_EN_BG;
            }
            break;
        case IDM_OPTIONVIDEOENWIN:
            if(video_enable&VID_EN_WIN)
            {
                emuMenu.uncheckOption(IDM_OPTIONVIDEOENWIN);
                video_enable &= ~VID_EN_WIN;
            } else
            {
                emuMenu.checkOption(IDM_OPTIONVIDEOENWIN);
                video_enable |= VID_EN_WIN;
            }
            break;
        case IDM_OPTIONVIDEOENSPRITE:
            if(video_enable&VID_EN_SPRITE)
            {
                emuMenu.uncheckOption(IDM_OPTIONVIDEOENSPRITE);
                video_enable &= ~VID_EN_SPRITE;
            } else
            {
                emuMenu.checkOption(IDM_OPTIONVIDEOENSPRITE);
                video_enable |= VID_EN_SPRITE;
            }
            break;
        case IDM_OPTIONVIDEOSPRLIM:
            if(options->video_sprite_limit)
            {
                emuMenu.uncheckOption(IDM_OPTIONVIDEOSPRLIM);
                options->video_sprite_limit = false;
            } else
            {
                emuMenu.checkOption(IDM_OPTIONVIDEOSPRLIM);
                options->video_sprite_limit = true;
            }
            break;
        case IDM_OPTIONVIDEOLCDOFF:
            if(options->video_LCDoff_clear_screen)
            {
                emuMenu.uncheckOption(IDM_OPTIONVIDEOLCDOFF);
                options->video_LCDoff_clear_screen = false;
            } else
            {
                emuMenu.checkOption(IDM_OPTIONVIDEOLCDOFF);
                options->video_LCDoff_clear_screen = true;
            }
            break;
        case IDM_OPTIONVIDEOGBCCOLORS:
            if(options->video_GBCBGA_real_colors)
            {
                emuMenu.uncheckOption(IDM_OPTIONVIDEOGBCCOLORS);
                options->video_GBCBGA_real_colors = false;

                palette.mixGbcColours();
            } else
            {
                emuMenu.checkOption(IDM_OPTIONVIDEOGBCCOLORS);
                options->video_GBCBGA_real_colors = true;

                if(GB1->romloaded && GB1->gbc_mode)
                    palette.mixGbcColours();
            }
            break;
        case IDM_OPTIONVIDEOVISUALRUMBLE:
            if(options->video_visual_rumble)
            {
                emuMenu.uncheckOption(IDM_OPTIONVIDEOVISUALRUMBLE);
                options->video_visual_rumble = false;
            } else
            {
                emuMenu.checkOption(IDM_OPTIONVIDEOVISUALRUMBLE);
                options->video_visual_rumble = true;
            }
            break;
        case IDM_OPTIONCONTROL1:
            current_controller = 0;
            hwndCtrl = CreateDialog(hinst, MAKEINTRESOURCE(ID_CTRL_DIALOG), hwnd, (DLGPROC) CtrlProc);
            ShowWindow(hwndCtrl, SW_SHOW);
            SetFocus(hwndCtrl);

            Init_DI_change(hwndCtrl);
            break;
        case IDM_OPTIONCONTROL2:
            current_controller = 1;
            hwndCtrl = CreateDialog(hinst, MAKEINTRESOURCE(ID_CTRLMULTI_DIALOG), hwnd, (DLGPROC) MultiCtrlProc);
            ShowWindow(hwndCtrl, SW_SHOW);
            SetFocus(hwndCtrl);

            Init_DI_change(hwndCtrl);
            break;
        case IDM_OPTIONCONTROL3:
            current_controller = 2;
            hwndCtrl = CreateDialog(hinst, MAKEINTRESOURCE(ID_CTRLMULTI_DIALOG), hwnd, (DLGPROC) MultiCtrlProc);
            ShowWindow(hwndCtrl, SW_SHOW);
            SetFocus(hwndCtrl);

            Init_DI_change(hwndCtrl);
            break;
        case IDM_OPTIONCONTROL4:
            current_controller = 3;
            hwndCtrl = CreateDialog(hinst, MAKEINTRESOURCE(ID_CTRLMULTI_DIALOG), hwnd, (DLGPROC) MultiCtrlProc);
            ShowWindow(hwndCtrl, SW_SHOW);
            SetFocus(hwndCtrl);

            Init_DI_change(hwndCtrl);
            break;
        case IDM_OPTIONSENSOR:
            hwndCtrl = CreateDialog(hinst, MAKEINTRESOURCE(ID_SENSOR_DIALOG), hwnd, (DLGPROC) SensorProc);
            ShowWindow(hwndCtrl, SW_SHOW);
            SetFocus(hwndCtrl);

            Init_DI_change(hwndCtrl);
            break;
        case IDM_OPTIONCONTROLJOYCH:
            hwndCtrl = CreateDialog(hinst, MAKEINTRESOURCE(ID_CTRLJOY_DIALOG), hwnd, (DLGPROC) JoyCtrlProc);
            ShowWindow(hwndCtrl, SW_SHOW);
            SetFocus(hwndCtrl);

            Init_DI_change_joy(hwndCtrl);
            break;
        case IDM_OPTIONCONTROLJOY1:
            if(options->use_joystick_input == 0)
            {
                options->use_joystick_input = -1;
                emuMenu.uncheckOption(IDM_OPTIONCONTROLJOY1);
            } else
            {
                options->use_joystick_input = 0;
                emuMenu.checkOption(IDM_OPTIONCONTROLJOY1);
            }
            break;
        case IDM_OPTIONCONTROLJOY2:
            if(options->use_joystick_input == 1)
            {
                options->use_joystick_input = -1;
                emuMenu.uncheckOption(IDM_OPTIONCONTROLJOY2);
            } else
            {
                options->use_joystick_input = 1;
                emuMenu.checkOption(IDM_OPTIONCONTROLJOY2);
            }
            break;
        case IDM_OPTIONCONTROLJOY3:
            if(options->use_joystick_input == 2)
            {
                options->use_joystick_input = -1;
                emuMenu.uncheckOption(IDM_OPTIONCONTROLJOY3);
            } else
            {
                options->use_joystick_input = 2;
                emuMenu.checkOption(IDM_OPTIONCONTROLJOY3);
            }
            break;
        case IDM_OPTIONCONTROLJOY4:
            if(options->use_joystick_input == 3)
            {
                options->use_joystick_input = -1;
                emuMenu.uncheckOption(IDM_OPTIONCONTROLJOY4);
            } else
            {
                options->use_joystick_input = 3;
                emuMenu.checkOption(IDM_OPTIONCONTROLJOY4);
            }
            break;

        case IDM_OPTIONCONTROL_AUTOFIRE_SPEED_FASTEST:
            emuMenu.checkOption(IDM_OPTIONCONTROL_AUTOFIRE_SPEED_FASTEST);
            options->autofire_speed = AUTOFIRE_DELAY_FASTEST;
            break;

        case IDM_OPTIONCONTROL_AUTOFIRE_SPEED_FAST:
            emuMenu.checkOption(IDM_OPTIONCONTROL_AUTOFIRE_SPEED_FAST);
            options->autofire_speed = AUTOFIRE_DELAY_FAST;
            break;

        case IDM_OPTIONCONTROL_AUTOFIRE_SPEED_MEDIUM:
            emuMenu.checkOption(IDM_OPTIONCONTROL_AUTOFIRE_SPEED_MEDIUM);
            options->autofire_speed = AUTOFIRE_DELAY_MEDIUM;
            break;

        case IDM_OPTIONCONTROL_AUTOFIRE_SPEED_SLOW:
            emuMenu.checkOption(IDM_OPTIONCONTROL_AUTOFIRE_SPEED_SLOW);
            options->autofire_speed = AUTOFIRE_DELAY_SLOW;
            break;

        case IDM_OPTIONSOUND:
            if(options->sound_on == -1)
                break;
            if(options->sound_on)
            {
                FSOUND_StopSound(channel_n);
                emuMenu.uncheckOption(IDM_OPTIONSOUND);
                options->sound_on = 0;
            }
            else
            {
                channel_n = FSOUND_PlaySound(FSOUND_FREE,FSbuffer);
                emuMenu.checkOption(IDM_OPTIONSOUND);
                options->sound_on = 1;
            }
            break;
        case IDM_OPTIONSOUNDENCH1:
            if(sound_enable&SND_EN_CH1)
            {
                emuMenu.uncheckOption(IDM_OPTIONSOUNDENCH1);
                sound_enable &= ~SND_EN_CH1;
            } else
            {
                emuMenu.checkOption(IDM_OPTIONSOUNDENCH1);
                sound_enable |= SND_EN_CH1;
            }
            break;
        case IDM_OPTIONSOUNDENCH2:
            if(sound_enable&SND_EN_CH2)
            {
                emuMenu.uncheckOption(IDM_OPTIONSOUNDENCH2);
                sound_enable &= ~SND_EN_CH2;
            } else
            {
                emuMenu.checkOption(IDM_OPTIONSOUNDENCH2);
                sound_enable |= SND_EN_CH2;
            }
            break;
        case IDM_OPTIONSOUNDENCH4:
            if(sound_enable&SND_EN_CH4)
            {
                emuMenu.uncheckOption(IDM_OPTIONSOUNDENCH4);
                sound_enable &= ~SND_EN_CH4;
            } else
            {
                emuMenu.checkOption(IDM_OPTIONSOUNDENCH4);
                sound_enable |= SND_EN_CH4;
            }
            break;
        case IDM_OPTIONSOUNDENCH3:
            if(sound_enable&SND_EN_CH3)
            {
                emuMenu.uncheckOption(IDM_OPTIONSOUNDENCH3);
                sound_enable &= ~SND_EN_CH3;
            } else
            {
                emuMenu.checkOption(IDM_OPTIONSOUNDENCH3);
                sound_enable |= SND_EN_CH3;
            }
            break;
        case IDM_OPTIONSOUNDLOWPASSNONE:
            options->sound_lowpass_filter = 0;
            emuMenu.checkOption(IDM_OPTIONSOUNDLOWPASSNONE);
            break;
        case IDM_OPTIONSOUNDLOWPASS1:
            options->sound_lowpass_filter = LOWPASS_LEVEL1;
            emuMenu.checkOption(IDM_OPTIONSOUNDLOWPASS1);
            break;
        case IDM_OPTIONSOUNDLOWPASS2:
            options->sound_lowpass_filter = LOWPASS_LEVEL2;
            emuMenu.checkOption(IDM_OPTIONSOUNDLOWPASS2);
            break;
        case IDM_OPTIONSOUNDRSTEREO:
            options->sound_reverse_stereo = !options->sound_reverse_stereo;
            if (options->sound_reverse_stereo)
                emuMenu.checkOption(IDM_OPTIONSOUNDRSTEREO);
            else
                emuMenu.uncheckOption(IDM_OPTIONSOUNDRSTEREO);
            break;
        case IDM_OPTIONSOUNDVOL025:
            options->sound_volume = VOLUME_025X;
            emuMenu.checkOption(IDM_OPTIONSOUNDVOL025);
            break;
        case IDM_OPTIONSOUNDVOL05:
            options->sound_volume = VOLUME_05X;
            emuMenu.checkOption(IDM_OPTIONSOUNDVOL05);
            break;
        case IDM_OPTIONSOUNDVOL1:
            options->sound_volume = VOLUME_1X;
            emuMenu.checkOption(IDM_OPTIONSOUNDVOL1);
            break;
        case IDM_OPTIONSOUNDVOL2:
            options->sound_volume = VOLUME_2X;
            emuMenu.checkOption(IDM_OPTIONSOUNDVOL2);
            break;
        case IDM_OPTIONSOUNDVOL3:
            options->sound_volume = VOLUME_3X;
            emuMenu.checkOption(IDM_OPTIONSOUNDVOL3);
            break;
        case IDM_OPTIONSOUNDVOL4:
            options->sound_volume = VOLUME_4X;
            emuMenu.checkOption(IDM_OPTIONSOUNDVOL4);
            break;
        case IDM_HELPABOUT:
            wchar_t about_str[200];
            wsprintf(about_str,str_table[ABOUT_STR],prg_version);
            MessageBoxW(NULL, about_str, w_emu_title, 0);
            break;

    }
}
