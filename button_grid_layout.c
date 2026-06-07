#include "button_grid_internal.h"

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

    InvalidateRect(hwnd, NULL, TRUE);
    UpdateWindow(hwnd);
}

static int ButtonGrid_HasVisibleBorder(ButtonGrid *grid)
{
    if (!grid)
        return 0;

    if (!grid->showBorder)
        return 0;

    if (grid->borderStyle == BUTTON_GRID_BORDER_STYLE_NONE)
        return 0;

    return 1;
}

static void ButtonGrid_GetScaledBorderMetrics(
    ButtonGrid *grid,
    int *padding,
    int *titleHeight
)
{
    if (padding)
        *padding = 0;

    if (titleHeight)
        *titleHeight = 0;

    if (!ButtonGrid_HasVisibleBorder(grid))
        return;

    if (padding)
        *padding = ButtonGrid_DpiScale(grid, grid->borderPadding);

    if (titleHeight)
    {
        if (grid->borderTitle[0])
            *titleHeight = ButtonGrid_DpiScale(grid, grid->borderTitleHeight);
        else
            *titleHeight = 0;
    }
}

static void ButtonGrid_GetContentRect(ButtonGrid *grid, RECT *rc)
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

static void ButtonGrid_GetOuterDecorationSize(
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

static void ButtonGrid_SetButtonWindowRect(
    ButtonGrid *grid,
    int index,
    int x,
    int y,
    int width,
    int height,
    int visible
)
{
    UINT flags;

    if (!grid || !grid->buttons)
        return;

    if (!grid->buttons[index].hwnd)
        return;

    flags =
        SWP_NOZORDER |
        SWP_NOACTIVATE |
        SWP_NOCOPYBITS;

    if (visible)
        flags |= SWP_SHOWWINDOW;
    else
        flags |= SWP_HIDEWINDOW;

    SetWindowPos(
        grid->buttons[index].hwnd,
        NULL,
        x,
        y,
        width,
        height,
        flags
    );
}

static int ButtonGrid_RectIsFullyVisible(const RECT *bounds, int x, int y, int w, int h)
{
    if (!bounds)
        return 0;

    if (x < bounds->left)
        return 0;

    if (y < bounds->top)
        return 0;

    if (x + w > bounds->right)
        return 0;

    if (y + h > bounds->bottom)
        return 0;

    return 1;
}

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

static void ButtonGrid_CalculateHorizontalPositions(
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

static void ButtonGrid_CalculateVerticalPositions(
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

static void ButtonGrid_GetAlignmentOffset(
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

static void ButtonGrid_ApplyPositions(
    ButtonGrid *grid,
    RECT *content,
    GridPosition *positions,
    RECT *used
)
{
    int i;
    int offsetX;
    int offsetY;
    int visible;

    offsetX = 0;
    offsetY = 0;

    ButtonGrid_GetAlignmentOffset(
        grid,
        content,
        used,
        &offsetX,
        &offsetY
    );

    for (i = 0; i < grid->buttonCount; i++)
    {
        ButtonItem *button;
        int x;
        int y;

        button = &grid->buttons[i];

        x = positions[i].x + offsetX;
        y = positions[i].y + offsetY;

        visible = 1;

        if (grid->hidePartialButtons)
        {
            visible = ButtonGrid_RectIsFullyVisible(
                content,
                x,
                y,
                button->width,
                button->height
            );
        }

        ButtonGrid_SetButtonWindowRect(
            grid,
            i,
            x,
            y,
            button->width,
            button->height,
            visible
        );
    }
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

    InvalidateRect(grid->hwnd, NULL, TRUE);
}

static int ButtonGrid_CeilDiv(int a, int b)
{
    if (b <= 0)
        return 0;

    if (a <= 0)
        return 0;

    return (a + b - 1) / b;
}

void ButtonGrid_AdjustRectToLayoutSteps(ButtonGrid *grid, int *width, int *height)
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