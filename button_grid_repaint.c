#include "button_grid_core_internal.h"

static int ButtonGrid_GetClientSize(
    ButtonGrid *grid,
    int *width,
    int *height
)
{
    RECT rc;

    if (width)
        *width = 0;

    if (height)
        *height = 0;

    if (!grid || !grid->hwnd)
        return 0;

    GetClientRect(grid->hwnd, &rc);

    if (width)
        *width = rc.right - rc.left;

    if (height)
        *height = rc.bottom - rc.top;

    return 1;
}

static void ButtonGrid_RememberClientSize(ButtonGrid *grid)
{
    int width;
    int height;

    if (!ButtonGrid_GetClientSize(grid, &width, &height))
        return;

    grid->hasLastLayoutClientSize = 1;
    grid->lastLayoutClientWidth = width;
    grid->lastLayoutClientHeight = height;
}

static int ButtonGrid_ClientSizeChanged(ButtonGrid *grid)
{
    int width;
    int height;

    if (!ButtonGrid_GetClientSize(grid, &width, &height))
        return 0;

    if (!grid->hasLastLayoutClientSize)
        return 1;

    if (grid->lastLayoutClientWidth != width)
        return 1;

    if (grid->lastLayoutClientHeight != height)
        return 1;

    return 0;
}

void ButtonGrid_RedrawGridWindow(ButtonGrid *grid, int updateNow)
{
    UINT flags;

    if (!grid || !grid->hwnd)
        return;

    flags = RDW_INVALIDATE | RDW_NOERASE;

    if (updateNow)
        flags |= RDW_UPDATENOW;

    RedrawWindow(grid->hwnd, NULL, NULL, flags);
}

void ButtonGrid_RelayoutForResize(ButtonGrid *grid, int updateNow)
{
    if (!grid)
        return;

    if (!ButtonGrid_ClientSizeChanged(grid))
        return;

    ButtonGrid_UpdateDpi(grid);
    ButtonGrid_Layout(grid);
    ButtonGrid_LayoutSettingsPage(grid);
    ButtonGrid_RememberClientSize(grid);
    ButtonGrid_RedrawGridWindow(grid, updateNow);
}

void ButtonGrid_RelayoutAndRedraw(ButtonGrid *grid, int updateNow)
{
    if (!grid)
        return;

    ButtonGrid_UpdateDpi(grid);
    ButtonGrid_Layout(grid);
    ButtonGrid_LayoutSettingsPage(grid);
    ButtonGrid_RememberClientSize(grid);
    ButtonGrid_RedrawAllButtons(grid);
    ButtonGrid_RedrawGridWindow(grid, updateNow);
}

void ButtonGrid_HandleDpiChanged(ButtonGrid *grid)
{
    if (!grid)
        return;

    ButtonGrid_UpdateDpi(grid);
    ButtonGrid_Layout(grid);
    ButtonGrid_LayoutSettingsPage(grid);
    ButtonGrid_RememberClientSize(grid);
    ButtonGrid_RedrawAllButtons(grid);
    ButtonGrid_RedrawGridWindow(grid, 1);
}