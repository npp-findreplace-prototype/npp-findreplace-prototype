#include "button_grid_draw_internal.h"

static void ButtonGridDraw_DrawGridChrome(
    ButtonGrid *grid,
    HDC hdc
)
{
    RECT rc;

    if (!grid || !hdc)
        return;

    GetClientRect(grid->hwnd, &rc);

    ButtonGridDraw_FillSolid(
        hdc,
        &rc,
        ButtonGridDraw_GetGridBackColor(grid)
    );

    ButtonGridDraw_DrawBorder(grid, hdc);
    ButtonGrid_DrawGearIcon(grid, hdc);
}

static void ButtonGridDraw_DrawGridChromeBuffered(
    void *context,
    HDC hdc,
    RECT *rc
)
{
    (void)rc;

    ButtonGridDraw_DrawGridChrome(
        (ButtonGrid *)context,
        hdc
    );
}

LRESULT ButtonGrid_HandlePaint(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc;
    RECT rc;
    ButtonGrid *grid;

    grid = ButtonGrid_Get(hwnd);

    hdc = BeginPaint(hwnd, &ps);

    GetClientRect(hwnd, &rc);

    if (!grid)
    {
        ButtonGridDraw_FillSolid(
            hdc,
            &rc,
            ButtonGridDraw_GetFallbackBackColor()
        );

        EndPaint(hwnd, &ps);
        return 0;
    }

    ButtonGrid_UpdateDpi(grid);

    if (!Ui_DrawToMemoryThenBlit(
            hdc,
            &rc,
            grid,
            ButtonGridDraw_DrawGridChromeBuffered
        ))
    {
        ButtonGridDraw_DrawGridChrome(grid, hdc);
    }

    EndPaint(hwnd, &ps);

    return 0;
}

LRESULT ButtonGrid_HandleDrawItem(ButtonGrid *grid, LPARAM lParam)
{
    DRAWITEMSTRUCT *draw;

    draw = (DRAWITEMSTRUCT *)lParam;

    if (!grid || !draw)
        return 0;

    ButtonGrid_UpdateDpi(grid);
    ButtonGridDraw_DrawButton(grid, draw);

    return TRUE;
}

LRESULT ButtonGrid_HandleEraseBackground(HWND hwnd, WPARAM wParam)
{
    (void)hwnd;
    (void)wParam;

    /*
        WM_PAINT fills the complete grid chrome using the buffered paint path.
        Painting here causes an extra background pass during live resize and
        can flicker with tools that resize by sending window-position changes.
    */
    return 1;
}