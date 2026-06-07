#include <windows.h>
#include <stdio.h>

#include "console.h"
#include "image_loader.h"

#include "grid_tester_window.h"
#include "grid_tester_ini_window.h"
#include "new_test_layout_window.h"
#include "find_replace_dialog_window.h"
#include "npp_mockup_window.h"
#include "debug_window.h"

#ifndef LOGPIXELSX
#define LOGPIXELSX 88
#endif

#define MAIN_WINDOW_CLASS_NAME "MainLauncherWindowClass"
#define MAIN_WINDOW_TITLE "Window Test Launcher"

#define MAIN_MARGIN 16
#define MAIN_BUTTON_WIDTH 300
#define MAIN_BUTTON_HEIGHT 36
#define MAIN_BUTTON_SPACING 10

#define ID_SHOW_GRID_TESTER_BUTTON 1001
#define ID_SHOW_GRID_TESTER_INI_BUTTON 1002
#define ID_SHOW_NEW_TEST_LAYOUT_BUTTON 1003
#define ID_SHOW_FIND_REPLACE_DIALOG_BUTTON 1004
#define ID_SHOW_NPP_MOCKUP_BUTTON 1005
#define ID_SHOW_DEBUG_WINDOW_BUTTON 1006
#define ID_SHOW_CONSOLE_BUTTON 1007

static HINSTANCE g_hInstance = NULL;
static HWND g_mainWindow = NULL;

static HWND g_gridTesterButton = NULL;
static HWND g_gridTesterIniButton = NULL;
static HWND g_newTestLayoutButton = NULL;
static HWND g_findReplaceDialogButton = NULL;
static HWND g_nppMockupButton = NULL;
static HWND g_debugWindowButton = NULL;
static HWND g_consoleButton = NULL;

static char g_dpiAwarenessStatus[128] = "DPI awareness: not attempted.";
static int g_debugVariablesRegistered = 0;

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

static HMODULE MainWindow_GetKernel32(void)
{
    HMODULE kernel32;

    kernel32 = GetModuleHandle("kernel32.dll");

    if (!kernel32)
        kernel32 = LoadLibrary("kernel32.dll");

    return kernel32;
}

static HWND MainWindow_GetConsoleWindowHandle(void)
{
    HMODULE kernel32;
    HWND (WINAPI *pGetConsoleWindow)(void);

    kernel32 = MainWindow_GetKernel32();

    if (!kernel32)
        return NULL;

    pGetConsoleWindow =
        (HWND (WINAPI *)(void))GetProcAddress(
            kernel32,
            "GetConsoleWindow"
        );

    if (!pGetConsoleWindow)
        return NULL;

    return pGetConsoleWindow();
}

static int MainWindow_AllocConsoleDynamic(void)
{
    HMODULE kernel32;
    BOOL (WINAPI *pAllocConsole)(void);

    kernel32 = MainWindow_GetKernel32();

    if (!kernel32)
        return 0;

    pAllocConsole =
        (BOOL (WINAPI *)(void))GetProcAddress(
            kernel32,
            "AllocConsole"
        );

    if (!pAllocConsole)
        return 0;

    return pAllocConsole() ? 1 : 0;
}

static void MainWindow_ReopenConsoleStreams(void)
{
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
    freopen("CONIN$", "r", stdin);
}

static HWND MainWindow_EnsureConsoleWindowHandle(void)
{
    HWND consoleHwnd;

    consoleHwnd = MainWindow_GetConsoleWindowHandle();

    if (consoleHwnd)
        return consoleHwnd;

    if (!MainWindow_AllocConsoleDynamic())
        return NULL;

    MainWindow_ReopenConsoleStreams();

    printf("Console created.\n");

    return MainWindow_GetConsoleWindowHandle();
}

static int MainWindow_IsConsoleVisible(void)
{
    HWND consoleHwnd;

    consoleHwnd = MainWindow_GetConsoleWindowHandle();

    if (!consoleHwnd)
        return 0;

    return IsWindowVisible(consoleHwnd) ? 1 : 0;
}

static void MainWindow_ShowConsole(int show)
{
    HWND consoleHwnd;

    if (show)
        consoleHwnd = MainWindow_EnsureConsoleWindowHandle();
    else
        consoleHwnd = MainWindow_GetConsoleWindowHandle();

    if (!consoleHwnd)
        return;

    ShowWindow(consoleHwnd, show ? SW_SHOW : SW_HIDE);

    if (show)
        SetForegroundWindow(consoleHwnd);
}

static void Debug_FormatDpiAwareness(char *buffer, int bufferSize, void *userData)
{
    (void)userData;
    App_CopyText(buffer, bufferSize, g_dpiAwarenessStatus);
}

static void Debug_FormatGridTesterOpen(char *buffer, int bufferSize, void *userData)
{
    (void)userData;
    App_CopyText(buffer, bufferSize, GridTesterWindow_IsOpen() ? "true" : "false");
}

static void Debug_FormatGridTesterIniOpen(char *buffer, int bufferSize, void *userData)
{
    (void)userData;
    App_CopyText(buffer, bufferSize, GridTesterIniWindow_IsOpen() ? "true" : "false");
}

static void Debug_FormatNewTestLayoutOpen(char *buffer, int bufferSize, void *userData)
{
    (void)userData;
    App_CopyText(buffer, bufferSize, NewTestLayoutWindow_IsOpen() ? "true" : "false");
}

static void Debug_FormatFindReplaceOpen(char *buffer, int bufferSize, void *userData)
{
    (void)userData;
    App_CopyText(buffer, bufferSize, FindReplaceDialogWindow_IsOpen() ? "true" : "false");
}

static void Debug_FormatNppMockupOpen(char *buffer, int bufferSize, void *userData)
{
    (void)userData;
    App_CopyText(buffer, bufferSize, NppMockupWindow_IsOpen() ? "true" : "false");
}

static void Debug_FormatDebugWindowOpen(char *buffer, int bufferSize, void *userData)
{
    (void)userData;
    App_CopyText(buffer, bufferSize, DebugWindow_IsOpen() ? "true" : "false");
}

static void Debug_FormatConsoleVisible(char *buffer, int bufferSize, void *userData)
{
    (void)userData;
    App_CopyText(buffer, bufferSize, MainWindow_IsConsoleVisible() ? "true" : "false");
}

static void MainWindow_RegisterDebugVariables(void)
{
    if (g_debugVariablesRegistered)
        return;

    g_debugVariablesRegistered = 1;

    Debug_RegisterVariable("App", "DPI awareness", Debug_FormatDpiAwareness, NULL);

    Debug_RegisterHwndPointer("Main", "main window HWND", &g_mainWindow);
    Debug_RegisterHwndPointer("Main", "grid tester button HWND", &g_gridTesterButton);
    Debug_RegisterHwndPointer("Main", "grid tester ini button HWND", &g_gridTesterIniButton);
    Debug_RegisterHwndPointer("Main", "new test layout button HWND", &g_newTestLayoutButton);
    Debug_RegisterHwndPointer("Main", "find replace button HWND", &g_findReplaceDialogButton);
    Debug_RegisterHwndPointer("Main", "np++ mockup button HWND", &g_nppMockupButton);
    Debug_RegisterHwndPointer("Main", "debug button HWND", &g_debugWindowButton);
    Debug_RegisterHwndPointer("Main", "console button HWND", &g_consoleButton);

    Debug_RegisterVariable("Windows", "grid tester open", Debug_FormatGridTesterOpen, NULL);
    Debug_RegisterVariable("Windows", "grid tester ini open", Debug_FormatGridTesterIniOpen, NULL);
    Debug_RegisterVariable("Windows", "new test layout open", Debug_FormatNewTestLayoutOpen, NULL);
    Debug_RegisterVariable("Windows", "find replace dialog open", Debug_FormatFindReplaceOpen, NULL);
    Debug_RegisterVariable("Windows", "np++ mockup open", Debug_FormatNppMockupOpen, NULL);
    Debug_RegisterVariable("Windows", "debug window open", Debug_FormatDebugWindowOpen, NULL);
    Debug_RegisterVariable("Windows", "console visible", Debug_FormatConsoleVisible, NULL);
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

    if (g_gridTesterIniButton)
    {
        SetWindowText(
            g_gridTesterIniButton,
            GridTesterIniWindow_IsOpen() ? "Hide grid tester (ini)" : "Show grid tester (ini)"
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

    if (g_debugWindowButton)
    {
        SetWindowText(
            g_debugWindowButton,
            DebugWindow_IsOpen() ? "Hide debug window" : "Show debug window"
        );
    }

    if (g_consoleButton)
    {
        SetWindowText(
            g_consoleButton,
            MainWindow_IsConsoleVisible() ? "Hide console" : "Show console"
        );
    }
}

static void OnChildWindowClosed(void)
{
    Debug_Log("Main", "ChildWindowClosed", "A child/test window closed.");
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

    g_gridTesterButton = MainWindow_CreateToggleButton(hwnd, ID_SHOW_GRID_TESTER_BUTTON, y);
    y += MAIN_BUTTON_HEIGHT + MAIN_BUTTON_SPACING;

    g_gridTesterIniButton = MainWindow_CreateToggleButton(hwnd, ID_SHOW_GRID_TESTER_INI_BUTTON, y);
    y += MAIN_BUTTON_HEIGHT + MAIN_BUTTON_SPACING;

    g_newTestLayoutButton = MainWindow_CreateToggleButton(hwnd, ID_SHOW_NEW_TEST_LAYOUT_BUTTON, y);
    y += MAIN_BUTTON_HEIGHT + MAIN_BUTTON_SPACING;

    g_findReplaceDialogButton = MainWindow_CreateToggleButton(hwnd, ID_SHOW_FIND_REPLACE_DIALOG_BUTTON, y);
    y += MAIN_BUTTON_HEIGHT + MAIN_BUTTON_SPACING;

    g_nppMockupButton = MainWindow_CreateToggleButton(hwnd, ID_SHOW_NPP_MOCKUP_BUTTON, y);
    y += MAIN_BUTTON_HEIGHT + MAIN_BUTTON_SPACING;

    g_debugWindowButton = MainWindow_CreateToggleButton(hwnd, ID_SHOW_DEBUG_WINDOW_BUTTON, y);
    y += MAIN_BUTTON_HEIGHT + MAIN_BUTTON_SPACING;

    g_consoleButton = MainWindow_CreateToggleButton(hwnd, ID_SHOW_CONSOLE_BUTTON, y);

    MainWindow_UpdateButtons();
}

static void MainWindow_SetButtonRect(HWND button, int x, int y, int width)
{
    if (!button)
        return;

    SetWindowPos(
        button,
        NULL,
        x,
        y,
        width,
        MAIN_BUTTON_HEIGHT,
        SWP_NOZORDER | SWP_NOACTIVATE
    );
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

    MainWindow_SetButtonRect(g_gridTesterButton, x, y, w);
    y += MAIN_BUTTON_HEIGHT + MAIN_BUTTON_SPACING;

    MainWindow_SetButtonRect(g_gridTesterIniButton, x, y, w);
    y += MAIN_BUTTON_HEIGHT + MAIN_BUTTON_SPACING;

    MainWindow_SetButtonRect(g_newTestLayoutButton, x, y, w);
    y += MAIN_BUTTON_HEIGHT + MAIN_BUTTON_SPACING;

    MainWindow_SetButtonRect(g_findReplaceDialogButton, x, y, w);
    y += MAIN_BUTTON_HEIGHT + MAIN_BUTTON_SPACING;

    MainWindow_SetButtonRect(g_nppMockupButton, x, y, w);
    y += MAIN_BUTTON_HEIGHT + MAIN_BUTTON_SPACING;

    MainWindow_SetButtonRect(g_debugWindowButton, x, y, w);
    y += MAIN_BUTTON_HEIGHT + MAIN_BUTTON_SPACING;

    MainWindow_SetButtonRect(g_consoleButton, x, y, w);
}

static void ToggleGridTesterWindow(void)
{
    if (GridTesterWindow_IsOpen())
    {
        Debug_Log("Main", "HideGridTester", "Closing grid tester.");
        GridTesterWindow_Close();
    }
    else
    {
        Debug_Log("Main", "ShowGridTester", "Opening grid tester.");
        GridTesterWindow_Show(g_hInstance, OnChildWindowClosed);
    }

    MainWindow_UpdateButtons();
}

static void ToggleGridTesterIniWindow(void)
{
    if (GridTesterIniWindow_IsOpen())
    {
        Debug_Log("Main", "HideGridTesterIni", "Closing grid tester INI.");
        GridTesterIniWindow_Close();
    }
    else
    {
        Debug_Log("Main", "ShowGridTesterIni", "Opening grid tester INI.");
        GridTesterIniWindow_Show(g_hInstance, OnChildWindowClosed);
    }

    MainWindow_UpdateButtons();
}

static void ToggleNewTestLayoutWindow(void)
{
    if (NewTestLayoutWindow_IsOpen())
    {
        Debug_Log("Main", "HideNewTestLayout", "Closing new test layout.");
        NewTestLayoutWindow_Close();
    }
    else
    {
        Debug_Log("Main", "ShowNewTestLayout", "Opening new test layout.");
        NewTestLayoutWindow_Show(g_hInstance, OnChildWindowClosed);
    }

    MainWindow_UpdateButtons();
}

static void ToggleFindReplaceDialogWindow(void)
{
    if (FindReplaceDialogWindow_IsOpen())
    {
        Debug_Log("Main", "HideFindReplace", "Closing find and replace dialog.");
        FindReplaceDialogWindow_Close();
    }
    else
    {
        Debug_Log("Main", "ShowFindReplace", "Opening find and replace dialog.");
        FindReplaceDialogWindow_Show(g_hInstance, OnChildWindowClosed);
    }

    MainWindow_UpdateButtons();
}

static void ToggleNppMockupWindow(void)
{
    if (NppMockupWindow_IsOpen())
    {
        Debug_Log("Main", "HideNppMockup", "Closing NP++ mockup.");
        NppMockupWindow_Close();
    }
    else
    {
        Debug_Log("Main", "ShowNppMockup", "Opening NP++ mockup.");
        NppMockupWindow_Show(g_hInstance, OnChildWindowClosed);
    }

    MainWindow_UpdateButtons();
}

static void ToggleDebugWindow(void)
{
    if (DebugWindow_IsOpen())
    {
        Debug_Log("Main", "HideDebugWindow", "Closing debug window.");
        DebugWindow_Close();
    }
    else
    {
        Debug_Log("Main", "ShowDebugWindow", "Opening debug window.");
        DebugWindow_Show(g_hInstance, OnChildWindowClosed);
    }

    MainWindow_UpdateButtons();
}

static void ToggleConsoleWindow(void)
{
    if (MainWindow_IsConsoleVisible())
    {
        Debug_Log("Main", "HideConsole", "Hiding console window.");
        MainWindow_ShowConsole(0);
    }
    else
    {
        MainWindow_ShowConsole(1);
        Debug_Log("Main", "ShowConsole", "Showing console window.");
    }

    MainWindow_UpdateButtons();
}

static void MainWindow_CloseChildWindows(void)
{
    GridTesterWindow_Close();
    GridTesterIniWindow_Close();
    NewTestLayoutWindow_Close();
    FindReplaceDialogWindow_Close();
    NppMockupWindow_Close();
    DebugWindow_Close();
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
            MainWindow_RegisterDebugVariables();

            printf("Main launcher window created.\n");
            Debug_Log("Main", "WindowCreate", "Main launcher window created.");

            return 0;
        }

        case WM_SIZE:
        {
            MainWindow_LayoutButtons(hwnd);
            return 0;
        }

        case WM_ACTIVATE:
        case WM_SETFOCUS:
        {
            MainWindow_UpdateButtons();
            break;
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

                if (controlId == ID_SHOW_GRID_TESTER_INI_BUTTON)
                {
                    ToggleGridTesterIniWindow();
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

                if (controlId == ID_SHOW_DEBUG_WINDOW_BUTTON)
                {
                    ToggleDebugWindow();
                    return 0;
                }

                if (controlId == ID_SHOW_CONSOLE_BUTTON)
                {
                    ToggleConsoleWindow();
                    return 0;
                }
            }

            break;
        }

        case WM_DESTROY:
        {
            Debug_Log("Main", "WindowDestroy", "Main launcher window destroyed.");

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

    hwnd = GridTesterIniWindow_GetHwnd();

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

    hwnd = DebugWindow_GetHwnd();

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
        390,
        420,
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

    Debug_Log("App", "Startup", "Application started.");

    printf("Application started.\n");
    printf("Use the launcher buttons to show or hide test windows.\n");

    MainWindow_UpdateButtons();

    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (App_IsDialogMessageForOpenWindow(&msg))
            continue;

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    Debug_Log("App", "Shutdown", "Application exiting.");

    printf("Application exiting.\n");

    ImageLoader_Shutdown();

    return 0;
}