#include "new_test_layout_settings_internal.h"

static int Settings_NormalizePercent(
    int value
)
{
    if (value < 1)
        value = 1;

    if (value > 400)
        value = 400;

    return value;
}

static int Settings_NormalizeIntValue(
    int index,
    int value
)
{
    size_t offset;

    if (index < 0 || index >= NTL_SETTINGS_INT_COUNT)
        return value;

    offset = g_ntlSettingsIntDefs[index].offset;

    if (offset == offsetof(NewTestLayoutSettingsConfig, overlayWidth))
    {
        if (value < 180)
            value = 180;

        return value;
    }

    if (offset == offsetof(NewTestLayoutSettingsConfig, overlayMargin))
    {
        if (value < 0)
            value = 0;

        return value;
    }

    if (offset == offsetof(NewTestLayoutSettingsConfig, rowHeight))
    {
        if (value < 20)
            value = 20;

        if (value > 80)
            value = 80;

        return value;
    }

    if (offset == offsetof(NewTestLayoutSettingsConfig, gap))
    {
        if (value < 0)
            value = 0;

        if (value > 40)
            value = 40;

        return value;
    }

    if (offset == offsetof(NewTestLayoutSettingsConfig, singleRowModeGridHeight))
    {
        if (value < 44)
            value = 44;

        if (value > 140)
            value = 140;

        return value;
    }

    if (offset == offsetof(NewTestLayoutSettingsConfig, replaceBoxMinHeightPercent) ||
        offset == offsetof(NewTestLayoutSettingsConfig, utilityButtonsMinWidthPercent) ||
        offset == offsetof(NewTestLayoutSettingsConfig, modeGridMinHeightPercent) ||
        offset == offsetof(NewTestLayoutSettingsConfig, findDocumentGridMinHeightPercent) ||
        offset == offsetof(NewTestLayoutSettingsConfig, replaceDocumentGridMinHeightPercent) ||
        offset == offsetof(NewTestLayoutSettingsConfig, replaceSelectionGridMinHeightPercent) ||
        offset == offsetof(NewTestLayoutSettingsConfig, leftModePanelMinWidthPercent) ||
        offset == offsetof(NewTestLayoutSettingsConfig, leftModePanelMinGroupWidthPercent))
    {
        return Settings_NormalizePercent(value);
    }

    if (value < 0)
        value = 0;

    return value;
}

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
    newValue = Settings_NormalizeIntValue(index, newValue);

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

static void Settings_ResetDefaults(
    NewTestLayoutSettingsPanel *panel
)
{
    RECT rc;

    if (!panel)
        return;

    NewTestLayoutSettings_LoadDefaultConfig(&panel->config);

    panel->scrollY = 0;

    Settings_RecreateBrushes(panel);
    NewTestLayoutSettings_RequestRefresh(panel);

    if (panel->parent)
    {
        GetClientRect(panel->parent, &rc);
        NewTestLayoutSettings_Layout(panel, &rc);
    }

    Settings_NotifyChanged(panel);
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

    if (id == NTL_SETTINGS_ID_SAVE_CLOSE_BUTTON && notifyCode == BN_CLICKED)
    {
        if (!NewTestLayoutSettings_SaveConfig(&panel->config))
        {
            MessageBox(
                panel->container,
                "Could not save new_test_layout.ini.",
                "Layout Settings",
                MB_ICONWARNING | MB_OK
            );

            return 1;
        }

        NewTestLayoutSettings_Show(panel, 0);
        return 1;
    }

    if (id == NTL_SETTINGS_ID_RESET_DEFAULTS_BUTTON && notifyCode == BN_CLICKED)
    {
        Settings_ResetDefaults(panel);
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
    RECT rc;
    int delta;
    int step;
    int visibleHeight;
    int maxScroll;

    (void)lParam;

    if (!panel || !panel->parent)
        return 0;

    delta = (short)HIWORD(wParam);
    step = panel->config.rowHeight + panel->config.gap;

    if (step < 16)
        step = 16;

    if (delta > 0)
        panel->scrollY -= step * 3;
    else if (delta < 0)
        panel->scrollY += step * 3;
    else
        return 1;

    GetClientRect(panel->parent, &rc);

    visibleHeight =
        rc.bottom - rc.top -
        panel->config.overlayMargin * 2 -
        panel->config.rowHeight -
        panel->config.gap * 2 -
        panel->config.overlayMargin;

    if (visibleHeight < 1)
        visibleHeight = 1;

    maxScroll = panel->contentHeight - visibleHeight;

    if (maxScroll < 0)
        maxScroll = 0;

    if (panel->scrollY < 0)
        panel->scrollY = 0;

    if (panel->scrollY > maxScroll)
        panel->scrollY = maxScroll;

    NewTestLayoutSettings_Layout(panel, &rc);

    return 1;
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