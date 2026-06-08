#ifndef BUTTON_GRID_DRAW_INTERNAL_H
#define BUTTON_GRID_DRAW_INTERNAL_H

#include "button_grid_internal.h"
#include "button_grid_settings.h"
#include "ui_drawing.h"

COLORREF ButtonGridDraw_GetFallbackBackColor(void);
COLORREF ButtonGridDraw_GetGridBackColor(ButtonGrid *grid);
COLORREF ButtonGridDraw_GetAutoTitleBackColor(ButtonGrid *grid);
COLORREF ButtonGridDraw_GetButtonBackgroundColor(ButtonGrid *grid);

void ButtonGridDraw_FillSolid(
    HDC hdc,
    const RECT *rc,
    COLORREF color
);

void ButtonGridDraw_DrawBorder(
    ButtonGrid *grid,
    HDC hdc
);

void ButtonGridDraw_DrawKeyboardFocus(
    ButtonGrid *grid,
    HDC hdc,
    RECT *rc,
    ButtonItem *button
);

void ButtonGridDraw_DrawButton(
    ButtonGrid *grid,
    DRAWITEMSTRUCT *draw
);

#endif