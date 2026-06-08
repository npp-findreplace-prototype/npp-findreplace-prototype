#include "grid_tester_window_internal.h"

LRESULT CALLBACK GridTesterWindowProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam
)
{
    switch (msg)
    {
        case WM_CREATE:
        {
            CREATESTRUCT *cs;

            cs = (CREATESTRUCT *)lParam;

            g_gridTesterHInstance = cs->hInstance;
            g_gridTesterWindow = hwnd;

            printf("Grid tester window created.\n");

            if (!GridTester_CreateSearchGrid(hwnd))
            {
                AppNotify("Error", "Could not create grid tester button grid.");
                return -1;
            }

            return 0;
        }

        case WM_SIZE:
        {
            GridTester_LayoutWindow(hwnd);
            return 0;
        }

        case WM_DPICHANGED:
        {
            GridTester_HandleDpiChanged(hwnd, lParam);
            return 0;
        }

        case WM_KEYDOWN:
        {
            if (wParam == VK_ADD || wParam == VK_OEM_PLUS)
            {
                GridTester_SetSquareSize(hwnd, g_gridTesterSquareSize + 10);
                return 0;
            }

            if (wParam == VK_SUBTRACT || wParam == VK_OEM_MINUS)
            {
                GridTester_SetSquareSize(hwnd, g_gridTesterSquareSize - 10);
                return 0;
            }

            break;
        }

        case WM_CLOSE:
        {
            DestroyWindow(hwnd);
            return 0;
        }

        case WM_NCDESTROY:
        {
            GridTesterWindowClosedCallback callback;

            printf("Grid tester window destroyed.\n");

            g_gridTesterWindow = NULL;
            g_gridTesterButtonGrid = NULL;

            callback = g_gridTesterOnClosed;
            g_gridTesterOnClosed = NULL;

            if (callback)
                callback();

            return 0;
        }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}