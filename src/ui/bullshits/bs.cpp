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

BOOL CALLBACK MegaBullshitLogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    HWND hwndbox = GetDlgItem(hwndDlg, ID_SOME_BULLSHITS_LOG);
    switch (message)
    {
        case WM_INITDIALOG:
            megaBullshitDialog = hwndDlg;
            wchar_t str[100];
            wsprintf(str,L"SECRET BULLSHIT2 LOG ACTIVATE");

            SendMessage(hwndbox, LB_ADDSTRING, 0, (LPARAM)str );

            for(int x=0;x<=0xffff;x++) {
                if (GB1->memory[x] == 99) {
                    char msg[420];
                    sprintf(msg, "found 99 at %04x", x);
                    addMessage(msg);
                }
            }

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
