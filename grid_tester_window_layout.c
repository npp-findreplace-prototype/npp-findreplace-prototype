#include "grid_tester_window_internal.h"

void GridTester_SetWindowTitle(HWND hwnd)
{
    char title[256];

    wsprintf(
        title,
        "%s - square size %d x %d",
        GRID_TESTER_WINDOW_TITLE,
        g_gridTesterSquareSize,
        g_gridTesterSquareSize
    );

    SetWindowText(hwnd, title);
}

void GridTester_LayoutWindow(HWND hwnd)
{
    RECT rc;
    int x;
    int y;
    int w;
    int h;

    if (!g_gridTesterButtonGrid)
        return;

    GetClientRect(hwnd, &rc);

    x = GRID_MARGIN;
    y = GRID_MARGIN;
    w = (rc.right - rc.left) - (GRID_MARGIN * 2);
    h = (rc.bottom - rc.top) - (GRID_MARGIN * 2);

    if (w < 1)
        w = 1;

    if (h < 1)
        h = 1;

    ButtonGrid_SetRect(g_gridTesterButtonGrid, x, y, w, h);
}

void GridTester_SetSquareSize(HWND hwnd, int newSize)
{
    if (newSize < 20)
        newSize = 20;

    if (newSize > 300)
        newSize = 300;

    if (g_gridTesterSquareSize == newSize)
        return;

    g_gridTesterSquareSize = newSize;

    ButtonGrid_SetButtonSize(
        g_gridTesterButtonGrid,
        g_gridTesterSquareSize,
        g_gridTesterSquareSize
    );

    GridTester_SetWindowTitle(hwnd);

    printf(
        "Grid tester square size changed to %d x %d\n",
        g_gridTesterSquareSize,
        g_gridTesterSquareSize
    );
}

void GridTester_HandleDpiChanged(HWND hwnd, LPARAM lParam)
{
    RECT *suggested;

    suggested = (RECT *)lParam;

    if (suggested)
    {
        SetWindowPos(
            hwnd,
            NULL,
            suggested->left,
            suggested->top,
            suggested->right - suggested->left,
            suggested->bottom - suggested->top,
            SWP_NOZORDER | SWP_NOACTIVATE
        );
    }

    GridTester_LayoutWindow(hwnd);

    if (g_gridTesterButtonGrid)
        ButtonGrid_Relayout(g_gridTesterButtonGrid);
}