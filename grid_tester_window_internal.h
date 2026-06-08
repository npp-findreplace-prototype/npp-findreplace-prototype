#ifndef GRID_TESTER_WINDOW_INTERNAL_H
#define GRID_TESTER_WINDOW_INTERNAL_H

#include <windows.h>
#include <stdio.h>

#include "console.h"
#include "button_grid.h"
#include "grid_tester_window.h"

#ifndef VK_OEM_PLUS
#define VK_OEM_PLUS 0xBB
#endif

#ifndef VK_OEM_MINUS
#define VK_OEM_MINUS 0xBD
#endif

#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0
#endif

#ifndef LOGPIXELSX
#define LOGPIXELSX 88
#endif

#define GRID_TESTER_WINDOW_CLASS_NAME "GridTesterWindowClass"
#define GRID_TESTER_WINDOW_TITLE "Grid Tester Window"

#define GRID_MARGIN 12
#define SEARCH_MODE_RADIO_GROUP 1

extern HINSTANCE g_gridTesterHInstance;
extern HWND g_gridTesterWindow;
extern HWND g_gridTesterButtonGrid;
extern int g_gridTesterSquareSize;
extern GridTesterWindowClosedCallback g_gridTesterOnClosed;

int GridTester_CreateSearchGrid(HWND hwnd);

void GridTester_SetWindowTitle(HWND hwnd);
void GridTester_LayoutWindow(HWND hwnd);
void GridTester_SetSquareSize(HWND hwnd, int newSize);
void GridTester_HandleDpiChanged(HWND hwnd, LPARAM lParam);

LRESULT CALLBACK GridTesterWindowProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam
);

#endif