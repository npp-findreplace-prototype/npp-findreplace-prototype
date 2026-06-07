#ifndef NEW_TEST_LAYOUT_WINDOW_H
#define NEW_TEST_LAYOUT_WINDOW_H

#include <windows.h>

typedef void (*NewTestLayoutWindowClosedCallback)(void);

HWND NewTestLayoutWindow_Show(
    HINSTANCE hInstance,
    NewTestLayoutWindowClosedCallback onClosed
);

void NewTestLayoutWindow_Close(void);
HWND NewTestLayoutWindow_GetHwnd(void);
int NewTestLayoutWindow_IsOpen(void);

#endif