#include "main_internal.h"

HINSTANCE g_main_hInstance = NULL;
HWND g_mainWindow = NULL;

HWND g_gridTesterButton = NULL;
HWND g_gridTesterIniButton = NULL;
HWND g_newTestLayoutButton = NULL;
HWND g_findReplaceDialogButton = NULL;
HWND g_nppMockupButton = NULL;
HWND g_debugWindowButton = NULL;
HWND g_consoleButton = NULL;

char g_mainDpiAwarenessStatus[128] = "DPI awareness: not attempted.";
int g_mainDebugVariablesRegistered = 0;

void Main_CopyText(char *dest, int destSize, const char *src)
{
    if (!dest || destSize <= 0)
        return;

    if (!src)
        src = "";

    lstrcpyn(dest, src, destSize);
    dest[destSize - 1] = '\0';
}

void Main_SetDpiAwarenessStatus(const char *text)
{
    Main_CopyText(
        g_mainDpiAwarenessStatus,
        sizeof(g_mainDpiAwarenessStatus),
        text
    );
}