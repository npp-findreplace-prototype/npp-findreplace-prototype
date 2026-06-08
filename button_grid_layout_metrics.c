#include "button_grid_layout_internal.h"

int ButtonGrid_LayoutHasVisibleBorder(ButtonGrid *grid)
{
    if (!grid)
        return 0;

    if (!grid->showBorder)
        return 0;

    if (grid->borderStyle == BUTTON_GRID_BORDER_STYLE_NONE)
        return 0;

    return 1;
}

int ButtonGrid_LayoutShouldShowBorderTitle(ButtonGrid *grid)
{
    if (!ButtonGrid_LayoutHasVisibleBorder(grid))
        return 0;

    if (!grid->showBorderTitle)
        return 0;

    if (!grid->borderTitle[0])
        return 0;

    return 1;
}

void ButtonGrid_GetScaledBorderMetrics(
    ButtonGrid *grid,
    int *padding,
    int *titleHeight
)
{
    if (padding)
        *padding = 0;

    if (titleHeight)
        *titleHeight = 0;

    if (!ButtonGrid_LayoutHasVisibleBorder(grid))
        return;

    if (padding)
        *padding = ButtonGrid_DpiScale(grid, grid->borderPadding);

    if (titleHeight)
    {
        if (ButtonGrid_LayoutShouldShowBorderTitle(grid))
            *titleHeight = ButtonGrid_DpiScale(grid, grid->borderTitleHeight);
        else
            *titleHeight = 0;
    }
}

void ButtonGrid_GetContentRect(ButtonGrid *grid, RECT *rc)
{
    int padding;
    int titleHeight;

    SetRectEmpty(rc);

    if (!grid || !grid->hwnd)
        return;

    GetClientRect(grid->hwnd, rc);

    ButtonGrid_GetScaledBorderMetrics(grid, &padding, &titleHeight);

    rc->left += padding;
    rc->top += padding + titleHeight;
    rc->right -= padding;
    rc->bottom -= padding;

    if (rc->right < rc->left)
        rc->right = rc->left;

    if (rc->bottom < rc->top)
        rc->bottom = rc->top;
}

void ButtonGrid_GetOuterDecorationSize(
    ButtonGrid *grid,
    int *extraWidth,
    int *extraHeight
)
{
    int padding;
    int titleHeight;

    if (extraWidth)
        *extraWidth = 0;

    if (extraHeight)
        *extraHeight = 0;

    ButtonGrid_GetScaledBorderMetrics(grid, &padding, &titleHeight);

    if (extraWidth)
        *extraWidth = padding * 2;

    if (extraHeight)
        *extraHeight = padding * 2 + titleHeight;
}

static AppImage *ButtonGrid_GetSizingImage(ButtonItem *button)
{
    if (!button)
        return NULL;

    if (button->isOn && button->pictureOn)
        return button->pictureOn;

    if (!button->isOn && button->pictureOff)
        return button->pictureOff;

    if (button->pictureOff)
        return button->pictureOff;

    if (button->pictureOn)
        return button->pictureOn;

    return NULL;
}

static int ButtonGrid_GetButtonSizeMode(ButtonGrid *grid, ButtonItem *button)
{
    int sizeMode;

    if (!grid || !button)
        return BUTTON_GRID_SIZE_FIXED;

    if (button->sizeModeOverride != BUTTON_GRID_SIZE_USE_DEFAULT)
        sizeMode = button->sizeModeOverride;
    else
        sizeMode = grid->sizeMode;

    sizeMode = ButtonGrid_NormalizeSizeMode(sizeMode);

    if (sizeMode == BUTTON_GRID_SIZE_MATCH_IMAGE_ASPECT_BY_LAYOUT)
    {
        if (grid->layout == BUTTON_GRID_LAYOUT_VERTICAL)
            return BUTTON_GRID_SIZE_MATCH_IMAGE_ASPECT_VERTICAL;

        return BUTTON_GRID_SIZE_MATCH_IMAGE_ASPECT_HORIZONTAL;
    }

    return sizeMode;
}

static void ButtonGrid_GetLogicalBaseButtonSize(
    ButtonGrid *grid,
    ButtonItem *button,
    int *width,
    int *height
)
{
    int w;
    int h;

    w = grid ? grid->buttonWidth : BUTTON_GRID_DEFAULT_BUTTON_WIDTH;
    h = grid ? grid->buttonHeight : BUTTON_GRID_DEFAULT_BUTTON_HEIGHT;

    if (button)
    {
        if (button->widthOverride > 0)
            w = button->widthOverride;

        if (button->heightOverride > 0)
            h = button->heightOverride;
    }

    if (w < 1)
        w = 1;

    if (h < 1)
        h = 1;

    if (width)
        *width = w;

    if (height)
        *height = h;
}

static void ButtonGrid_GetScaledBaseButtonSize(
    ButtonGrid *grid,
    ButtonItem *button,
    int *width,
    int *height
)
{
    int logicalW;
    int logicalH;
    int scaledW;
    int scaledH;

    ButtonGrid_GetLogicalBaseButtonSize(
        grid,
        button,
        &logicalW,
        &logicalH
    );

    scaledW = ButtonGrid_DpiScale(grid, logicalW);
    scaledH = ButtonGrid_DpiScale(grid, logicalH);

    if (scaledW < 1)
        scaledW = 1;

    if (scaledH < 1)
        scaledH = 1;

    if (width)
        *width = scaledW;

    if (height)
        *height = scaledH;
}

void ButtonGrid_ResolveButtonSize(ButtonGrid *grid, ButtonItem *button)
{
    int baseW;
    int baseH;
    int imageW;
    int imageH;
    int sizeMode;
    AppImage *image;

    if (!grid || !button)
        return;

    ButtonGrid_GetScaledBaseButtonSize(grid, button, &baseW, &baseH);

    button->width = baseW;
    button->height = baseH;

    sizeMode = ButtonGrid_GetButtonSizeMode(grid, button);
    image = ButtonGrid_GetSizingImage(button);

    if (!image)
        return;

    imageW = 0;
    imageH = 0;

    if (!ImageLoader_GetSize(image, &imageW, &imageH))
        return;

    if (imageW < 1 || imageH < 1)
        return;

    if (sizeMode == BUTTON_GRID_SIZE_MATCH_IMAGE_SIZE)
    {
        button->width = ButtonGrid_DpiScale(grid, imageW);
        button->height = ButtonGrid_DpiScale(grid, imageH);
    }
    else if (sizeMode == BUTTON_GRID_SIZE_MATCH_IMAGE_ASPECT_HORIZONTAL)
    {
        button->width = baseW;
        button->height = MulDiv(baseW, imageH, imageW);
    }
    else if (sizeMode == BUTTON_GRID_SIZE_MATCH_IMAGE_ASPECT_VERTICAL)
    {
        button->height = baseH;
        button->width = MulDiv(baseH, imageW, imageH);
    }

    if (button->width < 1)
        button->width = 1;

    if (button->height < 1)
        button->height = 1;
}

void ButtonGrid_UpdateAllButtonSizes(ButtonGrid *grid)
{
    int i;

    if (!grid || !grid->buttons)
        return;

    ButtonGrid_UpdateDpi(grid);

    for (i = 0; i < grid->buttonCount; i++)
        ButtonGrid_ResolveButtonSize(grid, &grid->buttons[i]);
}