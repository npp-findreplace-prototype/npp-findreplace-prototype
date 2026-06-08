#include "button_grid_ini_internal.h"

static void Ini_InitLoadedDefaults(
    const IniSource *source,
    ButtonGridIniConfig *loaded
)
{
    ZeroMemory(loaded, sizeof(*loaded));

    Ini_CopyText(
        loaded->fileName,
        sizeof(loaded->fileName),
        source ? source->displayName : ""
    );

    ButtonGrid_GetDefaultConfig(&loaded->config);

    Ini_CopyText(
        loaded->themeName,
        sizeof(loaded->themeName),
        BUTTON_GRID_DEFAULT_THEME_NAME
    );

    Ini_CopyText(
        loaded->borderTitle,
        sizeof(loaded->borderTitle),
        BUTTON_GRID_DEFAULT_BORDER_TITLE
    );

    Ini_CopyText(
        loaded->namePrefix,
        sizeof(loaded->namePrefix),
        BUTTON_GRID_DEFAULT_NAME_PREFIX
    );

    Ini_CopyText(
        loaded->actionPrefix,
        sizeof(loaded->actionPrefix),
        BUTTON_GRID_DEFAULT_ACTION_PREFIX
    );

    Ini_CopyText(
        loaded->textFormat,
        sizeof(loaded->textFormat),
        BUTTON_GRID_DEFAULT_TEXT_FORMAT
    );

    Ini_CopyText(
        loaded->clickIdentifierFormat,
        sizeof(loaded->clickIdentifierFormat),
        BUTTON_GRID_DEFAULT_CLICK_IDENTIFIER_FORMAT
    );

    loaded->config.themeName = loaded->themeName;
    loaded->config.borderTitle = loaded->borderTitle;
    loaded->config.namePrefix = loaded->namePrefix;
    loaded->config.actionPrefix = loaded->actionPrefix;
    loaded->config.textFormat = loaded->textFormat;
    loaded->config.clickIdentifierFormat = loaded->clickIdentifierFormat;
}

static int Ini_AllocateItems(ButtonGridIniConfig *loaded)
{
    if (!loaded)
        return 0;

    if (loaded->config.buttonCount < 1)
        loaded->config.buttonCount = 1;

    loaded->itemCount = loaded->config.buttonCount;

    loaded->items = (ButtonGridItemConfig *)malloc(
        sizeof(ButtonGridItemConfig) * loaded->itemCount
    );

    loaded->itemTexts = (ButtonGridIniItemText *)malloc(
        sizeof(ButtonGridIniItemText) * loaded->itemCount
    );

    if (!loaded->items || !loaded->itemTexts)
    {
        ButtonGridIni_Free(loaded);
        return 0;
    }

    ZeroMemory(
        loaded->items,
        sizeof(ButtonGridItemConfig) * loaded->itemCount
    );

    ZeroMemory(
        loaded->itemTexts,
        sizeof(ButtonGridIniItemText) * loaded->itemCount
    );

    return 1;
}

int ButtonGridIni_LoadFromSource(
    const IniSource *source,
    ButtonGridIniConfig *loaded
)
{
    int i;

    if (!source || !loaded || !source->memoryText)
        return 0;

    Ini_InitLoadedDefaults(source, loaded);

    Ini_ReadGridSection(source, loaded);

    if (!Ini_AllocateItems(loaded))
        return 0;

    for (i = 0; i < loaded->itemCount; i++)
        Ini_ReadButtonSection(source, loaded, i);

    loaded->config.items = loaded->items;
    loaded->config.buttonCount = loaded->itemCount;

    ButtonGrid_NormalizeConfig(&loaded->config);

    return 1;
}