#include "debug_window_internal.h"

void Debug_CopyText(char *dest, int destSize, const char *src)
{
    if (!dest || destSize <= 0)
        return;

    if (!src)
        src = "";

    lstrcpyn(dest, src, destSize);
    dest[destSize - 1] = '\0';
}

void Debug_AppendText(char *dest, int destSize, const char *src)
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

int Debug_TextContainsNoCase(const char *text, const char *filter)
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

void Debug_GetWindowTextSafe(HWND hwnd, char *buffer, int bufferSize)
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

void Debug_SetEditTextPreserveSelection(HWND hwnd, const char *text)
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

void Debug_UpdateButtons(void)
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

void Debug_CreateControls(HWND hwnd)
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