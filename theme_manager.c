#include <windows.h>

#include "theme_manager.h"

static void ThemeManager_GetExeDirectory(char *buffer, int bufferSize)
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

static int ThemeManager_SameText(const char *a, const char *b)
{
    if (!a || !b)
        return 0;

    return lstrcmpi(a, b) == 0;
}

static void ThemeManager_Add(ThemeManager *manager, const char *name)
{
    int i;

    if (!manager || !name || !name[0])
        return;

    for (i = 0; i < manager->count; i++)
    {
        if (ThemeManager_SameText(manager->names[i], name))
            return;
    }

    if (manager->count >= THEME_MANAGER_MAX_THEMES)
        return;

    lstrcpyn(manager->names[manager->count], name, THEME_MANAGER_NAME_SIZE);
    manager->names[manager->count][THEME_MANAGER_NAME_SIZE - 1] = '\0';

    manager->count++;
}

void ThemeManager_Rebuild(
    ThemeManager *manager,
    const char **builtInThemes,
    int builtInThemeCount
)
{
    char exeDir[MAX_PATH];
    char searchPath[MAX_PATH];
    WIN32_FIND_DATA findData;
    HANDLE findHandle;
    int i;

    if (!manager)
        return;

    ZeroMemory(manager, sizeof(ThemeManager));

    for (i = 0; i < builtInThemeCount; i++)
        ThemeManager_Add(manager, builtInThemes[i]);

    ThemeManager_GetExeDirectory(exeDir, MAX_PATH);

    wsprintf(searchPath, "%s\\themes\\*", exeDir);

    findHandle = FindFirstFile(searchPath, &findData);

    if (findHandle != INVALID_HANDLE_VALUE)
    {
        do
        {
            if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
                lstrcmp(findData.cFileName, ".") != 0 &&
                lstrcmp(findData.cFileName, "..") != 0)
            {
                ThemeManager_Add(manager, findData.cFileName);
            }
        }
        while (FindNextFile(findHandle, &findData));

        FindClose(findHandle);
    }

    if (manager->count < 1)
        ThemeManager_Add(manager, "Default");

    manager->currentIndex = 0;
}

const char *ThemeManager_GetCurrentDisplayName(ThemeManager *manager)
{
    if (!manager || manager->count < 1)
        return "Default";

    return manager->names[manager->currentIndex];
}

const char *ThemeManager_GetCurrentLoadName(ThemeManager *manager)
{
    const char *name;

    name = ThemeManager_GetCurrentDisplayName(manager);

    if (ThemeManager_SameText(name, "Default"))
        return NULL;

    return name;
}

void ThemeManager_Cycle(ThemeManager *manager, int direction)
{
    if (!manager || manager->count < 1)
        return;

    manager->currentIndex += direction;

    while (manager->currentIndex < 0)
        manager->currentIndex += manager->count;

    while (manager->currentIndex >= manager->count)
        manager->currentIndex -= manager->count;
}