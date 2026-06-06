#ifndef THEME_MANAGER_H
#define THEME_MANAGER_H

#define THEME_MANAGER_MAX_THEMES 64
#define THEME_MANAGER_NAME_SIZE 64

typedef struct ThemeManager
{
    char names[THEME_MANAGER_MAX_THEMES][THEME_MANAGER_NAME_SIZE];
    int count;
    int currentIndex;
} ThemeManager;

void ThemeManager_Rebuild(
    ThemeManager *manager,
    const char **builtInThemes,
    int builtInThemeCount
);

const char *ThemeManager_GetCurrentDisplayName(ThemeManager *manager);
const char *ThemeManager_GetCurrentLoadName(ThemeManager *manager);

void ThemeManager_Cycle(ThemeManager *manager, int direction);

#endif