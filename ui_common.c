#include "ui_common.h"

void Ui_CopyText(
    char *dest,
    int destSize,
    const char *src
)
{
    if (!dest || destSize <= 0)
        return;

    if (!src)
        src = "";

    lstrcpyn(dest, src, destSize);
    dest[destSize - 1] = '\0';
}

void Ui_AppendText(
    char *dest,
    int destSize,
    const char *src
)
{
    int len;

    if (!dest || destSize <= 0 || !src)
        return;

    len = lstrlen(dest);

    if (len >= destSize - 1)
        return;

    lstrcpyn(dest + len, src, destSize - len);
    dest[destSize - 1] = '\0';
}

int Ui_SameText(
    const char *a,
    const char *b
)
{
    if (!a)
        a = "";

    if (!b)
        b = "";

    return lstrcmp(a, b) == 0;
}

int Ui_SameTextI(
    const char *a,
    const char *b
)
{
    if (!a)
        a = "";

    if (!b)
        b = "";

    return lstrcmpi(a, b) == 0;
}

int Ui_MinInt(
    int a,
    int b
)
{
    return a < b ? a : b;
}

int Ui_MaxInt(
    int a,
    int b
)
{
    return a > b ? a : b;
}

int Ui_ClampInt(
    int value,
    int minValue,
    int maxValue
)
{
    if (value < minValue)
        return minValue;

    if (value > maxValue)
        return maxValue;

    return value;
}

void Ui_SetRect(
    RECT *rc,
    int left,
    int top,
    int right,
    int bottom
)
{
    if (!rc)
        return;

    rc->left = left;
    rc->top = top;
    rc->right = right;
    rc->bottom = bottom;
}

int Ui_RectWidth(
    const RECT *rc
)
{
    if (!rc)
        return 0;

    return rc->right - rc->left;
}

int Ui_RectHeight(
    const RECT *rc
)
{
    if (!rc)
        return 0;

    return rc->bottom - rc->top;
}

int Ui_GetWindowRectInParent(
    HWND hwnd,
    RECT *rc
)
{
    HWND parent;

    if (!hwnd || !rc)
        return 0;

    if (!GetWindowRect(hwnd, rc))
        return 0;

    parent = GetParent(hwnd);

    if (parent)
        MapWindowPoints(NULL, parent, (LPPOINT)rc, 2);

    return 1;
}

int Ui_WindowRectMatches(
    HWND hwnd,
    int x,
    int y,
    int width,
    int height
)
{
    RECT rc;
    int currentWidth;
    int currentHeight;

    if (!Ui_GetWindowRectInParent(hwnd, &rc))
        return 0;

    currentWidth = rc.right - rc.left;
    currentHeight = rc.bottom - rc.top;

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

int Ui_SetWindowPosIfChanged(
    HWND hwnd,
    HWND insertAfter,
    int x,
    int y,
    int width,
    int height,
    UINT flags
)
{
    if (!hwnd)
        return 0;

    if (width < 1)
        width = 1;

    if (height < 1)
        height = 1;

    if ((flags & SWP_NOMOVE) && (flags & SWP_NOSIZE))
    {
        SetWindowPos(
            hwnd,
            insertAfter,
            x,
            y,
            width,
            height,
            flags
        );

        return 1;
    }

    if (!(flags & SWP_NOMOVE) && !(flags & SWP_NOSIZE))
    {
        if (Ui_WindowRectMatches(hwnd, x, y, width, height))
            return 0;
    }

    SetWindowPos(
        hwnd,
        insertAfter,
        x,
        y,
        width,
        height,
        flags
    );

    return 1;
}

int Ui_SetWindowRectIfChanged(
    HWND hwnd,
    const RECT *rc,
    UINT flags
)
{
    if (!rc)
        return 0;

    return Ui_SetWindowPosIfChanged(
        hwnd,
        NULL,
        rc->left,
        rc->top,
        rc->right - rc->left,
        rc->bottom - rc->top,
        flags | SWP_NOZORDER | SWP_NOACTIVATE
    );
}

void Ui_ShowWindowIfChanged(
    HWND hwnd,
    int show
)
{
    int currentlyVisible;

    if (!hwnd)
        return;

    currentlyVisible = IsWindowVisible(hwnd) ? 1 : 0;

    if (currentlyVisible == (show ? 1 : 0))
        return;

    ShowWindow(hwnd, show ? SW_SHOW : SW_HIDE);
}

void Ui_EnableWindowIfChanged(
    HWND hwnd,
    int enabled
)
{
    int currentlyEnabled;

    if (!hwnd)
        return;

    currentlyEnabled = IsWindowEnabled(hwnd) ? 1 : 0;

    if (currentlyEnabled == (enabled ? 1 : 0))
        return;

    EnableWindow(hwnd, enabled ? TRUE : FALSE);
}

void Ui_InvalidateNoErase(
    HWND hwnd
)
{
    if (!hwnd)
        return;

    InvalidateRect(hwnd, NULL, FALSE);
}

void Ui_RedrawNoErase(
    HWND hwnd,
    int updateNow,
    int includeChildren
)
{
    UINT flags;

    if (!hwnd)
        return;

    flags = RDW_INVALIDATE | RDW_NOERASE;

    if (updateNow)
        flags |= RDW_UPDATENOW;

    if (includeChildren)
        flags |= RDW_ALLCHILDREN;
    else
        flags |= RDW_NOCHILDREN;

    RedrawWindow(hwnd, NULL, NULL, flags);
}

void Ui_BringWindowToTopNoActivate(
    HWND hwnd
)
{
    if (!hwnd)
        return;

    SetWindowPos(
        hwnd,
        HWND_TOP,
        0,
        0,
        0,
        0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE
    );
}