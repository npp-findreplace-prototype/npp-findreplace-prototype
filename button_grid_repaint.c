#include "button_grid_core_internal.h"

void ButtonGrid_RedrawGridWindow(ButtonGrid *grid, int updateNow)
{
    UINT flags;

    if (!grid || !grid->hwnd)
        return;

    flags = RDW_INVALIDATE | RDW_NOERASE | RDW_ALLCHILDREN;

    if (updateNow)
        flags |= RDW_UPDATENOW;

    RedrawWindow(grid->hwnd, NULL, NULL, flags);
}

void ButtonGrid_RelayoutAndRedraw(ButtonGrid *grid, int updateNow)
{
    if (!grid)
        return;

    ButtonGrid_UpdateDpi(grid);
    ButtonGrid_UpdateAllButtonSizes(grid);
    ButtonGrid_Layout(grid);
    ButtonGrid_LayoutSettingsPage(grid);
    ButtonGrid_RedrawAllButtons(grid);
    ButtonGrid_RedrawGridWindow(grid, updateNow);
}

void ButtonGrid_HandleDpiChanged(ButtonGrid *grid)
{
    if (!grid)
        return;

    ButtonGrid_UpdateDpi(grid);
    ButtonGrid_UpdateAllButtonSizes(grid);
    ButtonGrid_Layout(grid);
    ButtonGrid_LayoutSettingsPage(grid);
    ButtonGrid_RedrawAllButtons(grid);
    ButtonGrid_RedrawGridWindow(grid, 1);
}