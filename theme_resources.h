#ifndef THEME_RESOURCES_H
#define THEME_RESOURCES_H

typedef struct ThemeResourceFile
{
    const char *themeName;
    const char *fileName;
    const unsigned char *data;
    unsigned int size;
} ThemeResourceFile;

int ThemeResources_GetThemeCount(void);
const char *ThemeResources_GetThemeName(int index);

int ThemeResources_GetFileCount(void);
const ThemeResourceFile *ThemeResources_GetFile(int index);

const unsigned char *ThemeResources_FindFile(
    const char *themeName,
    const char *fileName,
    unsigned int *size
);

#endif