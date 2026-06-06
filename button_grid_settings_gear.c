#include <windows.h>

#include "button_grid_internal.h"
#include "button_grid_settings.h"
#include "button_grid_settings_internal.h"

static void ButtonGrid_GetGearRect(ButtonGrid *grid, RECT *rc)
{
    RECT client;
    int size;
    int margin;

    SetRectEmpty(rc);

    if (!grid)
        return;

    GetClientRect(grid->hwnd, &client);

    size = grid->gearSize;
    margin = grid->gearMargin;

    if (size < 8)
        size = 8;

    if (margin < 0)
        margin = 0;

    if (grid->gearCorner == BUTTON_GRID_GEAR_CORNER_TOP_LEFT)
    {
        rc->left = client.left + margin;
        rc->top = client.top + margin;
    }
    else if (grid->gearCorner == BUTTON_GRID_GEAR_CORNER_BOTTOM_LEFT)
    {
        rc->left = client.left + margin;
        rc->top = client.bottom - margin - size;
    }
    else if (grid->gearCorner == BUTTON_GRID_GEAR_CORNER_BOTTOM_RIGHT)
    {
        rc->left = client.right - margin - size;
        rc->top = client.bottom - margin - size;
    }
    else
    {
        rc->left = client.right - margin - size;
        rc->top = client.top + margin;
    }

    rc->right = rc->left + size;
    rc->bottom = rc->top + size;
}

static void ButtonGrid_DrawGearToothRects(
    HDC hdc,
    const RECT *rc,
    int cx,
    int cy
)
{
    Rectangle(hdc, cx - 2, rc->top + 4, cx + 2, rc->top + 10);
    Rectangle(hdc, cx - 2, rc->bottom - 10, cx + 2, rc->bottom - 4);
    Rectangle(hdc, rc->left + 4, cy - 2, rc->left + 10, cy + 2);
    Rectangle(hdc, rc->right - 10, cy - 2, rc->right - 4, cy + 2);
}

static void ButtonGrid_DrawGearSpokes(
    HDC hdc,
    const RECT *rc,
    int cx,
    int cy
)
{
    MoveToEx(hdc, rc->left + 7, rc->top + 7, NULL);
    LineTo(hdc, cx, cy);

    MoveToEx(hdc, rc->right - 7, rc->top + 7, NULL);
    LineTo(hdc, cx, cy);

    MoveToEx(hdc, rc->left + 7, rc->bottom - 7, NULL);
    LineTo(hdc, cx, cy);

    MoveToEx(hdc, rc->right - 7, rc->bottom - 7, NULL);
    LineTo(hdc, cx, cy);
}

static void ButtonGrid_DrawGearBackground(
    ButtonGrid *grid,
    HDC hdc,
    const RECT *rc
)
{
    HBRUSH backBrush;
    HPEN borderPen;
    HGDIOBJ oldBrush;
    HGDIOBJ oldPen;

    backBrush = CreateSolidBrush(grid->gearBackColor);
    borderPen = CreatePen(PS_SOLID, 1, grid->gearBorderColor);

    oldBrush = SelectObject(hdc, backBrush);
    oldPen = SelectObject(hdc, borderPen);

    RoundRect(hdc, rc->left, rc->top, rc->right, rc->bottom, 6, 6);

    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);

    DeleteObject(borderPen);
    DeleteObject(backBrush);
}

static void ButtonGrid_DrawGearBody(
    ButtonGrid *grid,
    HDC hdc,
    const RECT *rc
)
{
    int cx;
    int cy;
    int size;
    int outerR;
    int innerR;
    HBRUSH gearBrush;
    HBRUSH holeBrush;
    HPEN gearPen;
    HGDIOBJ oldBrush;
    HGDIOBJ oldPen;

    size = rc->right - rc->left;

    cx = (rc->left + rc->right) / 2;
    cy = (rc->top + rc->bottom) / 2;

    outerR = size / 4;
    innerR = size / 10;

    if (innerR < 2)
        innerR = 2;

    gearBrush = CreateSolidBrush(grid->gearColor);
    gearPen = CreatePen(PS_SOLID, 2, grid->gearColor);

    oldBrush = SelectObject(hdc, gearBrush);
    oldPen = SelectObject(hdc, gearPen);

    ButtonGrid_DrawGearToothRects(hdc, rc, cx, cy);
    ButtonGrid_DrawGearSpokes(hdc, rc, cx, cy);

    Ellipse(
        hdc,
        cx - outerR,
        cy - outerR,
        cx + outerR,
        cy + outerR
    );

    holeBrush = CreateSolidBrush(grid->gearBackColor);
    SelectObject(hdc, holeBrush);

    Ellipse(
        hdc,
        cx - innerR,
        cy - innerR,
        cx + innerR,
        cy + innerR
    );

    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);

    DeleteObject(holeBrush);
    DeleteObject(gearPen);
    DeleteObject(gearBrush);
}

void ButtonGrid_DrawGearIcon(ButtonGrid *grid, HDC hdc)
{
    RECT rc;
    int size;

    if (!grid || !hdc)
        return;

    if (!grid->showGearIcon)
        return;

    ButtonGrid_GetGearRect(grid, &rc);

    size = rc.right - rc.left;

    if (size < 8)
        return;

    ButtonGrid_DrawGearBackground(grid, hdc, &rc);
    ButtonGrid_DrawGearBody(grid, hdc, &rc);
}

int ButtonGrid_HandleGearClick(ButtonGrid *grid, int x, int y)
{
    RECT rc;

    if (!grid)
        return 0;

    if (!grid->showGearIcon)
        return 0;

    ButtonGrid_GetGearRect(grid, &rc);

    if (x < rc.left || x >= rc.right)
        return 0;

    if (y < rc.top || y >= rc.bottom)
        return 0;

    ButtonGrid_ToggleSettingsPage(grid);

    return 1;
}