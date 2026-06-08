#include "button_grid_draw_internal.h"

COLORREF ButtonGridDraw_GetFallbackBackColor(void)
{
    return GetSysColor(COLOR_WINDOW);
}

COLORREF ButtonGridDraw_GetGridBackColor(ButtonGrid *grid)
{
    if (!grid)
        return ButtonGridDraw_GetFallbackBackColor();

    return grid->backColor;
}

COLORREF ButtonGridDraw_GetAutoTitleBackColor(ButtonGrid *grid)
{
    return ButtonGridDraw_GetGridBackColor(grid);
}

COLORREF ButtonGridDraw_GetButtonBackgroundColor(ButtonGrid *grid)
{
    if (!grid)
        return ButtonGridDraw_GetFallbackBackColor();

    if (grid->buttonBackMode == BUTTON_GRID_BUTTON_BACK_TRANSPARENT)
        return ButtonGridDraw_GetGridBackColor(grid);

    return grid->backColor;
}

void ButtonGridDraw_FillSolid(
    HDC hdc,
    const RECT *rc,
    COLORREF color
)
{
    Ui_FillSolidRect(hdc, rc, color);
}