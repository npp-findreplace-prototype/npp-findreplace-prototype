#include "button_grid_ini_internal.h"

static int Ini_TextIsAny2(
    const char *text,
    const char *a,
    const char *b
)
{
    return Ui_SameTextI(text, a) || Ui_SameTextI(text, b);
}

static int Ini_TextIsAny3(
    const char *text,
    const char *a,
    const char *b,
    const char *c
)
{
    return Ui_SameTextI(text, a) || Ui_SameTextI(text, b) || Ui_SameTextI(text, c);
}

static int Ini_ReadLayout(
    const IniSource *source,
    const char *section,
    const char *key,
    int defaultValue
)
{
    char text[128];

    Ini_ReadString(source, section, key, "", text, sizeof(text));

    if (!text[0])
        return defaultValue;

    if (Ui_SameTextI(text, "horizontal"))
        return BUTTON_GRID_LAYOUT_HORIZONTAL;

    if (Ui_SameTextI(text, "vertical"))
        return BUTTON_GRID_LAYOUT_VERTICAL;

    return atoi(text);
}

static int Ini_ReadSizeMode(
    const IniSource *source,
    const char *section,
    const char *key,
    int defaultValue
)
{
    char text[128];

    Ini_ReadString(source, section, key, "", text, sizeof(text));

    if (!text[0])
        return defaultValue;

    if (Ini_TextIsAny2(text, "default", "useDefault"))
        return BUTTON_GRID_SIZE_USE_DEFAULT;

    if (Ui_SameTextI(text, "fixed"))
        return BUTTON_GRID_SIZE_FIXED;

    if (Ini_TextIsAny3(text, "matchImageSize", "match_image_size", "imageSize"))
        return BUTTON_GRID_SIZE_MATCH_IMAGE_SIZE;

    if (Ini_TextIsAny3(text, "aspectHorizontal", "aspect_horizontal", "horizontalAspect"))
        return BUTTON_GRID_SIZE_MATCH_IMAGE_ASPECT_HORIZONTAL;

    if (Ini_TextIsAny3(text, "aspectVertical", "aspect_vertical", "verticalAspect"))
        return BUTTON_GRID_SIZE_MATCH_IMAGE_ASPECT_VERTICAL;

    if (Ini_TextIsAny3(text, "aspectByLayout", "aspect_by_layout", "byLayout"))
        return BUTTON_GRID_SIZE_MATCH_IMAGE_ASPECT_BY_LAYOUT;

    return atoi(text);
}

static int Ini_ReadButtonBackMode(
    const IniSource *source,
    const char *section,
    const char *key,
    int defaultValue
)
{
    char text[128];

    Ini_ReadString(source, section, key, "", text, sizeof(text));

    if (!text[0])
        return defaultValue;

    if (Ui_SameTextI(text, "opaque"))
        return BUTTON_GRID_BUTTON_BACK_OPAQUE;

    if (Ini_TextIsAny3(text, "transparent", "transparentSimulated", "simulatedTransparent"))
        return BUTTON_GRID_BUTTON_BACK_TRANSPARENT;

    return atoi(text);
}

static int Ini_ReadBorderStyle(
    const IniSource *source,
    const char *section,
    const char *key,
    int defaultValue
)
{
    char text[128];

    Ini_ReadString(source, section, key, "", text, sizeof(text));

    if (!text[0])
        return defaultValue;

    if (Ui_SameTextI(text, "none"))
        return BUTTON_GRID_BORDER_STYLE_NONE;

    if (Ui_SameTextI(text, "simple"))
        return BUTTON_GRID_BORDER_STYLE_SIMPLE;

    if (Ui_SameTextI(text, "etched"))
        return BUTTON_GRID_BORDER_STYLE_ETCHED;

    if (Ui_SameTextI(text, "rounded"))
        return BUTTON_GRID_BORDER_STYLE_ROUNDED;

    if (Ini_TextIsAny2(text, "etchedRounded", "etched_rounded"))
        return BUTTON_GRID_BORDER_STYLE_ETCHED_ROUNDED;

    if (Ui_SameTextI(text, "container"))
        return BUTTON_GRID_BORDER_STYLE_CONTAINER;

    if (Ui_SameTextI(text, "sunken"))
        return BUTTON_GRID_BORDER_STYLE_SUNKEN;

    if (Ui_SameTextI(text, "raised"))
        return BUTTON_GRID_BORDER_STYLE_RAISED;

    if (Ui_SameTextI(text, "double"))
        return BUTTON_GRID_BORDER_STYLE_DOUBLE;

    return atoi(text);
}

static int Ini_ReadGearCorner(
    const IniSource *source,
    const char *section,
    const char *key,
    int defaultValue
)
{
    char text[128];

    Ini_ReadString(source, section, key, "", text, sizeof(text));

    if (!text[0])
        return defaultValue;

    if (Ini_TextIsAny2(text, "topLeft", "top_left"))
        return BUTTON_GRID_GEAR_CORNER_TOP_LEFT;

    if (Ini_TextIsAny2(text, "topRight", "top_right"))
        return BUTTON_GRID_GEAR_CORNER_TOP_RIGHT;

    if (Ini_TextIsAny2(text, "bottomLeft", "bottom_left"))
        return BUTTON_GRID_GEAR_CORNER_BOTTOM_LEFT;

    if (Ini_TextIsAny2(text, "bottomRight", "bottom_right"))
        return BUTTON_GRID_GEAR_CORNER_BOTTOM_RIGHT;

    return atoi(text);
}

static int Ini_ReadContentAlignment(
    const IniSource *source,
    const char *section,
    const char *key,
    int defaultValue
)
{
    char text[128];

    Ini_ReadString(source, section, key, "", text, sizeof(text));

    if (!text[0])
        return defaultValue;

    if (Ini_TextIsAny2(text, "topLeft", "top_left"))
        return BUTTON_GRID_ALIGN_TOP_LEFT;

    if (Ui_SameTextI(text, "top"))
        return BUTTON_GRID_ALIGN_TOP;

    if (Ini_TextIsAny2(text, "topRight", "top_right"))
        return BUTTON_GRID_ALIGN_TOP_RIGHT;

    if (Ui_SameTextI(text, "left"))
        return BUTTON_GRID_ALIGN_LEFT;

    if (Ui_SameTextI(text, "center"))
        return BUTTON_GRID_ALIGN_CENTER;

    if (Ui_SameTextI(text, "right"))
        return BUTTON_GRID_ALIGN_RIGHT;

    if (Ini_TextIsAny2(text, "bottomLeft", "bottom_left"))
        return BUTTON_GRID_ALIGN_BOTTOM_LEFT;

    if (Ui_SameTextI(text, "bottom"))
        return BUTTON_GRID_ALIGN_BOTTOM;

    if (Ini_TextIsAny2(text, "bottomRight", "bottom_right"))
        return BUTTON_GRID_ALIGN_BOTTOM_RIGHT;

    if (Ini_TextIsAny2(text, "xy", "x/y"))
        return BUTTON_GRID_ALIGN_XY;

    if (Ini_TextIsAny2(text, "percent", "percentage"))
        return BUTTON_GRID_ALIGN_PERCENT;

    return atoi(text);
}

static int Ini_ReadBehavior(
    const IniSource *source,
    const char *section,
    const char *key,
    int defaultValue
)
{
    char text[128];

    Ini_ReadString(source, section, key, "", text, sizeof(text));

    if (!text[0])
        return defaultValue;

    if (Ui_SameTextI(text, "toggle"))
        return BUTTON_GRID_BUTTON_TOGGLE;

    if (Ui_SameTextI(text, "radio"))
        return BUTTON_GRID_BUTTON_RADIO;

    if (Ui_SameTextI(text, "disabled"))
        return BUTTON_GRID_BUTTON_DISABLED;

    return atoi(text);
}

static int Ini_ReadShowTextOverride(
    const IniSource *source,
    const char *section,
    const char *key,
    int defaultValue
)
{
    char text[128];

    Ini_ReadString(source, section, key, "", text, sizeof(text));

    if (!text[0])
        return defaultValue;

    if (Ini_TextIsAny2(text, "default", "useDefault"))
        return BUTTON_GRID_TEXT_USE_DEFAULT;

    if (Ui_SameTextI(text, "show") ||
        Ui_SameTextI(text, "on") ||
        Ui_SameTextI(text, "true"))
    {
        return BUTTON_GRID_TEXT_SHOW;
    }

    if (Ui_SameTextI(text, "hide") ||
        Ui_SameTextI(text, "off") ||
        Ui_SameTextI(text, "false"))
    {
        return BUTTON_GRID_TEXT_HIDE;
    }

    return atoi(text);
}

static void Ini_ReadGridSection(
    const IniSource *source,
    ButtonGridIniConfig *loaded
)
{
    ButtonGridConfig *config;
    const char *section;

    section = "Grid";
    config = &loaded->config;

    config->buttonCount = Ini_ReadInt(source, section, "buttonCount", config->buttonCount);

    config->buttonWidth = Ini_ReadInt(source, section, "buttonWidth", config->buttonWidth);
    config->buttonHeight = Ini_ReadInt(source, section, "buttonHeight", config->buttonHeight);
    config->horizontalSpacing = Ini_ReadInt(source, section, "horizontalSpacing", config->horizontalSpacing);
    config->verticalSpacing = Ini_ReadInt(source, section, "verticalSpacing", config->verticalSpacing);

    config->layout = Ini_ReadLayout(source, section, "layout", config->layout);
    config->sizeMode = Ini_ReadSizeMode(source, section, "sizeMode", config->sizeMode);

    config->showText = Ini_ReadBool(source, section, "showText", config->showText);
    config->hidePartialButtons = Ini_ReadBool(source, section, "hidePartialButtons", config->hidePartialButtons);
    config->resizeInLayoutSteps = Ini_ReadBool(source, section, "resizeInLayoutSteps", config->resizeInLayoutSteps);
    config->settingsWheelScrub = Ini_ReadBool(source, section, "settingsWheelScrub", config->settingsWheelScrub);

    config->dpiScaleEnabled = Ini_ReadBool(source, section, "dpiScaleEnabled", config->dpiScaleEnabled);

    config->contentAlignment = Ini_ReadContentAlignment(source, section, "contentAlignment", config->contentAlignment);
    config->contentAlignX = Ini_ReadInt(source, section, "contentAlignX", config->contentAlignX);
    config->contentAlignY = Ini_ReadInt(source, section, "contentAlignY", config->contentAlignY);
    config->contentAlignPercentX = Ini_ReadInt(source, section, "contentAlignPercentX", config->contentAlignPercentX);
    config->contentAlignPercentY = Ini_ReadInt(source, section, "contentAlignPercentY", config->contentAlignPercentY);

    Ini_ReadString(source, section, "themeName", loaded->themeName, loaded->themeName, sizeof(loaded->themeName));
    config->themeName = loaded->themeName;

    config->allowThemeSelection = Ini_ReadBool(source, section, "allowThemeSelection", config->allowThemeSelection);

    config->showBorder = Ini_ReadBool(source, section, "showBorder", config->showBorder);
    config->showBorderTitle = Ini_ReadBool(source, section, "showBorderTitle", config->showBorderTitle);

    Ini_ReadString(source, section, "borderTitle", loaded->borderTitle, loaded->borderTitle, sizeof(loaded->borderTitle));
    config->borderTitle = loaded->borderTitle;

    config->borderPadding = Ini_ReadInt(source, section, "borderPadding", config->borderPadding);
    config->borderTitleHeight = Ini_ReadInt(source, section, "borderTitleHeight", config->borderTitleHeight);
    config->borderTitlePadding = Ini_ReadInt(source, section, "borderTitlePadding", config->borderTitlePadding);
    config->borderTitleFontSize = Ini_ReadInt(source, section, "borderTitleFontSize", config->borderTitleFontSize);
    config->borderTitleTransparent = Ini_ReadBool(source, section, "borderTitleTransparent", config->borderTitleTransparent);
    config->borderTitleAutoBackColor = Ini_ReadBool(source, section, "borderTitleAutoBackColor", config->borderTitleAutoBackColor);

    config->borderStyle = Ini_ReadBorderStyle(source, section, "borderStyle", config->borderStyle);
    config->borderThickness = Ini_ReadInt(source, section, "borderThickness", config->borderThickness);
    config->borderCornerRadius = Ini_ReadInt(source, section, "borderCornerRadius", config->borderCornerRadius);

    config->borderColor = Ini_ReadColor(source, section, "borderColor", config->borderColor);
    config->borderLightColor = Ini_ReadColor(source, section, "borderLightColor", config->borderLightColor);
    config->borderShadowColor = Ini_ReadColor(source, section, "borderShadowColor", config->borderShadowColor);
    config->borderTitleColor = Ini_ReadColor(source, section, "borderTitleColor", config->borderTitleColor);
    config->borderTitleBackColor = Ini_ReadColor(source, section, "borderTitleBackColor", config->borderTitleBackColor);

    config->showGearIcon = Ini_ReadBool(source, section, "showGearIcon", config->showGearIcon);
    config->gearCorner = Ini_ReadGearCorner(source, section, "gearCorner", config->gearCorner);
    config->gearSize = Ini_ReadInt(source, section, "gearSize", config->gearSize);
    config->gearMargin = Ini_ReadInt(source, section, "gearMargin", config->gearMargin);

    config->gearColor = Ini_ReadColor(source, section, "gearColor", config->gearColor);
    config->gearBackColor = Ini_ReadColor(source, section, "gearBackColor", config->gearBackColor);
    config->gearBorderColor = Ini_ReadColor(source, section, "gearBorderColor", config->gearBorderColor);

    config->idBase = Ini_ReadInt(source, section, "idBase", config->idBase);
    config->firstIndex = Ini_ReadInt(source, section, "firstIndex", config->firstIndex);

    Ini_ReadString(source, section, "namePrefix", loaded->namePrefix, loaded->namePrefix, sizeof(loaded->namePrefix));
    Ini_ReadString(source, section, "actionPrefix", loaded->actionPrefix, loaded->actionPrefix, sizeof(loaded->actionPrefix));
    Ini_ReadString(source, section, "textFormat", loaded->textFormat, loaded->textFormat, sizeof(loaded->textFormat));
    Ini_ReadString(source, section, "clickIdentifierFormat", loaded->clickIdentifierFormat, loaded->clickIdentifierFormat, sizeof(loaded->clickIdentifierFormat));

    config->namePrefix = loaded->namePrefix;
    config->actionPrefix = loaded->actionPrefix;
    config->textFormat = loaded->textFormat;
    config->clickIdentifierFormat = loaded->clickIdentifierFormat;

    config->buttonBackMode = Ini_ReadButtonBackMode(source, section, "buttonBackMode", config->buttonBackMode);
    config->showButtonBorder = Ini_ReadBool(source, section, "showButtonBorder", config->showButtonBorder);
    config->buttonBorderThickness = Ini_ReadInt(source, section, "buttonBorderThickness", config->buttonBorderThickness);
    config->buttonBorderColor = Ini_ReadColor(source, section, "buttonBorderColor", config->buttonBorderColor);

    config->backColor = Ini_ReadColor(source, section, "backColor", config->backColor);
    config->foreColor = Ini_ReadColor(source, section, "foreColor", config->foreColor);

    config->usePictures = Ini_ReadBool(source, section, "usePictures", config->usePictures);
    config->toggleOnClick = Ini_ReadBool(source, section, "toggleOnClick", config->toggleOnClick);
    config->defaultState = Ini_ReadBool(source, section, "defaultState", config->defaultState);
    config->stretchPictures = Ini_ReadBool(source, section, "stretchPictures", config->stretchPictures);

    config->generatedOffPictureColor = Ini_ReadColor(source, section, "generatedOffPictureColor", config->generatedOffPictureColor);
    config->generatedOnPictureColor = Ini_ReadColor(source, section, "generatedOnPictureColor", config->generatedOnPictureColor);
    config->generatedErrorPictureColor = Ini_ReadColor(source, section, "generatedErrorPictureColor", config->generatedErrorPictureColor);
}

static void Ini_DefaultButtonName(char *buffer, int bufferSize, int index)
{
    wsprintf(buffer, "Button%d", index);
    buffer[bufferSize - 1] = '\0';
}

static void Ini_ReadButtonSection(
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

int ButtonGridIni_LoadFromSource(
    const IniSource *source,
    ButtonGridIniConfig *loaded
)
{
    int i;

    if (!source || !loaded || !source->memoryText)
        return 0;

    ZeroMemory(loaded, sizeof(*loaded));

    Ini_CopyText(loaded->fileName, sizeof(loaded->fileName), source->displayName);

    ButtonGrid_GetDefaultConfig(&loaded->config);

    Ini_CopyText(loaded->themeName, sizeof(loaded->themeName), BUTTON_GRID_DEFAULT_THEME_NAME);
    Ini_CopyText(loaded->borderTitle, sizeof(loaded->borderTitle), BUTTON_GRID_DEFAULT_BORDER_TITLE);

    Ini_CopyText(loaded->namePrefix, sizeof(loaded->namePrefix), BUTTON_GRID_DEFAULT_NAME_PREFIX);
    Ini_CopyText(loaded->actionPrefix, sizeof(loaded->actionPrefix), BUTTON_GRID_DEFAULT_ACTION_PREFIX);
    Ini_CopyText(loaded->textFormat, sizeof(loaded->textFormat), BUTTON_GRID_DEFAULT_TEXT_FORMAT);
    Ini_CopyText(loaded->clickIdentifierFormat, sizeof(loaded->clickIdentifierFormat), BUTTON_GRID_DEFAULT_CLICK_IDENTIFIER_FORMAT);

    loaded->config.themeName = loaded->themeName;
    loaded->config.borderTitle = loaded->borderTitle;
    loaded->config.namePrefix = loaded->namePrefix;
    loaded->config.actionPrefix = loaded->actionPrefix;
    loaded->config.textFormat = loaded->textFormat;
    loaded->config.clickIdentifierFormat = loaded->clickIdentifierFormat;

    Ini_ReadGridSection(source, loaded);

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

    ZeroMemory(loaded->items, sizeof(ButtonGridItemConfig) * loaded->itemCount);
    ZeroMemory(loaded->itemTexts, sizeof(ButtonGridIniItemText) * loaded->itemCount);

    for (i = 0; i < loaded->itemCount; i++)
        Ini_ReadButtonSection(source, loaded, i);

    loaded->config.items = loaded->items;
    loaded->config.buttonCount = loaded->itemCount;

    ButtonGrid_NormalizeConfig(&loaded->config);

    return 1;
}