#include <windows.h>

#include "button_grid_internal.h"
#include "button_grid_settings.h"
#include "button_grid_settings_internal.h"

static int g_settingsUpdatingControls = 0;

int ButtonGrid_SettingsControlsAreUpdating(void)
{
    return g_settingsUpdatingControls;
}

void ButtonGrid_SettingsBeginControlUpdate(void)
{
    g_settingsUpdatingControls = 1;
}

void ButtonGrid_SettingsEndControlUpdate(void)
{
    g_settingsUpdatingControls = 0;
}

int ButtonGrid_SettingsGetLabelId(int index)
{
    return BG_SETTINGS_ID_BASE + index * BG_SETTINGS_ID_STEP + BG_SETTINGS_PART_LABEL;
}

int ButtonGrid_SettingsGetPrimaryId(int index)
{
    return BG_SETTINGS_ID_BASE + index * BG_SETTINGS_ID_STEP + BG_SETTINGS_PART_PRIMARY;
}

int ButtonGrid_SettingsGetRawId(int index)
{
    return BG_SETTINGS_ID_BASE + index * BG_SETTINGS_ID_STEP + BG_SETTINGS_PART_RAW;
}

int ButtonGrid_SettingsIdToIndex(int id)
{
    int rel;

    rel = id - BG_SETTINGS_ID_BASE;

    if (rel < 0)
        return -1;

    return rel / BG_SETTINGS_ID_STEP;
}

int ButtonGrid_SettingsIdToPart(int id)
{
    int rel;

    rel = id - BG_SETTINGS_ID_BASE;

    if (rel < 0)
        return -1;

    return rel % BG_SETTINGS_ID_STEP;
}

HWND ButtonGrid_SettingsGetLabelControl(HWND pageHwnd, int index)
{
    return GetDlgItem(pageHwnd, ButtonGrid_SettingsGetLabelId(index));
}

HWND ButtonGrid_SettingsGetPrimaryControl(HWND pageHwnd, int index)
{
    return GetDlgItem(pageHwnd, ButtonGrid_SettingsGetPrimaryId(index));
}

HWND ButtonGrid_SettingsGetRawControl(HWND pageHwnd, int index)
{
    return GetDlgItem(pageHwnd, ButtonGrid_SettingsGetRawId(index));
}