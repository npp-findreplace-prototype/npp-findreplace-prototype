#include <windows.h>

#include "button_grid_internal.h"
#include "button_grid_settings.h"
#include "button_grid_settings_internal.h"
#include "theme_resources.h"

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

void ButtonGrid_SettingsPopulateThemeCombo(HWND combo, ButtonGrid *grid)
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