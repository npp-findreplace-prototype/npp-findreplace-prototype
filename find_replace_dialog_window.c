#include <windows.h>
#include <stdio.h>

#include "find_replace_dialog_window.h"

#define FIND_REPLACE_DIALOG_WINDOW_CLASS_NAME "FindReplaceDialogWindowClass"
#define FIND_REPLACE_DIALOG_WINDOW_TITLE "Find and Replace Dialog"

static HWND g_window = NULL;
static FindReplaceDialogWindowClosedCallback g_onClosed = NULL;

static LRESULT CALLBACK FindReplaceDialogWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_CREATE:
        {
            g_window = hwnd;
            printf("Find and replace dialog placeholder created.\n");
            return 0;
        }

        case WM_CLOSE:
        {
            DestroyWindow(hwnd);
            return 0;
        }

        case WM_NCDESTROY:
        {
            FindReplaceDialogWindowClosedCallback callback;

            printf("Find and replace dialog placeholder destroyed.\n");

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

static BOOL FindReplaceDialogWindow_RegisterClass(HINSTANCE hInstance)
{
    static int registered = 0;
    WNDCLASS wc;

    if (registered)
        return TRUE;

    ZeroMemory(&wc, sizeof(wc));

    wc.lpfnWndProc = FindReplaceDialogWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = FIND_REPLACE_DIALOG_WINDOW_CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClass(&wc))
        return FALSE;

    registered = 1;
    return TRUE;
}

HWND FindReplaceDialogWindow_Show(
    HINSTANCE hInstance,
    FindReplaceDialogWindowClosedCallback onClosed
)
{
    g_onClosed = onClosed;

    if (g_window && IsWindow(g_window))
    {
        ShowWindow(g_window, SW_SHOW);
        SetForegroundWindow(g_window);
        return g_window;
    }

    if (!FindReplaceDialogWindow_RegisterClass(hInstance))
        return NULL;

    g_window = CreateWindowEx(
        0,
        FIND_REPLACE_DIALOG_WINDOW_CLASS_NAME,
        FIND_REPLACE_DIALOG_WINDOW_TITLE,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        640,
        360,
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

void FindReplaceDialogWindow_Close(void)
{
    if (g_window && IsWindow(g_window))
        DestroyWindow(g_window);
}

HWND FindReplaceDialogWindow_GetHwnd(void)
{
    return g_window;
}

int FindReplaceDialogWindow_IsOpen(void)
{
    return g_window && IsWindow(g_window);
}