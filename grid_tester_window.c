#include "grid_tester_window_internal.h"

static BOOL GridTesterWindow_RegisterClass(HINSTANCE hInstance)
{
    static int registered = 0;
    WNDCLASS wc;

    if (registered)
        return TRUE;

    ZeroMemory(&wc, sizeof(wc));

    wc.lpfnWndProc = GridTesterWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = GRID_TESTER_WINDOW_CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClass(&wc))
        return FALSE;

    registered = 1;
    return TRUE;
}

HWND GridTesterWindow_Show(
    HINSTANCE hInstance,
    GridTesterWindowClosedCallback onClosed
)
{
    g_gridTesterHInstance = hInstance;
    g_gridTesterOnClosed = onClosed;

    if (g_gridTesterWindow && IsWindow(g_gridTesterWindow))
    {
        ShowWindow(g_gridTesterWindow, SW_SHOW);
        SetForegroundWindow(g_gridTesterWindow);
        return g_gridTesterWindow;
    }

    if (!GridTesterWindow_RegisterClass(hInstance))
        return NULL;

    g_gridTesterWindow = CreateWindowEx(
        0,
        GRID_TESTER_WINDOW_CLASS_NAME,
        GRID_TESTER_WINDOW_TITLE,
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        1000,
        760,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!g_gridTesterWindow)
        return NULL;

    ShowWindow(g_gridTesterWindow, SW_SHOW);
    UpdateWindow(g_gridTesterWindow);

    return g_gridTesterWindow;
}

void GridTesterWindow_Close(void)
{
    if (g_gridTesterWindow && IsWindow(g_gridTesterWindow))
        DestroyWindow(g_gridTesterWindow);
}

HWND GridTesterWindow_GetHwnd(void)
{
    return g_gridTesterWindow;
}

int GridTesterWindow_IsOpen(void)
{
    return g_gridTesterWindow && IsWindow(g_gridTesterWindow);
}