#include "new_test_layout_settings_internal.h"

static int Settings_HandleIntEditCommand(
    NewTestLayoutSettingsPanel *panel,
    int id,
    int notifyCode
)
{
    int index;
    int *field;
    int oldValue;
    int newValue;

    if (!panel)
        return 0;

    index = id - NTL_SETTINGS_ID_INT_BASE;

    if (index < 0 || index >= NTL_SETTINGS_INT_COUNT)
        return 0;

    if (notifyCode != EN_CHANGE)
        return 1;

    if (panel->updatingControls)
        return 1;

    field = Settings_IntField(
        &panel->config,
        g_ntlSettingsIntDefs[index].offset
    );

    oldValue = *field;
    newValue = Settings_GetEditInt(panel->intEdits[index], oldValue);

    if (index == 0 && newValue < 180)
        newValue = 180;

    if (index == 4)
    {
        if (newValue < 44)
            newValue = 44;

        if (newValue > 140)
            newValue = 140;
    }
    else if (index != 0 && newValue < 0)
    {
        newValue = 0;
    }

    if (oldValue != newValue)
    {
        *field = newValue;
        Settings_NotifyChanged(panel);
    }

    return 1;
}

static int Settings_HandleBoolCommand(
    NewTestLayoutSettingsPanel *panel,
    int id,
    int notifyCode
)
{
    int index;
    int *field;
    int checked;

    if (!panel)
        return 0;

    index = id - NTL_SETTINGS_ID_BOOL_BASE;

    if (index < 0 || index >= NTL_SETTINGS_BOOL_COUNT)
        return 0;

    if (notifyCode != BN_CLICKED)
        return 1;

    if (panel->updatingControls)
        return 1;

    field = Settings_IntField(
        &panel->config,
        g_ntlSettingsBoolDefs[index].offset
    );

    checked = SendMessage(panel->boolChecks[index], BM_GETCHECK, 0, 0) == BST_CHECKED;

    if (*field != checked)
    {
        *field = checked;
        Settings_NotifyChanged(panel);
    }

    return 1;
}

int NewTestLayoutSettings_HandleCommand(
    NewTestLayoutSettingsPanel *panel,
    WPARAM wParam,
    LPARAM lParam
)
{
    int id;
    int notifyCode;

    (void)lParam;

    if (!panel)
        return 0;

    id = LOWORD(wParam);
    notifyCode = HIWORD(wParam);

    if (id == NTL_SETTINGS_ID_CLOSE_BUTTON && notifyCode == BN_CLICKED)
    {
        NewTestLayoutSettings_Show(panel, 0);
        return 1;
    }

    if (id >= NTL_SETTINGS_ID_INT_BASE &&
        id < NTL_SETTINGS_ID_INT_BASE + NTL_SETTINGS_INT_COUNT)
    {
        return Settings_HandleIntEditCommand(panel, id, notifyCode);
    }

    if (id >= NTL_SETTINGS_ID_BOOL_BASE &&
        id < NTL_SETTINGS_ID_BOOL_BASE + NTL_SETTINGS_BOOL_COUNT)
    {
        return Settings_HandleBoolCommand(panel, id, notifyCode);
    }

    return 0;
}

int NewTestLayoutSettings_HandleNotify(
    NewTestLayoutSettingsPanel *panel,
    WPARAM wParam,
    LPARAM lParam
)
{
    (void)panel;
    (void)wParam;
    (void)lParam;

    return 0;
}

int NewTestLayoutSettings_HandleMouseWheel(
    NewTestLayoutSettingsPanel *panel,
    WPARAM wParam,
    LPARAM lParam
)
{
    (void)panel;
    (void)wParam;
    (void)lParam;

    return 0;
}

int NewTestLayoutSettings_HandleCtlColorStatic(
    NewTestLayoutSettingsPanel *panel,
    HDC hdc,
    HWND hwnd,
    LRESULT *result
)
{
    int i;

    if (!panel || !hdc || !result)
        return 0;

    if (hwnd == panel->container)
    {
        SetBkColor(hdc, panel->config.overlayBackColor);
        *result = (LRESULT)panel->backBrush;
        return 1;
    }

    if (hwnd == panel->titleLabel)
    {
        SetBkColor(hdc, panel->config.overlayBackColor);
        SetTextColor(hdc, panel->config.overlayTitleColor);
        *result = (LRESULT)panel->backBrush;
        return 1;
    }

    for (i = 0; i < NTL_SETTINGS_INT_COUNT; i++)
    {
        if (hwnd == panel->intLabels[i])
        {
            SetBkColor(hdc, panel->config.overlayBackColor);
            SetTextColor(hdc, panel->config.labelColor);
            *result = (LRESULT)panel->backBrush;
            return 1;
        }
    }

    return 0;
}