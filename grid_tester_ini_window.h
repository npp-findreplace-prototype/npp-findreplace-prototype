#ifndef GRID_TESTER_INI_WINDOW_H
#define GRID_TESTER_INI_WINDOW_H

#include <windows.h>

typedef void (*GridTesterIniWindowClosedCallback)(void);

HWND GridTesterIniWindow_Show(
    HINSTANCE hInstance,
    GridTesterIniWindowClosedCallback onClosed
);

void GridTesterIniWindow_Close(void);
HWND GridTesterIniWindow_GetHwnd(void);
int GridTesterIniWindow_IsOpen(void);

#endif