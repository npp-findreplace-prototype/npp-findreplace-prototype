#include "new_test_layout_window_internal.h"

static LRESULT CALLBACK NewTestLayoutWindowProc(
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

            g_hInstance = cs->hInstance;
            g_window = hwnd;

            NewTestLayout_CreateControls(hwnd);
            NewTestLayout_Layout(hwnd);

            SetTimer(hwnd, NTL_TIMER_ID, NTL_TIMER_MS, NULL);

            Debug_Log(
                "NewLayout",
                "WindowCreate",
                "New test layout window created."
            );

            return 0;
        }

        case WM_SIZE:
        {
            NewTestLayout_Layout(hwnd);
            return 0;
        }

        case WM_DPICHANGED:
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

            NewTestLayout_Layout(hwnd);
            return 0;
        }

        case WM_COMMAND:
        {
            if (NewTestLayout_HandleCommand(wParam, lParam))
                return 0;

            break;
        }

        case WM_TIMER:
        {
            if (wParam == NTL_TIMER_ID)
            {
                NewTestLayout_ApplyCounts();
                return 0;
            }

            break;
        }

        case WM_CTLCOLOREDIT:
        {
            HDC hdc;

            hdc = (HDC)wParam;

            SetBkColor(hdc, g_theme.editBackColor);
            SetTextColor(hdc, g_theme.editTextColor);

            return (LRESULT)GetStockObject(NULL_BRUSH);
        }

        case WM_CTLCOLORSTATIC:
        {
            LRESULT result;

            result = 0;

            if (g_settingsPanel &&
                NewTestLayoutSettings_HandleCtlColorStatic(
                    g_settingsPanel,
                    (HDC)wParam,
                    (HWND)lParam,
                    &result
                ))
            {
                return result;
            }

            SetBkColor((HDC)wParam, g_theme.windowBackColor);
            SetTextColor((HDC)wParam, g_theme.buttonTextColor);
            return (LRESULT)g_backBrush;
        }

        case WM_ERASEBKGND:
        {
            return 1;
        }

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc;
            RECT rc;

            hdc = BeginPaint(hwnd, &ps);

            GetClientRect(hwnd, &rc);

            if (g_backBrush)
                FillRect(hdc, &rc, g_backBrush);
            else
                FillRect(hdc, &rc, (HBRUSH)(COLOR_WINDOW + 1));

            EndPaint(hwnd, &ps);
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

            KillTimer(hwnd, NTL_TIMER_ID);

            Debug_Log(
                "NewLayout",
                "WindowDestroy",
                "New test layout window destroyed."
            );

            NewTestLayout_DestroyControls();

            g_window = NULL;
            g_modeGrid = NULL;

            ZeroMemory(&g_counts, sizeof(g_counts));
            g_lastFindText[0] = '\0';

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
    wc.hbrBackground = NULL;

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
    g_hInstance = hInstance;
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
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        860,
        520,
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