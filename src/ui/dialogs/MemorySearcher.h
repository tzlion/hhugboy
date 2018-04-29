/*
   Memory searcher for hhugboy Game Boy emulator
   copyright 2018 taizou

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

#include <windef.h>

#ifndef MEMORY_SEARCHER_H
#define MEMORY_SEARCHER_H

#define ID_MEM_SEARCH 199
#define ID_MEM_SEARCH_LOG 198
#define ID_MEM_SEARCH_BTN 197
#define ID_MEM_SEARCH_BOX 196
#define ID_MEM_SEARCH_START_BOX 195
#define ID_MEM_SEARCH_END_BOX 194
#define ID_MEM_SEARCH_PREV_CHECKBOX 193

class MemorySearcher {
    private:
        static HWND memorySearcherDialog;
        static bool resultSet[0xffff];
        static int searchCount;
        static void resetResultSet();
        static void searchForValue(wchar_t* string, wchar_t* fromString, wchar_t* toString, bool inLastResultSet);
        static void addMessage(const wchar_t* message);
        static void addMessage(const char* message);
    public:
        static BOOL CALLBACK MemorySearcherDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
        static void SpawnMemorySearcher();
};

#endif
