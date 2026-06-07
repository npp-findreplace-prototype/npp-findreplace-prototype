#include <windows.h>
#include <stdio.h>

#include "console.h"
#include "image_loader.h"

#include "grid_tester_window.h"
#include "new_test_layout_window.h"
#include "find_replace_dialog_window.h"
#include "npp_mockup_window.h"

#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0
#endif

#ifndef LOGPIXELSX
#define LOGPIXELSX 88
#endif

#define MAIN_WINDOW_CLASS_NAME "MainLauncherWindowClass"
#define MAIN_WINDOW_TITLE "Window Test Launcher"

#define MAIN_MARGIN 16
#define MAIN_BUTTON_WIDTH 260
#define MAIN_BUTTON_HEIGHT 36
#define MAIN_BUTTON_SPACING 10

#define ID_SHOW_GRID_TESTER_BUTTON 1001
#define ID_SHOW_NEW_TEST_LAYOUT_BUTTON 1002
#define ID_SHOW_FIND_REPLACE_DIALOG_BUTTON 1003
#define ID_SHOW_NPP_MOCKUP_BUTTON 1004

static HINSTANCE g_hInstance = NULL;
static HWND g_mainWindow = NULL;

static HWND g_gridTesterButton = NULL;
static HWND g_newTestLayoutButton = NULL;
static HWND g_findReplaceDialogButton = NULL;
static HWND g_nppMockupButton = NULL;

static char g_dpiAwarenessStatus[128] = "DPI awareness: not attempted.";

static void App_CopyText(char *dest, int destSize, const char *src)
{
    if (!dest || destSize <= 0)
        return;

    if (!src)
        src = "";

    lstrcpyn(dest, src, destSize);
    dest[destSize - 1] = '\0';
}

static void App_SetDpiAwarenessStatus(const char *text)
{
    App_CopyText(
        g_dpiAwarenessStatus,
        sizeof(g_dpiAwarenessStatus),
        text
    );
}

static void App_EnableDpiAwareness(void)
{
    HMODULE user32;
    HMODULE shcore;

    BOOL (WINAPI *setProcessDpiAwarenessContext)(HANDLE);
    HRESULT (WINAPI *setProcessDpiAwareness)(int);
    BOOL (WINAPI *setProcessDpiAware)(void);

    App_SetDpiAwarenessStatus("DPI awareness: unavailable or already fixed by Windows.");

    user32 = LoadLibrary("user32.dll");

    if (user32)
    {
        setProcessDpiAwarenessContext =
            (BOOL (WINAPI *)(HANDLE))GetProcAddress(
                user32,
                "SetProcessDpiAwarenessContext"
            );

        if (setProcessDpiAwarenessContext)
        {
            if (setProcessDpiAwarenessContext((HANDLE)-4))
            {
                App_SetDpiAwarenessStatus("DPI awareness: Per-monitor V2.");
                FreeLibrary(user32);
                return;
            }

            if (setProcessDpiAwarenessContext((HANDLE)-3))
            {
                App_SetDpiAwarenessStatus("DPI awareness: Per-monitor V1.");
                FreeLibrary(user32);
                return;
            }

            if (setProcessDpiAwarenessContext((HANDLE)-2))
            {
                App_SetDpiAwarenessStatus("DPI awareness: System aware.");
                FreeLibrary(user32);
                return;
            }
        }

        FreeLibrary(user32);
    }

    shcore = LoadLibrary("shcore.dll");

    if (shcore)
    {
        setProcessDpiAwareness =
            (HRESULT (WINAPI *)(int))GetProcAddress(
                shcore,
                "SetProcessDpiAwareness"
            );

        if (setProcessDpiAwareness)
        {
            if (setProcessDpiAwareness(2) == 0)
            {
                App_SetDpiAwarenessStatus("DPI awareness: shcore per-monitor.");
                FreeLibrary(shcore);
                return;
            }

            if (setProcessDpiAwareness(1) == 0)
            {
                App_SetDpiAwarenessStatus("DPI awareness: shcore system aware.");
                FreeLibrary(shcore);
                return;
            }
        }

        FreeLibrary(shcore);
    }

    user32 = LoadLibrary("user32.dll");

    if (user32)
    {
        setProcessDpiAware =
            (BOOL (WINAPI *)(void))GetProcAddress(
                user32,
                "SetProcessDPIAware"
            );

        if (setProcessDpiAware && setProcessDpiAware())
            App_SetDpiAwarenessStatus("DPI awareness: legacy system aware.");

        FreeLibrary(user32);
    }
}

static void MainWindow_UpdateButtons(void)
{
    if (!g_mainWindow)
        return;

    if (g_gridTesterButton)
    {
        SetWindowText(
            g_gridTesterButton,
            GridTesterWindow_IsOpen() ? "Hide grid tester" : "Show grid tester"
        );
    }

    if (g_newTestLayoutButton)
    {
        SetWindowText(
            g_newTestLayoutButton,
            NewTestLayoutWindow_IsOpen() ? "Hide new test layout" : "Show new test layout"
        );
    }

    if (g_findReplaceDialogButton)
    {
        SetWindowText(
            g_findReplaceDialogButton,
            FindReplaceDialogWindow_IsOpen() ? "Hide find and replace dialog" : "Show find and replace dialog"
        );
    }

    if (g_nppMockupButton)
    {
        SetWindowText(
            g_nppMockupButton,
            NppMockupWindow_IsOpen() ? "Hide NP++ mockup" : "Show NP++ mockup"
        );
    }
}

static void OnChildWindowClosed(void)
{
    MainWindow_UpdateButtons();
}

static HWND MainWindow_CreateToggleButton(HWND parent, int id, int y)
{
    return CreateWindowEx(
        0,
        "BUTTON",
        "",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
        MAIN_MARGIN,
        y,
        MAIN_BUTTON_WIDTH,
        MAIN_BUTTON_HEIGHT,
        parent,
        (HMENU)id,
        g_hInstance,
        NULL
    );
}

static void MainWindow_CreateButtons(HWND hwnd)
{
    int y;

    y = MAIN_MARGIN;

    g_gridTesterButton = MainWindow_CreateToggleButton(
        hwnd,
        ID_SHOW_GRID_TESTER_BUTTON,
        y
    );

    y += MAIN_BUTTON_HEIGHT + MAIN_BUTTON_SPACING;

    g_newTestLayoutButton = MainWindow_CreateToggleButton(
        hwnd,
        ID_SHOW_NEW_TEST_LAYOUT_BUTTON,
        y
    );

    y += MAIN_BUTTON_HEIGHT + MAIN_BUTTON_SPACING;

    g_findReplaceDialogButton = MainWindow_CreateToggleButton(
        hwnd,
        ID_SHOW_FIND_REPLACE_DIALOG_BUTTON,
        y
    );

    y += MAIN_BUTTON_HEIGHT + MAIN_BUTTON_SPACING;

    g_nppMockupButton = MainWindow_CreateToggleButton(
        hwnd,
        ID_SHOW_NPP_MOCKUP_BUTTON,
        y
    );

    MainWindow_UpdateButtons();
}

static void MainWindow_LayoutButtons(HWND hwnd)
{
    RECT rc;
    int x;
    int y;
    int w;

    GetClientRect(hwnd, &rc);

    x = MAIN_MARGIN;
    y = MAIN_MARGIN;

    w = rc.right - rc.left - (MAIN_MARGIN * 2);

    if (w > MAIN_BUTTON_WIDTH)
        w = MAIN_BUTTON_WIDTH;

    if (w < 80)
        w = 80;

    if (g_gridTesterButton)
    {
        SetWindowPos(
            g_gridTesterButton,
            NULL,
            x,
            y,
            w,
            MAIN_BUTTON_HEIGHT,
            SWP_NOZORDER | SWP_NOACTIVATE
        );
    }

    y += MAIN_BUTTON_HEIGHT + MAIN_BUTTON_SPACING;

    if (g_newTestLayoutButton)
    {
        SetWindowPos(
            g_newTestLayoutButton,
            NULL,
            x,
            y,
            w,
            MAIN_BUTTON_HEIGHT,
            SWP_NOZORDER | SWP_NOACTIVATE
        );
    }

    y += MAIN_BUTTON_HEIGHT + MAIN_BUTTON_SPACING;

    if (g_findReplaceDialogButton)
    {
        SetWindowPos(
            g_findReplaceDialogButton,
            NULL,
            x,
            y,
            w,
            MAIN_BUTTON_HEIGHT,
            SWP_NOZORDER | SWP_NOACTIVATE
        );
    }

    y += MAIN_BUTTON_HEIGHT + MAIN_BUTTON_SPACING;

    if (g_nppMockupButton)
    {
        SetWindowPos(
            g_nppMockupButton,
            NULL,
            x,
            y,
            w,
            MAIN_BUTTON_HEIGHT,
            SWP_NOZORDER | SWP_NOACTIVATE
        );
    }
}

static void ToggleGridTesterWindow(void)
{
    if (GridTesterWindow_IsOpen())
        GridTesterWindow_Close();
    else
        GridTesterWindow_Show(g_hInstance, OnChildWindowClosed);

    MainWindow_UpdateButtons();
}

static void ToggleNewTestLayoutWindow(void)
{
    if (NewTestLayoutWindow_IsOpen())
        NewTestLayoutWindow_Close();
    else
        NewTestLayoutWindow_Show(g_hInstance, OnChildWindowClosed);

    MainWindow_UpdateButtons();
}

static void ToggleFindReplaceDialogWindow(void)
{
    if (FindReplaceDialogWindow_IsOpen())
        FindReplaceDialogWindow_Close();
    else
        FindReplaceDialogWindow_Show(g_hInstance, OnChildWindowClosed);

    MainWindow_UpdateButtons();
}

static void ToggleNppMockupWindow(void)
{
    if (NppMockupWindow_IsOpen())
        NppMockupWindow_Close();
    else
        NppMockupWindow_Show(g_hInstance, OnChildWindowClosed);

    MainWindow_UpdateButtons();
}

static void MainWindow_CloseChildWindows(void)
{
    GridTesterWindow_Close();
    NewTestLayoutWindow_Close();
    FindReplaceDialogWindow_Close();
    NppMockupWindow_Close();
}

static LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_CREATE:
        {
            CREATESTRUCT *cs;

            cs = (CREATESTRUCT *)lParam;

            g_hInstance = cs->hInstance;
            g_mainWindow = hwnd;

            MainWindow_CreateButtons(hwnd);
            MainWindow_LayoutButtons(hwnd);

            printf("Main launcher window created.\n");

            return 0;
        }

        case WM_SIZE:
        {
            MainWindow_LayoutButtons(hwnd);
            return 0;
        }

        case WM_COMMAND:
        {
            int controlId;
            int notifyCode;

            controlId = LOWORD(wParam);
            notifyCode = HIWORD(wParam);

            if (notifyCode == BN_CLICKED)
            {
                if (controlId == ID_SHOW_GRID_TESTER_BUTTON)
                {
                    ToggleGridTesterWindow();
                    return 0;
                }

                if (controlId == ID_SHOW_NEW_TEST_LAYOUT_BUTTON)
                {
                    ToggleNewTestLayoutWindow();
                    return 0;
                }

                if (controlId == ID_SHOW_FIND_REPLACE_DIALOG_BUTTON)
                {
                    ToggleFindReplaceDialogWindow();
                    return 0;
                }

                if (controlId == ID_SHOW_NPP_MOCKUP_BUTTON)
                {
                    ToggleNppMockupWindow();
                    return 0;
                }
            }

            break;
        }

        case WM_DESTROY:
        {
            MainWindow_CloseChildWindows();

            g_mainWindow = NULL;

            printf("Main launcher window destroyed.\n");

            PostQuitMessage(0);
            return 0;
        }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

static BOOL MainWindow_RegisterClass(HINSTANCE hInstance)
{
    WNDCLASS wc;

    ZeroMemory(&wc, sizeof(wc));

    wc.lpfnWndProc = MainWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = MAIN_WINDOW_CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    return RegisterClass(&wc) ? TRUE : FALSE;
}

static int App_IsDialogMessageForOpenWindow(MSG *msg)
{
    HWND hwnd;

    hwnd = GridTesterWindow_GetHwnd();

    if (hwnd && IsWindow(hwnd) && IsDialogMessage(hwnd, msg))
        return 1;

    hwnd = NewTestLayoutWindow_GetHwnd();

    if (hwnd && IsWindow(hwnd) && IsDialogMessage(hwnd, msg))
        return 1;

    hwnd = FindReplaceDialogWindow_GetHwnd();

    if (hwnd && IsWindow(hwnd) && IsDialogMessage(hwnd, msg))
        return 1;

    hwnd = NppMockupWindow_GetHwnd();

    if (hwnd && IsWindow(hwnd) && IsDialogMessage(hwnd, msg))
        return 1;

    hwnd = g_mainWindow;

    if (hwnd && IsWindow(hwnd) && IsDialogMessage(hwnd, msg))
        return 1;

    return 0;
}

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow
)
{
    HWND hwnd;
    MSG msg;

    (void)hPrevInstance;
    (void)lpCmdLine;

    App_EnableDpiAwareness();

    Console_Setup();

    printf("%s\n", g_dpiAwarenessStatus);

    if (!ImageLoader_Startup())
        printf("GDI+ image loader unavailable. Falling back to generated button art.\n");

    if (!MainWindow_RegisterClass(hInstance))
    {
        AppNotify("Error", "Could not register main launcher window class.");
        ImageLoader_Shutdown();
        return 0;
    }

    hwnd = CreateWindowEx(
        0,
        MAIN_WINDOW_CLASS_NAME,
        MAIN_WINDOW_TITLE,
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        340,
        260,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hwnd)
    {
        AppNotify("Error", "Could not create main launcher window.");
        ImageLoader_Shutdown();
        return 0;
    }

    g_mainWindow = hwnd;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    printf("Application started.\n");
    printf("Use the launcher buttons to show or hide test windows.\n");

    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (App_IsDialogMessageForOpenWindow(&msg))
            continue;

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    printf("Application exiting.\n");

    ImageLoader_Shutdown();

    return 0;
}