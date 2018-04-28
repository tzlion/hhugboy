#include <windef.h>

#ifndef BS_H
#define BS_H

#define ID_MEM_SEARCH 199
#define ID_MEM_SEARCH_LOG 198
#define ID_MEM_SEARCH_BTN 197
#define ID_MEM_SEARCH_BOX 196

BOOL CALLBACK MemorySearcherLogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
void SpawnMemorySearcher();

#endif
