#include "button_grid_internal.h"

int ButtonGrid_NormalizeSizeMode(int sizeMode)
{
    if (sizeMode != BUTTON_GRID_SIZE_FIXED &&
        sizeMode != BUTTON_GRID_SIZE_MATCH_IMAGE_SIZE &&
        sizeMode != BUTTON_GRID_SIZE_MATCH_IMAGE_ASPECT_HORIZONTAL &&
        sizeMode != BUTTON_GRID_SIZE_MATCH_IMAGE_ASPECT_VERTICAL)
    {
        return BUTTON_GRID_SIZE_FIXED;
    }

    return sizeMode;
}

static void ButtonGrid_MoveChildClean(HWND hwnd, int x, int y, int w, int h)
{
    if (!hwnd)
        return;

    SetWindowPos(
        hwnd,
        NULL,
        x,
        y,
        w,
        h,
        SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS
    );

    InvalidateRect(hwnd, NULL, TRUE);
}

void ButtonGrid_RedrawContainer(HWND hwnd)
{
    RedrawWindow(
        hwnd,
        NULL,
        NULL,
        RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_UPDATENOW
    );
}

static void ButtonGrid_GetBestImageSize(ButtonItem *button, int *imageW, int *imageH)
{
    int w1;
    int h1;
    int w2;
    int h2;
    int hasOff;
    int hasOn;

    w1 = 0;
    h1 = 0;
    w2 = 0;
    h2 = 0;

    hasOff = ImageLoader_GetSize(button->pictureOff, &w1, &h1);
    hasOn = ImageLoader_GetSize(button->pictureOn, &w2, &h2);

    if (hasOff && hasOn)
    {
        if (w2 * h2 >= w1 * h1)
        {
            *imageW = w2;
            *imageH = h2;
        }
        else
        {
            *imageW = w1;
            *imageH = h1;
        }

        return;
    }

    if (hasOn)
    {
        *imageW = w2;
        *imageH = h2;
        return;
    }

    if (hasOff)
    {
        *imageW = w1;
        *imageH = h1;
        return;
    }

    *imageW = 0;
    *imageH = 0;
}

void ButtonGrid_ResolveButtonSize(ButtonGrid *grid, ButtonItem *button)
{
    int sizeMode;
    int imageW;
    int imageH;
    int w;
    int h;

    if (!grid || !button)
        return;

    sizeMode = button->sizeModeOverride;

    if (sizeMode == BUTTON_GRID_SIZE_USE_DEFAULT)
        sizeMode = grid->sizeMode;

    sizeMode = ButtonGrid_NormalizeSizeMode(sizeMode);

    imageW = 0;
    imageH = 0;

    ButtonGrid_GetBestImageSize(button, &imageW, &imageH);

    w = grid->buttonWidth;
    h = grid->buttonHeight;

    if (sizeMode == BUTTON_GRID_SIZE_MATCH_IMAGE_SIZE)
    {
        if (imageW > 0)
            w = imageW;

        if (imageH > 0)
            h = imageH;
    }
    else if (sizeMode == BUTTON_GRID_SIZE_MATCH_IMAGE_ASPECT_HORIZONTAL)
    {
        w = grid->buttonWidth;

        if (imageW > 0 && imageH > 0)
            h = (w * imageH) / imageW;
    }
    else if (sizeMode == BUTTON_GRID_SIZE_MATCH_IMAGE_ASPECT_VERTICAL)
    {
        h = grid->buttonHeight;

        if (imageW > 0 && imageH > 0)
            w = (h * imageW) / imageH;
    }

    if (button->widthOverride > 0)
        w = button->widthOverride;

    if (button->heightOverride > 0)
        h = button->heightOverride;

    if (w < 1)
        w = 1;

    if (h < 1)
        h = 1;

    button->width = w;
    button->height = h;
}

void ButtonGrid_UpdateAllButtonSizes(ButtonGrid *grid)
{
    int i;

    if (!grid || !grid->buttons)
        return;

    for (i = 0; i < grid->buttonCount; i++)
        ButtonGrid_ResolveButtonSize(grid, &grid->buttons[i]);
}

static void ButtonGrid_GetContentRect(ButtonGrid *grid, RECT *rc)
{
    GetClientRect(grid->hwnd, rc);

    if (!grid->showBorder)
        return;

    rc->left += grid->borderPadding;
    rc->right -= grid->borderPadding;
    rc->top += grid->borderPadding;
    rc->bottom -= grid->borderPadding;

    if (grid->borderTitle[0])
        rc->top += grid->borderTitleHeight;

    if (rc->right <= rc->left)
        rc->right = rc->left + 1;

    if (rc->bottom <= rc->top)
        rc->bottom = rc->top + 1;
}

static void ButtonGrid_LayoutHorizontal(
    ButtonGrid *grid,
    int clientW,
    GridPosition *positions,
    GridSize *size
)
{
    int i;
    int x;
    int y;
    int rowH;
    int rowItems;
    int maxW;

    x = 0;
    y = 0;
    rowH = 0;
    rowItems = 0;
    maxW = 0;

    for (i = 0; i < grid->buttonCount; i++)
    {
        ButtonItem *button;
        int nextX;

        button = &grid->buttons[i];

        if (rowItems > 0)
            nextX = x + grid->horizontalSpacing + button->width;
        else
            nextX = x + button->width;

        if (rowItems > 0 && nextX > clientW)
        {
            if (x > maxW)
                maxW = x;

            y += rowH + grid->verticalSpacing;

            x = 0;
            rowH = 0;
            rowItems = 0;
        }

        if (rowItems > 0)
            x += grid->horizontalSpacing;

        positions[i].x = x;
        positions[i].y = y;

        x += button->width;

        if (button->height > rowH)
            rowH = button->height;

        rowItems++;
    }

    if (x > maxW)
        maxW = x;

    size->width = maxW;
    size->height = y + rowH;

    if (size->width < 1)
        size->width = 1;

    if (size->height < 1)
        size->height = 1;
}

static void ButtonGrid_LayoutVertical(
    ButtonGrid *grid,
    int clientH,
    GridPosition *positions,
    GridSize *size
)
{
    int i;
    int x;
    int y;
    int colW;
    int colItems;
    int maxH;

    x = 0;
    y = 0;
    colW = 0;
    colItems = 0;
    maxH = 0;

    for (i = 0; i < grid->buttonCount; i++)
    {
        ButtonItem *button;
        int nextY;

        button = &grid->buttons[i];

        if (colItems > 0)
            nextY = y + grid->verticalSpacing + button->height;
        else
            nextY = y + button->height;

        if (colItems > 0 && nextY > clientH)
        {
            if (y > maxH)
                maxH = y;

            x += colW + grid->horizontalSpacing;

            y = 0;
            colW = 0;
            colItems = 0;
        }

        if (colItems > 0)
            y += grid->verticalSpacing;

        positions[i].x = x;
        positions[i].y = y;

        y += button->height;

        if (button->width > colW)
            colW = button->width;

        if (y > maxH)
            maxH = y;

        colItems++;
    }

    size->width = x + colW;
    size->height = maxH;

    if (size->width < 1)
        size->width = 1;

    if (size->height < 1)
        size->height = 1;
}

void ButtonGrid_Layout(ButtonGrid *grid)
{
    RECT rc;
    int clientW;
    int clientH;
    int i;
    int offsetX;
    int offsetY;

    GridPosition *positions;
    GridSize gridSize;

    if (!grid || !grid->buttons)
        return;

    ButtonGrid_GetContentRect(grid, &rc);

    clientW = rc.right - rc.left;
    clientH = rc.bottom - rc.top;

    if (clientW < 1)
        clientW = 1;

    if (clientH < 1)
        clientH = 1;

    ButtonGrid_UpdateAllButtonSizes(grid);

    positions = (GridPosition *)malloc(sizeof(GridPosition) * grid->buttonCount);

    if (!positions)
        return;

    ZeroMemory(positions, sizeof(GridPosition) * grid->buttonCount);

    if (grid->layout == BUTTON_GRID_LAYOUT_VERTICAL)
        ButtonGrid_LayoutVertical(grid, clientH, positions, &gridSize);
    else
        ButtonGrid_LayoutHorizontal(grid, clientW, positions, &gridSize);

    offsetX = rc.left + (clientW - gridSize.width) / 2;
    offsetY = rc.top + (clientH - gridSize.height) / 2;

    if (offsetX < rc.left)
        offsetX = rc.left;

    if (offsetY < rc.top)
        offsetY = rc.top;

    for (i = 0; i < grid->buttonCount; i++)
    {
        ButtonGrid_MoveChildClean(
            grid->buttons[i].hwnd,
            positions[i].x + offsetX,
            positions[i].y + offsetY,
            grid->buttons[i].width,
            grid->buttons[i].height
        );
    }

    free(positions);

    ButtonGrid_RedrawContainer(grid->hwnd);
}