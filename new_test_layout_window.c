#include <windows.h>
#include <stdio.h>

#include "new_test_layout_window.h"

#define NEW_TEST_LAYOUT_WINDOW_CLASS_NAME "NewTestLayoutWindowClass"
#define NEW_TEST_LAYOUT_WINDOW_TITLE "New Test Layout"

static HWND g_window = NULL;
static NewTestLayoutWindowClosedCallback g_onClosed = NULL;

static LRESULT CALLBACK NewTestLayoutWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_CREATE:
        {
            g_window = hwnd;
            printf("New test layout window created.\n");
            return 0;
        }

        case WM_CLOSE:
        {
            DestroyWindow(hwnd);
            return 0;
        }

        case WM_NCDESTROY:
        {
            NewTestLayoutWindowClosedCallback callback;

            printf("New test layout window destroyed.\n");

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

static BOOL NewTestLayoutWindow_RegisterClass(HINSTANCE hInstance)
{
    static int registered = 0;
    WNDCLASS wc;

    if (registered)
        return TRUE;

    ZeroMemory(&wc, sizeof(wc));

    wc.lpfnWndProc = NewTestLayoutWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = NEW_TEST_LAYOUT_WINDOW_CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClass(&wc))
        return FALSE;

    registered = 1;
    return TRUE;
}

HWND NewTestLayoutWindow_Show(
    HINSTANCE hInstance,
    NewTestLayoutWindowClosedCallback onClosed
)
{
    g_onClosed = onClosed;

    if (g_window && IsWindow(g_window))
    {
        ShowWindow(g_window, SW_SHOW);
        SetForegroundWindow(g_window);
        return g_window;
    }

    if (!NewTestLayoutWindow_RegisterClass(hInstance))
        return NULL;

    g_window = CreateWindowEx(
        0,
        NEW_TEST_LAYOUT_WINDOW_CLASS_NAME,
        NEW_TEST_LAYOUT_WINDOW_TITLE,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        800,
        600,
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

void NewTestLayoutWindow_Close(void)
{
    if (g_window && IsWindow(g_window))
        DestroyWindow(g_window);
}

HWND NewTestLayoutWindow_GetHwnd(void)
{
    return g_window;
}

int NewTestLayoutWindow_IsOpen(void)
{
    return g_window && IsWindow(g_window);
}