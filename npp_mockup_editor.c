#include "npp_mockup_internal.h"

void Npp_LoadRichEditClass(void)
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

void Npp_SetEditTheme(HWND edit)
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

void Npp_GetEditSelection(HWND edit, DWORD *start, DWORD *end)
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

void Npp_SetEditSelection(HWND edit, DWORD start, DWORD end)
{
    if (!edit)
        return;

    SendMessage(edit, EM_SETSEL, (WPARAM)start, (LPARAM)end);
}

int Npp_GetLineFromChar(HWND edit, int charIndex)
{
    if (!edit)
        return 0;

    if (g_usingRichEdit)
        return (int)SendMessage(edit, EM_EXLINEFROMCHAR, 0, charIndex);

    return (int)SendMessage(edit, EM_LINEFROMCHAR, (WPARAM)charIndex, 0);
}

int Npp_GetLineIndex(HWND edit, int line)
{
    if (!edit)
        return 0;

    return (int)SendMessage(edit, EM_LINEINDEX, (WPARAM)line, 0);
}

int Npp_GetLineCount(HWND edit)
{
    if (!edit)
        return 0;

    return (int)SendMessage(edit, EM_GETLINECOUNT, 0, 0);
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

void Npp_SubclassEdit(NppPane *pane)
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

HWND Npp_CreateEditor(HWND parent, int id)
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