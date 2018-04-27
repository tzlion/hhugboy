#define UNICODE

#include <windows.h>

#include <string>
#include <cstdio>
#include "bs.h"
#include "../../debug.h"
#include "../../GB.h"

using namespace std;

extern HINSTANCE hinst;
extern HWND hwnd;

HWND megaBullshitDialog;
HWND megaBullshitDialogHandle = NULL;

void addMessage(const wchar_t* message)
{
    if (megaBullshitDialog) {
        HWND hwndbox = GetDlgItem(megaBullshitDialog, ID_SOME_BULLSHITS_LOG);
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

int parseStringPart2( wchar_t* string, int start, int length )
{
    // copied from cheats whatever
    char parser_str[10];
    for(int i=0;i<length;++i)
        parser_str[i] = string[i+start];
    return (int)strtol(parser_str, NULL, 16);
}

void searchForValue(wchar_t* string)
{
    debug_print(string);
    int fart = parseStringPart2(string, 0, 2);

    char msg[420];
    sprintf(msg, "Searching for %02x...", fart);
    addMessage(msg);

    for(int x=0;x<=0xffff;x++) {
        if (GB1->memory[x] == fart) {
            char msg[420];
            sprintf(msg, "Found %02x at %04x", fart, x);
            addMessage(msg);
        }
    }

}

BOOL CALLBACK MegaBullshitLogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    HWND hwndbox = GetDlgItem(hwndDlg, ID_SOME_BULLSHITS_LOG);
    switch (message)
    {
        case WM_INITDIALOG:
            megaBullshitDialog = hwndDlg;
            wchar_t str[100];
            wsprintf(str,L"RUDIMENTARY MEMORY SEARCH ACTIVATED");

            SendMessage(hwndbox, LB_ADDSTRING, 0, (LPARAM)str );

            break;
        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case ID_SOME_BULLSHITS_BTN:
                    wchar_t cheat_str[10];
                    GetDlgItemText(hwndDlg, ID_SOME_BULLSHITS_BOX, cheat_str, 4);
                    searchForValue(cheat_str);
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

void SpawnMegaBullshit()
{
    // Seems to rquire the .rc file being included in menu.rc ... For some reason like that is the Everything RC
    // Maybe go like this:
    // 1. One 'base RC' that only INCLUDES all the rest INCLUDING menu.
    // 2. An 'Everything Header' that defines all the resource IDS
    // -- OR MAYBE -- it only needs to define the first 2 bytes & then the second 2 can be defined per res.
    //  NAH SOMETHING LIKE THE MENU WOULD OVERRUN THAT
    // 3. The rest goes under its folders
    megaBullshitDialogHandle = CreateDialog(hinst, MAKEINTRESOURCE(ID_SOME_BULLSHITS), hwnd, (DLGPROC)MegaBullshitLogProc);
    ShowWindow(megaBullshitDialogHandle, SW_SHOW);
}
