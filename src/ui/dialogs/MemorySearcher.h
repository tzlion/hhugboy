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
