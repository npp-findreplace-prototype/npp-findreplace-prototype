#include "button_grid_layout_internal.h"

void ButtonGrid_GetAlignmentOffset(
    ButtonGrid *grid,
    RECT *content,
    RECT *used,
    int *offsetX,
    int *offsetY
)
{
    int contentW;
    int contentH;
    int usedW;
    int usedH;
    int freeW;
    int freeH;

    *offsetX = 0;
    *offsetY = 0;

    if (!grid || !content || !used)
        return;

    contentW = content->right - content->left;
    contentH = content->bottom - content->top;
    usedW = used->right - used->left;
    usedH = used->bottom - used->top;

    freeW = contentW - usedW;
    freeH = contentH - usedH;

    if (grid->contentAlignment == BUTTON_GRID_ALIGN_TOP_LEFT)
    {
        *offsetX = 0;
        *offsetY = 0;
    }
    else if (grid->contentAlignment == BUTTON_GRID_ALIGN_TOP)
    {
        *offsetX = freeW / 2;
        *offsetY = 0;
    }
    else if (grid->contentAlignment == BUTTON_GRID_ALIGN_TOP_RIGHT)
    {
        *offsetX = freeW;
        *offsetY = 0;
    }
    else if (grid->contentAlignment == BUTTON_GRID_ALIGN_LEFT)
    {
        *offsetX = 0;
        *offsetY = freeH / 2;
    }
    else if (grid->contentAlignment == BUTTON_GRID_ALIGN_CENTER)
    {
        *offsetX = freeW / 2;
        *offsetY = freeH / 2;
    }
    else if (grid->contentAlignment == BUTTON_GRID_ALIGN_RIGHT)
    {
        *offsetX = freeW;
        *offsetY = freeH / 2;
    }
    else if (grid->contentAlignment == BUTTON_GRID_ALIGN_BOTTOM_LEFT)
    {
        *offsetX = 0;
        *offsetY = freeH;
    }
    else if (grid->contentAlignment == BUTTON_GRID_ALIGN_BOTTOM)
    {
        *offsetX = freeW / 2;
        *offsetY = freeH;
    }
    else if (grid->contentAlignment == BUTTON_GRID_ALIGN_BOTTOM_RIGHT)
    {
        *offsetX = freeW;
        *offsetY = freeH;
    }
    else if (grid->contentAlignment == BUTTON_GRID_ALIGN_XY)
    {
        *offsetX = ButtonGrid_DpiScale(grid, grid->contentAlignX);
        *offsetY = ButtonGrid_DpiScale(grid, grid->contentAlignY);
    }
    else if (grid->contentAlignment == BUTTON_GRID_ALIGN_PERCENT)
    {
        *offsetX = MulDiv(freeW, grid->contentAlignPercentX, 100);
        *offsetY = MulDiv(freeH, grid->contentAlignPercentY, 100);
    }
}