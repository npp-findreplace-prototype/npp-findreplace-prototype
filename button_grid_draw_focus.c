#include "button_grid_draw_internal.h"

void ButtonGridDraw_DrawKeyboardFocus(
    ButtonGrid *grid,
    HDC hdc,
    RECT *rc,
    ButtonItem *button
)
{
    RECT focusRc;

    if (!grid || !button || !hdc || !rc)
        return;

    if (GetFocus() != button->hwnd)
        return;

    focusRc = *rc;

    InflateRect(
        &focusRc,
        -ButtonGrid_DpiScaleMin(grid, 4, 2),
        -ButtonGrid_DpiScaleMin(grid, 4, 2)
    );

    DrawFocusRect(hdc, &focusRc);
}