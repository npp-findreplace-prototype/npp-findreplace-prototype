#include <windows.h>
#include <commctrl.h>

#include "button_grid_internal.h"
#include "button_grid_settings.h"
#include "button_grid_settings_internal.h"

void ButtonGrid_SettingsApplyRawEdit(HWND pageHwnd, int index)
{
    char buffer[256];

    buffer[0] = '\0';

    GetWindowText(
        ButtonGrid_SettingsGetRawControl(pageHwnd, index),
        buffer,
        sizeof(buffer)
    );

    ButtonGrid_SettingsApplyValue(pageHwnd, index, buffer, 0, 0);
}

void ButtonGrid_SettingsApplyPrimary(HWND pageHwnd, int index)
{
    const ButtonGridSettingDefinition *def;
    HWND primary;
    int value;
    int selection;

    def = ButtonGrid_SettingsGetDefinition(index);

    if (!def)
        return;

    primary = ButtonGrid_SettingsGetPrimaryControl(pageHwnd, index);

    if (!primary)
        return;

    value = 0;

    if (def->type == BG_SETTING_BOOL)
    {
        value = SendMessage(primary, BM_GETCHECK, 0, 0) == BST_CHECKED;
    }
    else if (def->type == BG_SETTING_INT)
    {
        value = (int)SendMessage(primary, TBM_GETPOS, 0, 0);
    }
    else if (def->type == BG_SETTING_ENUM)
    {
        selection = (int)SendMessage(primary, CB_GETCURSEL, 0, 0);

        if (selection < 0)
            return;

        value = (int)SendMessage(primary, CB_GETITEMDATA, selection, 0);
    }
    else if (def->type == BG_SETTING_THEME)
    {
        char themeName[BUTTON_GRID_THEME_NAME_SIZE];

        selection = (int)SendMessage(primary, CB_GETCURSEL, 0, 0);

        if (selection < 0)
            return;

        themeName[0] = '\0';

        SendMessage(primary, CB_GETLBTEXT, selection, (LPARAM)themeName);

        ButtonGrid_SettingsApplyValue(pageHwnd, index, themeName, 0, 0);
        return;
    }
    else
    {
        return;
    }

    ButtonGrid_SettingsApplyValue(pageHwnd, index, NULL, value, 1);
}