/*
   Memory searcher for hhugboy Game Boy emulator
   copyright 2013-2018 taizou

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

#include <string>
#include <cstdio>
#include "MemorySearcher.h"
#include "../../debug.h"
#include "../../GB.h"

using namespace std;

extern HINSTANCE hinst;
extern HWND hwnd;

HWND MemorySearcher::memorySearcherDialog;
bool MemorySearcher::resultSet[0xffff];
int MemorySearcher::searchCount = 0;

void MemorySearcher::addMessage(const wchar_t* message)
{
    if (memorySearcherDialog) {
        HWND hwndbox = GetDlgItem(memorySearcherDialog, ID_MEM_SEARCH_LOG);
        SendMessage(hwndbox, LB_ADDSTRING, 0, (LPARAM)message );
        SendMessage(hwndbox, LB_SETCARETINDEX, SendMessage(hwndbox,LB_GETCOUNT,0,0)-1, true );
    }
}

void MemorySearcher::addMessage(const char* message)
{
    wchar_t wmessage[1000];
    mbstowcs(wmessage,message,1000);
    addMessage(wmessage);
}

void MemorySearcher::resetResultSet()
{
    for(int x=0; x<=0xffff; x++) {
        resultSet[x] = false;
    }
}

void MemorySearcher::searchForValue(wchar_t* string, wchar_t* fromString, wchar_t* toString, bool inLastResultSet)
{
    int searchVal = wcstol(string, nullptr, 16) & 0xff;
    int searchFrom = wcstol(fromString, nullptr, 16) & 0xffff;
    int searchTo = wcstol(toString, nullptr, 16) & 0xffff;

    if (!inLastResultSet) {
        resetResultSet();
    }

    char msg[420];
    if (inLastResultSet) {
        sprintf(msg, "Searching for %02X from %04X to %04X and within previous result set...", searchVal, searchFrom, searchTo);
    } else {
        sprintf(msg, "Searching for %02X from %04X to %04X...", searchVal, searchFrom, searchTo);
    }
    addMessage(msg);

    char addressesOnRow[16][5];
    for(int y=0;y<16;y++) {
        strcpy(addressesOnRow[y],"    ");
    }
    int rowPos = 0;
    int count = 0;

    for(int x=searchFrom;x<=searchTo;x++) {
        if (inLastResultSet && !resultSet[x]) {
            continue;
        }
        if (GB1->memory[x] == searchVal) {
            if (count == 0) {
                addMessage(" ");
                addMessage(" Found at locations:");
                addMessage(" ");
            }
            count++;
            sprintf(addressesOnRow[rowPos], "%04X", x);
            resultSet[x] = true;
            rowPos++;
            if (rowPos == 16) {
                sprintf(msg, " %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",
                        addressesOnRow[0], addressesOnRow[1], addressesOnRow[2], addressesOnRow[3],
                        addressesOnRow[4], addressesOnRow[5], addressesOnRow[6], addressesOnRow[7],
                        addressesOnRow[8], addressesOnRow[9], addressesOnRow[10], addressesOnRow[11],
                        addressesOnRow[12], addressesOnRow[13], addressesOnRow[14], addressesOnRow[15]
                );
                addMessage(msg);
                for(int y=0;y<16;y++) {
                    strcpy(addressesOnRow[y],"    ");
                }
                rowPos = 0;
            }
        } else {
            resultSet[x] = false;
        }
    }
    if (rowPos > 0) {
        sprintf(msg, " %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",
                addressesOnRow[0], addressesOnRow[1], addressesOnRow[2], addressesOnRow[3],
                addressesOnRow[4], addressesOnRow[5], addressesOnRow[6], addressesOnRow[7],
                addressesOnRow[8], addressesOnRow[9], addressesOnRow[10], addressesOnRow[11],
                addressesOnRow[12], addressesOnRow[13], addressesOnRow[14], addressesOnRow[15]
        );
        addMessage(msg);
    }

    addMessage(" ");
    if (count == 0) {
        sprintf(msg, " Not found");
        addMessage(msg);
    } else {
        sprintf(msg, " %d instances found", count);
        addMessage(msg);
    }
    addMessage(" ");

}

BOOL CALLBACK MemorySearcher::MemorySearcherDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_INITDIALOG:
            memorySearcherDialog = hwndDlg;
            SetDlgItemTextW(hwndDlg, ID_MEM_SEARCH_BOX, L"00");
            SetDlgItemTextW(hwndDlg, ID_MEM_SEARCH_START_BOX, L"C000");
            SetDlgItemTextW(hwndDlg, ID_MEM_SEARCH_END_BOX, L"DFFF");
            searchCount = 0;
            resetResultSet();
            break;
        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case ID_MEM_SEARCH_BTN:
                    wchar_t valueStr[3];
                    GetDlgItemText(hwndDlg, ID_MEM_SEARCH_BOX, valueStr, 3);
                    wchar_t fromStr[5];
                    GetDlgItemText(hwndDlg, ID_MEM_SEARCH_START_BOX, fromStr, 5);
                    wchar_t toStr[5];
                    GetDlgItemText(hwndDlg, ID_MEM_SEARCH_END_BOX, toStr, 5);
                    if (searchCount > 0) {
                        addMessage("=================================================================================");
                        addMessage(" ");
                    }
                    searchCount++;
                    searchForValue(valueStr, fromStr, toStr, SendDlgItemMessage(hwndDlg, ID_MEM_SEARCH_PREV_CHECKBOX, BM_GETCHECK, 0, 0));
                    break;
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

void MemorySearcher::SpawnMemorySearcher()
{
    HWND memorySearcherDialogHandle = CreateDialog(hinst, MAKEINTRESOURCE(ID_MEM_SEARCH), hwnd, (DLGPROC)MemorySearcher::MemorySearcherDialogProc);
    ShowWindow(memorySearcherDialogHandle, SW_SHOW);
}
