#include "button_grid_layout_internal.h"

static int ButtonGrid_ButtonWindowAlreadyMatches(
    ButtonGrid *grid,
    int index,
    int x,
    int y,
    int width,
    int height,
    int visible
)
{
    HWND hwnd;
    RECT rc;
    LONG style;
    int currentlyVisible;

    if (!grid || !grid->buttons)
        return 1;

    hwnd = grid->buttons[index].hwnd;

    if (!hwnd)
        return 1;

    style = GetWindowLong(hwnd, GWL_STYLE);
    currentlyVisible = (style & WS_VISIBLE) ? 1 : 0;

    if (currentlyVisible != visible)
        return 0;

    if (!visible)
        return 1;

    GetWindowRect(hwnd, &rc);
    MapWindowPoints(NULL, grid->hwnd, (LPPOINT)&rc, 2);

    if (rc.left != x)
        return 0;

    if (rc.top != y)
        return 0;

    if ((rc.right - rc.left) != width)
        return 0;

    if ((rc.bottom - rc.top) != height)
        return 0;

    return 1;
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

    if (ButtonGrid_ButtonWindowAlreadyMatches(
            grid,
            index,
            x,
            y,
            width,
            height,
            visible
        ))
    {
        return;
    }

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

static HDWP ButtonGrid_DeferButtonWindowRect(
    ButtonGrid *grid,
    HDWP hdwp,
    int index,
    int x,
    int y,
    int width,
    int height,
    int visible
)
{
    UINT flags;

    if (!grid || !grid->buttons || !hdwp)
        return NULL;

    if (!grid->buttons[index].hwnd)
        return hdwp;

    if (ButtonGrid_ButtonWindowAlreadyMatches(
            grid,
            index,
            x,
            y,
            width,
            height,
            visible
        ))
    {
        return hdwp;
    }

    flags =
        SWP_NOZORDER |
        SWP_NOACTIVATE |
        SWP_NOCOPYBITS;

    if (visible)
        flags |= SWP_SHOWWINDOW;
    else
        flags |= SWP_HIDEWINDOW;

    return DeferWindowPos(
        hdwp,
        grid->buttons[index].hwnd,
        NULL,
        x,
        y,
        width,
        height,
        flags
    );
}

static int ButtonGrid_RectIsFullyVisible(
    const RECT *bounds,
    int x,
    int y,
    int w,
    int h
)
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

static int ButtonGrid_GetTargetVisibility(
    ButtonGrid *grid,
    RECT *content,
    ButtonItem *button,
    int x,
    int y
)
{
    if (!grid->hidePartialButtons)
        return 1;

    return ButtonGrid_RectIsFullyVisible(
        content,
        x,
        y,
        button->width,
        button->height
    );
}

static int ButtonGrid_ApplyPositionsDeferred(
    ButtonGrid *grid,
    RECT *content,
    GridPosition *positions,
    int offsetX,
    int offsetY
)
{
    int i;
    HDWP hdwp;
    HDWP nextHdwp;

    hdwp = BeginDeferWindowPos(grid->buttonCount);

    if (!hdwp)
        return 0;

    for (i = 0; i < grid->buttonCount; i++)
    {
        ButtonItem *button;
        int x;
        int y;
        int visible;

        button = &grid->buttons[i];

        x = positions[i].x + offsetX;
        y = positions[i].y + offsetY;

        visible = ButtonGrid_GetTargetVisibility(
            grid,
            content,
            button,
            x,
            y
        );

        nextHdwp = ButtonGrid_DeferButtonWindowRect(
            grid,
            hdwp,
            i,
            x,
            y,
            button->width,
            button->height,
            visible
        );

        if (!nextHdwp)
            return 0;

        hdwp = nextHdwp;
    }

    return EndDeferWindowPos(hdwp) ? 1 : 0;
}

static void ButtonGrid_ApplyPositionsImmediate(
    ButtonGrid *grid,
    RECT *content,
    GridPosition *positions,
    int offsetX,
    int offsetY
)
{
    int i;

    for (i = 0; i < grid->buttonCount; i++)
    {
        ButtonItem *button;
        int x;
        int y;
        int visible;

        button = &grid->buttons[i];

        x = positions[i].x + offsetX;
        y = positions[i].y + offsetY;

        visible = ButtonGrid_GetTargetVisibility(
            grid,
            content,
            button,
            x,
            y
        );

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

void ButtonGrid_ApplyPositions(
    ButtonGrid *grid,
    RECT *content,
    GridPosition *positions,
    RECT *used
)
{
    int offsetX;
    int offsetY;

    offsetX = 0;
    offsetY = 0;

    ButtonGrid_GetAlignmentOffset(
        grid,
        content,
        used,
        &offsetX,
        &offsetY
    );

    if (ButtonGrid_ApplyPositionsDeferred(
            grid,
            content,
            positions,
            offsetX,
            offsetY
        ))
    {
        return;
    }

    ButtonGrid_ApplyPositionsImmediate(
        grid,
        content,
        positions,
        offsetX,
        offsetY
    );
}