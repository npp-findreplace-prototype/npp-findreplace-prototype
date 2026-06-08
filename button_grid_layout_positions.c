#include "button_grid_layout_internal.h"

static void ButtonGrid_InitLayoutBounds(
    RECT *used,
    int x,
    int y,
    int width,
    int height
)
{
    used->left = x;
    used->top = y;
    used->right = x + width;
    used->bottom = y + height;
}

static void ButtonGrid_ExpandLayoutBounds(
    RECT *used,
    int x,
    int y,
    int width,
    int height
)
{
    if (x < used->left)
        used->left = x;

    if (y < used->top)
        used->top = y;

    if (x + width > used->right)
        used->right = x + width;

    if (y + height > used->bottom)
        used->bottom = y + height;
}

void ButtonGrid_CalculateHorizontalPositions(
    ButtonGrid *grid,
    RECT *content,
    GridPosition *positions,
    RECT *used
)
{
    int i;
    int x;
    int y;
    int rowHeight;
    int spacingX;
    int spacingY;
    int initialized;

    spacingX = ButtonGrid_DpiScale(grid, grid->horizontalSpacing);
    spacingY = ButtonGrid_DpiScale(grid, grid->verticalSpacing);

    if (spacingX < 0)
        spacingX = 0;

    if (spacingY < 0)
        spacingY = 0;

    x = content->left;
    y = content->top;
    rowHeight = 0;
    initialized = 0;

    for (i = 0; i < grid->buttonCount; i++)
    {
        ButtonItem *button;

        button = &grid->buttons[i];

        if (x > content->left &&
            x + button->width > content->right)
        {
            x = content->left;
            y += rowHeight + spacingY;
            rowHeight = 0;
        }

        positions[i].x = x;
        positions[i].y = y;

        if (!initialized)
        {
            ButtonGrid_InitLayoutBounds(
                used,
                x,
                y,
                button->width,
                button->height
            );

            initialized = 1;
        }
        else
        {
            ButtonGrid_ExpandLayoutBounds(
                used,
                x,
                y,
                button->width,
                button->height
            );
        }

        if (button->height > rowHeight)
            rowHeight = button->height;

        x += button->width + spacingX;
    }

    if (!initialized)
        SetRectEmpty(used);
}

void ButtonGrid_CalculateVerticalPositions(
    ButtonGrid *grid,
    RECT *content,
    GridPosition *positions,
    RECT *used
)
{
    int i;
    int x;
    int y;
    int columnWidth;
    int spacingX;
    int spacingY;
    int initialized;

    spacingX = ButtonGrid_DpiScale(grid, grid->horizontalSpacing);
    spacingY = ButtonGrid_DpiScale(grid, grid->verticalSpacing);

    if (spacingX < 0)
        spacingX = 0;

    if (spacingY < 0)
        spacingY = 0;

    x = content->left;
    y = content->top;
    columnWidth = 0;
    initialized = 0;

    for (i = 0; i < grid->buttonCount; i++)
    {
        ButtonItem *button;

        button = &grid->buttons[i];

        if (y > content->top &&
            y + button->height > content->bottom)
        {
            y = content->top;
            x += columnWidth + spacingX;
            columnWidth = 0;
        }

        positions[i].x = x;
        positions[i].y = y;

        if (!initialized)
        {
            ButtonGrid_InitLayoutBounds(
                used,
                x,
                y,
                button->width,
                button->height
            );

            initialized = 1;
        }
        else
        {
            ButtonGrid_ExpandLayoutBounds(
                used,
                x,
                y,
                button->width,
                button->height
            );
        }

        if (button->width > columnWidth)
            columnWidth = button->width;

        y += button->height + spacingY;
    }

    if (!initialized)
        SetRectEmpty(used);
}