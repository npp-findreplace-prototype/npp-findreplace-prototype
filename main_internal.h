#ifndef MAIN_INTERNAL_H
#define MAIN_INTERNAL_H

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

extern HINSTANCE g_main_hInstance;
extern HWND g_mainWindow;

extern HWND g_gridTesterButton;
extern HWND g_gridTesterIniButton;
extern HWND g_newTestLayoutButton;
extern HWND g_findReplaceDialogButton;
extern HWND g_nppMockupButton;
extern HWND g_debugWindowButton;
extern HWND g_consoleButton;

extern char g_mainDpiAwarenessStatus[128];
extern int g_mainDebugVariablesRegistered;

void Main_CopyText(
    char *dest,
    int destSize,
    const char *src
);

void Main_SetDpiAwarenessStatus(
    const char *text
);

void Main_EnableDpiAwareness(void);

int Main_IsConsoleVisible(void);

void Main_ShowConsole(
    int show
);

void Main_RegisterDebugVariables(void);

void Main_UpdateButtons(void);

void Main_CreateButtons(
    HWND hwnd
);

void Main_LayoutButtons(
    HWND hwnd
);

void Main_OnChildWindowClosed(void);

void Main_ToggleGridTesterWindow(void);

void Main_ToggleGridTesterIniWindow(void);

void Main_ToggleNewTestLayoutWindow(void);

void Main_ToggleFindReplaceDialogWindow(void);

void Main_ToggleNppMockupWindow(void);

void Main_ToggleDebugWindow(void);

void Main_ToggleConsoleWindow(void);

void Main_CloseChildWindows(void);

BOOL MainWindow_RegisterClass(
    HINSTANCE hInstance
);

int Main_IsDialogMessageForOpenWindow(
    MSG *msg
);

#endif