#include "button_grid_core_internal.h"

void ButtonGrid_SetClickCallback(
    HWND gridHwnd,
    ButtonGridClickCallback onClick
)
{
    ButtonGrid *grid;

    grid = ButtonGrid_Get(gridHwnd);

    if (!grid)
        return;

    grid->onClick = onClick;
}

static int ButtonGrid_RectAlreadyMatches(
    HWND hwnd,
    int x,
    int y,
    int width,
    int height,
    int *sizeChanged
)
{
    RECT rc;
    int currentWidth;
    int currentHeight;

    if (sizeChanged)
        *sizeChanged = 1;

    if (!Ui_GetWindowRectInParent(hwnd, &rc))
        return 0;

    currentWidth = rc.right - rc.left;
    currentHeight = rc.bottom - rc.top;

    if (sizeChanged)
    {
        if (currentWidth == width && currentHeight == height)
            *sizeChanged = 0;
        else
            *sizeChanged = 1;
    }

    if (rc.left != x)
        return 0;

    if (rc.top != y)
        return 0;

    if (currentWidth != width)
        return 0;

    if (currentHeight != height)
        return 0;

    return 1;
}

void ButtonGrid_SetRect(
    HWND gridHwnd,
    int x,
    int y,
    int width,
    int height
)
{
    ButtonGrid *grid;
    int sizeChanged;

    if (!gridHwnd)
        return;

    if (width < 1)
        width = 1;

    if (height < 1)
        height = 1;

    grid = ButtonGrid_Get(gridHwnd);

    if (grid)
    {
        ButtonGrid_UpdateDpi(grid);
        ButtonGrid_AdjustRectToLayoutSteps(grid, &width, &height);
    }

    sizeChanged = 1;

    if (ButtonGrid_RectAlreadyMatches(
            gridHwnd,
            x,
            y,
            width,
            height,
            &sizeChanged
        ))
    {
        return;
    }

    SetWindowPos(
        gridHwnd,
        NULL,
        x,
        y,
        width,
        height,
        SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS
    );

    /*
        Do not relayout here. SetWindowPos will produce a window-position/size
        notification for the grid itself, and button_grid_window.c handles that.
        Doing it here as well doubles layout and owner-draw traffic during resize.
    */
}

void ButtonGrid_SetButtonSize(
    HWND gridHwnd,
    int buttonWidth,
    int buttonHeight
)
{
    ButtonGrid *grid;

    grid = ButtonGrid_Get(gridHwnd);

    if (!grid)
        return;

    if (buttonWidth < 1)
        buttonWidth = 1;

    if (buttonHeight < 1)
        buttonHeight = 1;

    if (grid->buttonWidth == buttonWidth &&
        grid->buttonHeight == buttonHeight)
    {
        return;
    }

    grid->buttonWidth = buttonWidth;
    grid->buttonHeight = buttonHeight;

    ButtonGrid_RelayoutAndRedraw(grid, 0);
}

void ButtonGrid_SetSizeMode(
    HWND gridHwnd,
    int sizeMode
)
{
    ButtonGrid *grid;
    int normalized;

    grid = ButtonGrid_Get(gridHwnd);

    if (!grid)
        return;

    normalized = ButtonGrid_NormalizeSizeMode(sizeMode);

    if (grid->sizeMode == normalized)
        return;

    grid->sizeMode = normalized;

    ButtonGrid_RelayoutAndRedraw(grid, 0);
}

void ButtonGrid_SetSpacing(
    HWND gridHwnd,
    int horizontalSpacing,
    int verticalSpacing
)
{
    ButtonGrid *grid;

    grid = ButtonGrid_Get(gridHwnd);

    if (!grid)
        return;

    if (horizontalSpacing < 0)
        horizontalSpacing = 0;

    if (verticalSpacing < 0)
        verticalSpacing = 0;

    if (grid->horizontalSpacing == horizontalSpacing &&
        grid->verticalSpacing == verticalSpacing)
    {
        return;
    }

    grid->horizontalSpacing = horizontalSpacing;
    grid->verticalSpacing = verticalSpacing;

    ButtonGrid_RelayoutAndRedraw(grid, 0);
}

void ButtonGrid_SetLayout(HWND gridHwnd, int layout)
{
    ButtonGrid *grid;

    grid = ButtonGrid_Get(gridHwnd);

    if (!grid)
        return;

    if (layout != BUTTON_GRID_LAYOUT_HORIZONTAL &&
        layout != BUTTON_GRID_LAYOUT_VERTICAL)
    {
        layout = BUTTON_GRID_LAYOUT_HORIZONTAL;
    }

    if (grid->layout == layout)
        return;

    grid->layout = layout;

    ButtonGrid_RelayoutAndRedraw(grid, 0);
}

void ButtonGrid_Relayout(HWND gridHwnd)
{
    ButtonGrid *grid;

    grid = ButtonGrid_Get(gridHwnd);

    if (!grid)
        return;

    ButtonGrid_RelayoutAndRedraw(grid, 0);
}