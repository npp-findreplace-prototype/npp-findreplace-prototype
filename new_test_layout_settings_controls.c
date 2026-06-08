#include "new_test_layout_settings_internal.h"

void Settings_RecreateBrushes(
    NewTestLayoutSettingsPanel *panel
)
{
    if (!panel)
        return;

    if (panel->backBrush)
        DeleteObject(panel->backBrush);

    if (panel->editBrush)
        DeleteObject(panel->editBrush);

    panel->backBrush = CreateSolidBrush(panel->config.overlayBackColor);
    panel->editBrush = CreateSolidBrush(RGB(18, 18, 18));
}

static HWND Settings_CreateStatic(
    NewTestLayoutSettingsPanel *panel,
    const char *text
)
{
    HWND hwnd;

    hwnd = CreateWindowEx(
        0,
        "STATIC",
        text ? text : "",
        WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE,
        0,
        0,
        10,
        10,
        panel->container,
        NULL,
        panel->hInstance,
        NULL
    );

    if (hwnd)
        SendMessage(hwnd, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);

    return hwnd;
}

static HWND Settings_CreateEdit(
    NewTestLayoutSettingsPanel *panel,
    int id
)
{
    HWND hwnd;

    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        "EDIT",
        "",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
        0,
        0,
        10,
        10,
        panel->container,
        (HMENU)id,
        panel->hInstance,
        NULL
    );

    if (hwnd)
        SendMessage(hwnd, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);

    return hwnd;
}

static HWND Settings_CreateCheckbox(
    NewTestLayoutSettingsPanel *panel,
    int id,
    const char *text
)
{
    HWND hwnd;

    hwnd = CreateWindowEx(
        0,
        "BUTTON",
        text ? text : "",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX,
        0,
        0,
        10,
        10,
        panel->container,
        (HMENU)id,
        panel->hInstance,
        NULL
    );

    if (hwnd)
        SendMessage(hwnd, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);

    return hwnd;
}

int Settings_CreateChildControls(
    NewTestLayoutSettingsPanel *panel
)
{
    int i;

    if (!panel || !panel->container)
        return 0;

    panel->titleLabel = Settings_CreateStatic(panel, "Layout Settings");

    panel->closeButton = CreateWindowEx(
        0,
        "BUTTON",
        "X",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
        0,
        0,
        28,
        24,
        panel->container,
        (HMENU)NTL_SETTINGS_ID_CLOSE_BUTTON,
        panel->hInstance,
        NULL
    );

    if (panel->closeButton)
        SendMessage(panel->closeButton, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);

    for (i = 0; i < NTL_SETTINGS_INT_COUNT; i++)
    {
        panel->intLabels[i] = Settings_CreateStatic(
            panel,
            g_ntlSettingsIntDefs[i].label
        );

        panel->intEdits[i] = Settings_CreateEdit(
            panel,
            NTL_SETTINGS_ID_INT_BASE + i
        );
    }

    for (i = 0; i < NTL_SETTINGS_BOOL_COUNT; i++)
    {
        panel->boolChecks[i] = Settings_CreateCheckbox(
            panel,
            NTL_SETTINGS_ID_BOOL_BASE + i,
            g_ntlSettingsBoolDefs[i].label
        );
    }

    return 1;
}