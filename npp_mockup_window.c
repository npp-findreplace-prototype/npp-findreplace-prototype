#include <windows.h>
#include <commctrl.h>
#include <richedit.h>
#include <shellapi.h>
#include <stdio.h>
#include <stdlib.h>

#include "npp_mockup_window.h"
#include "debug_window.h"

#ifndef MSFTEDIT_CLASSA
#define MSFTEDIT_CLASSA "RICHEDIT50A"
#endif

#ifndef EM_SETBKGNDCOLOR
#define EM_SETBKGNDCOLOR (WM_USER + 67)
#endif

#ifndef EM_SETEVENTMASK
#define EM_SETEVENTMASK (WM_USER + 69)
#endif

#ifndef EM_GETEVENTMASK
#define EM_GETEVENTMASK (WM_USER + 59)
#endif

#ifndef ENM_SELCHANGE
#define ENM_SELCHANGE 0x00080000
#endif

#ifndef EM_EXLINEFROMCHAR
#define EM_EXLINEFROMCHAR (WM_USER + 54)
#endif

#ifndef EM_GETFIRSTVISIBLELINE
#define EM_GETFIRSTVISIBLELINE 0x00CE
#endif

#ifndef EM_LINESCROLL
#define EM_LINESCROLL 0x00B6
#endif

#ifndef WS_EX_ACCEPTFILES
#define WS_EX_ACCEPTFILES 0x00000010L
#endif

#ifndef GWLP_WNDPROC
#define GWLP_WNDPROC (-4)
#endif

#ifndef GWLP_USERDATA
#define GWLP_USERDATA (-21)
#endif

#define NPP_MOCKUP_WINDOW_CLASS_NAME "NppMockupWindowClass"
#define NPP_MOCKUP_WINDOW_TITLE "NP++ Mockup"

#define NPP_MAX_DOCS 64
#define NPP_MAX_TOOL_BUTTONS 64

#define NPP_TIMER_ID 1
#define NPP_TIMER_MS 250

#define NPP_TOOLBAR_HEIGHT 30
#define NPP_TAB_HEIGHT 28
#define NPP_STATUS_HEIGHT 24
#define NPP_SPLITTER_WIDTH 6
#define NPP_MIN_PANE_WIDTH 160

#define ID_NPP_LEFT_TAB 3001
#define ID_NPP_RIGHT_TAB 3002
#define ID_NPP_LEFT_EDIT 3003
#define ID_NPP_RIGHT_EDIT 3004

typedef struct NppDocument
{
    char title[MAX_PATH];
    char path[MAX_PATH];
    char *text;
    DWORD selStart;
    DWORD selEnd;
    int firstVisibleLine;
} NppDocument;

typedef struct NppPane
{
    HWND tab;
    HWND edit;
    WNDPROC oldEditProc;

    NppDocument docs[NPP_MAX_DOCS];
    int docCount;
    int activeDoc;
    int paneIndex;
} NppPane;

typedef UINT (WINAPI *NppDragQueryFileProc)(HDROP, UINT, LPSTR, UINT);
typedef void (WINAPI *NppDragFinishProc)(HDROP);

static HINSTANCE g_hInstance = NULL;
static HWND g_window = NULL;
static HWND g_status = NULL;

static HWND g_toolbarButtons[NPP_MAX_TOOL_BUTTONS];
static int g_toolbarButtonCount = 0;

static NppPane g_panes[2];
static int g_activePane = 0;

static int g_splitPercent = 500;
static int g_draggingSplitter = 0;
static RECT g_splitterRect;

static HMODULE g_richEditModule = NULL;
static const char *g_editClassName = "EDIT";
static int g_usingRichEdit = 0;

static HBRUSH g_statusBrush = NULL;
static NppMockupWindowClosedCallback g_onClosed = NULL;

static HMODULE g_shell32 = NULL;
static NppDragQueryFileProc g_dragQueryFile = NULL;
static NppDragFinishProc g_dragFinish = NULL;

static void Npp_CopyText(char *dest, int destSize, const char *src)
{
    if (!dest || destSize <= 0)
        return;

    if (!src)
        src = "";

    lstrcpyn(dest, src, destSize);
    dest[destSize - 1] = '\0';
}

static char *Npp_DuplicateText(const char *text)
{
    char *copy;
    int len;

    if (!text)
        text = "";

    len = lstrlen(text);

    copy = (char *)malloc(len + 1);

    if (!copy)
        return NULL;

    CopyMemory(copy, text, len + 1);
    return copy;
}

static void Npp_FreeDocument(NppDocument *doc)
{
    if (!doc)
        return;

    if (doc->text)
        free(doc->text);

    ZeroMemory(doc, sizeof(*doc));
}

static void Npp_FreePane(NppPane *pane)
{
    int i;

    if (!pane)
        return;

    for (i = 0; i < pane->docCount; i++)
        Npp_FreeDocument(&pane->docs[i]);

    pane->docCount = 0;
    pane->activeDoc = -1;
}

static void Npp_LoadShellDropFunctions(void)
{
    if (g_dragQueryFile && g_dragFinish)
        return;

    if (!g_shell32)
        g_shell32 = LoadLibrary("shell32.dll");

    if (!g_shell32)
        return;

    g_dragQueryFile =
        (NppDragQueryFileProc)GetProcAddress(
            g_shell32,
            "DragQueryFileA"
        );

    g_dragFinish =
        (NppDragFinishProc)GetProcAddress(
            g_shell32,
            "DragFinish"
        );
}

static UINT Npp_DragQueryFileSafe(HDROP drop, UINT index, LPSTR buffer, UINT bufferSize)
{
    Npp_LoadShellDropFunctions();

    if (!g_dragQueryFile)
        return 0;

    return g_dragQueryFile(drop, index, buffer, bufferSize);
}

static void Npp_DragFinishSafe(HDROP drop)
{
    Npp_LoadShellDropFunctions();

    if (g_dragFinish)
        g_dragFinish(drop);
}

static void Npp_GetBaseName(const char *path, char *buffer, int bufferSize)
{
    const char *base;
    int i;

    base = path ? path : "";

    for (i = 0; path && path[i]; i++)
    {
        if (path[i] == '\\' || path[i] == '/')
            base = path + i + 1;
    }

    Npp_CopyText(buffer, bufferSize, base);
}

static void Npp_LoadRichEditClass(void)
{
    HWND test;

    if (g_richEditModule)
        return;

    g_richEditModule = LoadLibrary("Msftedit.dll");

    if (g_richEditModule)
    {
        test = CreateWindowEx(
            0,
            MSFTEDIT_CLASSA,
            "",
            WS_CHILD,
            0,
            0,
            1,
            1,
            g_window,
            NULL,
            g_hInstance,
            NULL
        );

        if (test)
        {
            DestroyWindow(test);
            g_editClassName = MSFTEDIT_CLASSA;
            g_usingRichEdit = 1;
            return;
        }
    }

    g_richEditModule = LoadLibrary("Riched20.dll");

    if (g_richEditModule)
    {
        test = CreateWindowEx(
            0,
            "RichEdit20A",
            "",
            WS_CHILD,
            0,
            0,
            1,
            1,
            g_window,
            NULL,
            g_hInstance,
            NULL
        );

        if (test)
        {
            DestroyWindow(test);
            g_editClassName = "RichEdit20A";
            g_usingRichEdit = 1;
            return;
        }
    }

    g_editClassName = "EDIT";
    g_usingRichEdit = 0;
}

static void Npp_SetEditTheme(HWND edit)
{
    CHARFORMAT cf;
    HFONT font;

    if (!edit)
        return;

    font = (HFONT)GetStockObject(ANSI_FIXED_FONT);

    if (font)
        SendMessage(edit, WM_SETFONT, (WPARAM)font, TRUE);

    if (g_usingRichEdit)
    {
        SendMessage(edit, EM_SETBKGNDCOLOR, 0, RGB(54, 54, 54));

        ZeroMemory(&cf, sizeof(cf));
        cf.cbSize = sizeof(cf);
        cf.dwMask = CFM_COLOR;
        cf.crTextColor = RGB(230, 230, 220);

        SendMessage(edit, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&cf);
        SendMessage(edit, EM_SETEVENTMASK, 0, ENM_SELCHANGE);
    }
}

static void Npp_GetEditSelection(HWND edit, DWORD *start, DWORD *end)
{
    DWORD a;
    DWORD b;

    a = 0;
    b = 0;

    if (edit)
        SendMessage(edit, EM_GETSEL, (WPARAM)&a, (LPARAM)&b);

    if (a > b)
    {
        DWORD temp;

        temp = a;
        a = b;
        b = temp;
    }

    if (start)
        *start = a;

    if (end)
        *end = b;
}

static void Npp_SetEditSelection(HWND edit, DWORD start, DWORD end)
{
    if (!edit)
        return;

    SendMessage(edit, EM_SETSEL, (WPARAM)start, (LPARAM)end);
}

static int Npp_GetLineFromChar(HWND edit, int charIndex)
{
    if (!edit)
        return 0;

    if (g_usingRichEdit)
        return (int)SendMessage(edit, EM_EXLINEFROMCHAR, 0, charIndex);

    return (int)SendMessage(edit, EM_LINEFROMCHAR, (WPARAM)charIndex, 0);
}

static int Npp_GetLineIndex(HWND edit, int line)
{
    if (!edit)
        return 0;

    return (int)SendMessage(edit, EM_LINEINDEX, (WPARAM)line, 0);
}

static int Npp_GetLineCount(HWND edit)
{
    if (!edit)
        return 0;

    return (int)SendMessage(edit, EM_GETLINECOUNT, 0, 0);
}

static void Npp_PaneSaveActiveDocument(NppPane *pane)
{
    NppDocument *doc;
    int len;
    char *text;

    if (!pane || !pane->edit)
        return;

    if (pane->activeDoc < 0 || pane->activeDoc >= pane->docCount)
        return;

    doc = &pane->docs[pane->activeDoc];

    len = GetWindowTextLength(pane->edit);

    text = (char *)malloc(len + 1);

    if (!text)
        return;

    GetWindowText(pane->edit, text, len + 1);
    text[len] = '\0';

    if (doc->text)
        free(doc->text);

    doc->text = text;

    Npp_GetEditSelection(pane->edit, &doc->selStart, &doc->selEnd);
    doc->firstVisibleLine = (int)SendMessage(pane->edit, EM_GETFIRSTVISIBLELINE, 0, 0);
}

static void Npp_PaneLoadActiveDocument(NppPane *pane)
{
    NppDocument *doc;
    int currentFirstLine;

    if (!pane || !pane->edit)
        return;

    if (pane->activeDoc < 0 || pane->activeDoc >= pane->docCount)
    {
        SetWindowText(pane->edit, "");
        return;
    }

    doc = &pane->docs[pane->activeDoc];

    SendMessage(pane->edit, WM_SETREDRAW, FALSE, 0);

    SetWindowText(pane->edit, doc->text ? doc->text : "");
    Npp_SetEditTheme(pane->edit);
    Npp_SetEditSelection(pane->edit, doc->selStart, doc->selEnd);

    currentFirstLine = (int)SendMessage(pane->edit, EM_GETFIRSTVISIBLELINE, 0, 0);

    if (doc->firstVisibleLine != currentFirstLine)
    {
        SendMessage(
            pane->edit,
            EM_LINESCROLL,
            0,
            doc->firstVisibleLine - currentFirstLine
        );
    }

    SendMessage(pane->edit, WM_SETREDRAW, TRUE, 0);
    InvalidateRect(pane->edit, NULL, FALSE);
}

static int Npp_PaneAddDocumentFromText(
    NppPane *pane,
    const char *title,
    const char *path,
    const char *text,
    int selectNew
)
{
    NppDocument *doc;
    TCITEM item;
    int index;

    if (!pane)
        return 0;

    if (pane->docCount >= NPP_MAX_DOCS)
        return 0;

    if (pane->activeDoc >= 0)
        Npp_PaneSaveActiveDocument(pane);

    index = pane->docCount;
    doc = &pane->docs[index];

    ZeroMemory(doc, sizeof(*doc));

    Npp_CopyText(doc->title, sizeof(doc->title), title ? title : "new");
    Npp_CopyText(doc->path, sizeof(doc->path), path ? path : "");

    doc->text = Npp_DuplicateText(text ? text : "");
    doc->selStart = 0;
    doc->selEnd = 0;
    doc->firstVisibleLine = 0;

    ZeroMemory(&item, sizeof(item));
    item.mask = TCIF_TEXT;
    item.pszText = doc->title;

    SendMessage(pane->tab, TCM_INSERTITEM, index, (LPARAM)&item);

    pane->docCount++;

    if (selectNew || pane->activeDoc < 0)
    {
        pane->activeDoc = index;
        SendMessage(pane->tab, TCM_SETCURSEL, index, 0);
        Npp_PaneLoadActiveDocument(pane);
    }

    return 1;
}

static void Npp_PaneSelectDocument(NppPane *pane, int index)
{
    if (!pane)
        return;

    if (index < 0 || index >= pane->docCount)
        return;

    if (pane->activeDoc == index)
        return;

    Npp_PaneSaveActiveDocument(pane);

    pane->activeDoc = index;

    SendMessage(pane->tab, TCM_SETCURSEL, index, 0);
    Npp_PaneLoadActiveDocument(pane);
}

static char *Npp_TextFromUtf16Le(const BYTE *data, DWORD size, DWORD start)
{
    DWORD i;
    DWORD out;
    DWORD count;
    char *text;
    unsigned int ch;

    count = (size - start) / 2;
    text = (char *)malloc(count + 1);

    if (!text)
        return NULL;

    out = 0;

    for (i = start; i + 1 < size; i += 2)
    {
        ch = (unsigned int)data[i] | ((unsigned int)data[i + 1] << 8);

        if (ch == 0)
            continue;

        text[out++] = (ch <= 255) ? (char)ch : '?';
    }

    text[out] = '\0';
    return text;
}

static char *Npp_TextFromUtf16Be(const BYTE *data, DWORD size, DWORD start)
{
    DWORD i;
    DWORD out;
    DWORD count;
    char *text;
    unsigned int ch;

    count = (size - start) / 2;
    text = (char *)malloc(count + 1);

    if (!text)
        return NULL;

    out = 0;

    for (i = start; i + 1 < size; i += 2)
    {
        ch = ((unsigned int)data[i] << 8) | (unsigned int)data[i + 1];

        if (ch == 0)
            continue;

        text[out++] = (ch <= 255) ? (char)ch : '?';
    }

    text[out] = '\0';
    return text;
}

static int Npp_LooksLikeUtf16Le(const BYTE *data, DWORD size)
{
    DWORD i;
    DWORD zeros;

    if (!data || size < 4)
        return 0;

    zeros = 0;

    for (i = 1; i < size && i < 64; i += 2)
    {
        if (data[i] == 0)
            zeros++;
    }

    return zeros >= 4;
}

static int Npp_LooksLikeUtf16Be(const BYTE *data, DWORD size)
{
    DWORD i;
    DWORD zeros;

    if (!data || size < 4)
        return 0;

    zeros = 0;

    for (i = 0; i < size && i < 64; i += 2)
    {
        if (data[i] == 0)
            zeros++;
    }

    return zeros >= 4;
}

static char *Npp_TextFromBytes(const BYTE *data, DWORD size)
{
    char *text;
    DWORD start;

    if (!data || size == 0)
        return Npp_DuplicateText("");

    if (size >= 2 && data[0] == 0xFF && data[1] == 0xFE)
        return Npp_TextFromUtf16Le(data, size, 2);

    if (size >= 2 && data[0] == 0xFE && data[1] == 0xFF)
        return Npp_TextFromUtf16Be(data, size, 2);

    if (Npp_LooksLikeUtf16Le(data, size))
        return Npp_TextFromUtf16Le(data, size, 0);

    if (Npp_LooksLikeUtf16Be(data, size))
        return Npp_TextFromUtf16Be(data, size, 0);

    start = 0;

    if (size >= 3 && data[0] == 0xEF && data[1] == 0xBB && data[2] == 0xBF)
        start = 3;

    text = (char *)malloc((size - start) + 1);

    if (!text)
        return NULL;

    CopyMemory(text, data + start, size - start);
    text[size - start] = '\0';

    return text;
}

static char *Npp_ReadFileText(const char *path)
{
    HANDLE file;
    DWORD size;
    DWORD readSize;
    BYTE *bytes;
    char *text;

    if (!path || !path[0])
        return NULL;

    file = CreateFile(
        path,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (file == INVALID_HANDLE_VALUE)
        return NULL;

    size = GetFileSize(file, NULL);

    if (size == INVALID_FILE_SIZE)
    {
        CloseHandle(file);
        return NULL;
    }

    bytes = (BYTE *)malloc(size ? size : 1);

    if (!bytes)
    {
        CloseHandle(file);
        return NULL;
    }

    readSize = 0;

    if (size > 0)
    {
        if (!ReadFile(file, bytes, size, &readSize, NULL) || readSize != size)
        {
            free(bytes);
            CloseHandle(file);
            return NULL;
        }
    }

    CloseHandle(file);

    text = Npp_TextFromBytes(bytes, size);

    free(bytes);

    return text;
}

static void Npp_OpenFileInPane(int paneIndex, const char *path, int selectNew)
{
    char title[MAX_PATH];
    char *text;
    NppPane *pane;

    if (paneIndex < 0 || paneIndex > 1)
        paneIndex = g_activePane;

    pane = &g_panes[paneIndex];

    text = Npp_ReadFileText(path);

    if (!text)
    {
        Debug_Log("NppMockup", "OpenFileFailed", "%s", path ? path : "");
        return;
    }

    Npp_GetBaseName(path, title, sizeof(title));

    if (!title[0])
        Npp_CopyText(title, sizeof(title), "untitled");

    Npp_PaneAddDocumentFromText(
        pane,
        title,
        path,
        text,
        selectNew
    );

    Debug_Log("NppMockup", "OpenFile", "%s", path);

    free(text);
}

static void Npp_HandleDropFiles(HDROP drop, int paneIndex)
{
    UINT count;
    UINT i;
    char path[MAX_PATH];

    count = Npp_DragQueryFileSafe(drop, 0xFFFFFFFF, NULL, 0);

    for (i = 0; i < count; i++)
    {
        path[0] = '\0';

        if (Npp_DragQueryFileSafe(drop, i, path, sizeof(path)) > 0)
            Npp_OpenFileInPane(paneIndex, path, 1);
    }

    Npp_DragFinishSafe(drop);
}

static void Npp_UpdateStatus(void)
{
    NppMockupTextInfo info;
    NppPane *pane;
    NppDocument *doc;
    char status[512];

    if (!g_status)
        return;

    if (!NppMockupWindow_GetActiveTextInfo(&info))
    {
        SetWindowText(g_status, "");
        return;
    }

    pane = &g_panes[g_activePane];

    doc = NULL;

    if (pane->activeDoc >= 0 && pane->activeDoc < pane->docCount)
        doc = &pane->docs[pane->activeDoc];

    if (info.hasSelection)
    {
        wsprintf(
            status,
            "Pane: %s | File: %s | length: %d | lines: %d | Ln: %d | Col: %d | Pos: %d | Sel: %d chars, %d lines | SelStart: %d/Ln %d | SelEnd: %d/Ln %d",
            g_activePane == 0 ? "left" : "right",
            doc ? doc->title : "",
            info.characterCount,
            info.lineCount,
            info.caretLine,
            info.caretColumn,
            info.caretCharacter,
            info.selectedCharacterCount,
            info.selectedLineCount,
            info.selectionStartCharacter,
            info.selectionStartLine,
            info.selectionEndCharacter,
            info.selectionEndLine
        );
    }
    else
    {
        wsprintf(
            status,
            "Pane: %s | File: %s | length: %d | lines: %d | Ln: %d | Col: %d | Pos: %d",
            g_activePane == 0 ? "left" : "right",
            doc ? doc->title : "",
            info.characterCount,
            info.lineCount,
            info.caretLine,
            info.caretColumn,
            info.caretCharacter
        );
    }

    SetWindowText(g_status, status);
}

static LRESULT CALLBACK Npp_EditSubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    NppPane *pane;
    LRESULT result;

    pane = (NppPane *)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    if (!pane)
        return DefWindowProc(hwnd, msg, wParam, lParam);

    if (msg == WM_SETFOCUS || msg == WM_LBUTTONDOWN || msg == WM_KEYDOWN)
    {
        g_activePane = pane->paneIndex;
        Npp_UpdateStatus();
    }

    if (msg == WM_DROPFILES)
    {
        g_activePane = pane->paneIndex;
        Npp_HandleDropFiles((HDROP)wParam, pane->paneIndex);
        Npp_UpdateStatus();
        return 0;
    }

    result = CallWindowProc(pane->oldEditProc, hwnd, msg, wParam, lParam);

    if (msg == WM_KEYUP || msg == WM_CHAR || msg == WM_LBUTTONUP || msg == WM_MOUSEMOVE)
        Npp_UpdateStatus();

    return result;
}

static void Npp_SubclassEdit(NppPane *pane)
{
    if (!pane || !pane->edit)
        return;

    SetWindowLongPtr(pane->edit, GWLP_USERDATA, (LONG_PTR)pane);

    pane->oldEditProc =
        (WNDPROC)SetWindowLongPtr(
            pane->edit,
            GWLP_WNDPROC,
            (LONG_PTR)Npp_EditSubclassProc
        );
}

static void Npp_CreateMenuBar(HWND hwnd)
{
    HMENU menu;
    HMENU popup;

    const char *names[] =
    {
        "File", "Edit", "Search", "View", "Encoding", "Language",
        "Settings", "Tools", "Macro", "Run", "Plugins", "Window", "?"
    };

    int i;

    menu = CreateMenu();

    for (i = 0; i < (int)(sizeof(names) / sizeof(names[0])); i++)
    {
        popup = CreatePopupMenu();

        AppendMenu(
            popup,
            MF_STRING | MF_GRAYED,
            0,
            "(placeholder)"
        );

        AppendMenu(
            menu,
            MF_POPUP,
            (UINT_PTR)popup,
            names[i]
        );
    }

    SetMenu(hwnd, menu);
}

static HWND Npp_CreateSmallToolbarButton(HWND parent, const char *text)
{
    HWND button;

    button = CreateWindowEx(
        0,
        "BUTTON",
        text,
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        0,
        0,
        26,
        24,
        parent,
        NULL,
        g_hInstance,
        NULL
    );

    SendMessage(button, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);

    return button;
}

static void Npp_CreateToolbar(HWND hwnd)
{
    const char *buttons[] =
    {
        "N", "O", "S", "SA", "X", "C", "P", "U", "R",
        "F", "Z", "+", "-", "<", ">", "[]", "{}", "fx",
        "A", "B", "C", "1", "2", "3", "Py", "Run",
        "Blk", "K", "<", ">", "|<", ">|", "L", "R",
        "&", "&&", "<<"
    };

    int i;

    g_toolbarButtonCount = (int)(sizeof(buttons) / sizeof(buttons[0]));

    if (g_toolbarButtonCount > NPP_MAX_TOOL_BUTTONS)
        g_toolbarButtonCount = NPP_MAX_TOOL_BUTTONS;

    for (i = 0; i < g_toolbarButtonCount; i++)
        g_toolbarButtons[i] = Npp_CreateSmallToolbarButton(hwnd, buttons[i]);
}

static HWND Npp_CreateTabControl(HWND parent, int id)
{
    HWND hwnd;

    hwnd = CreateWindowEx(
        0,
        WC_TABCONTROL,
        "",
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | TCS_TABS,
        0,
        0,
        100,
        24,
        parent,
        (HMENU)id,
        g_hInstance,
        NULL
    );

    SendMessage(hwnd, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);

    return hwnd;
}

static HWND Npp_CreateEditor(HWND parent, int id)
{
    HWND edit;

    edit = CreateWindowEx(
        WS_EX_CLIENTEDGE | WS_EX_ACCEPTFILES,
        g_editClassName,
        "",
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL |
        ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_WANTRETURN,
        0,
        0,
        100,
        100,
        parent,
        (HMENU)id,
        g_hInstance,
        NULL
    );

    Npp_SetEditTheme(edit);

    return edit;
}

static void Npp_CreateControls(HWND hwnd)
{
    INITCOMMONCONTROLSEX icc;

    ZeroMemory(&icc, sizeof(icc));
    icc.dwSize = sizeof(icc);
    icc.dwICC = ICC_TAB_CLASSES | ICC_BAR_CLASSES;

    InitCommonControlsEx(&icc);

    Npp_LoadRichEditClass();

    Npp_CreateToolbar(hwnd);

    g_panes[0].paneIndex = 0;
    g_panes[0].activeDoc = -1;
    g_panes[0].tab = Npp_CreateTabControl(hwnd, ID_NPP_LEFT_TAB);
    g_panes[0].edit = Npp_CreateEditor(hwnd, ID_NPP_LEFT_EDIT);
    Npp_SubclassEdit(&g_panes[0]);

    g_panes[1].paneIndex = 1;
    g_panes[1].activeDoc = -1;
    g_panes[1].tab = Npp_CreateTabControl(hwnd, ID_NPP_RIGHT_TAB);
    g_panes[1].edit = Npp_CreateEditor(hwnd, ID_NPP_RIGHT_EDIT);
    Npp_SubclassEdit(&g_panes[1]);

    g_status = CreateWindowEx(
        0,
        "STATIC",
        "",
        WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE,
        0,
        0,
        100,
        NPP_STATUS_HEIGHT,
        hwnd,
        NULL,
        g_hInstance,
        NULL
    );

    SendMessage(g_status, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);

    if (!g_statusBrush)
        g_statusBrush = CreateSolidBrush(RGB(28, 28, 28));
}

static void Npp_CreateInitialDocuments(void)
{
    const char *leftSample =
        "3540    Find: (?im)^(set-cookie|authorization|token|api_key)\\s*[:=].*$  | Replace: [REDACTED]\r\n"
        "3541    Find: (?im)^\\s*password\\s*[:=].*$                         | Replace: [REDACTED]\r\n"
        "3542    Find: (?m)^\\s*$                                             | Replace:\r\n"
        "\r\n"
        "47. Secrets, credentials, and PII\r\n"
        "48. Database connection strings\r\n"
        "49. Licensing / copyright / SPDX\r\n"
        "50. Additional programming languages\r\n";

    const char *rightSample =
        "LiteralSearch\r\n"
        "EscapedLiteralSearch\r\n"
        "RegExSearch\r\n"
        "SemanticSearch\r\n"
        "CaseSensitive\r\n"
        "DiacriticSensitive\r\n"
        "DotIncludesNewline\r\n"
        "FuzzyLogicSearch\r\n"
        "WrapAround\r\n"
        "WholeWord\r\n"
        "BooleanSearch\r\n"
        "Settings\r\n"
        "\r\n"
        "the tooltip for each will be the names above but with spaces and normal case\r\n"
        "\r\n"
        "These buttons are radio buttons with each other.\r\n"
        "The rest of the buttons are toggle buttons.\r\n";

    Npp_PaneAddDocumentFromText(&g_panes[0], "new 167", "", leftSample, 1);
    Npp_PaneAddDocumentFromText(&g_panes[0], "new 176", "", "", 0);
    Npp_PaneAddDocumentFromText(&g_panes[0], "new 177", "", "", 0);

    Npp_PaneAddDocumentFromText(&g_panes[1], "new 173", "", rightSample, 1);
    Npp_PaneAddDocumentFromText(&g_panes[1], "new 172", "", "", 0);
    Npp_PaneAddDocumentFromText(&g_panes[1], "new 174", "", "", 0);
    Npp_PaneAddDocumentFromText(&g_panes[1], "new 175", "", "", 0);

    g_activePane = 1;
    SetFocus(g_panes[1].edit);
}

static void Npp_LayoutToolbar(HWND hwnd, int width)
{
    int i;
    int x;
    int y;
    int w;
    int h;

    (void)hwnd;

    x = 4;
    y = 3;
    h = 23;

    for (i = 0; i < g_toolbarButtonCount; i++)
    {
        w = 26;

        if (x + w > width - 4)
        {
            ShowWindow(g_toolbarButtons[i], SW_HIDE);
            continue;
        }

        ShowWindow(g_toolbarButtons[i], SW_SHOW);

        MoveWindow(
            g_toolbarButtons[i],
            x,
            y,
            w,
            h,
            TRUE
        );

        x += w + 2;
    }
}

static void Npp_LayoutPane(NppPane *pane, RECT rc)
{
    if (!pane)
        return;

    MoveWindow(
        pane->tab,
        rc.left,
        rc.top,
        rc.right - rc.left,
        NPP_TAB_HEIGHT,
        TRUE
    );

    MoveWindow(
        pane->edit,
        rc.left,
        rc.top + NPP_TAB_HEIGHT,
        rc.right - rc.left,
        rc.bottom - rc.top - NPP_TAB_HEIGHT,
        TRUE
    );
}

static void Npp_Layout(HWND hwnd)
{
    RECT rc;
    RECT leftRc;
    RECT rightRc;
    int width;
    int height;
    int usableWidth;
    int paneTop;
    int paneBottom;
    int leftWidth;

    GetClientRect(hwnd, &rc);

    width = rc.right - rc.left;
    height = rc.bottom - rc.top;

    Npp_LayoutToolbar(hwnd, width);

    paneTop = NPP_TOOLBAR_HEIGHT;
    paneBottom = height - NPP_STATUS_HEIGHT;

    usableWidth = width - NPP_SPLITTER_WIDTH;

    if (usableWidth < NPP_MIN_PANE_WIDTH * 2)
        usableWidth = NPP_MIN_PANE_WIDTH * 2;

    leftWidth = (usableWidth * g_splitPercent) / 1000;

    if (leftWidth < NPP_MIN_PANE_WIDTH)
        leftWidth = NPP_MIN_PANE_WIDTH;

    if (leftWidth > usableWidth - NPP_MIN_PANE_WIDTH)
        leftWidth = usableWidth - NPP_MIN_PANE_WIDTH;

    SetRect(&leftRc, 0, paneTop, leftWidth, paneBottom);

    SetRect(
        &g_splitterRect,
        leftWidth,
        paneTop,
        leftWidth + NPP_SPLITTER_WIDTH,
        paneBottom
    );

    SetRect(
        &rightRc,
        leftWidth + NPP_SPLITTER_WIDTH,
        paneTop,
        width,
        paneBottom
    );

    Npp_LayoutPane(&g_panes[0], leftRc);
    Npp_LayoutPane(&g_panes[1], rightRc);

    MoveWindow(
        g_status,
        0,
        height - NPP_STATUS_HEIGHT,
        width,
        NPP_STATUS_HEIGHT,
        TRUE
    );

    InvalidateRect(hwnd, &g_splitterRect, FALSE);
}

static int Npp_PointInSplitter(int x, int y)
{
    POINT pt;

    pt.x = x;
    pt.y = y;

    return PtInRect(&g_splitterRect, pt);
}

static void Npp_UpdateSplitterFromMouse(HWND hwnd, int x)
{
    RECT rc;
    int width;
    int usableWidth;

    GetClientRect(hwnd, &rc);

    width = rc.right - rc.left;
    usableWidth = width - NPP_SPLITTER_WIDTH;

    if (usableWidth <= 1)
        return;

    if (x < NPP_MIN_PANE_WIDTH)
        x = NPP_MIN_PANE_WIDTH;

    if (x > usableWidth - NPP_MIN_PANE_WIDTH)
        x = usableWidth - NPP_MIN_PANE_WIDTH;

    g_splitPercent = (x * 1000) / usableWidth;

    if (g_splitPercent < 100)
        g_splitPercent = 100;

    if (g_splitPercent > 900)
        g_splitPercent = 900;

    Npp_Layout(hwnd);
}

static void Npp_HandleTabNotify(NMHDR *hdr)
{
    NppPane *pane;
    int index;

    if (!hdr)
        return;

    pane = NULL;

    if (hdr->idFrom == ID_NPP_LEFT_TAB)
        pane = &g_panes[0];
    else if (hdr->idFrom == ID_NPP_RIGHT_TAB)
        pane = &g_panes[1];

    if (!pane)
        return;

    if (hdr->code == TCN_SELCHANGING)
    {
        Npp_PaneSaveActiveDocument(pane);
        return;
    }

    if (hdr->code == TCN_SELCHANGE)
    {
        index = (int)SendMessage(pane->tab, TCM_GETCURSEL, 0, 0);

        if (index >= 0 && index < pane->docCount)
        {
            pane->activeDoc = index;
            g_activePane = pane->paneIndex;
            Npp_PaneLoadActiveDocument(pane);
            SetFocus(pane->edit);
            Npp_UpdateStatus();
        }
    }
}

static LRESULT CALLBACK NppMockupWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_CREATE:
        {
            g_window = hwnd;

            Npp_CreateMenuBar(hwnd);
            Npp_CreateControls(hwnd);
            Npp_CreateInitialDocuments();
            Npp_Layout(hwnd);

            SetTimer(hwnd, NPP_TIMER_ID, NPP_TIMER_MS, NULL);

            Debug_Log("NppMockup", "WindowCreate", "NP++ mockup window created.");

            return 0;
        }

        case WM_SIZE:
        {
            Npp_Layout(hwnd);
            return 0;
        }

        case WM_SETCURSOR:
        {
            POINT pt;

            GetCursorPos(&pt);
            ScreenToClient(hwnd, &pt);

            if (Npp_PointInSplitter(pt.x, pt.y) || g_draggingSplitter)
            {
                SetCursor(LoadCursor(NULL, IDC_SIZEWE));
                return TRUE;
            }

            break;
        }

        case WM_LBUTTONDOWN:
        {
            int x;
            int y;

            x = LOWORD(lParam);
            y = HIWORD(lParam);

            if (Npp_PointInSplitter(x, y))
            {
                g_draggingSplitter = 1;
                SetCapture(hwnd);
                SetCursor(LoadCursor(NULL, IDC_SIZEWE));
                return 0;
            }

            break;
        }

        case WM_MOUSEMOVE:
        {
            if (g_draggingSplitter)
            {
                Npp_UpdateSplitterFromMouse(hwnd, LOWORD(lParam));
                return 0;
            }

            break;
        }

        case WM_LBUTTONUP:
        {
            if (g_draggingSplitter)
            {
                g_draggingSplitter = 0;
                ReleaseCapture();
                return 0;
            }

            break;
        }

        case WM_NOTIFY:
        {
            Npp_HandleTabNotify((NMHDR *)lParam);
            Npp_UpdateStatus();
            break;
        }

        case WM_TIMER:
        {
            if (wParam == NPP_TIMER_ID)
            {
                Npp_UpdateStatus();
                return 0;
            }

            break;
        }

        case WM_DROPFILES:
        {
            Npp_HandleDropFiles((HDROP)wParam, g_activePane);
            return 0;
        }

        case WM_CTLCOLORSTATIC:
        {
            if ((HWND)lParam == g_status)
            {
                SetBkColor((HDC)wParam, RGB(28, 28, 28));
                SetTextColor((HDC)wParam, RGB(220, 220, 220));
                return (LRESULT)g_statusBrush;
            }

            break;
        }

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc;
            HBRUSH brush;

            hdc = BeginPaint(hwnd, &ps);

            brush = CreateSolidBrush(RGB(24, 24, 24));
            FillRect(hdc, &g_splitterRect, brush);
            DeleteObject(brush);

            EndPaint(hwnd, &ps);

            return 0;
        }

        case WM_CLOSE:
        {
            DestroyWindow(hwnd);
            return 0;
        }

        case WM_NCDESTROY:
        {
            NppMockupWindowClosedCallback callback;

            KillTimer(hwnd, NPP_TIMER_ID);

            Npp_PaneSaveActiveDocument(&g_panes[0]);
            Npp_PaneSaveActiveDocument(&g_panes[1]);

            Npp_FreePane(&g_panes[0]);
            Npp_FreePane(&g_panes[1]);

            if (g_statusBrush)
            {
                DeleteObject(g_statusBrush);
                g_statusBrush = NULL;
            }

            Debug_Log("NppMockup", "WindowDestroy", "NP++ mockup window destroyed.");

            g_window = NULL;
            g_status = NULL;
            g_toolbarButtonCount = 0;

            callback = g_onClosed;
            g_onClosed = NULL;

            if (callback)
                callback();

            return 0;
        }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

static BOOL NppMockupWindow_RegisterClass(HINSTANCE hInstance)
{
    static int registered = 0;
    WNDCLASS wc;

    if (registered)
        return TRUE;

    ZeroMemory(&wc, sizeof(wc));

    wc.lpfnWndProc = NppMockupWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = NPP_MOCKUP_WINDOW_CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClass(&wc))
        return FALSE;

    registered = 1;
    return TRUE;
}

HWND NppMockupWindow_Show(
    HINSTANCE hInstance,
    NppMockupWindowClosedCallback onClosed
)
{
    g_hInstance = hInstance;
    g_onClosed = onClosed;

    if (g_window && IsWindow(g_window))
    {
        ShowWindow(g_window, SW_SHOW);
        SetForegroundWindow(g_window);
        return g_window;
    }

    if (!NppMockupWindow_RegisterClass(hInstance))
        return NULL;

    g_window = CreateWindowEx(
        WS_EX_ACCEPTFILES,
        NPP_MOCKUP_WINDOW_CLASS_NAME,
        NPP_MOCKUP_WINDOW_TITLE,
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        1100,
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

void NppMockupWindow_Close(void)
{
    if (g_window && IsWindow(g_window))
        DestroyWindow(g_window);
}

HWND NppMockupWindow_GetHwnd(void)
{
    return g_window;
}

int NppMockupWindow_IsOpen(void)
{
    return g_window && IsWindow(g_window);
}

HWND NppMockupWindow_GetActiveRichEdit(void)
{
    if (g_activePane < 0 || g_activePane > 1)
        g_activePane = 0;

    return g_panes[g_activePane].edit;
}

int NppMockupWindow_GetActiveTextInfo(NppMockupTextInfo *info)
{
    HWND edit;
    DWORD selStart;
    DWORD selEnd;
    int caret;
    int lineStart;
    int selectionEndForLine;
    int selStartLine;
    int selEndLine;

    if (!info)
        return 0;

    ZeroMemory(info, sizeof(*info));

    edit = NppMockupWindow_GetActiveRichEdit();

    if (!edit)
        return 0;

    Npp_GetEditSelection(edit, &selStart, &selEnd);

    caret = (int)selEnd;

    info->characterCount = GetWindowTextLength(edit);
    info->lineCount = Npp_GetLineCount(edit);

    info->caretCharacter = caret;
    info->caretLine = Npp_GetLineFromChar(edit, caret) + 1;

    lineStart = Npp_GetLineIndex(edit, info->caretLine - 1);

    if (lineStart < 0)
        lineStart = 0;

    info->caretColumn = caret - lineStart + 1;

    info->selectionStartCharacter = (int)selStart;
    info->selectionEndCharacter = (int)selEnd;
    info->selectedCharacterCount = (int)(selEnd - selStart);
    info->hasSelection = info->selectedCharacterCount > 0;

    if (info->hasSelection)
    {
        selectionEndForLine = (int)selEnd;

        if (selectionEndForLine > (int)selStart)
            selectionEndForLine--;

        selStartLine = Npp_GetLineFromChar(edit, (int)selStart) + 1;
        selEndLine = Npp_GetLineFromChar(edit, selectionEndForLine) + 1;

        info->selectionStartLine = selStartLine;
        info->selectionEndLine = selEndLine;
        info->selectedLineCount = selEndLine - selStartLine + 1;

        if (info->selectedLineCount < 1)
            info->selectedLineCount = 1;
    }
    else
    {
        info->selectionStartLine = info->caretLine;
        info->selectionEndLine = info->caretLine;
        info->selectedLineCount = 0;
    }

    return 1;
}

char *NppMockupWindow_AllocActiveText(int *length)
{
    HWND edit;
    int len;
    char *text;

    if (length)
        *length = 0;

    edit = NppMockupWindow_GetActiveRichEdit();

    if (!edit)
        return NULL;

    len = GetWindowTextLength(edit);

    text = (char *)malloc(len + 1);

    if (!text)
        return NULL;

    GetWindowText(edit, text, len + 1);
    text[len] = '\0';

    if (length)
        *length = len;

    return text;
}

void NppMockupWindow_FreeText(char *text)
{
    if (text)
        free(text);
}