#include "button_grid_internal.h"

void ButtonGrid_CreateTooltipWindow(ButtonGrid *grid)
{
    InitCommonControls();

    grid->tooltipHwnd = CreateWindowEx(
        WS_EX_TOPMOST,
        TOOLTIPS_CLASSA,
        NULL,
        WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        grid->hwnd,
        NULL,
        grid->hInstance,
        NULL
    );

    if (grid->tooltipHwnd)
    {
        SetWindowPos(
            grid->tooltipHwnd,
            HWND_TOPMOST,
            0,
            0,
            0,
            0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE
        );
    }
}

void ButtonGrid_AddTooltip(ButtonGrid *grid, int buttonIndex)
{
    TOOLINFOA ti;

    if (!grid || !grid->tooltipHwnd)
        return;

    if (!grid->buttons[buttonIndex].hwnd)
        return;

    if (!grid->buttons[buttonIndex].tooltip[0])
        return;

    ZeroMemory(&ti, sizeof(ti));

    ti.cbSize = sizeof(ti);
    ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
    ti.hwnd = grid->hwnd;
    ti.uId = (UINT_PTR)grid->buttons[buttonIndex].hwnd;
    ti.lpszText = grid->buttons[buttonIndex].tooltip;

    SendMessage(grid->tooltipHwnd, TTM_ADDTOOLA, 0, (LPARAM)&ti);
}