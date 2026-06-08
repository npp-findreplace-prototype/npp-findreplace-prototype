#include "main_internal.h"

static HMODULE Main_GetKernel32(void)
{
    HMODULE kernel32;

    kernel32 = GetModuleHandle("kernel32.dll");

    if (!kernel32)
        kernel32 = LoadLibrary("kernel32.dll");

    return kernel32;
}

static HWND Main_GetConsoleWindowHandle(void)
{
    HMODULE kernel32;
    HWND (WINAPI *pGetConsoleWindow)(void);

    kernel32 = Main_GetKernel32();

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

static int Main_AllocConsoleDynamic(void)
{
    HMODULE kernel32;
    BOOL (WINAPI *pAllocConsole)(void);

    kernel32 = Main_GetKernel32();

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

static void Main_ReopenConsoleStreams(void)
{
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
    freopen("CONIN$", "r", stdin);
}

static HWND Main_EnsureConsoleWindowHandle(void)
{
    HWND consoleHwnd;

    consoleHwnd = Main_GetConsoleWindowHandle();

    if (consoleHwnd)
        return consoleHwnd;

    if (!Main_AllocConsoleDynamic())
        return NULL;

    Main_ReopenConsoleStreams();

    printf("Console created.\n");

    return Main_GetConsoleWindowHandle();
}

int Main_IsConsoleVisible(void)
{
    HWND consoleHwnd;

    consoleHwnd = Main_GetConsoleWindowHandle();

    if (!consoleHwnd)
        return 0;

    return IsWindowVisible(consoleHwnd) ? 1 : 0;
}

void Main_ShowConsole(int show)
{
    HWND consoleHwnd;

    if (show)
        consoleHwnd = Main_EnsureConsoleWindowHandle();
    else
        consoleHwnd = Main_GetConsoleWindowHandle();

    if (!consoleHwnd)
        return;

    ShowWindow(consoleHwnd, show ? SW_SHOW : SW_HIDE);

    if (show)
        SetForegroundWindow(consoleHwnd);
}