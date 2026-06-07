#include <windows.h>
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

static HWND g_window = NULL;
static HWND g_updateButton = NULL;
static HWND g_loggingButton = NULL;
static HWND g_clearLogButton = NULL;
static HWND g_dumpFilterEdit = NULL;
static HWND g_dumpEdit = NULL;

static HWND g_availableList = NULL;
static HWND g_addVariableButton = NULL;
static HWND g_removeVariableButton = NULL;
static HWND g_showAllVariablesButton = NULL;
static HWND g_hideAllVariablesButton = NULL;

static HWND g_logFilterEdit = NULL;
static HWND g_logEdit = NULL;

static HINSTANCE g_hInstance = NULL;
static DebugWindowClosedCallback g_onClosed = NULL;

static HMODULE g_richEditModule = NULL;
static int g_logIsRichEdit = 0;

static int g_updatesEnabled = 1;
static int g_loggingEnabled = 1;
static int g_controlsUpdating = 0;

static DebugVariable g_variables[DEBUG_MAX_VARIABLES];
static int g_variableCount = 0;

static DebugLogEntry g_logEntries[DEBUG_MAX_LOG_ENTRIES];
static int g_logCount = 0;

static void Debug_CopyText(char *dest, int destSize, const char *src)
{
    if (!dest || destSize <= 0)
        return;

    if (!src)
        src = "";

    lstrcpyn(dest, src, destSize);
    dest[destSize - 1] = '\0';
}

static void Debug_AppendText(char *dest, int destSize, const char *src)
{
    int len;

    if (!dest || destSize <= 0 || !src)
        return;

    len = lstrlen(dest);

    if (len >= destSize - 1)
        return;

    lstrcpyn(dest + len, src, destSize - len);
    dest[destSize - 1] = '\0';
}

static int Debug_ToLowerChar(int c)
{
    if (c >= 'A' && c <= 'Z')
        return c + ('a' - 'A');

    return c;
}

static int Debug_TextContainsNoCase(const char *text, const char *filter)
{
    int i;
    int j;
    int textLen;
    int filterLen;

    if (!filter || !filter[0])
        return 1;

    if (!text)
        return 0;

    textLen = lstrlen(text);
    filterLen = lstrlen(filter);

    if (filterLen <= 0)
        return 1;

    if (filterLen > textLen)
        return 0;

    for (i = 0; i <= textLen - filterLen; i++)
    {
        for (j = 0; j < filterLen; j++)
        {
            if (Debug_ToLowerChar(text[i + j]) != Debug_ToLowerChar(filter[j]))
                break;
        }

        if (j == filterLen)
            return 1;
    }

    return 0;
}

static COLORREF Debug_ColorForAspect(const char *aspect)
{
    unsigned int hash;
    int i;

    static COLORREF colors[] =
    {
        RGB(220, 220, 220),
        RGB(140, 210, 255),
        RGB(160, 255, 160),
        RGB(255, 210, 120),
        RGB(255, 160, 160),
        RGB(210, 170, 255),
        RGB(255, 150, 220),
        RGB(180, 240, 220)
    };

    hash = 0;

    if (aspect)
    {
        for (i = 0; aspect[i]; i++)
            hash = hash * 131u + (unsigned char)aspect[i];
    }

    return colors[hash % (sizeof(colors) / sizeof(colors[0]))];
}

static void Debug_GetCurrentTimeText(char *buffer, int bufferSize)
{
    SYSTEMTIME st;

    GetLocalTime(&st);

    wsprintf(
        buffer,
        "%02d:%02d:%02d.%03d",
        st.wHour,
        st.wMinute,
        st.wSecond,
        st.wMilliseconds
    );

    buffer[bufferSize - 1] = '\0';
}

static void Debug_GetWindowTextSafe(HWND hwnd, char *buffer, int bufferSize)
{
    if (!buffer || bufferSize <= 0)
        return;

    buffer[0] = '\0';

    if (hwnd)
        GetWindowText(hwnd, buffer, bufferSize);

    buffer[bufferSize - 1] = '\0';
}

static int Debug_GetWindowTextMatches(HWND hwnd, const char *newText)
{
    int oldLen;
    char *oldText;
    int same;

    if (!hwnd || !newText)
        return 0;

    oldLen = GetWindowTextLength(hwnd);

    if (oldLen != lstrlen(newText))
        return 0;

    oldText = (char *)malloc(oldLen + 1);

    if (!oldText)
        return 0;

    GetWindowText(hwnd, oldText, oldLen + 1);
    oldText[oldLen] = '\0';

    same = lstrcmp(oldText, newText) == 0;

    free(oldText);

    return same;
}

static void Debug_SetEditTextPreserveSelection(HWND hwnd, const char *text)
{
    DWORD selStart;
    DWORD selEnd;
    int firstVisibleLine;
    int newFirstVisibleLine;
    int newLen;

    if (!hwnd || !text)
        return;

    if (Debug_GetWindowTextMatches(hwnd, text))
        return;

    selStart = 0;
    selEnd = 0;

    SendMessage(hwnd, EM_GETSEL, (WPARAM)&selStart, (LPARAM)&selEnd);
    firstVisibleLine = (int)SendMessage(hwnd, EM_GETFIRSTVISIBLELINE, 0, 0);

    g_controlsUpdating = 1;

    SendMessage(hwnd, WM_SETREDRAW, FALSE, 0);
    SetWindowText(hwnd, text);

    newLen = GetWindowTextLength(hwnd);

    if ((int)selStart > newLen)
        selStart = (DWORD)newLen;

    if ((int)selEnd > newLen)
        selEnd = (DWORD)newLen;

    SendMessage(hwnd, EM_SETSEL, (WPARAM)selStart, (LPARAM)selEnd);

    newFirstVisibleLine = (int)SendMessage(hwnd, EM_GETFIRSTVISIBLELINE, 0, 0);

    if (firstVisibleLine != newFirstVisibleLine)
    {
        SendMessage(
            hwnd,
            EM_LINESCROLL,
            0,
            firstVisibleLine - newFirstVisibleLine
        );
    }

    SendMessage(hwnd, WM_SETREDRAW, TRUE, 0);
    InvalidateRect(hwnd, NULL, FALSE);

    g_controlsUpdating = 0;
}

static void Debug_FormatIntValue(char *buffer, int bufferSize, void *userData)
{
    int *value;

    value = (int *)userData;

    if (!value)
    {
        Debug_CopyText(buffer, bufferSize, "(null)");
        return;
    }

    wsprintf(buffer, "%d", *value);
}

static void Debug_FormatHwndValue(char *buffer, int bufferSize, void *userData)
{
    HWND *value;

    value = (HWND *)userData;

    if (!value || !*value)
    {
        Debug_CopyText(buffer, bufferSize, "NULL");
        return;
    }

    wsprintf(
        buffer,
        "0x%08lX%s",
        (DWORD)(ULONG_PTR)(*value),
        IsWindow(*value) ? " valid" : " invalid"
    );
}

static int Debug_FindVariableIndex(const char *aspect, const char *name)
{
    int i;

    for (i = 0; i < g_variableCount; i++)
    {
        if (lstrcmpi(g_variables[i].aspect, aspect ? aspect : "General") == 0 &&
            lstrcmpi(g_variables[i].name, name ? name : "(unnamed)") == 0)
        {
            return i;
        }
    }

    return -1;
}

static void Debug_UpdateAvailableList(void)
{
    int i;
    int selected;
    char line[256];

    if (!g_availableList)
        return;

    selected = (int)SendMessage(g_availableList, LB_GETCURSEL, 0, 0);

    g_controlsUpdating = 1;

    SendMessage(g_availableList, WM_SETREDRAW, FALSE, 0);
    SendMessage(g_availableList, LB_RESETCONTENT, 0, 0);

    for (i = 0; i < g_variableCount; i++)
    {
        wsprintf(
            line,
            "%s  %-14s  %s",
            g_variables[i].active ? "[x]" : "[ ]",
            g_variables[i].aspect,
            g_variables[i].name
        );

        SendMessage(g_availableList, LB_ADDSTRING, 0, (LPARAM)line);
    }

    if (selected >= g_variableCount)
        selected = g_variableCount - 1;

    if (selected >= 0)
        SendMessage(g_availableList, LB_SETCURSEL, selected, 0);

    SendMessage(g_availableList, WM_SETREDRAW, TRUE, 0);
    InvalidateRect(g_availableList, NULL, FALSE);

    g_controlsUpdating = 0;
}

static int Debug_RegisterVariableInternal(
    const char *aspect,
    const char *name,
    DebugVariableValueCallback callback,
    void *userData,
    int active
)
{
    DebugVariable *variable;
    int existingIndex;

    if (!callback)
        return 0;

    existingIndex = Debug_FindVariableIndex(aspect, name);

    if (existingIndex >= 0)
    {
        g_variables[existingIndex].callback = callback;
        g_variables[existingIndex].userData = userData;
        g_variables[existingIndex].active = active ? 1 : g_variables[existingIndex].active;

        Debug_UpdateAvailableList();
        return 1;
    }

    if (g_variableCount >= DEBUG_MAX_VARIABLES)
        return 0;

    variable = &g_variables[g_variableCount];

    Debug_CopyText(variable->aspect, DEBUG_ASPECT_SIZE, aspect ? aspect : "General");
    Debug_CopyText(variable->name, DEBUG_NAME_SIZE, name ? name : "(unnamed)");
    variable->callback = callback;
    variable->userData = userData;
    variable->active = active ? 1 : 0;

    g_variableCount++;

    Debug_UpdateAvailableList();

    Debug_Log(
        "Debug",
        active ? "RegisterVariable" : "RegisterAvailableVariable",
        "%s.%s",
        variable->aspect,
        variable->name
    );

    return 1;
}

int Debug_RegisterVariable(
    const char *aspect,
    const char *name,
    DebugVariableValueCallback callback,
    void *userData
)
{
    return Debug_RegisterVariableInternal(
        aspect,
        name,
        callback,
        userData,
        1
    );
}

int Debug_RegisterAvailableVariable(
    const char *aspect,
    const char *name,
    DebugVariableValueCallback callback,
    void *userData
)
{
    return Debug_RegisterVariableInternal(
        aspect,
        name,
        callback,
        userData,
        0
    );
}

int Debug_RegisterIntPointer(
    const char *aspect,
    const char *name,
    int *value
)
{
    return Debug_RegisterVariable(
        aspect,
        name,
        Debug_FormatIntValue,
        value
    );
}

int Debug_RegisterAvailableIntPointer(
    const char *aspect,
    const char *name,
    int *value
)
{
    return Debug_RegisterAvailableVariable(
        aspect,
        name,
        Debug_FormatIntValue,
        value
    );
}

int Debug_RegisterHwndPointer(
    const char *aspect,
    const char *name,
    HWND *value
)
{
    return Debug_RegisterVariable(
        aspect,
        name,
        Debug_FormatHwndValue,
        value
    );
}

int Debug_RegisterAvailableHwndPointer(
    const char *aspect,
    const char *name,
    HWND *value
)
{
    return Debug_RegisterAvailableVariable(
        aspect,
        name,
        Debug_FormatHwndValue,
        value
    );
}

int Debug_SetVariableActive(
    const char *aspect,
    const char *name,
    int active
)
{
    int index;

    index = Debug_FindVariableIndex(aspect, name);

    if (index < 0)
        return 0;

    g_variables[index].active = active ? 1 : 0;

    Debug_UpdateAvailableList();

    if (g_dumpEdit)
        Debug_SetEditTextPreserveSelection(g_dumpEdit, "");

    return 1;
}

void Debug_SetAllVariablesActive(int active)
{
    int i;

    for (i = 0; i < g_variableCount; i++)
        g_variables[i].active = active ? 1 : 0;

    Debug_UpdateAvailableList();
}

static void Debug_AppendRichText(HWND hwnd, const char *text, COLORREF color)
{
    CHARFORMAT cf;
    int len;

    if (!hwnd || !text)
        return;

    len = GetWindowTextLength(hwnd);

    SendMessage(hwnd, EM_SETSEL, (WPARAM)len, (LPARAM)len);

    ZeroMemory(&cf, sizeof(cf));
    cf.cbSize = sizeof(cf);
    cf.dwMask = CFM_COLOR;
    cf.crTextColor = color;

    SendMessage(hwnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
    SendMessage(hwnd, EM_REPLACESEL, FALSE, (LPARAM)text);
}

static void Debug_AppendPlainText(HWND hwnd, const char *text)
{
    int len;

    if (!hwnd || !text)
        return;

    len = GetWindowTextLength(hwnd);

    SendMessage(hwnd, EM_SETSEL, (WPARAM)len, (LPARAM)len);
    SendMessage(hwnd, EM_REPLACESEL, FALSE, (LPARAM)text);
}

static int Debug_LogEntryMatchesFilter(const DebugLogEntry *entry, const char *filter)
{
    if (!filter || !filter[0])
        return 1;

    if (Debug_TextContainsNoCase(entry->aspect, filter))
        return 1;

    if (Debug_TextContainsNoCase(entry->eventName, filter))
        return 1;

    if (Debug_TextContainsNoCase(entry->text, filter))
        return 1;

    return 0;
}

static void Debug_RebuildLogView(void)
{
    char filter[256];
    int i;

    if (!g_logEdit)
        return;

    Debug_GetWindowTextSafe(g_logFilterEdit, filter, sizeof(filter));

    g_controlsUpdating = 1;

    SendMessage(g_logEdit, WM_SETREDRAW, FALSE, 0);
    SetWindowText(g_logEdit, "");

    for (i = 0; i < g_logCount; i++)
    {
        if (!Debug_LogEntryMatchesFilter(&g_logEntries[i], filter))
            continue;

        if (g_logIsRichEdit)
            Debug_AppendRichText(g_logEdit, g_logEntries[i].text, g_logEntries[i].color);
        else
            Debug_AppendPlainText(g_logEdit, g_logEntries[i].text);
    }

    SendMessage(g_logEdit, EM_SETSEL, (WPARAM)-1, (LPARAM)-1);
    SendMessage(g_logEdit, WM_SETREDRAW, TRUE, 0);
    InvalidateRect(g_logEdit, NULL, FALSE);

    g_controlsUpdating = 0;
}

void Debug_ClearLog(void)
{
    g_logCount = 0;

    if (g_logEdit)
        SetWindowText(g_logEdit, "");
}

static void Debug_AddLogEntry(
    const char *aspect,
    const char *eventName,
    COLORREF color,
    const char *message
)
{
    DebugLogEntry *entry;
    char timeText[64];

    if (g_logCount >= DEBUG_MAX_LOG_ENTRIES)
    {
        MoveMemory(
            &g_logEntries[0],
            &g_logEntries[1],
            sizeof(DebugLogEntry) * (DEBUG_MAX_LOG_ENTRIES - 1)
        );

        g_logCount = DEBUG_MAX_LOG_ENTRIES - 1;
    }

    entry = &g_logEntries[g_logCount];

    Debug_CopyText(entry->aspect, DEBUG_ASPECT_SIZE, aspect ? aspect : "General");
    Debug_CopyText(entry->eventName, DEBUG_NAME_SIZE, eventName ? eventName : "Event");
    entry->color = color;

    Debug_GetCurrentTimeText(timeText, sizeof(timeText));

    wsprintf(
        entry->text,
        "%s  %-12s  %-22s  ",
        timeText,
        entry->aspect,
        entry->eventName
    );

    Debug_AppendText(entry->text, DEBUG_LOG_TEXT_SIZE, message ? message : "");
    Debug_AppendText(entry->text, DEBUG_LOG_TEXT_SIZE, "\r\n");

    g_logCount++;

    if (g_window && IsWindowVisible(g_window))
        Debug_RebuildLogView();
}

void Debug_LogColor(
    const char *aspect,
    const char *eventName,
    COLORREF color,
    const char *format,
    ...
)
{
    char message[DEBUG_LOG_TEXT_SIZE];
    va_list args;

    if (!g_loggingEnabled)
        return;

    message[0] = '\0';

    if (format)
    {
        va_start(args, format);
        wvsprintf(message, format, args);
        va_end(args);
    }

    message[DEBUG_LOG_TEXT_SIZE - 1] = '\0';

    Debug_AddLogEntry(
        aspect,
        eventName,
        color,
        message
    );
}

void Debug_Log(
    const char *aspect,
    const char *eventName,
    const char *format,
    ...
)
{
    char message[DEBUG_LOG_TEXT_SIZE];
    va_list args;

    if (!g_loggingEnabled)
        return;

    message[0] = '\0';

    if (format)
    {
        va_start(args, format);
        wvsprintf(message, format, args);
        va_end(args);
    }

    message[DEBUG_LOG_TEXT_SIZE - 1] = '\0';

    Debug_AddLogEntry(
        aspect,
        eventName,
        Debug_ColorForAspect(aspect),
        message
    );
}

static int Debug_VariableMatchesFilter(
    const DebugVariable *variable,
    const char *value,
    const char *filter
)
{
    if (!filter || !filter[0])
        return 1;

    if (Debug_TextContainsNoCase(variable->aspect, filter))
        return 1;

    if (Debug_TextContainsNoCase(variable->name, filter))
        return 1;

    if (Debug_TextContainsNoCase(value, filter))
        return 1;

    return 0;
}

static void Debug_UpdateDumpView(void)
{
    char filter[256];
    char dump[32768];
    char line[1024];
    char value[DEBUG_VALUE_SIZE];
    int i;

    if (!g_dumpEdit)
        return;

    if (!g_window || !IsWindowVisible(g_window))
        return;

    if (!g_updatesEnabled)
        return;

    Debug_GetWindowTextSafe(g_dumpFilterEdit, filter, sizeof(filter));

    dump[0] = '\0';

    for (i = 0; i < g_variableCount; i++)
    {
        if (!g_variables[i].active)
            continue;

        value[0] = '\0';

        if (g_variables[i].callback)
            g_variables[i].callback(value, sizeof(value), g_variables[i].userData);

        value[sizeof(value) - 1] = '\0';

        if (!Debug_VariableMatchesFilter(&g_variables[i], value, filter))
            continue;

        wsprintf(
            line,
            "%-14s  %-32s  %s\r\n",
            g_variables[i].aspect,
            g_variables[i].name,
            value
        );

        Debug_AppendText(dump, sizeof(dump), line);
    }

    Debug_SetEditTextPreserveSelection(g_dumpEdit, dump);
}

static void Debug_UpdateButtons(void)
{
    if (g_updateButton)
    {
        SetWindowText(
            g_updateButton,
            g_updatesEnabled ? "Pause updates" : "Resume updates"
        );
    }

    if (g_loggingButton)
    {
        SetWindowText(
            g_loggingButton,
            g_loggingEnabled ? "Disable logging" : "Enable logging"
        );
    }
}

static void Debug_LoadRichEdit(void)
{
    if (g_richEditModule)
        return;

    g_richEditModule = LoadLibrary("Msftedit.dll");

    if (!g_richEditModule)
        g_richEditModule = LoadLibrary("Riched20.dll");
}

static HWND Debug_CreateLogEdit(HWND parent)
{
    HWND hwnd;

    Debug_LoadRichEdit();

    if (g_richEditModule)
    {
        hwnd = CreateWindowEx(
            WS_EX_CLIENTEDGE,
            MSFTEDIT_CLASSA,
            "",
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL |
            ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_READONLY,
            0,
            0,
            100,
            100,
            parent,
            NULL,
            g_hInstance,
            NULL
        );

        if (hwnd)
        {
            g_logIsRichEdit = 1;
            SendMessage(hwnd, EM_SETBKGNDCOLOR, 0, RGB(30, 30, 30));
            return hwnd;
        }
    }

    g_logIsRichEdit = 0;

    return CreateWindowEx(
        WS_EX_CLIENTEDGE,
        "EDIT",
        "",
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL |
        ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_READONLY,
        0,
        0,
        100,
        100,
        parent,
        NULL,
        g_hInstance,
        NULL
    );
}

static HWND Debug_CreateReadonlyEdit(HWND parent)
{
    return CreateWindowEx(
        WS_EX_CLIENTEDGE,
        "EDIT",
        "",
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL |
        ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_READONLY,
        0,
        0,
        100,
        100,
        parent,
        NULL,
        g_hInstance,
        NULL
    );
}

static HWND Debug_CreateFilterEdit(HWND parent, int id)
{
    return CreateWindowEx(
        WS_EX_CLIENTEDGE,
        "EDIT",
        "",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
        0,
        0,
        100,
        24,
        parent,
        (HMENU)id,
        g_hInstance,
        NULL
    );
}

static HWND Debug_CreateButton(HWND parent, int id, const char *text)
{
    return CreateWindowEx(
        0,
        "BUTTON",
        text,
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
        0,
        0,
        100,
        28,
        parent,
        (HMENU)id,
        g_hInstance,
        NULL
    );
}

static HWND Debug_CreateAvailableList(HWND parent)
{
    return CreateWindowEx(
        WS_EX_CLIENTEDGE,
        "LISTBOX",
        "",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL |
        LBS_NOTIFY | LBS_NOINTEGRALHEIGHT,
        0,
        0,
        100,
        100,
        parent,
        (HMENU)ID_DEBUG_AVAILABLE_LIST,
        g_hInstance,
        NULL
    );
}

static void Debug_CreateControls(HWND hwnd)
{
    g_updateButton = Debug_CreateButton(hwnd, ID_DEBUG_UPDATE_BUTTON, "Pause updates");
    g_loggingButton = Debug_CreateButton(hwnd, ID_DEBUG_LOGGING_BUTTON, "Disable logging");
    g_clearLogButton = Debug_CreateButton(hwnd, ID_DEBUG_CLEAR_LOG_BUTTON, "Clear log");

    g_dumpFilterEdit = Debug_CreateFilterEdit(hwnd, ID_DEBUG_DUMP_FILTER_EDIT);
    g_dumpEdit = Debug_CreateReadonlyEdit(hwnd);

    g_availableList = Debug_CreateAvailableList(hwnd);
    g_addVariableButton = Debug_CreateButton(hwnd, ID_DEBUG_ADD_VARIABLE_BUTTON, "Add selected");
    g_removeVariableButton = Debug_CreateButton(hwnd, ID_DEBUG_REMOVE_VARIABLE_BUTTON, "Remove selected");
    g_showAllVariablesButton = Debug_CreateButton(hwnd, ID_DEBUG_SHOW_ALL_VARIABLES_BUTTON, "Show all");
    g_hideAllVariablesButton = Debug_CreateButton(hwnd, ID_DEBUG_HIDE_ALL_VARIABLES_BUTTON, "Hide all");

    g_logFilterEdit = Debug_CreateFilterEdit(hwnd, ID_DEBUG_LOG_FILTER_EDIT);
    g_logEdit = Debug_CreateLogEdit(hwnd);

    Debug_UpdateButtons();
    Debug_UpdateAvailableList();
}

static void Debug_LayoutControls(HWND hwnd)
{
    RECT rc;
    int margin;
    int gap;
    int buttonH;
    int filterH;
    int topH;
    int bottomY;
    int bottomH;
    int leftW;
    int rightX;
    int rightW;
    int y;
    int x;
    int dumpTop;
    int logTop;
    int updateW;
    int loggingW;
    int clearW;
    int rightButtonH;
    int rightButtonW;
    int rightButtonY;
    int rightListTop;

    if (!hwnd)
        return;

    GetClientRect(hwnd, &rc);

    margin = 8;
    gap = 6;
    buttonH = 28;
    filterH = 24;

    updateW = 130;
    loggingW = 145;
    clearW = 100;

    topH = ((rc.bottom - rc.top) * 52) / 100;
    bottomY = topH + gap;
    bottomH = rc.bottom - bottomY - margin;

    leftW = ((rc.right - rc.left) - margin * 3) / 2;
    rightX = margin + leftW + margin;
    rightW = rc.right - rightX - margin;

    if (leftW < 260)
        leftW = 260;

    if (rightW < 180)
        rightW = 180;

    x = margin;
    y = margin;

    SetWindowPos(g_updateButton, NULL, x, y, updateW, buttonH, SWP_NOZORDER | SWP_NOACTIVATE);

    x += updateW + gap;
    SetWindowPos(g_loggingButton, NULL, x, y, loggingW, buttonH, SWP_NOZORDER | SWP_NOACTIVATE);

    x += loggingW + gap;
    SetWindowPos(g_clearLogButton, NULL, x, y, clearW, buttonH, SWP_NOZORDER | SWP_NOACTIVATE);

    y += buttonH + gap;

    SetWindowPos(
        g_dumpFilterEdit,
        NULL,
        margin,
        y,
        leftW,
        filterH,
        SWP_NOZORDER | SWP_NOACTIVATE
    );

    dumpTop = y + filterH + gap;

    SetWindowPos(
        g_dumpEdit,
        NULL,
        margin,
        dumpTop,
        leftW,
        topH - dumpTop - margin,
        SWP_NOZORDER | SWP_NOACTIVATE
    );

    rightButtonH = 28;
    rightButtonW = (rightW - gap) / 2;
    rightButtonY = margin;

    SetWindowPos(
        g_addVariableButton,
        NULL,
        rightX,
        rightButtonY,
        rightButtonW,
        rightButtonH,
        SWP_NOZORDER | SWP_NOACTIVATE
    );

    SetWindowPos(
        g_removeVariableButton,
        NULL,
        rightX + rightButtonW + gap,
        rightButtonY,
        rightButtonW,
        rightButtonH,
        SWP_NOZORDER | SWP_NOACTIVATE
    );

    rightButtonY += rightButtonH + gap;

    SetWindowPos(
        g_showAllVariablesButton,
        NULL,
        rightX,
        rightButtonY,
        rightButtonW,
        rightButtonH,
        SWP_NOZORDER | SWP_NOACTIVATE
    );

    SetWindowPos(
        g_hideAllVariablesButton,
        NULL,
        rightX + rightButtonW + gap,
        rightButtonY,
        rightButtonW,
        rightButtonH,
        SWP_NOZORDER | SWP_NOACTIVATE
    );

    rightListTop = rightButtonY + rightButtonH + gap;

    SetWindowPos(
        g_availableList,
        NULL,
        rightX,
        rightListTop,
        rightW,
        topH - rightListTop - margin,
        SWP_NOZORDER | SWP_NOACTIVATE
    );

    SetWindowPos(
        g_logFilterEdit,
        NULL,
        margin,
        bottomY,
        rc.right - rc.left - margin * 2,
        filterH,
        SWP_NOZORDER | SWP_NOACTIVATE
    );

    logTop = bottomY + filterH + gap;

    SetWindowPos(
        g_logEdit,
        NULL,
        margin,
        logTop,
        rc.right - rc.left - margin * 2,
        bottomH - filterH - gap,
        SWP_NOZORDER | SWP_NOACTIVATE
    );
}

static void Debug_AddSelectedAvailableVariable(void)
{
    int index;

    if (!g_availableList)
        return;

    index = (int)SendMessage(g_availableList, LB_GETCURSEL, 0, 0);

    if (index < 0 || index >= g_variableCount)
        return;

    g_variables[index].active = 1;

    Debug_Log(
        "Debug",
        "VariableAdded",
        "%s.%s",
        g_variables[index].aspect,
        g_variables[index].name
    );

    Debug_UpdateAvailableList();
    Debug_UpdateDumpView();
}

static void Debug_RemoveSelectedAvailableVariable(void)
{
    int index;

    if (!g_availableList)
        return;

    index = (int)SendMessage(g_availableList, LB_GETCURSEL, 0, 0);

    if (index < 0 || index >= g_variableCount)
        return;

    g_variables[index].active = 0;

    Debug_Log(
        "Debug",
        "VariableRemoved",
        "%s.%s",
        g_variables[index].aspect,
        g_variables[index].name
    );

    Debug_UpdateAvailableList();
    Debug_UpdateDumpView();
}

static LRESULT CALLBACK DebugWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_CREATE:
        {
            g_window = hwnd;

            Debug_CreateControls(hwnd);
            Debug_LayoutControls(hwnd);
            Debug_UpdateDumpView();
            Debug_RebuildLogView();

            SetTimer(hwnd, DEBUG_TIMER_ID, DEBUG_TIMER_MS, NULL);

            Debug_Log("Debug", "WindowCreate", "Debug window created.");

            return 0;
        }

        case WM_SIZE:
        {
            Debug_LayoutControls(hwnd);
            return 0;
        }

        case WM_TIMER:
        {
            if (wParam == DEBUG_TIMER_ID)
            {
                Debug_UpdateDumpView();
                return 0;
            }

            break;
        }

        case WM_COMMAND:
        {
            int controlId;
            int notifyCode;

            controlId = LOWORD(wParam);
            notifyCode = HIWORD(wParam);

            if (controlId == ID_DEBUG_UPDATE_BUTTON && notifyCode == BN_CLICKED)
            {
                g_updatesEnabled = !g_updatesEnabled;
                Debug_UpdateButtons();
                Debug_Log("Debug", "ToggleUpdates", "%s", g_updatesEnabled ? "enabled" : "disabled");
                Debug_UpdateDumpView();
                return 0;
            }

            if (controlId == ID_DEBUG_LOGGING_BUTTON && notifyCode == BN_CLICKED)
            {
                if (g_loggingEnabled)
                {
                    Debug_Log("Debug", "ToggleLogging", "disabled");
                    g_loggingEnabled = 0;
                }
                else
                {
                    g_loggingEnabled = 1;
                    Debug_Log("Debug", "ToggleLogging", "enabled");
                }

                Debug_UpdateButtons();
                return 0;
            }

            if (controlId == ID_DEBUG_CLEAR_LOG_BUTTON && notifyCode == BN_CLICKED)
            {
                Debug_ClearLog();
                return 0;
            }

            if (controlId == ID_DEBUG_ADD_VARIABLE_BUTTON && notifyCode == BN_CLICKED)
            {
                Debug_AddSelectedAvailableVariable();
                return 0;
            }

            if (controlId == ID_DEBUG_REMOVE_VARIABLE_BUTTON && notifyCode == BN_CLICKED)
            {
                Debug_RemoveSelectedAvailableVariable();
                return 0;
            }

            if (controlId == ID_DEBUG_SHOW_ALL_VARIABLES_BUTTON && notifyCode == BN_CLICKED)
            {
                Debug_SetAllVariablesActive(1);
                Debug_Log("Debug", "ShowAllVariables", "All variables activated.");
                Debug_UpdateDumpView();
                return 0;
            }

            if (controlId == ID_DEBUG_HIDE_ALL_VARIABLES_BUTTON && notifyCode == BN_CLICKED)
            {
                Debug_SetAllVariablesActive(0);
                Debug_Log("Debug", "HideAllVariables", "All variables hidden.");
                Debug_UpdateDumpView();
                return 0;
            }

            if (controlId == ID_DEBUG_AVAILABLE_LIST && notifyCode == LBN_DBLCLK)
            {
                Debug_AddSelectedAvailableVariable();
                return 0;
            }

            if (controlId == ID_DEBUG_DUMP_FILTER_EDIT && notifyCode == EN_CHANGE)
            {
                if (!g_controlsUpdating)
                    Debug_UpdateDumpView();

                return 0;
            }

            if (controlId == ID_DEBUG_LOG_FILTER_EDIT && notifyCode == EN_CHANGE)
            {
                if (!g_controlsUpdating)
                    Debug_RebuildLogView();

                return 0;
            }

            break;
        }

        case WM_SHOWWINDOW:
        {
            if (wParam)
            {
                Debug_UpdateAvailableList();
                Debug_UpdateDumpView();
                Debug_RebuildLogView();
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
            DebugWindowClosedCallback callback;

            KillTimer(hwnd, DEBUG_TIMER_ID);

            Debug_Log("Debug", "WindowDestroy", "Debug window destroyed.");

            g_window = NULL;
            g_updateButton = NULL;
            g_loggingButton = NULL;
            g_clearLogButton = NULL;
            g_dumpFilterEdit = NULL;
            g_dumpEdit = NULL;

            g_availableList = NULL;
            g_addVariableButton = NULL;
            g_removeVariableButton = NULL;
            g_showAllVariablesButton = NULL;
            g_hideAllVariablesButton = NULL;

            g_logFilterEdit = NULL;
            g_logEdit = NULL;

            callback = g_onClosed;
            g_onClosed = NULL;

            if (callback)
                callback();

            return 0;
        }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

static BOOL DebugWindow_RegisterClass(HINSTANCE hInstance)
{
    static int registered = 0;
    WNDCLASS wc;

    if (registered)
        return TRUE;

    ZeroMemory(&wc, sizeof(wc));

    wc.lpfnWndProc = DebugWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = DEBUG_WINDOW_CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClass(&wc))
        return FALSE;

    registered = 1;
    return TRUE;
}

HWND DebugWindow_Show(
    HINSTANCE hInstance,
    DebugWindowClosedCallback onClosed
)
{
    g_hInstance = hInstance;
    g_onClosed = onClosed;

    if (g_window && IsWindow(g_window))
    {
        ShowWindow(g_window, SW_SHOW);
        SetForegroundWindow(g_window);
        Debug_UpdateAvailableList();
        Debug_UpdateDumpView();
        Debug_RebuildLogView();
        return g_window;
    }

    if (!DebugWindow_RegisterClass(hInstance))
        return NULL;

    g_window = CreateWindowEx(
        0,
        DEBUG_WINDOW_CLASS_NAME,
        DEBUG_WINDOW_TITLE,
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        1000,
        720,
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

void DebugWindow_Close(void)
{
    if (g_window && IsWindow(g_window))
        DestroyWindow(g_window);
}

HWND DebugWindow_GetHwnd(void)
{
    return g_window;
}

int DebugWindow_IsOpen(void)
{
    return g_window && IsWindow(g_window);
}