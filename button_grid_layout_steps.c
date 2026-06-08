#include "button_grid_layout_internal.h"

static int ButtonGrid_CeilDiv(int a, int b)
{
    if (b <= 0)
        return 0;

    if (a <= 0)
        return 0;

    return (a + b - 1) / b;
}

void ButtonGrid_AdjustRectToLayoutSteps(
    ButtonGrid *grid,
    int *width,
    int *height
)
{
    int buttonW;
    int buttonH;
    int spacingX;
    int spacingY;
    int extraW;
    int extraH;
    int contentW;
    int contentH;
    int columns;
    int rows;

    if (!grid || !width || !height)
        return;

    if (!grid->resizeInLayoutSteps)
        return;

    ButtonGrid_UpdateDpi(grid);

    buttonW = ButtonGrid_DpiScale(grid, grid->buttonWidth);
    buttonH = ButtonGrid_DpiScale(grid, grid->buttonHeight);

    if (buttonW < 1)
        buttonW = 1;

    if (buttonH < 1)
        buttonH = 1;

    spacingX = ButtonGrid_DpiScale(grid, grid->horizontalSpacing);
    spacingY = ButtonGrid_DpiScale(grid, grid->verticalSpacing);

    if (spacingX < 0)
        spacingX = 0;

    if (spacingY < 0)
        spacingY = 0;

    ButtonGrid_GetOuterDecorationSize(grid, &extraW, &extraH);

    contentW = *width - extraW;
    contentH = *height - extraH;

    if (contentW < buttonW)
        contentW = buttonW;

    if (contentH < buttonH)
        contentH = buttonH;

    if (grid->layout == BUTTON_GRID_LAYOUT_VERTICAL)
    {
        rows = (contentH + spacingY) / (buttonH + spacingY);

        if (rows < 1)
            rows = 1;

        columns = ButtonGrid_CeilDiv(grid->buttonCount, rows);

        if (columns < 1)
            columns = 1;
    }
    else
    {
        columns = (contentW + spacingX) / (buttonW + spacingX);

        if (columns < 1)
            columns = 1;

        rows = ButtonGrid_CeilDiv(grid->buttonCount, columns);

        if (rows < 1)
            rows = 1;
    }

    *width =
        extraW +
        columns * buttonW +
        (columns - 1) * spacingX;

    *height =
        extraH +
        rows * buttonH +
        (rows - 1) * spacingY;

    if (*width < 1)
        *width = 1;

    if (*height < 1)
        *height = 1;
}