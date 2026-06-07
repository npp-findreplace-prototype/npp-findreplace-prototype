#ifndef NPP_MOCKUP_WINDOW_H
#define NPP_MOCKUP_WINDOW_H

#include <windows.h>

typedef void (*NppMockupWindowClosedCallback)(void);

HWND NppMockupWindow_Show(
    HINSTANCE hInstance,
    NppMockupWindowClosedCallback onClosed
);

void NppMockupWindow_Close(void);
HWND NppMockupWindow_GetHwnd(void);
int NppMockupWindow_IsOpen(void);

#endif