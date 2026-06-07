#include <windows.h>
#include <stdio.h>

#include "npp_mockup_window.h"

#define NPP_MOCKUP_WINDOW_CLASS_NAME "NppMockupWindowClass"
#define NPP_MOCKUP_WINDOW_TITLE "NP++ Mockup"

static HWND g_window = NULL;
static NppMockupWindowClosedCallback g_onClosed = NULL;

static LRESULT CALLBACK NppMockupWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_CREATE:
        {
            g_window = hwnd;
            printf("NP++ mockup placeholder created.\n");
            return 0;
        }

        case WM_CLOSE:
        {
            DestroyWindow(hwnd);
            return 0;
        }

        case WM_NCDESTROY:
        {
            NppMockupWindowClosedCallback callback;

            printf("NP++ mockup placeholder destroyed.\n");

            g_window = NULL;

            callback = g_onClosed;
            g_onClosed = NULL;

            if (callback)
                callback();

            return 0;
        }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

static BOOL NppMockupWindow_RegisterClass(HINSTANCE hInstance)
{
    static int registered = 0;
    WNDCLASS wc;

    if (registered)
        return TRUE;

    ZeroMemory(&wc, sizeof(wc));

    wc.lpfnWndProc = NppMockupWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = NPP_MOCKUP_WINDOW_CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClass(&wc))
        return FALSE;

    registered = 1;
    return TRUE;
}

HWND NppMockupWindow_Show(
    HINSTANCE hInstance,
    NppMockupWindowClosedCallback onClosed
)
{
    g_onClosed = onClosed;

    if (g_window && IsWindow(g_window))
    {
        ShowWindow(g_window, SW_SHOW);
        SetForegroundWindow(g_window);
        return g_window;
    }

    if (!NppMockupWindow_RegisterClass(hInstance))
        return NULL;

    g_window = CreateWindowEx(
        0,
        NPP_MOCKUP_WINDOW_CLASS_NAME,
        NPP_MOCKUP_WINDOW_TITLE,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        900,
        650,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!g_window)
        return NULL;

    ShowWindow(g_window, SW_SHOW);
    UpdateWindow(g_window);

    return g_window;
}

void NppMockupWindow_Close(void)
{
    if (g_window && IsWindow(g_window))
        DestroyWindow(g_window);
}

HWND NppMockupWindow_GetHwnd(void)
{
    return g_window;
}

int NppMockupWindow_IsOpen(void)
{
    return g_window && IsWindow(g_window);
}