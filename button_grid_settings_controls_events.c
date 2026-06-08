#include <windows.h>

#include "button_grid_internal.h"
#include "button_grid_settings.h"
#include "button_grid_settings_internal.h"

int ButtonGrid_SettingsHandleCommand(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    ButtonGrid *grid;
    int id;
    int code;
    int index;
    int part;

    (void)lParam;

    grid = ButtonGrid_SettingsGetGrid(hwnd);

    id = LOWORD(wParam);
    code = HIWORD(wParam);

    if (id == BG_SETTINGS_ID_CLOSE)
    {
        ButtonGrid_ShowSettingsPage(grid, 0);
        return 1;
    }

    if (id == BG_SETTINGS_ID_FILTER_CLEAR)
    {
        SetWindowText(GetDlgItem(hwnd, BG_SETTINGS_ID_FILTER), "");

        ButtonGrid_SettingsSetScrollPos(hwnd, 0);
        ButtonGrid_SettingsLayoutControls(hwnd);

        SetFocus(GetDlgItem(hwnd, BG_SETTINGS_ID_FILTER));

        return 1;
    }

    if (id == BG_SETTINGS_ID_FILTER)
    {
        if (code == EN_CHANGE)
        {
            ButtonGrid_SettingsSetScrollPos(hwnd, 0);
            ButtonGrid_SettingsLayoutControls(hwnd);
            return 1;
        }
    }

    if (ButtonGrid_SettingsControlsAreUpdating())
        return 1;

    index = ButtonGrid_SettingsIdToIndex(id);
    part = ButtonGrid_SettingsIdToPart(id);

    if (index >= 0 && index < ButtonGrid_SettingsGetCount())
    {
        if (part == BG_SETTINGS_PART_PRIMARY)
        {
            if (code == BN_CLICKED || code == CBN_SELCHANGE)
            {
                ButtonGrid_SettingsApplyPrimary(hwnd, index);
                return 1;
            }
        }
        else if (part == BG_SETTINGS_PART_RAW)
        {
            if (code == EN_CHANGE)
            {
                ButtonGrid_SettingsApplyRawEdit(hwnd, index);
                return 1;
            }
        }
    }

    return 0;
}

int ButtonGrid_SettingsHandleHScroll(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    HWND source;
    int controlId;
    int index;

    (void)wParam;

    if (ButtonGrid_SettingsControlsAreUpdating())
        return 1;

    source = (HWND)lParam;

    if (!source)
        return 0;

    controlId = GetDlgCtrlID(source);
    index = ButtonGrid_SettingsIdToIndex(controlId);

    if (index >= 0 && index < ButtonGrid_SettingsGetCount())
    {
        ButtonGrid_SettingsApplyPrimary(hwnd, index);
        return 1;
    }

    return 0;
}