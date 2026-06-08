#include "button_grid_ini_internal.h"

static void Ini_DefaultButtonName(
    char *buffer,
    int bufferSize,
    int index
)
{
    wsprintf(buffer, "Button%d", index);
    buffer[bufferSize - 1] = '\0';
}

void Ini_ReadButtonSection(
    const IniSource *source,
    ButtonGridIniConfig *loaded,
    int index
)
{
    char section[64];
    char defaultName[64];
    ButtonGridItemConfig *item;
    ButtonGridIniItemText *text;

    wsprintf(section, "Button%d", index + 1);
    section[sizeof(section) - 1] = '\0';

    item = &loaded->items[index];
    text = &loaded->itemTexts[index];

    ZeroMemory(item, sizeof(*item));
    ZeroMemory(text, sizeof(*text));

    Ini_DefaultButtonName(defaultName, sizeof(defaultName), index + 1);

    Ini_ReadString(source, section, "name", defaultName, text->name, sizeof(text->name));
    Ini_ReadString(source, section, "action", "", text->action, sizeof(text->action));
    Ini_ReadString(source, section, "text", text->name, text->text, sizeof(text->text));
    Ini_ReadString(source, section, "tooltip", text->name, text->tooltip, sizeof(text->tooltip));
    Ini_ReadString(source, section, "iconBaseName", text->name, text->iconBaseName, sizeof(text->iconBaseName));

    item->name = text->name[0] ? text->name : NULL;
    item->action = text->action[0] ? text->action : NULL;
    item->text = text->text[0] ? text->text : NULL;
    item->tooltip = text->tooltip[0] ? text->tooltip : NULL;
    item->iconBaseName = text->iconBaseName[0] ? text->iconBaseName : NULL;

    item->behavior = Ini_ReadBehavior(source, section, "behavior", BUTTON_GRID_BUTTON_TOGGLE);
    item->radioGroup = Ini_ReadInt(source, section, "radioGroup", 0);
    item->defaultState = Ini_ReadBool(source, section, "defaultState", loaded->config.defaultState);

    item->widthOverride = Ini_ReadInt(source, section, "widthOverride", 0);
    item->heightOverride = Ini_ReadInt(source, section, "heightOverride", 0);

    item->sizeModeOverride = Ini_ReadSizeMode(
        source,
        section,
        "sizeModeOverride",
        BUTTON_GRID_SIZE_USE_DEFAULT
    );

    item->showTextOverride = Ini_ReadShowTextOverride(
        source,
        section,
        "showTextOverride",
        BUTTON_GRID_TEXT_USE_DEFAULT
    );

    item->pictureOff = NULL;
    item->pictureOn = NULL;
    item->ownsPictureOff = 0;
    item->ownsPictureOn = 0;
    item->pictureOffLoadFailed = 0;
    item->pictureOnLoadFailed = 0;
}