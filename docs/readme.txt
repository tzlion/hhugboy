hhugboy v1.3.0++dev
a Game Boy/Game Boy Color emulator for Windows
by taizou (cactusgunman@gmail.com)
http://hhug.me/
https://github.com/tzlion/hhugboy

 hhugboy is based on GEST v1.1.1 by TM (http://koti.mbnet.fi/gest_emu/)
 with the following additions:
  * Unicode filename support!
  * Unlicensed Mapper support! (Sintax, Li Cheng, BBD, Hitek, more!)
  * Support for Vast Fame and some other multicarts!
  * GBX ROM format support!
  * Bootstrap ROM support!
  * Screenshots!
  * Memory searching
  * Recent ROM list
  * Support for simple cheat format
  * Nearest neighbour scaling up to 8x for super sharpness
  * New onscreen text font, PCPaint Bold by codeman38
  * Bunch of minor tweaks/fixes

 Requires DirectX runtime 7+

[ UNLICENSED COMPATIBILITY MODES ] - - - - - - - - - - - - - - - - - - - - - - -

 Auto      For unlicensed publishers with unique boot logos, the emulator will
           attempt to automatically determine which mapper to use based on the
           logo present in the ROM.
           * "Niutoude" -> Li Cheng
           * part-inverted Nintendo (looks like "Kwichvu") -> Sintax
           * "GK.RX" -> Hitek
           * "Rocket", "Smartcom" -> Rocket Games
           This is not always a 100% reliable indicator though! 
           e.g. some Li Cheng games do not have the "Niutoude" logo.
           If a game doesn't work, try selecting the mode manually.
           Sachen and Wisdom Tree games will also be detected by other means.

           For other games and multicarts, the detection is currently done
           based on the ROM title and size, meaning only certain predefined
           carts can be autodetected for now:
              - Vast Fame 12-in-1
              - Vast Fame 18-in-1
              - SL 36-in-1 with Chinese Pokemon Gold & Silver
              - 23-in-1 CR2011 with Super Mario Special 3
              - 25-in-1 NT-9920 with Rockman 8
              - 24-in-1 CY2060 with Donkey Kong 5, Sonic 8
              - 24-in-1 CY2061/CR2020 with Rockman X4, Sonic 8
              - Super Mario Special 3
              - Super Donkey Kong 5
              - Rockman 8
              - Rockman X4
              - Sonic Adventure 8

 Li Cheng  For games released by Li Cheng, also known as Xing Xing, Niutoude,
           Winsen or Yongsheng.

 Sintax    For games by Sintax. Please note this mode will definitely not work
           for anything other than a raw Sintax dump from a game using their
           own mapper! If the game is hacked to work on other emulators or a
           generic release from another company, it won't work.
           (However these roms SHOULD now be picked up by Auto mode)

 BBD       For games by BBD using their own mapper. Support isn't complete yet
           but works with known dumps.

 Hitek     For games by Hitek.

 NT old type 1/2
           Earlier carts released by NT, including older Makon games and some
           multicarts.
           Type 1: Rockman 8
           Type 2: Super Mario 3, Donkey Kong 5, rumble carts

 NT new    Later carts released by NT, specifically Makon single carts.
           No multicart support yet.

 Liebao Technology
           Currently only known to support 'Dragon Ball Z 3', the pirate
           English translation of DBZ Goku Gekitouden. Other games may use this
           too, especially Chinese translations.

 Pokémon Jade/Diamond
           For the original protected versions of the Telefang translations.
           Currently only verified working with Jade.
           (Not necessary for most common dumps which are hacks/reprints)

 New GB Color Pokémon hacks
           Protection found on carts released under the "New GB Color" brand
           with HKxxxx PCBs. Known games are both Pokémon platformer hacks:
           'Pokémon Action Chapter', 'Monster Go! Go! II' (translated names)

 Vast Fame secondary releases
           Certain mainland China and English versions of Vast Fame games.
           Known so far: 'Harvest Moon 6', 'Lord of the Rings Legend',
           'Digimon Pocket' (Chinese), 'Digimon Sapphire' (English).
           The Chinese releases have DSHGGB-81 PCB code.

 'Last Bible' multicart
           A type of multicart using menu music taken from 'Megami Tensei
           Gaiden: Last Bible' aka 'Revelations: The Demon Slayer'.
           This notably covers all of Vast Fame's multicarts, in addition to a
           number of more generic carts.

 MBC1/3/5 compatible
           For unknown carts that fake their header value but are actually
           compatible with common mappers.

 Disabled  No compatibility mode

 * If a GBX format ROM is loaded, this setting will be ignored, and the mapper
   specified in the GBX footer will be used instead.

[ KEYS ] - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

 Default Gameboy Controls  Up         Up
                           Down       Down
                           Left       Left
                           Right      Right
                           X          A
                           Z          B
                           S          Turbo A
                           A          Turbo B
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

 The Memory Search functionality can assist in finding memory addresses to use
 with the address/value format. It allows you to search for specific values in
 memory, and by checking the "in previous result set" option, you can then
 search only in the results of the previous search, allowing you to narrow down
 memory locations that may contain something of interest (lives, time, etc).

 * Note this currently has the limitation that you have to know the *exact*
   value you are searching for- it doesn't allow you to do "greater than/
   less than" type searches like some other emulators.

[ COMPATIBILITY ]- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

 This emulator was primarily designed for Windows, but it will also run on
 Linux using WINE. However it has not been extensively tested in this
 environment - your mileage may vary. 

[ ISSUES ] - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

 * Loading from Zip fails with Unicode chars anywhere in the filename or path.
   (However it still works if the file inside the zip is named unicodely)
 * Unicode filenames will not be preserved for screenshots
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
 * On a multicart, loading a savestate from a mono game while playing a colour
   game (including the menu) will not work properly

[ FUTURE DEVELOPMENTS? ] - - - - - - - - - - - - - - - - - - - - - - - - - - - -

 If I ever have the time/skill/motivation

 * Action Replay cheats
 * Implement a rendering engine other than DirectDraw
 * Other language translations

[ RELEASE HISTORY ]- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

 vx.x.x xxth xxxx 2021
 * New mapper support: New GB Color HKxxxx PCB (used for Pokémon hacks),
   Vast Fame secondary releases on GGB-81 PCB (and others)
 * Allow undersized save files to be read (to avoid saves being wiped when
   an "unlicensed compatibility mode" change results in a RAM size change)

 v1.3.0 29th October 2020
 * New mapper support: Sachen MMC1, Sachen MMC2, Mani M161, Rocket Games,
   Wisdom Tree (NewRisingSun)
 * MMM01 mapper rewritten & implemented properly (NewRisingSun)
 * Support bootstrap ROMs named cgb_boot.bin or dmg_boot.bin (NewRisingSun)
 * Fix certain Unicode filenames corrupting config file (NewRisingSun)
   Note this changes the encoding of file paths, so if you are using a config
   file from an older version, you will have to redefine them
 * Fix rendering issues causing wrong colours in 32-bit rendering and 16-bit
   when using colour mixing (NewRisingSun)
 * Fix font file handle being kept open (NewRisingSun)
 * Reverse default A and B buttons to X and Z to reflect real console order
   (default turbo A and B buttons are also reversed accordingly)
 * Don't activate menus when pressing Alt alone so it can be used as an input

 v1.2.8 27th July 2020
 * Add debug logging for Sintax & BBD protection initialisation modes
   (to work with Sintaxinator in hhugtools)

 v1.2.7 31st October 2018 (BOO)
 * Add memory searcher
 * Add support for original Pokemon Jade/Diamond
 * Add selectable MBC3 compatible mode
 * Some refactoring of graphics code which should hopefully have no effect

 v1.2.6 11th March 2018
 * Fix MBC5 RAM banking bug observed to affect unlicensed games
   e.g. Fengkuang Dafuweng (V.Fame), Digimon Pocket (Makon)

 v1.2.5 5th March 2018
 * Add support for GBX file format
 * Add support for NT new carts supporting later Makon single cart games
 * Rename 'NT KL' compatibility mode to 'NT Old'
   (since KL PCB codes were also used on at least one cart with the new mapper)
 * Support for Mortal Kombat 1&2 1MB dump
 * Cart detection code updates, mostly should have no effect aside from a
   couple bad ROMs no longer being picked up
 * Added ability to select Liebao Technology mode manually
 * Refactored UI code & added secret debug window, which is a secret

 v1.2.1 24th August 2017
 * Detection for more NT KL carts
 * Visual rumble implemented for NT KL type 2 carts
 * Somewhat more-accurate rumble on official carts
 * Restore compatibility for old Rockman 8 dump
 * Tidy up detection code a bit

 v1.2 13th August 2017
 * Support for NT KL carts e.g. old Makon games
 * Option for compatibility with unlicensed games using MBC1/5
 * "Realistic" colour mode is now off by default

 v1.1.8 7th January 2017
 * Hitek game support
 * Add 25% and 50% volume options

 v1.1.6 23rd October 2016
 * Better Sintax support - fixes Dragon Ball Z 3 2002 Fighting
 * Auto-detection support for fixed/hacked/decrypted Sintax games

 v1.1.5 2nd October 2016
 * Support for (some) BBD games
 * Support for "Dragon Ball Z 3" English translation of DBZ Goku Gekitouden
 * Fix roms loaded from ZIP not showing up in the recent roms list
 * More code refactoring mostly around rom type detection

 v1.1.1 4th September 2016
 * Adds detection for Korean 'Bomberman Selection' cart
 * Fixes window layer issue affecting Sanguozhi Wudai

 v1.1 30th August 2016
 * Adds support for 'Last Bible' multicart mapper, with detection for three
   carts on that mapper: V.Fame 18 in 1, V.Fame 12 in 1, SL Pokemon 36 in 1
 * Fixed savestates in Sintax games
 * Fixes Mortal Kombat 1&2 ([a1] only)
 * Fixes issue where border would remain when loading a regular game after a
   Super Game Boy one
 * Way more code refactoring to make it easier to add new mappers in future

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
 copyright 2013-2020 taizou

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

 Massive contributions by NewRisingSun
 PCPaint Bold font by codeman38 (http://www.zone38.net/)
 Beta testers - MLX, Robyn, KingPepe, JP
