#include <windows.h>
#include <commctrl.h>

#include "button_grid_internal.h"
#include "button_grid_settings.h"
#include "button_grid_settings_internal.h"
#include "theme_resources.h"

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

static void ButtonGrid_SettingsAddComboText(HWND combo, const char *text)
{
    int i;
    int count;

    if (!combo || !text || !text[0])
        return;

    count = (int)SendMessage(combo, CB_GETCOUNT, 0, 0);

    for (i = 0; i < count; i++)
    {
        char existing[BUTTON_GRID_THEME_NAME_SIZE];

        existing[0] = '\0';

        SendMessage(combo, CB_GETLBTEXT, i, (LPARAM)existing);

        if (lstrcmpi(existing, text) == 0)
            return;
    }

    SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)text);
}

static void ButtonGrid_SettingsGetExeDirectory(char *buffer, int bufferSize)
{
    int len;
    int i;

    if (!buffer || bufferSize <= 0)
        return;

    buffer[0] = '\0';

    GetModuleFileName(NULL, buffer, bufferSize);

    len = lstrlen(buffer);

    for (i = len - 1; i >= 0; i--)
    {
        if (buffer[i] == '\\' || buffer[i] == '/')
        {
            buffer[i] = '\0';
            return;
        }
    }

    lstrcpy(buffer, ".");
}

static void ButtonGrid_SettingsAddFolderThemes(HWND combo)
{
    char exeDir[MAX_PATH];
    char searchPath[MAX_PATH];
    WIN32_FIND_DATA findData;
    HANDLE findHandle;

    ButtonGrid_SettingsGetExeDirectory(exeDir, MAX_PATH);

    wsprintf(searchPath, "%s\\themes\\*", exeDir);

    findHandle = FindFirstFile(searchPath, &findData);

    if (findHandle == INVALID_HANDLE_VALUE)
        return;

    do
    {
        if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
            lstrcmp(findData.cFileName, ".") != 0 &&
            lstrcmp(findData.cFileName, "..") != 0)
        {
            ButtonGrid_SettingsAddComboText(combo, findData.cFileName);
        }
    }
    while (FindNextFile(findHandle, &findData));

    FindClose(findHandle);
}

static void ButtonGrid_SettingsAddEmbeddedThemes(HWND combo)
{
    int i;
    char displayName[BUTTON_GRID_THEME_NAME_SIZE];

    for (i = 0; i < ThemeResources_GetThemeCount(); i++)
    {
        const char *themeName;

        themeName = ThemeResources_GetThemeName(i);

        if (!themeName || !themeName[0])
            continue;

        wsprintf(displayName, "exe:%s", themeName);

        ButtonGrid_SettingsAddComboText(combo, displayName);
    }
}

static void ButtonGrid_SettingsSelectComboText(HWND combo, const char *text)
{
    int i;
    int count;
    int found;

    found = -1;
    count = (int)SendMessage(combo, CB_GETCOUNT, 0, 0);

    for (i = 0; i < count; i++)
    {
        char itemText[BUTTON_GRID_THEME_NAME_SIZE];

        itemText[0] = '\0';

        SendMessage(combo, CB_GETLBTEXT, i, (LPARAM)itemText);

        if (text && lstrcmpi(itemText, text) == 0)
        {
            found = i;
            break;
        }
    }

    if (found < 0)
        found = 0;

    SendMessage(combo, CB_SETCURSEL, found, 0);
}

static void ButtonGrid_SettingsPopulateThemeCombo(HWND combo, ButtonGrid *grid)
{
    if (!combo || !grid)
        return;

    SendMessage(combo, CB_RESETCONTENT, 0, 0);

    ButtonGrid_SettingsAddComboText(combo, BUTTON_GRID_DEFAULT_THEME_NAME);
    ButtonGrid_SettingsAddFolderThemes(combo);
    ButtonGrid_SettingsAddEmbeddedThemes(combo);

    if (grid->themeName[0])
        ButtonGrid_SettingsAddComboText(combo, grid->themeName);

    ButtonGrid_SettingsSelectComboText(combo, grid->themeName);

    EnableWindow(combo, grid->allowThemeSelection ? TRUE : FALSE);
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

int ButtonGrid_SettingsHandleCommand(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    ButtonGrid *grid;
    int id;
    int code;
    int index;
    int part;

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