#include "new_test_layout_controls_internal.h"

static void GearButton_DrawToothLine(
    HDC hdc,
    int cx,
    int cy,
    int dx,
    int dy,
    int inner,
    int outer
)
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

static void GearButton_DrawBuffered(void *context, HDC hdc, RECT *rc)
{
    (void)rc;
    GearButton_Draw((NewTestLayoutGearButton *)context, hdc);
}

LRESULT CALLBACK NewTestLayoutGearButton_WndProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam
)
{
    NewTestLayoutGearButton *gear;

    gear = (NewTestLayoutGearButton *)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    switch (msg)
    {
        case WM_ERASEBKGND:
        {
            return 1;
        }

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
            Ntl_DoubleBufferedPaint(hwnd, gear, GearButton_DrawBuffered);
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

    InvalidateRect(gear->hwnd, NULL, FALSE);
}

void NewTestLayoutGearButton_SetRect(NewTestLayoutGearButton *gear, const RECT *rect)
{
    if (!gear || !rect)
        return;

    SetWindowPos(
        gear->hwnd,
        NULL,
        rect->left,
        rect->top,
        rect->right - rect->left,
        rect->bottom - rect->top,
        SWP_NOZORDER | SWP_NOACTIVATE
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