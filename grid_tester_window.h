#ifndef GRID_TESTER_WINDOW_H
#define GRID_TESTER_WINDOW_H

#include <windows.h>

typedef void (*GridTesterWindowClosedCallback)(void);

HWND GridTesterWindow_Show(
    HINSTANCE hInstance,
    GridTesterWindowClosedCallback onClosed
);

void GridTesterWindow_Close(void);
HWND GridTesterWindow_GetHwnd(void);
int GridTesterWindow_IsOpen(void);

#endif