#include "debug_window_internal.h"

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

void Debug_RebuildLogView(void)
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