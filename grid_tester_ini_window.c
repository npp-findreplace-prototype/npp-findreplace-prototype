#include <windows.h>
#include <stdio.h>

#include "console.h"
#include "button_grid.h"
#include "button_grid_ini.h"
#include "grid_tester_ini_window.h"
#include "debug_window.h"

#ifndef VK_OEM_PLUS
#define VK_OEM_PLUS 0xBB
#endif

#ifndef VK_OEM_MINUS
#define VK_OEM_MINUS 0xBD
#endif

#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0
#endif

#define GRID_TESTER_INI_WINDOW_CLASS_NAME "GridTesterIniWindowClass"
#define GRID_TESTER_INI_WINDOW_TITLE "Grid Tester (INI)"
#define GRID_TESTER_INI_FILE_NAME "grid_tester.ini"

#define GRID_MARGIN 12

static HINSTANCE g_hInstance = NULL;
static HWND g_window = NULL;
static HWND g_buttonGrid = NULL;

static int g_squareSize = BUTTON_GRID_DEFAULT_BUTTON_WIDTH;

static ButtonGridIniConfig g_iniConfig;
static int g_iniLoaded = 0;

static GridTesterIniWindowClosedCallback g_onClosed = NULL;

static void GridTesterIni_SetWindowTitle(HWND hwnd)
{
    char title[256];

    wsprintf(
        title,
        "%s - %s - button size %d x %d",
        GRID_TESTER_INI_WINDOW_TITLE,
        GRID_TESTER_INI_FILE_NAME,
        g_squareSize,
        g_squareSize
    );

    SetWindowText(hwnd, title);
}

static void GridTesterIni_Layout(HWND hwnd)
{
    RECT rc;
    int x;
    int y;
    int w;
    int h;

    if (!g_buttonGrid)
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

    ButtonGrid_SetRect(g_buttonGrid, x, y, w, h);
}

static void GridTesterIni_OnButtonClicked(const char *actionName)
{
    int isOn;

    if (!g_buttonGrid || !actionName)
        return;

    isOn = ButtonGrid_GetButtonStateByAction(g_buttonGrid, actionName);

    printf(
        "Grid tester INI clicked: %s is now %s\n",
        actionName,
        isOn ? "ON" : "OFF"
    );

    Debug_Log(
        "GridIni",
        "ButtonClick",
        "%s is now %s",
        actionName,
        isOn ? "ON" : "OFF"
    );
}

static int GridTesterIni_CreateGrid(HWND hwnd)
{
    if (!ButtonGridIni_LoadFromFileOrResource(GRID_TESTER_INI_FILE_NAME, &g_iniConfig))
    {
        AppNotify(
            "Grid Tester INI",
            "Could not load grid_tester.ini from file or embedded resource."
        );

        Debug_Log(
            "GridIni",
            "LoadFailed",
            "Could not load %s from file or embedded resource.",
            GRID_TESTER_INI_FILE_NAME
        );

        return 0;
    }

    g_iniLoaded = 1;
    g_squareSize = g_iniConfig.config.buttonWidth;

    g_buttonGrid = ButtonGrid_CreateEx(
        hwnd,
        g_hInstance,
        GRID_MARGIN,
        GRID_MARGIN,
        400,
        300,
        &g_iniConfig.config,
        GridTesterIni_OnButtonClicked
    );

    if (!g_buttonGrid)
    {
        AppNotify(
            "Grid Tester INI",
            "Could not create INI button grid."
        );

        Debug_Log(
            "GridIni",
            "CreateGridFailed",
            "ButtonGrid_CreateEx failed."
        );

        return 0;
    }

    GridTesterIni_Layout(hwnd);
    GridTesterIni_SetWindowTitle(hwnd);

    Debug_Log(
        "GridIni",
        "GridCreated",
        "Loaded %d buttons from %s",
        g_iniConfig.config.buttonCount,
        GRID_TESTER_INI_FILE_NAME
    );

    return 1;
}

static void GridTesterIni_SetSquareSize(HWND hwnd, int newSize)
{
    if (newSize < 20)
        newSize = 20;

    if (newSize > 300)
        newSize = 300;

    if (g_squareSize == newSize)
        return;

    g_squareSize = newSize;

    if (g_buttonGrid)
        ButtonGrid_SetButtonSize(g_buttonGrid, g_squareSize, g_squareSize);

    GridTesterIni_SetWindowTitle(hwnd);

    printf(
        "Grid tester INI button size changed to %d x %d\n",
        g_squareSize,
        g_squareSize
    );

    Debug_Log(
        "GridIni",
        "ButtonSizeChanged",
        "%d x %d",
        g_squareSize,
        g_squareSize
    );
}

static void GridTesterIni_HandleDpiChanged(HWND hwnd, LPARAM lParam)
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

    GridTesterIni_Layout(hwnd);

    if (g_buttonGrid)
        ButtonGrid_Relayout(g_buttonGrid);
}

static LRESULT CALLBACK GridTesterIniWindowProc(
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

            printf("Grid tester INI window created.\n");

            Debug_Log(
                "GridIni",
                "WindowCreate",
                "Grid tester INI window created."
            );

            if (!GridTesterIni_CreateGrid(hwnd))
                return -1;

            return 0;
        }

        case WM_SIZE:
        {
            GridTesterIni_Layout(hwnd);
            return 0;
        }

        case WM_DPICHANGED:
        {
            GridTesterIni_HandleDpiChanged(hwnd, lParam);
            return 0;
        }

        case WM_KEYDOWN:
        {
            if (wParam == VK_ADD || wParam == VK_OEM_PLUS)
            {
                GridTesterIni_SetSquareSize(hwnd, g_squareSize + 10);
                return 0;
            }

            if (wParam == VK_SUBTRACT || wParam == VK_OEM_MINUS)
            {
                GridTesterIni_SetSquareSize(hwnd, g_squareSize - 10);
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
            GridTesterIniWindowClosedCallback callback;

            printf("Grid tester INI window destroyed.\n");

            Debug_Log(
                "GridIni",
                "WindowDestroy",
                "Grid tester INI window destroyed."
            );

            g_window = NULL;
            g_buttonGrid = NULL;

            if (g_iniLoaded)
            {
                ButtonGridIni_Free(&g_iniConfig);
                g_iniLoaded = 0;
            }

            callback = g_onClosed;
            g_onClosed = NULL;

            if (callback)
                callback();

            return 0;
        }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

static BOOL GridTesterIniWindow_RegisterClass(HINSTANCE hInstance)
{
    static int registered = 0;
    WNDCLASS wc;

    if (registered)
        return TRUE;

    ZeroMemory(&wc, sizeof(wc));

    wc.lpfnWndProc = GridTesterIniWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = GRID_TESTER_INI_WINDOW_CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClass(&wc))
        return FALSE;

    registered = 1;
    return TRUE;
}

HWND GridTesterIniWindow_Show(
    HINSTANCE hInstance,
    GridTesterIniWindowClosedCallback onClosed
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

    if (!GridTesterIniWindow_RegisterClass(hInstance))
        return NULL;

    g_window = CreateWindowEx(
        0,
        GRID_TESTER_INI_WINDOW_CLASS_NAME,
        GRID_TESTER_INI_WINDOW_TITLE,
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

    if (!g_window)
        return NULL;

    ShowWindow(g_window, SW_SHOW);
    UpdateWindow(g_window);

    return g_window;
}

void GridTesterIniWindow_Close(void)
{
    if (g_window && IsWindow(g_window))
        DestroyWindow(g_window);
}

HWND GridTesterIniWindow_GetHwnd(void)
{
    return g_window;
}

int GridTesterIniWindow_IsOpen(void)
{
    return g_window && IsWindow(g_window);
}