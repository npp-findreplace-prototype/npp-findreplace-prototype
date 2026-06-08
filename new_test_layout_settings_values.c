#include "new_test_layout_settings_internal.h"

void Settings_SetEditInt(
    HWND hwnd,
    int value
)
{
    char text[64];

    wsprintf(text, "%d", value);
    SetWindowText(hwnd, text);
}

int Settings_GetEditInt(
    HWND hwnd,
    int defaultValue
)
{
    char text[64];

    if (!hwnd)
        return defaultValue;

    GetWindowText(hwnd, text, sizeof(text));
    text[sizeof(text) - 1] = '\0';

    if (!text[0])
        return defaultValue;

    return (int)strtol(text, NULL, 0);
}

void NewTestLayoutSettings_RequestRefresh(
    NewTestLayoutSettingsPanel *panel
)
{
    int i;
    int *field;

    if (!panel)
        return;

    panel->updatingControls = 1;

    for (i = 0; i < NTL_SETTINGS_INT_COUNT; i++)
    {
        field = Settings_IntField(
            &panel->config,
            g_ntlSettingsIntDefs[i].offset
        );

        Settings_SetEditInt(panel->intEdits[i], *field);
    }

    for (i = 0; i < NTL_SETTINGS_BOOL_COUNT; i++)
    {
        field = Settings_IntField(
            &panel->config,
            g_ntlSettingsBoolDefs[i].offset
        );

        SendMessage(
            panel->boolChecks[i],
            BM_SETCHECK,
            *field ? BST_CHECKED : BST_UNCHECKED,
            0
        );
    }

    panel->updatingControls = 0;
}