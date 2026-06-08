#include "button_grid_layout_internal.h"

int ButtonGrid_NormalizeSizeMode(int sizeMode)
{
    if (sizeMode != BUTTON_GRID_SIZE_FIXED &&
        sizeMode != BUTTON_GRID_SIZE_MATCH_IMAGE_SIZE &&
        sizeMode != BUTTON_GRID_SIZE_MATCH_IMAGE_ASPECT_HORIZONTAL &&
        sizeMode != BUTTON_GRID_SIZE_MATCH_IMAGE_ASPECT_VERTICAL &&
        sizeMode != BUTTON_GRID_SIZE_MATCH_IMAGE_ASPECT_BY_LAYOUT)
    {
        return BUTTON_GRID_DEFAULT_SIZE_MODE;
    }

    return sizeMode;
}

void ButtonGrid_RedrawContainer(HWND hwnd)
{
    if (!hwnd)
        return;

    InvalidateRect(hwnd, NULL, FALSE);
    UpdateWindow(hwnd);
}

void ButtonGrid_Layout(ButtonGrid *grid)
{
    RECT content;
    RECT used;
    GridPosition *positions;

    if (!grid)
        return;

    ButtonGrid_UpdateAllButtonSizes(grid);
    ButtonGrid_GetContentRect(grid, &content);

    if (grid->buttonCount < 1 || !grid->buttons)
        return;

    positions = (GridPosition *)malloc(sizeof(GridPosition) * grid->buttonCount);

    if (!positions)
        return;

    ZeroMemory(positions, sizeof(GridPosition) * grid->buttonCount);

    if (grid->layout == BUTTON_GRID_LAYOUT_VERTICAL)
    {
        ButtonGrid_CalculateVerticalPositions(
            grid,
            &content,
            positions,
            &used
        );
    }
    else
    {
        ButtonGrid_CalculateHorizontalPositions(
            grid,
            &content,
            positions,
            &used
        );
    }

    ButtonGrid_ApplyPositions(
        grid,
        &content,
        positions,
        &used
    );

    free(positions);
}