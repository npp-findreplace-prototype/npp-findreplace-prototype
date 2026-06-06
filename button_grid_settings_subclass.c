#include <windows.h>

#include "button_grid_internal.h"
#include "button_grid_settings.h"
#include "button_grid_settings_internal.h"

#ifndef GWLP_WNDPROC
#define GWLP_WNDPROC GWL_WNDPROC
#endif

#ifndef LONG_PTR
#define LONG_PTR LONG
#endif

#ifndef SetWindowLongPtr
#define SetWindowLongPtr SetWindowLong
#endif

#ifndef GetWindowLongPtr
#define GetWindowLongPtr GetWindowLong
#endif

static LRESULT CALLBACK ButtonGrid_SettingsControlSubclassProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam
)
{
    WNDPROC oldProc;
    HWND pageHwnd;
    ButtonGrid *grid;

    oldProc = (WNDPROC)GetProp(hwnd, BUTTON_GRID_SETTINGS_OLDPROC_PROP);

    if (msg == WM_MOUSEWHEEL)
    {
        pageHwnd = GetParent(hwnd);
        grid = ButtonGrid_SettingsGetGrid(pageHwnd);

        if (grid && !grid->settingsWheelScrub)
        {
            SendMessage(pageHwnd, WM_MOUSEWHEEL, wParam, lParam);
            return 0;
        }
    }

    if (msg == WM_NCDESTROY)
    {
        if (oldProc)
        {
            SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)oldProc);
            RemoveProp(hwnd, BUTTON_GRID_SETTINGS_OLDPROC_PROP);

            return CallWindowProc(oldProc, hwnd, msg, wParam, lParam);
        }
    }

    if (oldProc)
        return CallWindowProc(oldProc, hwnd, msg, wParam, lParam);

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void ButtonGrid_SettingsSubclassWheelControl(HWND hwnd)
{
    WNDPROC oldProc;

    if (!hwnd)
        return;

    if (GetProp(hwnd, BUTTON_GRID_SETTINGS_OLDPROC_PROP))
        return;

    oldProc = (WNDPROC)GetWindowLongPtr(hwnd, GWLP_WNDPROC);

    if (!oldProc)
        return;

    SetProp(hwnd, BUTTON_GRID_SETTINGS_OLDPROC_PROP, (HANDLE)oldProc);

    SetWindowLongPtr(
        hwnd,
        GWLP_WNDPROC,
        (LONG_PTR)ButtonGrid_SettingsControlSubclassProc
    );
}