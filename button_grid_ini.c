#include "button_grid_ini_internal.h"

int ButtonGridIni_Load(
    const char *fileName,
    ButtonGridIniConfig *loaded
)
{
    if (!fileName || !loaded)
        return 0;

    return Ini_LoadFileToConfig(fileName, loaded);
}

int ButtonGridIni_LoadFromFileOrResource(
    const char *fileName,
    ButtonGridIniConfig *loaded
)
{
    char exeRelativePath[MAX_PATH];

    if (!fileName || !loaded)
        return 0;

    if (Ini_FileExists(fileName))
        return Ini_LoadFileToConfig(fileName, loaded);

    if (!Ini_PathIsAbsolute(fileName))
    {
        if (Ini_BuildExeRelativePath(
                fileName,
                exeRelativePath,
                sizeof(exeRelativePath)
            ))
        {
            if (Ini_FileExists(exeRelativePath))
                return Ini_LoadFileToConfig(exeRelativePath, loaded);
        }
    }

    if (Ini_LoadWin32ResourceToConfig(fileName, loaded))
        return 1;

    if (Ini_LoadEmbeddedResourceToConfig(fileName, loaded))
        return 1;

    if (Ini_LoadBuiltinResourceToConfig(fileName, loaded))
        return 1;

    return 0;
}

void ButtonGridIni_Free(ButtonGridIniConfig *loaded)
{
    if (!loaded)
        return;

    if (loaded->items)
        free(loaded->items);

    if (loaded->itemTexts)
        free(loaded->itemTexts);

    ZeroMemory(loaded, sizeof(*loaded));
}