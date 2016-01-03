hhugboy v1.0.5
a Game Boy/Game Boy Color emulator for Windows
by taizou (cactusgunman@gmail.com)
http://hhug.me/
http://bitbucket.org/taizou/hhugboy

 hhugboy is based on GEST v1.1.1 by TM (http://koti.mbnet.fi/gest_emu/)
 with the following additions:
  * Unicode filename support!
  * Unlicensed Mapper support! (Sintax & Li Cheng)
  * Screenshots!
  * Recent ROM list
  * Support for simple cheat format
  * Nearest neighbour scaling up to 8x for super sharpness
  * New onscreen text font, PCPaint Bold by codeman38
  * (Visual) rumble always on for Makon games
  * Bunch of minor tweaks/fixes

 Requires DirectX runtime 7+

[ UNLICENSED COMPATIBILITY MODES ] - - - - - - - - - - - - - - - - - - - - - - -

 Auto      Will attempt to automatically determine which mode to use based on
           the boot logo present in the ROM. 
           "Niutoude" -> Li Cheng mode
           part-inverted Nintendo (looks like "Kwichvu") -> Sintax mode
           This is not always a 100% reliable indicator though! 
           e.g. some Li Cheng games do not have the "Niutoude" logo, and Sintax
           games hacked to work on generic mappers still have the weird logo.
           In these cases you will have to select the mode manually.

 Li Cheng  For games released by Li Cheng, also known as Xing Xing, Niutoude,
           Winsen or Yongsheng.

 Sintax    For games by Sintax. Please note this mode will definitely not work
           for anything other than a raw Sintax dump from a game using their
           own mapper! If the game is hacked to work on other emulators or a
           generic release from another company, it won't work.

 Disabled  No compatibility mode

[ KEYS ] - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

 Default Gameboy Controls  Up         Up
                           Down       Down
                           Left       Left
                           Right      Right
                           Z          A
                           X          B
                           A          Turbo A
                           S          Turbo B
                           Q          L (stretch in GBA mode)
                           W          R (unstretch in GBA mode)
                           Enter      Start
                           Backspace  Select
                           Numpad     Tilt sensor controls

 Emulator Controls         Esc        Exit
                           Ctrl+P     Pause
                           Ctrl+R     Hard Reset
                           Ctrl+F     Soft Reset
                           Spacebar   Speedup
                           F2         Save state
                           F3         Change state slot
                           F4         Load state
                           F5         Toggle BG layer
                           F6         Toggle WIN layer
                           F7         Toggle sprite layer
                           F12        Screenshot GB1
                           Ctrl+F12   Screenshot GB2
                           0~9, /, =  Change GBC palette for mono games

[ CHEATS ] - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

 Cheats are supported in two formats:
 
 Game Genie     xxx-xxx-xxx or xxx-xxx
                Same as the Game Genie cheat device. 
                Codes and documentation undoubtedly available online somewhere
 
 Address/Value  xxxx=yy
                xxxx = address, yy = value, in hexadecimal. e.g. 1234=1F

[ ISSUES ] - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

 * Loading from Zip fails with Unicode chars anywhere in the filename or path.
   (However it still works if the file inside the zip is named unicodely)
 * Unicode filenames will not be preserved for screenshots
 * Problems with Window layer in Sanguozhi 5 prevents dialogue boxes being
   displayed. (This was an existing issue in GEST, not the mapper emulation)
 * Compatibility mode selection does not autosave. May/may not be beneficial
 * Saved games will probably be lost if you load the same game in different
   compatibility modes
 * You can't choose the screenshot directory
 * Pirate mapper autodetection is not 100%, you may have to manually select
   for some games.
 * 8x nearest scaling is slow, if you have speed issues you may want to use
   another mode
 * Emu freezes unrecoverably if you press Ctrl+Alt+Del, standby or lock the PC 
 * (Visual) rumble only works for the first Makon game you load

[ FUTURE DEVELOPMENTS? ] - - - - - - - - - - - - - - - - - - - - - - - - - - - -

 If I ever have the time/skill/motivation

 * Action Replay cheats
 * Implement a rendering engine other than DirectDraw
 * Other language translations

[ RELEASE HISTORY ]- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

 v1.0.5 3rd January 2016
 * Adds support for simple address=value hex cheat format
 * Speed up nearest neighbour filtering a bit
 * Option to disable filters on speedup (now enabled by default)
 * Fix filterless rendering in 16-bit colour mode
 * A bunch of code refactoring to facilitate future changes - should have no
   effect from a user's perspective!

 v1.0 18th February 2013
 * First public release

[ COPYRIGHT/LICENSE ]- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

 hhugboy Game Boy emulator
 copyright 2013 taizou

 Based on GEST
 Copyright (C) 2003-2010 TM
 Incorporates code from VisualBoyAdvance
 Copyright (C) 1999-2004 by Forgotten

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

[ SPECIAL THANKS! ]- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

 PCPaint Bold font by codeman38 (http://www.zone38.net/)
 Beta testers - MLX & RobGBA
