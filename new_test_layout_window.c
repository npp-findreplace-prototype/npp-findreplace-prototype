#include "new_test_layout_window_internal.h"

static void NewTestLayout_HandlePaint(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc;
    RECT rc;

    hdc = BeginPaint(hwnd, &ps);

    GetClientRect(hwnd, &rc);

    if (g_ntl_backBrush)
        FillRect(hdc, &rc, g_ntl_backBrush);
    else
        Ui_FillSolidRect(hdc, &rc, GetSysColor(COLOR_WINDOW));

    EndPaint(hwnd, &ps);
}

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

            g_ntl_hInstance = cs->hInstance;
            g_ntl_window = hwnd;

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

            SetBkColor(hdc, g_ntl_theme.editBackColor);
            SetTextColor(hdc, g_ntl_theme.editTextColor);

            return (LRESULT)GetStockObject(NULL_BRUSH);
        }

        case WM_CTLCOLORSTATIC:
        {
            LRESULT result;

            result = 0;

            if (g_ntl_settingsPanel &&
                NewTestLayoutSettings_HandleCtlColorStatic(
                    g_ntl_settingsPanel,
                    (HDC)wParam,
                    (HWND)lParam,
                    &result
                ))
            {
                return result;
            }

            SetBkColor((HDC)wParam, g_ntl_theme.windowBackColor);
            SetTextColor((HDC)wParam, g_ntl_theme.buttonTextColor);

            if (g_ntl_backBrush)
                return (LRESULT)g_ntl_backBrush;

            return (LRESULT)(COLOR_WINDOW + 1);
        }

        case WM_ERASEBKGND:
        {
            return 1;
        }

        case WM_PAINT:
        {
            NewTestLayout_HandlePaint(hwnd);
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

            g_ntl_window = NULL;
            g_ntl_modeGrid = NULL;

            ZeroMemory(&g_ntl_counts, sizeof(g_ntl_counts));
            g_ntl_lastFindText[0] = '\0';

            callback = g_ntl_onClosed;
            g_ntl_onClosed = NULL;

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
    {
        if (GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
            return FALSE;
    }

    registered = 1;
    return TRUE;
}

HWND NewTestLayoutWindow_Show(
    HINSTANCE hInstance,
    NewTestLayoutWindowClosedCallback onClosed
)
{
    g_ntl_hInstance = hInstance;
    g_ntl_onClosed = onClosed;

    if (g_ntl_window && IsWindow(g_ntl_window))
    {
        ShowWindow(g_ntl_window, SW_SHOW);
        SetForegroundWindow(g_ntl_window);
        return g_ntl_window;
    }

    if (!NewTestLayoutWindow_RegisterClass(hInstance))
        return NULL;

    g_ntl_window = CreateWindowEx(
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

    if (!g_ntl_window)
        return NULL;

    ShowWindow(g_ntl_window, SW_SHOW);
    UpdateWindow(g_ntl_window);

    return g_ntl_window;
}

void NewTestLayoutWindow_Close(void)
{
    if (g_ntl_window && IsWindow(g_ntl_window))
        DestroyWindow(g_ntl_window);
}

HWND NewTestLayoutWindow_GetHwnd(void)
{
    return g_ntl_window;
}

int NewTestLayoutWindow_IsOpen(void)
{
    return g_ntl_window && IsWindow(g_ntl_window);
}