
#define UNICODE

#include <windows.h>

#include <cstdio>
#include "LinkerLog.h"
#include "../../GB.h"
#include "../../memory/linker/LinkerWrangler.h"

using namespace std;

extern HINSTANCE hinst;
extern HWND hwnd;

HWND LinkerLog::linkerLogDialog;
bool LinkerLog::linkerInitialised = false;

void LinkerLog::addMessage(const wchar_t* message)
{
    if (linkerLogDialog) {
        if (SendDlgItemMessage(linkerLogDialog, ID_LINKER_LOG_CHECKBOX, BM_GETCHECK, 0, 0)) {
            FILE* logfile;
            logfile = fopen("linkerlog.txt","a");
            fputws(message, logfile);
            fputws(L"\r\n", logfile);
            fclose(logfile);
        }
        HWND hwndbox = GetDlgItem(linkerLogDialog, ID_LINKER_LOG_LOG);
        SendMessage(hwndbox, LB_ADDSTRING, 0, (LPARAM)message );
        SendMessage(hwndbox, LB_SETCARETINDEX, SendMessage(hwndbox,LB_GETCOUNT,0,0)-1, true );
    }
}

void LinkerLog::addMessage(const char* message)
{
    wchar_t wmessage[1000];
    mbstowcs(wmessage,message,1000);
    addMessage(wmessage);
}

BOOL CALLBACK LinkerLog::LinkerLogDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_INITDIALOG:
            linkerLogDialog = hwndDlg;
            break;
        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case ID_LINKER_LOG_BTN:
                    if (linkerInitialised) {
                        LinkerWrangler::deinitLinker();
                    }
                    LinkerWrangler::initLinker();
                    linkerInitialised = true;
                    break;
                case ID_LINKER_LOG_BTN_STOP:
                    if (linkerInitialised) {
                        LinkerWrangler::deinitLinker();
                        linkerInitialised = false;
                    }
                    break;
                case IDOK:
                case IDCANCEL:
                    EndDialog(hwndDlg, wParam);
                    linkerLogDialog = NULL;
                    return TRUE;
            }
            break;
        case WM_VKEYTOITEM:
            return -2;
    }
    return FALSE;
}

void LinkerLog::SpawnLinkerLog()
{
    if (!linkerLogDialog) {
        linkerLogDialog = CreateDialog(hinst, MAKEINTRESOURCE(ID_LINKER_LOG), hwnd, (DLGPROC)LinkerLog::LinkerLogDialogProc);
        ShowWindow(linkerLogDialog, SW_SHOW);
    }
    SetFocus(linkerLogDialog);
}
