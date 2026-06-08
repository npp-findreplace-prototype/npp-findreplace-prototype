#include "new_test_layout_controls_internal.h"

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

static void ActionButton_DrawBuffered(void *context, HDC hdc, RECT *rc)
{
    (void)rc;
    ActionButton_Draw((NewTestLayoutActionButton *)context, hdc);
}

LRESULT CALLBACK NewTestLayoutActionButton_WndProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam
)
{
    NewTestLayoutActionButton *button;

    button = (NewTestLayoutActionButton *)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    switch (msg)
    {
        case WM_ERASEBKGND:
        {
            return 1;
        }

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
            Ntl_DoubleBufferedPaint(hwnd, button, ActionButton_DrawBuffered);
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

    InvalidateRect(button->hwnd, NULL, FALSE);
}

void NewTestLayoutActionButton_SetRect(NewTestLayoutActionButton *button, const RECT *rect)
{
    if (!button || !rect)
        return;

    SetWindowPos(
        button->hwnd,
        NULL,
        rect->left,
        rect->top,
        rect->right - rect->left,
        rect->bottom - rect->top,
        SWP_NOZORDER | SWP_NOACTIVATE
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