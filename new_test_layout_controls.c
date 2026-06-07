#include <windows.h>
#include <stdlib.h>

#include "new_test_layout_controls.h"

#ifndef GWLP_USERDATA
#define GWLP_USERDATA (-21)
#endif

#ifndef GWLP_WNDPROC
#define GWLP_WNDPROC (-4)
#endif

#ifndef IDC_HAND
#define IDC_HAND MAKEINTRESOURCE(32649)
#endif

#define NTL_FAUX_COMBO_CLASS_NAME "NewTestLayoutFauxComboClass"
#define NTL_ACTION_BUTTON_CLASS_NAME "NewTestLayoutActionButtonClass"
#define NTL_ACTION_GROUP_CLASS_NAME "NewTestLayoutActionGroupClass"
#define NTL_GEAR_BUTTON_CLASS_NAME "NewTestLayoutGearButtonClass"

#define NTL_FAUX_COMBO_ARROW_WIDTH 34
#define NTL_FAUX_COMBO_DROP_HEIGHT 160
#define NTL_FAUX_COMBO_EDIT_MARGIN 8
#define NTL_FAUX_COMBO_LIST_ID_OFFSET 20000

struct NewTestLayoutFauxCombo
{
    HWND parent;
    HWND hwnd;
    HWND edit;
    HWND list;

    WNDPROC oldEditProc;

    HINSTANCE hInstance;
    int id;

    char placeholder[NTL_CONTROLS_MAX_TEXT];
    char recentItems[NTL_CONTROLS_MAX_RECENT_ITEMS][NTL_CONTROLS_MAX_TEXT];
    int recentItemCount;

    NewTestLayoutTheme theme;

    int visible;
    int dropdownVisible;
    int placeholderLarge;
};

struct NewTestLayoutActionButton
{
    HWND parent;
    HWND hwnd;

    HINSTANCE hInstance;
    int id;

    char label[NTL_CONTROLS_MAX_TEXT];

    int count;
    int hasCount;
    int showCounts;
    int showZeroCounts;
    int countInParentheses;
    int colorCountText;

    int visible;
    int enabled;
    int hot;
    int pressed;

    NewTestLayoutTheme theme;
};

struct NewTestLayoutActionGroup
{
    HWND parent;
    HWND hwnd;

    HINSTANCE hInstance;
    int id;

    char title[NTL_CONTROLS_MAX_TEXT];

    NewTestLayoutActionButton *buttons[NTL_CONTROLS_MAX_ACTION_BUTTONS];
    int buttonCount;

    int visible;
    int borderVisible;
    int padding;

    NewTestLayoutTheme theme;
};

struct NewTestLayoutGearButton
{
    HWND parent;
    HWND hwnd;

    HINSTANCE hInstance;
    int id;

    int visible;
    int hot;
    int pressed;

    NewTestLayoutTheme theme;
};

static void Ntl_CopyText(char *dest, int destSize, const char *src)
{
    if (!dest || destSize <= 0)
        return;

    if (!src)
        src = "";

    lstrcpyn(dest, src, destSize);
    dest[destSize - 1] = '\0';
}

static void Ntl_CopyTheme(NewTestLayoutTheme *dest, const NewTestLayoutTheme *src)
{
    if (!dest)
        return;

    if (src)
        CopyMemory(dest, src, sizeof(*dest));
    else
        NewTestLayoutTheme_GetDefault(dest);
}

static void Ntl_SendCommand(HWND parent, HWND hwnd, int id, int notifyCode)
{
    if (!parent)
        return;

    SendMessage(
        parent,
        WM_COMMAND,
        MAKEWPARAM(id, notifyCode),
        (LPARAM)hwnd
    );
}

static void Ntl_FillRect(HDC hdc, const RECT *rc, COLORREF color)
{
    HBRUSH brush;

    brush = CreateSolidBrush(color);

    if (brush)
    {
        FillRect(hdc, rc, brush);
        DeleteObject(brush);
    }
}

static void Ntl_DrawBorder(HDC hdc, const RECT *rc, COLORREF color)
{
    HPEN pen;
    HPEN oldPen;
    HBRUSH oldBrush;

    pen = CreatePen(PS_SOLID, 1, color);

    if (!pen)
        return;

    oldPen = (HPEN)SelectObject(hdc, pen);
    oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));

    Rectangle(hdc, rc->left, rc->top, rc->right, rc->bottom);

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}

void NewTestLayoutTheme_GetDefault(NewTestLayoutTheme *theme)
{
    if (!theme)
        return;

    ZeroMemory(theme, sizeof(*theme));

    theme->windowBackColor = RGB(8, 8, 8);
    theme->panelBackColor = RGB(14, 14, 14);
    theme->panelBorderColor = RGB(70, 70, 70);

    theme->editBackColor = RGB(10, 10, 10);
    theme->editBorderColor = RGB(95, 95, 95);
    theme->editTextColor = RGB(230, 230, 230);
    theme->placeholderColor = RGB(135, 135, 145);
    theme->arrowColor = RGB(140, 140, 145);

    theme->buttonBackColor = RGB(18, 18, 18);
    theme->buttonBorderColor = RGB(80, 80, 80);
    theme->buttonTextColor = RGB(205, 205, 205);
    theme->countTextColor = RGB(0, 100, 210);

    theme->groupBackColor = RGB(10, 10, 10);
    theme->groupBorderColor = RGB(65, 65, 65);
    theme->groupTitleColor = RGB(175, 175, 175);

    theme->gearColor = RGB(170, 170, 170);
    theme->gearBackColor = RGB(22, 22, 22);
    theme->gearBorderColor = RGB(90, 90, 90);
}

void NewTestLayoutTheme_CreateDefaultFonts(NewTestLayoutTheme *theme)
{
    if (!theme)
        return;

    if (!theme->normalFont)
    {
        theme->normalFont = CreateFont(
            -18,
            0,
            0,
            0,
            FW_NORMAL,
            FALSE,
            FALSE,
            FALSE,
            ANSI_CHARSET,
            OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY,
            DEFAULT_PITCH | FF_DONTCARE,
            "Segoe UI"
        );
    }

    if (!theme->monoFont)
    {
        theme->monoFont = CreateFont(
            -18,
            0,
            0,
            0,
            FW_NORMAL,
            FALSE,
            FALSE,
            FALSE,
            ANSI_CHARSET,
            OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY,
            FIXED_PITCH | FF_MODERN,
            "Consolas"
        );
    }

    if (!theme->placeholderFont)
    {
        theme->placeholderFont = CreateFont(
            -28,
            0,
            0,
            0,
            FW_NORMAL,
            FALSE,
            FALSE,
            FALSE,
            ANSI_CHARSET,
            OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY,
            FIXED_PITCH | FF_MODERN,
            "Consolas"
        );
    }

    if (!theme->buttonFont)
    {
        theme->buttonFont = CreateFont(
            -17,
            0,
            0,
            0,
            FW_NORMAL,
            FALSE,
            FALSE,
            FALSE,
            ANSI_CHARSET,
            OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY,
            DEFAULT_PITCH | FF_DONTCARE,
            "Segoe UI"
        );
    }

    if (!theme->titleFont)
    {
        theme->titleFont = CreateFont(
            -17,
            0,
            0,
            0,
            FW_NORMAL,
            FALSE,
            FALSE,
            FALSE,
            ANSI_CHARSET,
            OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY,
            DEFAULT_PITCH | FF_DONTCARE,
            "Segoe UI"
        );
    }
}

void NewTestLayoutTheme_DeleteFonts(NewTestLayoutTheme *theme)
{
    if (!theme)
        return;

    if (theme->normalFont)
        DeleteObject(theme->normalFont);

    if (theme->monoFont)
        DeleteObject(theme->monoFont);

    if (theme->placeholderFont)
        DeleteObject(theme->placeholderFont);

    if (theme->buttonFont)
        DeleteObject(theme->buttonFont);

    if (theme->titleFont)
        DeleteObject(theme->titleFont);

    theme->normalFont = NULL;
    theme->monoFont = NULL;
    theme->placeholderFont = NULL;
    theme->buttonFont = NULL;
    theme->titleFont = NULL;
}

static void FauxCombo_PositionChildren(NewTestLayoutFauxCombo *combo)
{
    RECT rc;
    RECT screenRc;
    int h;

    if (!combo || !combo->hwnd)
        return;

    GetClientRect(combo->hwnd, &rc);

    MoveWindow(
        combo->edit,
        NTL_FAUX_COMBO_EDIT_MARGIN,
        2,
        rc.right - rc.left - NTL_FAUX_COMBO_ARROW_WIDTH - NTL_FAUX_COMBO_EDIT_MARGIN,
        rc.bottom - rc.top - 4,
        TRUE
    );

    GetWindowRect(combo->hwnd, &screenRc);
    MapWindowPoints(HWND_DESKTOP, combo->parent, (POINT *)&screenRc, 2);

    h = NTL_FAUX_COMBO_DROP_HEIGHT;

    MoveWindow(
        combo->list,
        screenRc.left,
        screenRc.bottom + 2,
        screenRc.right - screenRc.left,
        h,
        TRUE
    );

    SetWindowPos(
        combo->list,
        HWND_TOP,
        0,
        0,
        0,
        0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE
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
    char text[2];
    HFONT font;
    HFONT oldFont;
    int oldBkMode;

    combo = (NewTestLayoutFauxCombo *)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    if (!combo)
        return DefWindowProc(hwnd, msg, wParam, lParam);

    oldProc = combo->oldEditProc;

    if (msg == WM_SETFOCUS)
        Ntl_SendCommand(combo->parent, combo->hwnd, combo->id, NTL_FCN_SET_FOCUS);

    if (msg == WM_KILLFOCUS)
    {
        HWND newFocus;

        newFocus = (HWND)wParam;

        if (newFocus != combo->list)
            NewTestLayoutFauxCombo_ShowDropdown(combo, 0);

        Ntl_SendCommand(combo->parent, combo->hwnd, combo->id, NTL_FCN_KILL_FOCUS);
    }

    result = CallWindowProc(oldProc, hwnd, msg, wParam, lParam);

    if (msg == WM_PAINT)
    {
        GetWindowText(hwnd, text, sizeof(text));

        if (!text[0] && combo->placeholder[0])
        {
            HDC hdc;

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

static LRESULT CALLBACK FauxComboProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    NewTestLayoutFauxCombo *combo;
    PAINTSTRUCT ps;
    HDC hdc;
    RECT rc;
    int notifyCode;

    combo = (NewTestLayoutFauxCombo *)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    switch (msg)
    {
        case WM_SIZE:
        {
            FauxCombo_PositionChildren(combo);
            return 0;
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
                InvalidateRect(combo->edit, NULL, FALSE);
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
                return (LRESULT)GetStockObject(NULL_BRUSH);
            }

            break;
        }

        case WM_PAINT:
        {
            hdc = BeginPaint(hwnd, &ps);

            GetClientRect(hwnd, &rc);

            Ntl_FillRect(hdc, &rc, combo ? combo->theme.editBackColor : RGB(0, 0, 0));
            Ntl_DrawBorder(hdc, &rc, combo ? combo->theme.editBorderColor : RGB(80, 80, 80));

            if (combo)
                FauxCombo_DrawArrow(combo, hdc, rc);

            EndPaint(hwnd, &ps);
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
        free(combo);
        return NULL;
    }

    SetWindowLongPtr(combo->hwnd, GWLP_USERDATA, (LONG_PTR)combo);

    combo->edit = CreateWindowEx(
        0,
        "EDIT",
        "",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
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

    if (combo->edit)
        SendMessage(combo->edit, WM_SETFONT, (WPARAM)combo->theme.monoFont, TRUE);

    if (combo->list)
        SendMessage(combo->list, WM_SETFONT, (WPARAM)combo->theme.monoFont, TRUE);

    InvalidateRect(combo->hwnd, NULL, TRUE);
    InvalidateRect(combo->edit, NULL, TRUE);
}

void NewTestLayoutFauxCombo_SetRect(NewTestLayoutFauxCombo *combo, const RECT *rect)
{
    if (!combo || !rect)
        return;

    MoveWindow(
        combo->hwnd,
        rect->left,
        rect->top,
        rect->right - rect->left,
        rect->bottom - rect->top,
        TRUE
    );

    FauxCombo_PositionChildren(combo);
}

void NewTestLayoutFauxCombo_Show(NewTestLayoutFauxCombo *combo, int show)
{
    if (!combo)
        return;

    combo->visible = show ? 1 : 0;
    ShowWindow(combo->hwnd, combo->visible ? SW_SHOW : SW_HIDE);

    if (!combo->visible)
        NewTestLayoutFauxCombo_ShowDropdown(combo, 0);
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
    InvalidateRect(combo->edit, NULL, FALSE);
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
    InvalidateRect(combo->edit, NULL, FALSE);
}

void NewTestLayoutFauxCombo_SetPlaceholderLarge(NewTestLayoutFauxCombo *combo, int large)
{
    if (!combo)
        return;

    combo->placeholderLarge = large ? 1 : 0;
    InvalidateRect(combo->edit, NULL, FALSE);
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

static void ActionButton_Draw(NewTestLayoutActionButton *button, HDC hdc)
{
    RECT rc;
    char countText[64];
    int showCount;
    SIZE labelSize;
    SIZE countSize;
    HFONT font;
    HFONT oldFont;
    HPEN pen;
    HBRUSH brush;
    HGDIOBJ oldPen;
    HGDIOBJ oldBrush;
    int totalWidth;
    int x;
    int y;
    COLORREF backColor;
    COLORREF borderColor;
    COLORREF textColor;

    if (!button || !hdc)
        return;

    GetClientRect(button->hwnd, &rc);

    backColor = button->theme.buttonBackColor;
    borderColor = button->theme.buttonBorderColor;
    textColor = button->enabled ? button->theme.buttonTextColor : RGB(105, 105, 105);

    if (button->pressed)
        backColor = RGB(8, 8, 8);
    else if (button->hot)
        backColor = RGB(28, 28, 28);

    brush = CreateSolidBrush(backColor);
    pen = CreatePen(PS_SOLID, 1, borderColor);

    oldBrush = SelectObject(hdc, brush);
    oldPen = SelectObject(hdc, pen);

    Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);

    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);

    DeleteObject(pen);
    DeleteObject(brush);

    font = button->theme.buttonFont;

    if (!font)
        font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

    oldFont = (HFONT)SelectObject(hdc, font);

    SetBkMode(hdc, TRANSPARENT);

    showCount =
        button->showCounts &&
        button->hasCount &&
        (button->showZeroCounts || button->count != 0);

    countText[0] = '\0';

    if (showCount)
    {
        if (button->countInParentheses)
            wsprintf(countText, " (%d)", button->count);
        else
            wsprintf(countText, " %d", button->count);
    }

    GetTextExtentPoint32(hdc, button->label, lstrlen(button->label), &labelSize);

    if (showCount)
        GetTextExtentPoint32(hdc, countText, lstrlen(countText), &countSize);
    else
    {
        countSize.cx = 0;
        countSize.cy = 0;
    }

    totalWidth = labelSize.cx + countSize.cx;
    x = rc.left + ((rc.right - rc.left) - totalWidth) / 2;
    y = rc.top + ((rc.bottom - rc.top) - labelSize.cy) / 2;

    SetTextColor(hdc, textColor);
    TextOut(hdc, x, y, button->label, lstrlen(button->label));

    if (showCount)
    {
        if (button->colorCountText)
            SetTextColor(hdc, button->theme.countTextColor);
        else
            SetTextColor(hdc, textColor);

        TextOut(hdc, x + labelSize.cx, y, countText, lstrlen(countText));
    }

    SelectObject(hdc, oldFont);
}

static LRESULT CALLBACK ActionButtonProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    NewTestLayoutActionButton *button;
    PAINTSTRUCT ps;
    HDC hdc;

    button = (NewTestLayoutActionButton *)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    switch (msg)
    {
        case WM_MOUSEMOVE:
        {
            TRACKMOUSEEVENT tme;

            if (button && !button->hot)
            {
                button->hot = 1;
                InvalidateRect(hwnd, NULL, FALSE);

                ZeroMemory(&tme, sizeof(tme));
                tme.cbSize = sizeof(tme);
                tme.dwFlags = TME_LEAVE;
                tme.hwndTrack = hwnd;

                TrackMouseEvent(&tme);
            }

            break;
        }

        case WM_MOUSELEAVE:
        {
            if (button)
            {
                button->hot = 0;
                InvalidateRect(hwnd, NULL, FALSE);
            }

            return 0;
        }

        case WM_LBUTTONDOWN:
        {
            if (button && button->enabled)
            {
                button->pressed = 1;
                SetCapture(hwnd);
                SetFocus(hwnd);
                InvalidateRect(hwnd, NULL, FALSE);
            }

            return 0;
        }

        case WM_LBUTTONUP:
        {
            if (button && button->pressed)
            {
                RECT rc;
                POINT pt;

                button->pressed = 0;
                ReleaseCapture();
                InvalidateRect(hwnd, NULL, FALSE);

                GetClientRect(hwnd, &rc);
                pt.x = LOWORD(lParam);
                pt.y = HIWORD(lParam);

                if (PtInRect(&rc, pt))
                    Ntl_SendCommand(button->parent, hwnd, button->id, NTL_ACTION_BN_CLICKED);
            }

            return 0;
        }

        case WM_KEYDOWN:
        {
            if (button && button->enabled && (wParam == VK_SPACE || wParam == VK_RETURN))
            {
                Ntl_SendCommand(button->parent, hwnd, button->id, NTL_ACTION_BN_CLICKED);
                return 0;
            }

            break;
        }

        case WM_ENABLE:
        {
            if (button)
            {
                button->enabled = IsWindowEnabled(hwnd);
                InvalidateRect(hwnd, NULL, FALSE);
            }

            break;
        }

        case WM_PAINT:
        {
            hdc = BeginPaint(hwnd, &ps);
            ActionButton_Draw(button, hdc);
            EndPaint(hwnd, &ps);
            return 0;
        }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

NewTestLayoutActionButton *NewTestLayoutActionButton_Create(
    HWND parent,
    HINSTANCE hInstance,
    int id,
    const char *label,
    const NewTestLayoutTheme *theme
)
{
    NewTestLayoutActionButton *button;

    button = (NewTestLayoutActionButton *)malloc(sizeof(*button));

    if (!button)
        return NULL;

    ZeroMemory(button, sizeof(*button));

    button->parent = parent;
    button->hInstance = hInstance;
    button->id = id;
    button->visible = 1;
    button->enabled = 1;
    button->showCounts = 1;
    button->showZeroCounts = 0;
    button->countInParentheses = 1;
    button->colorCountText = 1;

    Ntl_CopyText(button->label, sizeof(button->label), label);
    Ntl_CopyTheme(&button->theme, theme);
    NewTestLayoutTheme_CreateDefaultFonts(&button->theme);

    button->hwnd = CreateWindowEx(
        0,
        NTL_ACTION_BUTTON_CLASS_NAME,
        "",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP,
        0,
        0,
        100,
        32,
        parent,
        (HMENU)id,
        hInstance,
        NULL
    );

    if (!button->hwnd)
    {
        NewTestLayoutTheme_DeleteFonts(&button->theme);
        free(button);
        return NULL;
    }

    SetWindowLongPtr(button->hwnd, GWLP_USERDATA, (LONG_PTR)button);

    return button;
}

void NewTestLayoutActionButton_Destroy(NewTestLayoutActionButton *button)
{
    if (!button)
        return;

    if (button->hwnd)
        DestroyWindow(button->hwnd);

    NewTestLayoutTheme_DeleteFonts(&button->theme);

    free(button);
}

HWND NewTestLayoutActionButton_GetHwnd(NewTestLayoutActionButton *button)
{
    return button ? button->hwnd : NULL;
}

void NewTestLayoutActionButton_SetTheme(NewTestLayoutActionButton *button, const NewTestLayoutTheme *theme)
{
    if (!button)
        return;

    NewTestLayoutTheme_DeleteFonts(&button->theme);
    Ntl_CopyTheme(&button->theme, theme);
    NewTestLayoutTheme_CreateDefaultFonts(&button->theme);

    InvalidateRect(button->hwnd, NULL, TRUE);
}

void NewTestLayoutActionButton_SetRect(NewTestLayoutActionButton *button, const RECT *rect)
{
    if (!button || !rect)
        return;

    MoveWindow(
        button->hwnd,
        rect->left,
        rect->top,
        rect->right - rect->left,
        rect->bottom - rect->top,
        TRUE
    );
}

void NewTestLayoutActionButton_Show(NewTestLayoutActionButton *button, int show)
{
    if (!button)
        return;

    button->visible = show ? 1 : 0;
    ShowWindow(button->hwnd, button->visible ? SW_SHOW : SW_HIDE);
}

int NewTestLayoutActionButton_IsVisible(NewTestLayoutActionButton *button)
{
    return button ? button->visible : 0;
}

void NewTestLayoutActionButton_SetText(NewTestLayoutActionButton *button, const char *label)
{
    if (!button)
        return;

    Ntl_CopyText(button->label, sizeof(button->label), label);
    InvalidateRect(button->hwnd, NULL, FALSE);
}

void NewTestLayoutActionButton_SetCount(NewTestLayoutActionButton *button, int count, int hasCount)
{
    if (!button)
        return;

    button->count = count;
    button->hasCount = hasCount ? 1 : 0;

    InvalidateRect(button->hwnd, NULL, FALSE);
}

void NewTestLayoutActionButton_SetCountOptions(
    NewTestLayoutActionButton *button,
    int showCounts,
    int showZeroCounts,
    int countInParentheses,
    int colorCountText
)
{
    if (!button)
        return;

    button->showCounts = showCounts ? 1 : 0;
    button->showZeroCounts = showZeroCounts ? 1 : 0;
    button->countInParentheses = countInParentheses ? 1 : 0;
    button->colorCountText = colorCountText ? 1 : 0;

    InvalidateRect(button->hwnd, NULL, FALSE);
}

void NewTestLayoutActionButton_SetEnabled(NewTestLayoutActionButton *button, int enabled)
{
    if (!button)
        return;

    button->enabled = enabled ? 1 : 0;
    EnableWindow(button->hwnd, button->enabled);
    InvalidateRect(button->hwnd, NULL, FALSE);
}

static void ActionGroup_Draw(NewTestLayoutActionGroup *group, HDC hdc)
{
    RECT rc;
    RECT titleRc;
    HFONT font;
    HFONT oldFont;
    int oldBkMode;

    if (!group || !hdc)
        return;

    GetClientRect(group->hwnd, &rc);

    Ntl_FillRect(hdc, &rc, group->theme.groupBackColor);

    if (group->borderVisible)
        Ntl_DrawBorder(hdc, &rc, group->theme.groupBorderColor);

    titleRc = rc;
    titleRc.left += group->padding;
    titleRc.right -= group->padding;
    titleRc.bottom = titleRc.top + 24;

    font = group->theme.titleFont;

    if (!font)
        font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

    oldFont = (HFONT)SelectObject(hdc, font);
    oldBkMode = SetBkMode(hdc, TRANSPARENT);

    SetTextColor(hdc, group->theme.groupTitleColor);

    DrawText(
        hdc,
        group->title,
        -1,
        &titleRc,
        DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX
    );

    SetBkMode(hdc, oldBkMode);
    SelectObject(hdc, oldFont);
}

static LRESULT CALLBACK ActionGroupProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    NewTestLayoutActionGroup *group;
    PAINTSTRUCT ps;
    HDC hdc;

    group = (NewTestLayoutActionGroup *)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    switch (msg)
    {
        case WM_SIZE:
        {
            NewTestLayoutActionGroup_LayoutButtons(group);
            return 0;
        }

        case WM_COMMAND:
        {
            if (group)
            {
                Ntl_SendCommand(
                    group->parent,
                    (HWND)lParam,
                    LOWORD(wParam),
                    HIWORD(wParam)
                );

                return 0;
            }

            break;
        }

        case WM_PAINT:
        {
            hdc = BeginPaint(hwnd, &ps);
            ActionGroup_Draw(group, hdc);
            EndPaint(hwnd, &ps);
            return 0;
        }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

NewTestLayoutActionGroup *NewTestLayoutActionGroup_Create(
    HWND parent,
    HINSTANCE hInstance,
    int id,
    const char *title,
    const NewTestLayoutActionButtonDef *buttons,
    int buttonCount,
    const NewTestLayoutTheme *theme
)
{
    NewTestLayoutActionGroup *group;
    int i;

    if (buttonCount < 0)
        buttonCount = 0;

    if (buttonCount > NTL_CONTROLS_MAX_ACTION_BUTTONS)
        buttonCount = NTL_CONTROLS_MAX_ACTION_BUTTONS;

    group = (NewTestLayoutActionGroup *)malloc(sizeof(*group));

    if (!group)
        return NULL;

    ZeroMemory(group, sizeof(*group));

    group->parent = parent;
    group->hInstance = hInstance;
    group->id = id;
    group->visible = 1;
    group->borderVisible = 0;
    group->padding = 10;
    group->buttonCount = buttonCount;

    Ntl_CopyText(group->title, sizeof(group->title), title);
    Ntl_CopyTheme(&group->theme, theme);
    NewTestLayoutTheme_CreateDefaultFonts(&group->theme);

    group->hwnd = CreateWindowEx(
        0,
        NTL_ACTION_GROUP_CLASS_NAME,
        "",
        WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
        0,
        0,
        100,
        80,
        parent,
        (HMENU)id,
        hInstance,
        NULL
    );

    if (!group->hwnd)
    {
        NewTestLayoutTheme_DeleteFonts(&group->theme);
        free(group);
        return NULL;
    }

    SetWindowLongPtr(group->hwnd, GWLP_USERDATA, (LONG_PTR)group);

    for (i = 0; i < group->buttonCount; i++)
    {
        group->buttons[i] = NewTestLayoutActionButton_Create(
            group->hwnd,
            hInstance,
            buttons[i].id,
            buttons[i].label,
            &group->theme
        );

        if (group->buttons[i])
            NewTestLayoutActionButton_SetCount(
                group->buttons[i],
                buttons[i].count,
                buttons[i].hasCount
            );
    }

    return group;
}

void NewTestLayoutActionGroup_Destroy(NewTestLayoutActionGroup *group)
{
    int i;

    if (!group)
        return;

    for (i = 0; i < group->buttonCount; i++)
        NewTestLayoutActionButton_Destroy(group->buttons[i]);

    if (group->hwnd)
        DestroyWindow(group->hwnd);

    NewTestLayoutTheme_DeleteFonts(&group->theme);

    free(group);
}

HWND NewTestLayoutActionGroup_GetHwnd(NewTestLayoutActionGroup *group)
{
    return group ? group->hwnd : NULL;
}

void NewTestLayoutActionGroup_SetTheme(NewTestLayoutActionGroup *group, const NewTestLayoutTheme *theme)
{
    int i;

    if (!group)
        return;

    NewTestLayoutTheme_DeleteFonts(&group->theme);
    Ntl_CopyTheme(&group->theme, theme);
    NewTestLayoutTheme_CreateDefaultFonts(&group->theme);

    for (i = 0; i < group->buttonCount; i++)
        NewTestLayoutActionButton_SetTheme(group->buttons[i], &group->theme);

    InvalidateRect(group->hwnd, NULL, TRUE);
}

void NewTestLayoutActionGroup_SetRect(NewTestLayoutActionGroup *group, const RECT *rect)
{
    if (!group || !rect)
        return;

    MoveWindow(
        group->hwnd,
        rect->left,
        rect->top,
        rect->right - rect->left,
        rect->bottom - rect->top,
        TRUE
    );
}

void NewTestLayoutActionGroup_Show(NewTestLayoutActionGroup *group, int show)
{
    if (!group)
        return;

    group->visible = show ? 1 : 0;
    ShowWindow(group->hwnd, group->visible ? SW_SHOW : SW_HIDE);
}

int NewTestLayoutActionGroup_IsVisible(NewTestLayoutActionGroup *group)
{
    return group ? group->visible : 0;
}

void NewTestLayoutActionGroup_SetTitle(NewTestLayoutActionGroup *group, const char *title)
{
    if (!group)
        return;

    Ntl_CopyText(group->title, sizeof(group->title), title);
    InvalidateRect(group->hwnd, NULL, FALSE);
}

void NewTestLayoutActionGroup_SetBorderVisible(NewTestLayoutActionGroup *group, int visible)
{
    if (!group)
        return;

    group->borderVisible = visible ? 1 : 0;
    InvalidateRect(group->hwnd, NULL, TRUE);
}

void NewTestLayoutActionGroup_SetPadding(NewTestLayoutActionGroup *group, int padding)
{
    if (!group)
        return;

    if (padding < 0)
        padding = 0;

    group->padding = padding;
    NewTestLayoutActionGroup_LayoutButtons(group);
    InvalidateRect(group->hwnd, NULL, TRUE);
}

void NewTestLayoutActionGroup_SetButtonCount(
    NewTestLayoutActionGroup *group,
    int buttonIndex,
    int count,
    int hasCount
)
{
    if (!group)
        return;

    if (buttonIndex < 0 || buttonIndex >= group->buttonCount)
        return;

    NewTestLayoutActionButton_SetCount(
        group->buttons[buttonIndex],
        count,
        hasCount
    );
}

void NewTestLayoutActionGroup_SetCountOptions(
    NewTestLayoutActionGroup *group,
    int showCounts,
    int showZeroCounts,
    int countInParentheses,
    int colorCountText
)
{
    int i;

    if (!group)
        return;

    for (i = 0; i < group->buttonCount; i++)
    {
        NewTestLayoutActionButton_SetCountOptions(
            group->buttons[i],
            showCounts,
            showZeroCounts,
            countInParentheses,
            colorCountText
        );
    }
}

void NewTestLayoutActionGroup_LayoutButtons(NewTestLayoutActionGroup *group)
{
    RECT rc;
    RECT buttonRc;
    int titleH;
    int gap;
    int x;
    int y;
    int w;
    int h;
    int buttonW;
    int i;

    if (!group || !group->hwnd)
        return;

    GetClientRect(group->hwnd, &rc);

    titleH = 28;
    gap = 12;

    x = group->padding;
    y = titleH + group->padding / 2;
    w = rc.right - rc.left - group->padding * 2;
    h = rc.bottom - y - group->padding;

    if (w < 1)
        w = 1;

    if (h < 1)
        h = 1;

    if (group->buttonCount <= 0)
        return;

    buttonW = (w - gap * (group->buttonCount - 1)) / group->buttonCount;

    if (buttonW < 40)
        buttonW = 40;

    for (i = 0; i < group->buttonCount; i++)
    {
        SetRect(
            &buttonRc,
            x + i * (buttonW + gap),
            y,
            x + i * (buttonW + gap) + buttonW,
            y + h
        );

        NewTestLayoutActionButton_SetRect(group->buttons[i], &buttonRc);
    }
}

static void GearButton_DrawToothLine(HDC hdc, int cx, int cy, int dx, int dy, int inner, int outer)
{
    int x1;
    int y1;
    int x2;
    int y2;

    x1 = cx + (dx * inner) / 100;
    y1 = cy + (dy * inner) / 100;
    x2 = cx + (dx * outer) / 100;
    y2 = cy + (dy * outer) / 100;

    MoveToEx(hdc, x1, y1, NULL);
    LineTo(hdc, x2, y2);
}

static void GearButton_Draw(NewTestLayoutGearButton *gear, HDC hdc)
{
    RECT rc;
    int cx;
    int cy;
    int rOuter;
    int rInner;
    int rToothInner;
    int rToothOuter;
    int i;
    HPEN pen;
    HBRUSH brush;
    HGDIOBJ oldPen;
    HGDIOBJ oldBrush;

    static const int dx[8] = { 100, 71, 0, -71, -100, -71, 0, 71 };
    static const int dy[8] = { 0, 71, 100, 71, 0, -71, -100, -71 };

    if (!gear || !hdc)
        return;

    GetClientRect(gear->hwnd, &rc);

    Ntl_FillRect(hdc, &rc, gear->theme.gearBackColor);
    Ntl_DrawBorder(hdc, &rc, gear->theme.gearBorderColor);

    cx = (rc.left + rc.right) / 2;
    cy = (rc.top + rc.bottom) / 2;

    rOuter = (rc.right - rc.left) / 3;

    if (rc.bottom - rc.top < rOuter * 3)
        rOuter = (rc.bottom - rc.top) / 3;

    if (rOuter < 5)
        rOuter = 5;

    rInner = rOuter / 2;
    rToothInner = rOuter - 2;
    rToothOuter = rOuter + 4;

    pen = CreatePen(PS_SOLID, 2, gear->theme.gearColor);
    brush = CreateSolidBrush(gear->theme.gearBackColor);

    if (!pen || !brush)
    {
        if (pen)
            DeleteObject(pen);

        if (brush)
            DeleteObject(brush);

        return;
    }

    oldPen = SelectObject(hdc, pen);
    oldBrush = SelectObject(hdc, brush);

    for (i = 0; i < 8; i++)
    {
        GearButton_DrawToothLine(
            hdc,
            cx,
            cy,
            dx[i],
            dy[i],
            rToothInner,
            rToothOuter
        );
    }

    Ellipse(
        hdc,
        cx - rOuter,
        cy - rOuter,
        cx + rOuter,
        cy + rOuter
    );

    Ellipse(
        hdc,
        cx - rInner,
        cy - rInner,
        cx + rInner,
        cy + rInner
    );

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);

    DeleteObject(brush);
    DeleteObject(pen);
}

static LRESULT CALLBACK GearButtonProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    NewTestLayoutGearButton *gear;
    PAINTSTRUCT ps;
    HDC hdc;

    gear = (NewTestLayoutGearButton *)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    switch (msg)
    {
        case WM_LBUTTONDOWN:
        {
            if (gear)
            {
                gear->pressed = 1;
                SetCapture(hwnd);
                InvalidateRect(hwnd, NULL, FALSE);
            }

            return 0;
        }

        case WM_LBUTTONUP:
        {
            if (gear && gear->pressed)
            {
                RECT rc;
                POINT pt;

                gear->pressed = 0;
                ReleaseCapture();
                InvalidateRect(hwnd, NULL, FALSE);

                GetClientRect(hwnd, &rc);
                pt.x = LOWORD(lParam);
                pt.y = HIWORD(lParam);

                if (PtInRect(&rc, pt))
                    Ntl_SendCommand(gear->parent, hwnd, gear->id, BN_CLICKED);
            }

            return 0;
        }

        case WM_PAINT:
        {
            hdc = BeginPaint(hwnd, &ps);
            GearButton_Draw(gear, hdc);
            EndPaint(hwnd, &ps);
            return 0;
        }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

NewTestLayoutGearButton *NewTestLayoutGearButton_Create(
    HWND parent,
    HINSTANCE hInstance,
    int id,
    const NewTestLayoutTheme *theme
)
{
    NewTestLayoutGearButton *gear;

    gear = (NewTestLayoutGearButton *)malloc(sizeof(*gear));

    if (!gear)
        return NULL;

    ZeroMemory(gear, sizeof(*gear));

    gear->parent = parent;
    gear->hInstance = hInstance;
    gear->id = id;
    gear->visible = 1;

    Ntl_CopyTheme(&gear->theme, theme);
    NewTestLayoutTheme_CreateDefaultFonts(&gear->theme);

    gear->hwnd = CreateWindowEx(
        0,
        NTL_GEAR_BUTTON_CLASS_NAME,
        "",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP,
        0,
        0,
        28,
        28,
        parent,
        (HMENU)id,
        hInstance,
        NULL
    );

    if (!gear->hwnd)
    {
        NewTestLayoutTheme_DeleteFonts(&gear->theme);
        free(gear);
        return NULL;
    }

    SetWindowLongPtr(gear->hwnd, GWLP_USERDATA, (LONG_PTR)gear);

    return gear;
}

void NewTestLayoutGearButton_Destroy(NewTestLayoutGearButton *gear)
{
    if (!gear)
        return;

    if (gear->hwnd)
        DestroyWindow(gear->hwnd);

    NewTestLayoutTheme_DeleteFonts(&gear->theme);

    free(gear);
}

HWND NewTestLayoutGearButton_GetHwnd(NewTestLayoutGearButton *gear)
{
    return gear ? gear->hwnd : NULL;
}

void NewTestLayoutGearButton_SetTheme(NewTestLayoutGearButton *gear, const NewTestLayoutTheme *theme)
{
    if (!gear)
        return;

    NewTestLayoutTheme_DeleteFonts(&gear->theme);
    Ntl_CopyTheme(&gear->theme, theme);
    NewTestLayoutTheme_CreateDefaultFonts(&gear->theme);

    InvalidateRect(gear->hwnd, NULL, TRUE);
}

void NewTestLayoutGearButton_SetRect(NewTestLayoutGearButton *gear, const RECT *rect)
{
    if (!gear || !rect)
        return;

    MoveWindow(
        gear->hwnd,
        rect->left,
        rect->top,
        rect->right - rect->left,
        rect->bottom - rect->top,
        TRUE
    );
}

void NewTestLayoutGearButton_Show(NewTestLayoutGearButton *gear, int show)
{
    if (!gear)
        return;

    gear->visible = show ? 1 : 0;
    ShowWindow(gear->hwnd, gear->visible ? SW_SHOW : SW_HIDE);
}

int NewTestLayoutGearButton_IsVisible(NewTestLayoutGearButton *gear)
{
    return gear ? gear->visible : 0;
}

int NewTestLayoutControls_RegisterClasses(HINSTANCE hInstance)
{
    WNDCLASS wc;

    ZeroMemory(&wc, sizeof(wc));
    wc.lpfnWndProc = FauxComboProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = NTL_FAUX_COMBO_CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_IBEAM);
    wc.hbrBackground = NULL;

    RegisterClass(&wc);

    ZeroMemory(&wc, sizeof(wc));
    wc.lpfnWndProc = ActionButtonProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = NTL_ACTION_BUTTON_CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_HAND);
    wc.hbrBackground = NULL;

    RegisterClass(&wc);

    ZeroMemory(&wc, sizeof(wc));
    wc.lpfnWndProc = ActionGroupProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = NTL_ACTION_GROUP_CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;

    RegisterClass(&wc);

    ZeroMemory(&wc, sizeof(wc));
    wc.lpfnWndProc = GearButtonProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = NTL_GEAR_BUTTON_CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_HAND);
    wc.hbrBackground = NULL;

    RegisterClass(&wc);

    return 1;
}

void NewTestLayoutControls_UnregisterClasses(HINSTANCE hInstance)
{
    UnregisterClass(NTL_FAUX_COMBO_CLASS_NAME, hInstance);
    UnregisterClass(NTL_ACTION_BUTTON_CLASS_NAME, hInstance);
    UnregisterClass(NTL_ACTION_GROUP_CLASS_NAME, hInstance);
    UnregisterClass(NTL_GEAR_BUTTON_CLASS_NAME, hInstance);
}
