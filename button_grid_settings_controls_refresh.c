#include <windows.h>
#include <commctrl.h>

#include "button_grid_internal.h"
#include "button_grid_settings.h"
#include "button_grid_settings_internal.h"

void ButtonGrid_SettingsRefreshOne(HWND pageHwnd, int index, ButtonGrid *grid)
{
    const ButtonGridSettingDefinition *def;
    HWND primary;
    int value;
    int i;

    def = ButtonGrid_SettingsGetDefinition(index);

    if (!def)
        return;

    primary = ButtonGrid_SettingsGetPrimaryControl(pageHwnd, index);

    if (primary)
    {
        if (def->type == BG_SETTING_BOOL)
        {
            value = ButtonGrid_SettingsGetIntField(grid, def);
            SendMessage(primary, BM_SETCHECK, value ? BST_CHECKED : BST_UNCHECKED, 0);
        }
        else if (def->type == BG_SETTING_INT)
        {
            value = ButtonGrid_SettingsGetIntField(grid, def);

            SendMessage(
                primary,
                TBM_SETRANGE,
                TRUE,
                MAKELONG(def->minValue, def->maxValue)
            );

            SendMessage(primary, TBM_SETPOS, TRUE, value);
        }
        else if (def->type == BG_SETTING_THEME)
        {
            ButtonGrid_SettingsPopulateThemeCombo(primary, grid);
        }
        else if (def->type == BG_SETTING_ENUM)
        {
            value = ButtonGrid_SettingsGetIntField(grid, def);

            SendMessage(primary, CB_RESETCONTENT, 0, 0);

            if (def->options)
            {
                for (i = 0; def->options[i].label; i++)
                {
                    int item;

                    item = (int)SendMessage(
                        primary,
                        CB_ADDSTRING,
                        0,
                        (LPARAM)def->options[i].label
                    );

                    SendMessage(primary, CB_SETITEMDATA, item, def->options[i].value);

                    if (def->options[i].value == value)
                        SendMessage(primary, CB_SETCURSEL, item, 0);
                }
            }
        }
    }

    ButtonGrid_SettingsWriteRawText(pageHwnd, index, grid);
}

void ButtonGrid_SettingsRefreshAll(HWND pageHwnd)
{
    ButtonGrid *grid;
    int i;
    int count;

    grid = ButtonGrid_SettingsGetGrid(pageHwnd);

    if (!grid)
        return;

    ButtonGrid_SettingsBeginControlUpdate();

    count = ButtonGrid_SettingsGetCount();

    for (i = 0; i < count; i++)
        ButtonGrid_SettingsRefreshOne(pageHwnd, i, grid);

    ButtonGrid_SettingsEndControlUpdate();
}