#include "button_grid_core_internal.h"

static LRESULT CALLBACK ButtonGrid_ButtonWndProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam
);

int ButtonGrid_FindButtonIndexByHwnd(ButtonGrid *grid, HWND hwnd)
{
    int i;

    if (!grid || !grid->buttons || !hwnd)
        return -1;

    for (i = 0; i < grid->buttonCount; i++)
    {
        if (grid->buttons[i].hwnd == hwnd)
            return i;
    }

    return -1;
}

static int ButtonGrid_ButtonCanTakeFocus(ButtonGrid *grid, int index)
{
    HWND hwnd;

    if (!grid || !grid->buttons)
        return 0;

    if (index < 0 || index >= grid->buttonCount)
        return 0;

    if (grid->buttons[index].behavior == BUTTON_GRID_BUTTON_DISABLED)
        return 0;

    hwnd = grid->buttons[index].hwnd;

    if (!hwnd)
        return 0;

    if (!IsWindowVisible(hwnd))
        return 0;

    return 1;
}

void ButtonGrid_FocusButtonByIndex(ButtonGrid *grid, int index)
{
    if (!ButtonGrid_ButtonCanTakeFocus(grid, index))
        return;

    SetFocus(grid->buttons[index].hwnd);
}

void ButtonGrid_FocusNextButton(ButtonGrid *grid, int currentIndex, int direction)
{
    int i;
    int index;

    if (!grid || !grid->buttons || grid->buttonCount < 1)
        return;

    if (direction == 0)
        direction = 1;

    if (currentIndex < 0 || currentIndex >= grid->buttonCount)
    {
        if (direction > 0)
            currentIndex = -1;
        else
            currentIndex = grid->buttonCount;
    }

    for (i = 0; i < grid->buttonCount; i++)
    {
        index = currentIndex + direction;

        if (index >= grid->buttonCount)
            index = 0;

        if (index < 0)
            index = grid->buttonCount - 1;

        if (ButtonGrid_ButtonCanTakeFocus(grid, index))
        {
            SetFocus(grid->buttons[index].hwnd);
            return;
        }

        currentIndex = index;
    }
}

static void ButtonGrid_PrepareButtonForKeyboard(ButtonGrid *grid, int index)
{
    HWND hwnd;
    LONG style;
    WNDPROC oldProc;

    if (!grid || !grid->buttons)
        return;

    if (index < 0 || index >= grid->buttonCount)
        return;

    hwnd = grid->buttons[index].hwnd;

    if (!hwnd)
        return;

    style = GetWindowLong(hwnd, GWL_STYLE);

    if (!(style & WS_TABSTOP))
        SetWindowLong(hwnd, GWL_STYLE, style | WS_TABSTOP);

    if (GetProp(hwnd, BUTTON_GRID_BUTTON_PROP_OLDPROC))
    {
        SetProp(hwnd, BUTTON_GRID_BUTTON_PROP_GRID, (HANDLE)grid);
        return;
    }

    SetProp(hwnd, BUTTON_GRID_BUTTON_PROP_GRID, (HANDLE)grid);

    oldProc = (WNDPROC)SetWindowLongPtr(
        hwnd,
        GWLP_WNDPROC,
        (LONG_PTR)ButtonGrid_ButtonWndProc
    );

    SetProp(hwnd, BUTTON_GRID_BUTTON_PROP_OLDPROC, (HANDLE)oldProc);
}

void ButtonGrid_PrepareButtonsForKeyboard(ButtonGrid *grid)
{
    int i;

    if (!grid || !grid->buttons)
        return;

    for (i = 0; i < grid->buttonCount; i++)
        ButtonGrid_PrepareButtonForKeyboard(grid, i);
}

static LRESULT ButtonGrid_CallOldButtonProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam
)
{
    WNDPROC oldProc;

    oldProc = (WNDPROC)GetProp(hwnd, BUTTON_GRID_BUTTON_PROP_OLDPROC);

    if (oldProc)
        return CallWindowProc(oldProc, hwnd, msg, wParam, lParam);

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

static LRESULT CALLBACK ButtonGrid_ButtonWndProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam
)
{
    ButtonGrid *grid;
    int index;

    grid = (ButtonGrid *)GetProp(hwnd, BUTTON_GRID_BUTTON_PROP_GRID);

    switch (msg)
    {
        case WM_GETDLGCODE:
        {
            return DLGC_WANTTAB | DLGC_WANTARROWS | DLGC_WANTCHARS;
        }

        case WM_SETFOCUS:
        case WM_KILLFOCUS:
        {
            InvalidateRect(hwnd, NULL, FALSE);
            break;
        }

        case WM_LBUTTONDOWN:
        {
            SetFocus(hwnd);
            break;
        }

        case WM_KEYDOWN:
        {
            index = ButtonGrid_FindButtonIndexByHwnd(grid, hwnd);

            if (wParam == VK_SPACE || wParam == VK_RETURN)
            {
                ButtonGrid_ActivateButtonByIndex(grid, index);
                return 0;
            }

            if (wParam == VK_TAB)
            {
                if (GetKeyState(VK_SHIFT) & 0x8000)
                    ButtonGrid_FocusNextButton(grid, index, -1);
                else
                    ButtonGrid_FocusNextButton(grid, index, 1);

                return 0;
            }

            break;
        }

        case WM_ERASEBKGND:
        {
            return 1;
        }

        case WM_NCDESTROY:
        {
            WNDPROC oldProc;

            oldProc = (WNDPROC)GetProp(hwnd, BUTTON_GRID_BUTTON_PROP_OLDPROC);

            RemoveProp(hwnd, BUTTON_GRID_BUTTON_PROP_GRID);
            RemoveProp(hwnd, BUTTON_GRID_BUTTON_PROP_OLDPROC);

            if (oldProc)
            {
                SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)oldProc);
                return CallWindowProc(oldProc, hwnd, msg, wParam, lParam);
            }

            break;
        }
    }

    return ButtonGrid_CallOldButtonProc(hwnd, msg, wParam, lParam);
}