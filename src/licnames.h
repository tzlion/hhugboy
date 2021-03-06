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
/*
old GB licensee names
*/
const wchar_t* lic_names[256] =
{
  L"none",
  L"Nintendo",
  L"none",
  L"none",
  L"none",
  L"none",
  L"none",
  L"none",
  L"Capcom", // 0x08
  L"Hot-b",
  L"Jaleco",
  L"Coconuts",
  L"Elite Systems",
  L"none",
  L"none",
  L"none",
  L"none",
  L"none",
  L"none",
  L"Electronic Arts", //0x13
  L"none",
  L"none",
  L"none",
  L"none",
  L"Hudsonsoft", //0x18
  L"ITC Entertainment",
  L"Yanoman",
  L"none",
  L"none",
  L"Clary", //0x1D
  L"none",
  L"Virgin",
  L"none",
  L"none",
  L"none",
  L"none",
  L"PCM Complete", //0x24
  L"San-x",
  L"none",
  L"none",
  L"Kotobuki Systems", //0x28
  L"Seta",
  L"none",
  L"none",
  L"none",
  L"none",
  L"none",
  L"none",
  L"Infogrames", //0x30
  L"Nintendo",
  L"Bandai",
  L"SGB new lic",
  L"Konami",
  L"Hector",
  L"none",
  L"none",
  L"Capcom",
  L"Banpresto",
  L"none",
  L"none",
  L"Entertainment i", //0x3C
  L"none",
  L"Gremlin",
  L"none",
  L"none",
  L"Ubi Soft", //0x41
  L"Atlus",
  L"none",
  L"Malibu",
  L"none",
  L"Angel",
  L"Spectrum Holoby",
  L"none",
  L"Irem",
  L"Virgin",
  L"none",
  L"none",
  L"Malibu", //0x4D
  L"none",
  L"U.S. Gold",
  L"Absolute",
  L"Acclaim",
  L"Activision",
  L"American Sammy",
  L"Gametek",
  L"Park Place",
  L"LJN",
  L"Matchbox",
  L"none",
  L"Milton Bradley",
  L"Mindscape",
  L"ROMStar",
  L"Naxat Soft",
  L"Tradewest",
  L"none",
  L"none",
  L"Titus", //0x60
  L"Virgin",
  L"none",
  L"none",
  L"none",
  L"none",
  L"none",
  L"Ocean", //0x67
  L"none",
  L"Electronic Arts",
  L"none",
  L"none",
  L"none",
  L"none",
  L"Elite Systems", //0x6E
  L"Electro Brain",
  L"Infogrames",
  L"Interplay",
  L"Broderbund",
  L"Sculptered Soft",
  L"none",
  L"The Sales Curve",
  L"none",
  L"none",
  L"T*HQ", //0x78
  L"Accolade",
  L"Triffix Entertainment",
  L"none",
  L"Microprose",
  L"none",
  L"none",
  L"Kemco",
  L"Misawa Entertainment",
  L"none",
  L"none",
  L"Lozc", //0x83
  L"none",
  L"none",
  L"Tokuma Shoten i",
  L"none",
  L"none",
  L"none",
  L"none",
  L"Bullet-proof Software", //0x8B
  L"Vic Tokai",
  L"none",
  L"Ape",
  L"I'max",
  L"none",
  L"Chun Soft",
  L"Video System",
  L"Tsuburava",
  L"none",
  L"Varie",
  L"Yonezawa/s'pal",
  L"Kaneko",
  L"none",
  L"Arc",
  L"Nihon Bussan",
  L"Tecmo",
  L"Imagineer",
  L"Banpresto",
  L"none",
  L"Nova",
  L"none",
  L"Hori Electric", //0xA1
  L"Bandai",
  L"none",
  L"Konami",
  L"none",
  L"Kawada",
  L"Takara",
  L"none",
  L"Technos Japan",
  L"Broderbund",
  L"none",
  L"Toei Animation",
  L"Toho",
  L"none",
  L"Namco",
  L"Acclaim",
  L"Ascii or Nexoft",
  L"Bandai",
  L"none",
  L"Enix",
  L"none",
  L"Hal",
  L"SNK",
  L"none",
  L"Pony Canyon",
  L"Culture Brain o",
  L"Sunsoft",
  L"none",
  L"Sony Imagesoft",
  L"none",
  L"Sammy",
  L"Taito",
  L"none",
  L"Kemco",
  L"Squaresoft",
  L"Tokuma Shoten i",
  L"Data East",
  L"Tonkin House",
  L"none",
  L"Koei",
  L"UFL",
  L"Ultra",
  L"Vap",
  L"Use",
  L"Meldac",
  L"Pony Canyon or",
  L"Angel",
  L"Taito", //0xD0
  L"Sofel",
  L"Quest",
  L"Sigma Enterprises",
  L"Ask Kodansha",
  L"none",
  L"Naxat Soft",
  L"Copya Systems",
  L"none",
  L"Banpresto",
  L"Tomy",
  L"LJN",
  L"none",
  L"NCS",
  L"Human",
  L"Altron",
  L"Jaleco",
  L"Towachiki",
  L"Uutaka",
  L"Varie",
  L"none",
  L"Epoch",
  L"none",
  L"Athena", //0xE7
  L"Asmik",
  L"Natsume",
  L"King Records",
  L"Atlus",
  L"Epic/Sony Records",
  L"none",
  L"IGS",
  L"none",
  L"A Wave",
  L"none",
  L"none",
  L"Extreme Entertainment",
  L"none",
  L"none",
  L"none",
  L"none",
  L"none",
  L"none",
  L"none",
  L"none",
  L"none",
  L"none",
  L"none",
  L"LJN"
};

