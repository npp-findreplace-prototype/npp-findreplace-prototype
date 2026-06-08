#include "main_internal.h"

static void Main_DebugFormatDpiAwareness(char *buffer, int bufferSize, void *userData)
{
    (void)userData;
    Main_CopyText(buffer, bufferSize, g_mainDpiAwarenessStatus);
}

static void Main_DebugFormatGridTesterOpen(char *buffer, int bufferSize, void *userData)
{
    (void)userData;
    Main_CopyText(buffer, bufferSize, GridTesterWindow_IsOpen() ? "true" : "false");
}

static void Main_DebugFormatGridTesterIniOpen(char *buffer, int bufferSize, void *userData)
{
    (void)userData;
    Main_CopyText(buffer, bufferSize, GridTesterIniWindow_IsOpen() ? "true" : "false");
}

static void Main_DebugFormatNewTestLayoutOpen(char *buffer, int bufferSize, void *userData)
{
    (void)userData;
    Main_CopyText(buffer, bufferSize, NewTestLayoutWindow_IsOpen() ? "true" : "false");
}

static void Main_DebugFormatFindReplaceOpen(char *buffer, int bufferSize, void *userData)
{
    (void)userData;
    Main_CopyText(buffer, bufferSize, FindReplaceDialogWindow_IsOpen() ? "true" : "false");
}

static void Main_DebugFormatNppMockupOpen(char *buffer, int bufferSize, void *userData)
{
    (void)userData;
    Main_CopyText(buffer, bufferSize, NppMockupWindow_IsOpen() ? "true" : "false");
}

static void Main_DebugFormatDebugWindowOpen(char *buffer, int bufferSize, void *userData)
{
    (void)userData;
    Main_CopyText(buffer, bufferSize, DebugWindow_IsOpen() ? "true" : "false");
}

static void Main_DebugFormatConsoleVisible(char *buffer, int bufferSize, void *userData)
{
    (void)userData;
    Main_CopyText(buffer, bufferSize, Main_IsConsoleVisible() ? "true" : "false");
}

void Main_RegisterDebugVariables(void)
{
    if (g_mainDebugVariablesRegistered)
        return;

    g_mainDebugVariablesRegistered = 1;

    Debug_RegisterVariable("App", "DPI awareness", Main_DebugFormatDpiAwareness, NULL);

    Debug_RegisterHwndPointer("Main", "main window HWND", &g_mainWindow);
    Debug_RegisterHwndPointer("Main", "grid tester button HWND", &g_gridTesterButton);
    Debug_RegisterHwndPointer("Main", "grid tester ini button HWND", &g_gridTesterIniButton);
    Debug_RegisterHwndPointer("Main", "new test layout button HWND", &g_newTestLayoutButton);
    Debug_RegisterHwndPointer("Main", "find replace button HWND", &g_findReplaceDialogButton);
    Debug_RegisterHwndPointer("Main", "np++ mockup button HWND", &g_nppMockupButton);
    Debug_RegisterHwndPointer("Main", "debug button HWND", &g_debugWindowButton);
    Debug_RegisterHwndPointer("Main", "console button HWND", &g_consoleButton);

    Debug_RegisterVariable("Windows", "grid tester open", Main_DebugFormatGridTesterOpen, NULL);
    Debug_RegisterVariable("Windows", "grid tester ini open", Main_DebugFormatGridTesterIniOpen, NULL);
    Debug_RegisterVariable("Windows", "new test layout open", Main_DebugFormatNewTestLayoutOpen, NULL);
    Debug_RegisterVariable("Windows", "find replace dialog open", Main_DebugFormatFindReplaceOpen, NULL);
    Debug_RegisterVariable("Windows", "np++ mockup open", Main_DebugFormatNppMockupOpen, NULL);
    Debug_RegisterVariable("Windows", "debug window open", Main_DebugFormatDebugWindowOpen, NULL);
    Debug_RegisterVariable("Windows", "console visible", Main_DebugFormatConsoleVisible, NULL);
}