#ifndef FIND_REPLACE_DIALOG_WINDOW_H
#define FIND_REPLACE_DIALOG_WINDOW_H

#include <windows.h>

typedef void (*FindReplaceDialogWindowClosedCallback)(void);

HWND FindReplaceDialogWindow_Show(
    HINSTANCE hInstance,
    FindReplaceDialogWindowClosedCallback onClosed
);

void FindReplaceDialogWindow_Close(void);
HWND FindReplaceDialogWindow_GetHwnd(void);
int FindReplaceDialogWindow_IsOpen(void);

#endif