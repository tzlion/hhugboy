#define UNICODE

#include <windows.h>

#include <string>
#include <cstdio>
#include "memsearch.h"
#include "../../debug.h"
#include "../../GB.h"

using namespace std;

extern HINSTANCE hinst;
extern HWND hwnd;

HWND memorySearcherDialog;

int searchCount = 0;

void addMessage(const wchar_t* message)
{
    if (memorySearcherDialog) {
        HWND hwndbox = GetDlgItem(memorySearcherDialog, ID_MEM_SEARCH_LOG);
        SendMessage(hwndbox, LB_ADDSTRING, 0, (LPARAM)message );
        SendMessage(hwndbox, LB_SETCARETINDEX, SendMessage(hwndbox,LB_GETCOUNT,0,0)-1, true );
    }
}

void addMessage(const char* message)
{
    wchar_t wmessage[1000];
    mbstowcs(wmessage,message,1000);
    addMessage(wmessage);
}

void searchForValue(wchar_t* string, wchar_t* fromString, wchar_t* toString)
{
    int searchVal = wcstol(string, nullptr, 16) & 0xff;
    int searchFrom = wcstol(fromString, nullptr, 16) & 0xffff;
    int searchTo = wcstol(toString, nullptr, 16) & 0xffff;

    char msg[420];
    sprintf(msg, "Searching for %02X from %04X to %04X...", searchVal, searchFrom, searchTo);
    addMessage(msg);

    char addressesOnRow[16][5];
    for(int y=0;y<16;y++) {
        strcpy(addressesOnRow[y],"    ");
    }
    int rowPos = 0;
    int count = 0;

    for(int x=searchFrom;x<=searchTo;x++) {
        if (GB1->memory[x] == searchVal) {
            if (count == 0) {
                addMessage(" ");
                addMessage(" Found at locations:");
                addMessage(" ");
            }
            count++;
            sprintf(addressesOnRow[rowPos], "%04X", x);
            rowPos++;
            if (rowPos == 16) {
                sprintf(msg, " %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",
                        addressesOnRow[0], addressesOnRow[1], addressesOnRow[2],  addressesOnRow[3],
                        addressesOnRow[4],  addressesOnRow[5], addressesOnRow[6],  addressesOnRow[7],
                        addressesOnRow[8], addressesOnRow[9], addressesOnRow[10],  addressesOnRow[11],
                        addressesOnRow[12],  addressesOnRow[13], addressesOnRow[14],  addressesOnRow[15]
                );
                addMessage(msg);
                for(int y=0;y<16;y++) {
                    strcpy(addressesOnRow[y],"    ");
                }
                rowPos = 0;
            }
        }
    }
    if (rowPos > 0) {
        sprintf(msg, " %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",
                addressesOnRow[0], addressesOnRow[1], addressesOnRow[2],  addressesOnRow[3],
                addressesOnRow[4],  addressesOnRow[5], addressesOnRow[6],  addressesOnRow[7],
                addressesOnRow[8], addressesOnRow[9], addressesOnRow[10],  addressesOnRow[11],
                addressesOnRow[12],  addressesOnRow[13], addressesOnRow[14],  addressesOnRow[15]
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

BOOL CALLBACK MemorySearcherDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_INITDIALOG:
            memorySearcherDialog = hwndDlg;
            SetDlgItemTextW(hwndDlg, ID_MEM_SEARCH_BOX, L"00");
            SetDlgItemTextW(hwndDlg, ID_MEM_SEARCH_START_BOX, L"0000");
            SetDlgItemTextW(hwndDlg, ID_MEM_SEARCH_END_BOX, L"7FFF");
            searchCount = 0;
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
                    searchForValue(valueStr, fromStr, toStr);
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

void SpawnMemorySearcher()
{
    HWND memorySearcherDialogHandle = CreateDialog(hinst, MAKEINTRESOURCE(ID_MEM_SEARCH), hwnd, (DLGPROC)MemorySearcherDialogProc);
    ShowWindow(memorySearcherDialogHandle, SW_SHOW);
}
