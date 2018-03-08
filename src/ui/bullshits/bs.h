#include <windef.h>

#ifndef BS_H
#define BS_H

#define ID_SOME_BULLSHITS 199
#define ID_SOME_BULLSHITS_LOG 198

BOOL CALLBACK MegaBullshitLogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
void SpawnMegaBullshit();

#endif
