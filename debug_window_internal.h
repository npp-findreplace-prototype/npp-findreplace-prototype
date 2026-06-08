#ifndef DEBUG_WINDOW_INTERNAL_H
#define DEBUG_WINDOW_INTERNAL_H

#include "win_compat.h"

#include <richedit.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "debug_window.h"

#ifndef MSFTEDIT_CLASSA
#define MSFTEDIT_CLASSA "RICHEDIT50A"
#endif

#ifndef EM_SETBKGNDCOLOR
#define EM_SETBKGNDCOLOR (WM_USER + 67)
#endif

#ifndef EM_GETFIRSTVISIBLELINE
#define EM_GETFIRSTVISIBLELINE 0x00CE
#endif

#ifndef EM_LINESCROLL
#define EM_LINESCROLL 0x00B6
#endif

#define DEBUG_WINDOW_CLASS_NAME "DebugWindowClass"
#define DEBUG_WINDOW_TITLE "Debug Window"

#define DEBUG_TIMER_ID 1
#define DEBUG_TIMER_MS 250

#define DEBUG_MAX_VARIABLES 256
#define DEBUG_MAX_LOG_ENTRIES 4096

#define DEBUG_ASPECT_SIZE 64
#define DEBUG_NAME_SIZE 128
#define DEBUG_VALUE_SIZE 512
#define DEBUG_LOG_TEXT_SIZE 1024

#define ID_DEBUG_UPDATE_BUTTON 2001
#define ID_DEBUG_LOGGING_BUTTON 2002
#define ID_DEBUG_CLEAR_LOG_BUTTON 2003
#define ID_DEBUG_DUMP_FILTER_EDIT 2004
#define ID_DEBUG_LOG_FILTER_EDIT 2005
#define ID_DEBUG_AVAILABLE_LIST 2006
#define ID_DEBUG_ADD_VARIABLE_BUTTON 2007
#define ID_DEBUG_REMOVE_VARIABLE_BUTTON 2008
#define ID_DEBUG_SHOW_ALL_VARIABLES_BUTTON 2009
#define ID_DEBUG_HIDE_ALL_VARIABLES_BUTTON 2010

typedef struct DebugVariable
{
    char aspect[DEBUG_ASPECT_SIZE];
    char name[DEBUG_NAME_SIZE];
    DebugVariableValueCallback callback;
    void *userData;
    int active;
} DebugVariable;

typedef struct DebugLogEntry
{
    char aspect[DEBUG_ASPECT_SIZE];
    char eventName[DEBUG_NAME_SIZE];
    char text[DEBUG_LOG_TEXT_SIZE];
    COLORREF color;
} DebugLogEntry;

extern HWND g_window;
extern HWND g_updateButton;
extern HWND g_loggingButton;
extern HWND g_clearLogButton;
extern HWND g_dumpFilterEdit;
extern HWND g_dumpEdit;

extern HWND g_availableList;
extern HWND g_addVariableButton;
extern HWND g_removeVariableButton;
extern HWND g_showAllVariablesButton;
extern HWND g_hideAllVariablesButton;

extern HWND g_logFilterEdit;
extern HWND g_logEdit;

extern HINSTANCE g_hInstance;
extern DebugWindowClosedCallback g_onClosed;

extern HMODULE g_richEditModule;
extern int g_logIsRichEdit;

extern int g_updatesEnabled;
extern int g_loggingEnabled;
extern int g_controlsUpdating;

extern DebugVariable g_variables[DEBUG_MAX_VARIABLES];
extern int g_variableCount;

extern DebugLogEntry g_logEntries[DEBUG_MAX_LOG_ENTRIES];
extern int g_logCount;

void Debug_CopyText(
    char *dest,
    int destSize,
    const char *src
);

void Debug_AppendText(
    char *dest,
    int destSize,
    const char *src
);

int Debug_TextContainsNoCase(
    const char *text,
    const char *filter
);

void Debug_GetWindowTextSafe(
    HWND hwnd,
    char *buffer,
    int bufferSize
);

void Debug_SetEditTextPreserveSelection(
    HWND hwnd,
    const char *text
);

void Debug_CreateControls(
    HWND hwnd
);

void Debug_UpdateButtons(void);

void Debug_LayoutControls(
    HWND hwnd
);

void Debug_UpdateDumpView(void);

void Debug_UpdateAvailableList(void);

void Debug_AddSelectedAvailableVariable(void);

void Debug_RemoveSelectedAvailableVariable(void);

void Debug_RebuildLogView(void);

#endif