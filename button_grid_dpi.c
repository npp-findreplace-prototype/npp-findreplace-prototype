#include <windows.h>

#include "button_grid_internal.h"

#ifndef LOGPIXELSX
#define LOGPIXELSX 88
#endif

typedef UINT (WINAPI *ButtonGrid_GetDpiForWindowProc)(HWND hwnd);

int ButtonGrid_GetWindowDpi(HWND hwnd)
{
    HMODULE user32;
    ButtonGrid_GetDpiForWindowProc getDpiForWindow;
    HDC hdc;
    int dpi;

    dpi = BUTTON_GRID_DEFAULT_DPI_BASE;

    user32 = GetModuleHandle("user32.dll");

    if (user32)
    {
        getDpiForWindow = (ButtonGrid_GetDpiForWindowProc)GetProcAddress(
            user32,
            "GetDpiForWindow"
        );

        if (getDpiForWindow && hwnd)
        {
            dpi = (int)getDpiForWindow(hwnd);

            if (dpi > 0)
                return dpi;
        }
    }

    hdc = GetDC(hwnd);

    if (hdc)
    {
        dpi = GetDeviceCaps(hdc, LOGPIXELSX);
        ReleaseDC(hwnd, hdc);
    }

    if (dpi <= 0)
        dpi = BUTTON_GRID_DEFAULT_DPI_BASE;

    return dpi;
}

void ButtonGrid_UpdateDpi(ButtonGrid *grid)
{
    if (!grid)
        return;

    grid->currentDpi = ButtonGrid_GetWindowDpi(grid->hwnd);

    if (grid->currentDpi <= 0)
        grid->currentDpi = BUTTON_GRID_DEFAULT_DPI_BASE;
}

int ButtonGrid_DpiScale(ButtonGrid *grid, int value)
{
    int dpi;

    if (!grid)
        return value;

    if (!grid->dpiScaleEnabled)
        return value;

    dpi = grid->currentDpi;

    if (dpi <= 0)
        dpi = BUTTON_GRID_DEFAULT_DPI_BASE;

    return MulDiv(value, dpi, BUTTON_GRID_DEFAULT_DPI_BASE);
}

int ButtonGrid_DpiScaleMin(ButtonGrid *grid, int value, int minValue)
{
    int scaled;

    scaled = ButtonGrid_DpiScale(grid, value);

    if (scaled < minValue)
        scaled = minValue;

    return scaled;
}