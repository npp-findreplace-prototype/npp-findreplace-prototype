#include <windows.h>
#include <commctrl.h>

#include "button_grid_internal.h"
#include "button_grid_settings.h"
#include "button_grid_settings_internal.h"

static HWND ButtonGrid_SettingsCreateLabel(
    HWND pageHwnd,
    ButtonGrid *grid,
    int id,
    const char *text
)
{
    return CreateWindowEx(
        0,
        "STATIC",
        text,
        WS_CHILD | WS_VISIBLE,
        0,
        0,
        10,
        10,
        pageHwnd,
        (HMENU)id,
        grid->hInstance,
        NULL
    );
}

static HWND ButtonGrid_SettingsCreateButton(
    HWND pageHwnd,
    ButtonGrid *grid,
    int id,
    const char *text,
    DWORD extraStyle
)
{
    return CreateWindowEx(
        0,
        "BUTTON",
        text,
        WS_CHILD | WS_VISIBLE | extraStyle,
        0,
        0,
        10,
        10,
        pageHwnd,
        (HMENU)id,
        grid->hInstance,
        NULL
    );
}

static HWND ButtonGrid_SettingsCreateEdit(
    HWND pageHwnd,
    ButtonGrid *grid,
    int id
)
{
    HWND hwnd;

    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        "EDIT",
        "",
        WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        0,
        0,
        10,
        10,
        pageHwnd,
        (HMENU)id,
        grid->hInstance,
        NULL
    );

    ButtonGrid_SettingsSubclassWheelControl(hwnd);

    return hwnd;
}

static HWND ButtonGrid_SettingsCreateTrackbar(
    HWND pageHwnd,
    ButtonGrid *grid,
    int id
)
{
    HWND hwnd;

    hwnd = CreateWindowEx(
        0,
        TRACKBAR_CLASSA,
        "",
        WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_TOOLTIPS,
        0,
        0,
        10,
        10,
        pageHwnd,
        (HMENU)id,
        grid->hInstance,
        NULL
    );

    ButtonGrid_SettingsSubclassWheelControl(hwnd);

    return hwnd;
}

static HWND ButtonGrid_SettingsCreateCombo(
    HWND pageHwnd,
    ButtonGrid *grid,
    int id
)
{
    HWND hwnd;

    hwnd = CreateWindowEx(
        0,
        "COMBOBOX",
        "",
        WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
        0,
        0,
        10,
        200,
        pageHwnd,
        (HMENU)id,
        grid->hInstance,
        NULL
    );

    ButtonGrid_SettingsSubclassWheelControl(hwnd);

    return hwnd;
}

static HWND ButtonGrid_SettingsCreatePrimaryControl(
    HWND pageHwnd,
    ButtonGrid *grid,
    int index,
    const ButtonGridSettingDefinition *def
)
{
    int id;

    id = ButtonGrid_SettingsGetPrimaryId(index);

    if (def->type == BG_SETTING_BOOL)
        return ButtonGrid_SettingsCreateButton(pageHwnd, grid, id, "On", BS_AUTOCHECKBOX);

    if (def->type == BG_SETTING_INT)
        return ButtonGrid_SettingsCreateTrackbar(pageHwnd, grid, id);

    if (def->type == BG_SETTING_ENUM)
        return ButtonGrid_SettingsCreateCombo(pageHwnd, grid, id);

    if (def->type == BG_SETTING_THEME)
        return ButtonGrid_SettingsCreateCombo(pageHwnd, grid, id);

    return NULL;
}

void ButtonGrid_SettingsCreateControls(HWND pageHwnd, ButtonGrid *grid)
{
    int i;
    int count;

    InitCommonControls();

    ButtonGrid_SettingsCreateLabel(
        pageHwnd,
        grid,
        0,
        "Grid settings"
    );

    ButtonGrid_SettingsCreateButton(
        pageHwnd,
        grid,
        BG_SETTINGS_ID_CLOSE,
        "Close",
        BS_PUSHBUTTON
    );

    ButtonGrid_SettingsCreateLabel(
        pageHwnd,
        grid,
        BG_SETTINGS_ID_FILTER_LABEL,
        "Filter:"
    );

    CreateWindowEx(
        WS_EX_CLIENTEDGE,
        "EDIT",
        "",
        WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        66,
        38,
        360,
        24,
        pageHwnd,
        (HMENU)BG_SETTINGS_ID_FILTER,
        grid->hInstance,
        NULL
    );

    CreateWindowEx(
        0,
        "BUTTON",
        "X",
        WS_CHILD | BS_PUSHBUTTON,
        0,
        0,
        24,
        24,
        pageHwnd,
        (HMENU)BG_SETTINGS_ID_FILTER_CLEAR,
        grid->hInstance,
        NULL
    );

    count = ButtonGrid_SettingsGetCount();

    for (i = 0; i < count; i++)
    {
        const ButtonGridSettingDefinition *def;

        def = ButtonGrid_SettingsGetDefinition(i);

        ButtonGrid_SettingsCreateLabel(
            pageHwnd,
            grid,
            ButtonGrid_SettingsGetLabelId(i),
            def->label
        );

        ButtonGrid_SettingsCreatePrimaryControl(pageHwnd, grid, i, def);

        ButtonGrid_SettingsCreateEdit(
            pageHwnd,
            grid,
            ButtonGrid_SettingsGetRawId(i)
        );
    }

    ButtonGrid_SettingsRefreshAll(pageHwnd);
}