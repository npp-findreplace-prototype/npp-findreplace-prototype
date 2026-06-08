#include "new_test_layout_controls_internal.h"

static void FauxCombo_RecreateEditBrush(NewTestLayoutFauxCombo *combo)
{
    if (!combo)
        return;

    if (combo->editBrush)
        DeleteObject(combo->editBrush);

    combo->editBrush = CreateSolidBrush(combo->theme.editBackColor);
}

static void FauxCombo_UpdateEditFormatRect(NewTestLayoutFauxCombo *combo)
{
    RECT rc;
    RECT formatRc;
    HDC hdc;
    HFONT oldFont;
    TEXTMETRIC tm;
    int textHeight;
    int top;

    if (!combo || !combo->edit)
        return;

    GetClientRect(combo->edit, &rc);

    hdc = GetDC(combo->edit);

    if (!hdc)
        return;

    oldFont = NULL;

    if (combo->theme.monoFont)
        oldFont = (HFONT)SelectObject(hdc, combo->theme.monoFont);

    ZeroMemory(&tm, sizeof(tm));
    GetTextMetrics(hdc, &tm);

    if (oldFont)
        SelectObject(hdc, oldFont);

    ReleaseDC(combo->edit, hdc);

    textHeight = tm.tmHeight + tm.tmExternalLeading;
    top = ((rc.bottom - rc.top) - textHeight) / 2;

    if (top < 0)
        top = 0;

    formatRc.left = 2;
    formatRc.top = top;
    formatRc.right = rc.right - 2;
    formatRc.bottom = rc.bottom;

    SendMessage(combo->edit, EM_SETRECT, 0, (LPARAM)&formatRc);
}

static void FauxCombo_Invalidate(NewTestLayoutFauxCombo *combo)
{
    if (!combo)
        return;

    if (combo->hwnd)
        InvalidateRect(combo->hwnd, NULL, FALSE);

    if (combo->edit)
        InvalidateRect(combo->edit, NULL, FALSE);
}

static void FauxCombo_PositionChildren(NewTestLayoutFauxCombo *combo)
{
    RECT rc;
    RECT screenRc;
    int editX;
    int editY;
    int editW;
    int editH;
    int h;

    if (!combo || !combo->hwnd)
        return;

    GetClientRect(combo->hwnd, &rc);

    editX = NTL_FAUX_COMBO_EDIT_MARGIN;
    editY = 2;
    editW =
        rc.right - rc.left -
        NTL_FAUX_COMBO_ARROW_WIDTH -
        NTL_FAUX_COMBO_EDIT_MARGIN -
        2;
    editH = rc.bottom - rc.top - 4;

    if (editW < 20)
        editW = 20;

    if (editH < 20)
        editH = 20;

    SetWindowPos(
        combo->edit,
        NULL,
        editX,
        editY,
        editW,
        editH,
        SWP_NOZORDER | SWP_NOACTIVATE
    );

    FauxCombo_UpdateEditFormatRect(combo);

    GetWindowRect(combo->hwnd, &screenRc);
    MapWindowPoints(HWND_DESKTOP, combo->parent, (POINT *)&screenRc, 2);

    h = NTL_FAUX_COMBO_DROP_HEIGHT;

    SetWindowPos(
        combo->list,
        HWND_TOP,
        screenRc.left,
        screenRc.bottom + 2,
        screenRc.right - screenRc.left,
        h,
        SWP_NOACTIVATE
    );
}

static void FauxCombo_RebuildList(NewTestLayoutFauxCombo *combo)
{
    int i;

    if (!combo || !combo->list)
        return;

    SendMessage(combo->list, LB_RESETCONTENT, 0, 0);

    for (i = 0; i < combo->recentItemCount; i++)
        SendMessage(combo->list, LB_ADDSTRING, 0, (LPARAM)combo->recentItems[i]);
}

static void FauxCombo_DrawArrow(NewTestLayoutFauxCombo *combo, HDC hdc, RECT rc)
{
    POINT pts[3];
    int cx;
    int cy;
    HBRUSH brush;
    HPEN pen;
    HGDIOBJ oldBrush;
    HGDIOBJ oldPen;

    if (!combo)
        return;

    cx = rc.right - NTL_FAUX_COMBO_ARROW_WIDTH / 2;
    cy = (rc.top + rc.bottom) / 2 + 1;

    pts[0].x = cx - 6;
    pts[0].y = cy - 3;
    pts[1].x = cx + 6;
    pts[1].y = cy - 3;
    pts[2].x = cx;
    pts[2].y = cy + 5;

    brush = CreateSolidBrush(combo->theme.arrowColor);
    pen = CreatePen(PS_SOLID, 1, combo->theme.arrowColor);

    if (!brush || !pen)
    {
        if (brush)
            DeleteObject(brush);

        if (pen)
            DeleteObject(pen);

        return;
    }

    oldBrush = SelectObject(hdc, brush);
    oldPen = SelectObject(hdc, pen);

    Polygon(hdc, pts, 3);

    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);

    DeleteObject(pen);
    DeleteObject(brush);
}

static LRESULT CALLBACK FauxCombo_EditSubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    NewTestLayoutFauxCombo *combo;
    WNDPROC oldProc;
    LRESULT result;
    RECT rc;
    HFONT font;
    HFONT oldFont;
    int oldBkMode;
    HDC hdc;

    combo = (NewTestLayoutFauxCombo *)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    if (!combo)
        return DefWindowProc(hwnd, msg, wParam, lParam);

    oldProc = combo->oldEditProc;

    switch (msg)
    {
        case WM_ERASEBKGND:
        {
            GetClientRect(hwnd, &rc);
            Ntl_FillRect((HDC)wParam, &rc, combo->theme.editBackColor);
            return 1;
        }

        case WM_SETFOCUS:
        {
            Ntl_SendCommand(combo->parent, combo->hwnd, combo->id, NTL_FCN_SET_FOCUS);
            break;
        }

        case WM_KILLFOCUS:
        {
            HWND newFocus;

            newFocus = (HWND)wParam;

            if (newFocus != combo->list)
                NewTestLayoutFauxCombo_ShowDropdown(combo, 0);

            Ntl_SendCommand(combo->parent, combo->hwnd, combo->id, NTL_FCN_KILL_FOCUS);
            break;
        }

        case WM_KEYDOWN:
        {
            if (wParam == VK_RETURN)
                return 0;

            break;
        }
    }

    result = CallWindowProc(oldProc, hwnd, msg, wParam, lParam);

    if (msg == WM_PAINT)
    {
        if (GetWindowTextLength(hwnd) == 0 && combo->placeholder[0])
        {
            hdc = GetDC(hwnd);

            if (hdc)
            {
                GetClientRect(hwnd, &rc);

                SetTextColor(hdc, combo->theme.placeholderColor);

                font = combo->placeholderLarge ?
                    combo->theme.placeholderFont :
                    combo->theme.monoFont;

                if (!font)
                    font = (HFONT)GetStockObject(ANSI_FIXED_FONT);

                oldFont = (HFONT)SelectObject(hdc, font);
                oldBkMode = SetBkMode(hdc, TRANSPARENT);

                DrawText(
                    hdc,
                    combo->placeholder,
                    -1,
                    &rc,
                    DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX
                );

                SetBkMode(hdc, oldBkMode);
                SelectObject(hdc, oldFont);

                ReleaseDC(hwnd, hdc);
            }
        }
    }

    return result;
}

static void FauxCombo_DrawBuffered(void *context, HDC hdc, RECT *rc)
{
    NewTestLayoutFauxCombo *combo;

    combo = (NewTestLayoutFauxCombo *)context;

    Ntl_FillRect(
        hdc,
        rc,
        combo ? combo->theme.editBackColor : RGB(0, 0, 0)
    );

    Ntl_DrawBorder(
        hdc,
        rc,
        combo ? combo->theme.editBorderColor : RGB(80, 80, 80)
    );

    if (combo)
        FauxCombo_DrawArrow(combo, hdc, *rc);
}

LRESULT CALLBACK NewTestLayoutFauxCombo_WndProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam
)
{
    NewTestLayoutFauxCombo *combo;
    RECT rc;
    int notifyCode;

    combo = (NewTestLayoutFauxCombo *)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    switch (msg)
    {
        case WM_SIZE:
        {
            FauxCombo_PositionChildren(combo);
            FauxCombo_Invalidate(combo);
            return 0;
        }

        case WM_ERASEBKGND:
        {
            if (combo)
            {
                GetClientRect(hwnd, &rc);
                Ntl_FillRect((HDC)wParam, &rc, combo->theme.editBackColor);
                return 1;
            }

            break;
        }

        case WM_LBUTTONDOWN:
        {
            if (combo)
            {
                GetClientRect(hwnd, &rc);

                if (LOWORD(lParam) >= rc.right - NTL_FAUX_COMBO_ARROW_WIDTH)
                {
                    NewTestLayoutFauxCombo_ToggleDropdown(combo);
                    return 0;
                }

                SetFocus(combo->edit);
            }

            return 0;
        }

        case WM_COMMAND:
        {
            if (!combo)
                break;

            notifyCode = HIWORD(wParam);

            if ((HWND)lParam == combo->edit && notifyCode == EN_CHANGE)
            {
                FauxCombo_Invalidate(combo);
                Ntl_SendCommand(combo->parent, combo->hwnd, combo->id, NTL_FCN_TEXT_CHANGED);
                return 0;
            }

            break;
        }

        case WM_CTLCOLOREDIT:
        {
            if (combo)
            {
                SetBkColor((HDC)wParam, combo->theme.editBackColor);
                SetTextColor((HDC)wParam, combo->theme.editTextColor);

                if (!combo->editBrush)
                    FauxCombo_RecreateEditBrush(combo);

                return (LRESULT)combo->editBrush;
            }

            break;
        }

        case WM_PAINT:
        {
            Ntl_DoubleBufferedPaint(hwnd, combo, FauxCombo_DrawBuffered);
            return 0;
        }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

NewTestLayoutFauxCombo *NewTestLayoutFauxCombo_Create(
    HWND parent,
    HINSTANCE hInstance,
    int id,
    const char *placeholder,
    const NewTestLayoutTheme *theme
)
{
    NewTestLayoutFauxCombo *combo;
    NewTestLayoutTheme useTheme;

    combo = (NewTestLayoutFauxCombo *)malloc(sizeof(*combo));

    if (!combo)
        return NULL;

    ZeroMemory(combo, sizeof(*combo));

    combo->parent = parent;
    combo->hInstance = hInstance;
    combo->id = id;
    combo->visible = 1;
    combo->placeholderLarge = 1;

    Ntl_CopyText(combo->placeholder, sizeof(combo->placeholder), placeholder);
    Ntl_CopyTheme(&combo->theme, theme);

    useTheme = combo->theme;
    NewTestLayoutTheme_CreateDefaultFonts(&useTheme);
    combo->theme = useTheme;

    FauxCombo_RecreateEditBrush(combo);

    combo->hwnd = CreateWindowEx(
        0,
        NTL_FAUX_COMBO_CLASS_NAME,
        "",
        WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
        0,
        0,
        100,
        36,
        parent,
        (HMENU)id,
        hInstance,
        NULL
    );

    if (!combo->hwnd)
    {
        NewTestLayoutFauxCombo_Destroy(combo);
        return NULL;
    }

    SetWindowLongPtr(combo->hwnd, GWLP_USERDATA, (LONG_PTR)combo);

    combo->edit = CreateWindowEx(
        0,
        "EDIT",
        "",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP |
        ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
        0,
        0,
        100,
        24,
        combo->hwnd,
        NULL,
        hInstance,
        NULL
    );

    combo->list = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        "LISTBOX",
        "",
        WS_CHILD | WS_VSCROLL | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT,
        0,
        0,
        100,
        NTL_FAUX_COMBO_DROP_HEIGHT,
        parent,
        (HMENU)(id + NTL_FAUX_COMBO_LIST_ID_OFFSET),
        hInstance,
        NULL
    );

    if (combo->edit)
    {
        SendMessage(combo->edit, WM_SETFONT, (WPARAM)combo->theme.monoFont, TRUE);
        SetWindowLongPtr(combo->edit, GWLP_USERDATA, (LONG_PTR)combo);

        combo->oldEditProc =
            (WNDPROC)SetWindowLongPtr(
                combo->edit,
                GWLP_WNDPROC,
                (LONG_PTR)FauxCombo_EditSubclassProc
            );

        FauxCombo_UpdateEditFormatRect(combo);
    }

    if (combo->list)
    {
        SendMessage(combo->list, WM_SETFONT, (WPARAM)combo->theme.monoFont, TRUE);
        ShowWindow(combo->list, SW_HIDE);
    }

    return combo;
}

void NewTestLayoutFauxCombo_Destroy(NewTestLayoutFauxCombo *combo)
{
    if (!combo)
        return;

    if (combo->edit && combo->oldEditProc)
    {
        SetWindowLongPtr(combo->edit, GWLP_WNDPROC, (LONG_PTR)combo->oldEditProc);
        combo->oldEditProc = NULL;
    }

    if (combo->list)
        DestroyWindow(combo->list);

    if (combo->hwnd)
        DestroyWindow(combo->hwnd);

    if (combo->editBrush)
        DeleteObject(combo->editBrush);

    NewTestLayoutTheme_DeleteFonts(&combo->theme);

    free(combo);
}

HWND NewTestLayoutFauxCombo_GetHwnd(NewTestLayoutFauxCombo *combo)
{
    return combo ? combo->hwnd : NULL;
}

HWND NewTestLayoutFauxCombo_GetEditHwnd(NewTestLayoutFauxCombo *combo)
{
    return combo ? combo->edit : NULL;
}

void NewTestLayoutFauxCombo_SetTheme(NewTestLayoutFauxCombo *combo, const NewTestLayoutTheme *theme)
{
    if (!combo)
        return;

    NewTestLayoutTheme_DeleteFonts(&combo->theme);
    Ntl_CopyTheme(&combo->theme, theme);
    NewTestLayoutTheme_CreateDefaultFonts(&combo->theme);

    FauxCombo_RecreateEditBrush(combo);

    if (combo->edit)
        SendMessage(combo->edit, WM_SETFONT, (WPARAM)combo->theme.monoFont, TRUE);

    if (combo->list)
        SendMessage(combo->list, WM_SETFONT, (WPARAM)combo->theme.monoFont, TRUE);

    FauxCombo_UpdateEditFormatRect(combo);
    FauxCombo_Invalidate(combo);
}

void NewTestLayoutFauxCombo_SetRect(NewTestLayoutFauxCombo *combo, const RECT *rect)
{
    if (!combo || !rect)
        return;

    SetWindowPos(
        combo->hwnd,
        NULL,
        rect->left,
        rect->top,
        rect->right - rect->left,
        rect->bottom - rect->top,
        SWP_NOZORDER | SWP_NOACTIVATE
    );

    FauxCombo_PositionChildren(combo);
    FauxCombo_Invalidate(combo);
}

void NewTestLayoutFauxCombo_Show(NewTestLayoutFauxCombo *combo, int show)
{
    if (!combo)
        return;

    combo->visible = show ? 1 : 0;
    ShowWindow(combo->hwnd, combo->visible ? SW_SHOW : SW_HIDE);

    if (!combo->visible)
        NewTestLayoutFauxCombo_ShowDropdown(combo, 0);

    FauxCombo_Invalidate(combo);
}

int NewTestLayoutFauxCombo_IsVisible(NewTestLayoutFauxCombo *combo)
{
    return combo ? combo->visible : 0;
}

void NewTestLayoutFauxCombo_SetText(NewTestLayoutFauxCombo *combo, const char *text)
{
    if (!combo || !combo->edit)
        return;

    SetWindowText(combo->edit, text ? text : "");
    FauxCombo_Invalidate(combo);
}

void NewTestLayoutFauxCombo_GetText(NewTestLayoutFauxCombo *combo, char *buffer, int bufferSize)
{
    if (!buffer || bufferSize <= 0)
        return;

    buffer[0] = '\0';

    if (!combo || !combo->edit)
        return;

    GetWindowText(combo->edit, buffer, bufferSize);
    buffer[bufferSize - 1] = '\0';
}

int NewTestLayoutFauxCombo_GetTextLength(NewTestLayoutFauxCombo *combo)
{
    if (!combo || !combo->edit)
        return 0;

    return GetWindowTextLength(combo->edit);
}

void NewTestLayoutFauxCombo_SetPlaceholder(NewTestLayoutFauxCombo *combo, const char *placeholder)
{
    if (!combo)
        return;

    Ntl_CopyText(combo->placeholder, sizeof(combo->placeholder), placeholder);
    FauxCombo_Invalidate(combo);
}

void NewTestLayoutFauxCombo_SetPlaceholderLarge(NewTestLayoutFauxCombo *combo, int large)
{
    if (!combo)
        return;

    combo->placeholderLarge = large ? 1 : 0;
    FauxCombo_Invalidate(combo);
}

void NewTestLayoutFauxCombo_SetRecentItems(NewTestLayoutFauxCombo *combo, const char **items, int itemCount)
{
    int i;

    if (!combo)
        return;

    if (itemCount < 0)
        itemCount = 0;

    if (itemCount > NTL_CONTROLS_MAX_RECENT_ITEMS)
        itemCount = NTL_CONTROLS_MAX_RECENT_ITEMS;

    combo->recentItemCount = itemCount;

    for (i = 0; i < itemCount; i++)
        Ntl_CopyText(combo->recentItems[i], sizeof(combo->recentItems[i]), items ? items[i] : "");

    FauxCombo_RebuildList(combo);
}

void NewTestLayoutFauxCombo_ShowDropdown(NewTestLayoutFauxCombo *combo, int show)
{
    if (!combo || !combo->list)
        return;

    combo->dropdownVisible = show ? 1 : 0;

    FauxCombo_PositionChildren(combo);

    ShowWindow(combo->list, combo->dropdownVisible ? SW_SHOW : SW_HIDE);

    if (combo->dropdownVisible)
    {
        SetWindowPos(
            combo->list,
            HWND_TOP,
            0,
            0,
            0,
            0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE
        );

        Ntl_SendCommand(combo->parent, combo->hwnd, combo->id, NTL_FCN_DROPDOWN_SHOWN);
    }
    else
    {
        Ntl_SendCommand(combo->parent, combo->hwnd, combo->id, NTL_FCN_DROPDOWN_HIDDEN);
    }
}

void NewTestLayoutFauxCombo_ToggleDropdown(NewTestLayoutFauxCombo *combo)
{
    if (!combo)
        return;

    NewTestLayoutFauxCombo_ShowDropdown(combo, !combo->dropdownVisible);
}

int NewTestLayoutFauxCombo_IsDropdownVisible(NewTestLayoutFauxCombo *combo)
{
    return combo ? combo->dropdownVisible : 0;
}

void NewTestLayoutFauxCombo_HandleParentCommand(NewTestLayoutFauxCombo *combo, WPARAM wParam, LPARAM lParam)
{
    int id;
    int notifyCode;
    int index;
    char text[NTL_CONTROLS_MAX_TEXT];

    if (!combo)
        return;

    id = LOWORD(wParam);
    notifyCode = HIWORD(wParam);

    if ((HWND)lParam != combo->list)
        return;

    if (id != combo->id + NTL_FAUX_COMBO_LIST_ID_OFFSET)
        return;

    if (notifyCode == LBN_SELCHANGE || notifyCode == LBN_DBLCLK)
    {
        index = (int)SendMessage(combo->list, LB_GETCURSEL, 0, 0);

        if (index >= 0)
        {
            text[0] = '\0';
            SendMessage(combo->list, LB_GETTEXT, index, (LPARAM)text);
            text[sizeof(text) - 1] = '\0';

            SetWindowText(combo->edit, text);
            FauxCombo_Invalidate(combo);

            Ntl_SendCommand(combo->parent, combo->hwnd, combo->id, NTL_FCN_RECENT_SELECTED);

            if (notifyCode == LBN_DBLCLK)
                NewTestLayoutFauxCombo_ShowDropdown(combo, 0);
        }
    }
}

int NewTestLayoutFauxCombo_IsChildWindow(NewTestLayoutFauxCombo *combo, HWND hwnd)
{
    if (!combo || !hwnd)
        return 0;

    if (hwnd == combo->hwnd || hwnd == combo->edit || hwnd == combo->list)
        return 1;

    return IsChild(combo->hwnd, hwnd) || IsChild(combo->list, hwnd);
}