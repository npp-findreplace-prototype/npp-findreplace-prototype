#include "button_grid_ini_internal.h"

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

    if (lstrcmpi(text, "horizontal") == 0)
        return BUTTON_GRID_LAYOUT_HORIZONTAL;

    if (lstrcmpi(text, "vertical") == 0)
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

    if (lstrcmpi(text, "default") == 0 ||
        lstrcmpi(text, "useDefault") == 0)
        return BUTTON_GRID_SIZE_USE_DEFAULT;

    if (lstrcmpi(text, "fixed") == 0)
        return BUTTON_GRID_SIZE_FIXED;

    if (lstrcmpi(text, "matchImageSize") == 0 ||
        lstrcmpi(text, "match_image_size") == 0 ||
        lstrcmpi(text, "imageSize") == 0)
        return BUTTON_GRID_SIZE_MATCH_IMAGE_SIZE;

    if (lstrcmpi(text, "aspectHorizontal") == 0 ||
        lstrcmpi(text, "aspect_horizontal") == 0 ||
        lstrcmpi(text, "horizontalAspect") == 0)
        return BUTTON_GRID_SIZE_MATCH_IMAGE_ASPECT_HORIZONTAL;

    if (lstrcmpi(text, "aspectVertical") == 0 ||
        lstrcmpi(text, "aspect_vertical") == 0 ||
        lstrcmpi(text, "verticalAspect") == 0)
        return BUTTON_GRID_SIZE_MATCH_IMAGE_ASPECT_VERTICAL;

    if (lstrcmpi(text, "aspectByLayout") == 0 ||
        lstrcmpi(text, "aspect_by_layout") == 0 ||
        lstrcmpi(text, "byLayout") == 0)
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

    if (lstrcmpi(text, "opaque") == 0)
        return BUTTON_GRID_BUTTON_BACK_OPAQUE;

    if (lstrcmpi(text, "transparent") == 0 ||
        lstrcmpi(text, "transparentSimulated") == 0 ||
        lstrcmpi(text, "simulatedTransparent") == 0)
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

    if (lstrcmpi(text, "none") == 0)
        return BUTTON_GRID_BORDER_STYLE_NONE;

    if (lstrcmpi(text, "simple") == 0)
        return BUTTON_GRID_BORDER_STYLE_SIMPLE;

    if (lstrcmpi(text, "etched") == 0)
        return BUTTON_GRID_BORDER_STYLE_ETCHED;

    if (lstrcmpi(text, "rounded") == 0)
        return BUTTON_GRID_BORDER_STYLE_ROUNDED;

    if (lstrcmpi(text, "etchedRounded") == 0 ||
        lstrcmpi(text, "etched_rounded") == 0)
        return BUTTON_GRID_BORDER_STYLE_ETCHED_ROUNDED;

    if (lstrcmpi(text, "container") == 0)
        return BUTTON_GRID_BORDER_STYLE_CONTAINER;

    if (lstrcmpi(text, "sunken") == 0)
        return BUTTON_GRID_BORDER_STYLE_SUNKEN;

    if (lstrcmpi(text, "raised") == 0)
        return BUTTON_GRID_BORDER_STYLE_RAISED;

    if (lstrcmpi(text, "double") == 0)
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

    if (lstrcmpi(text, "topLeft") == 0 ||
        lstrcmpi(text, "top_left") == 0)
        return BUTTON_GRID_GEAR_CORNER_TOP_LEFT;

    if (lstrcmpi(text, "topRight") == 0 ||
        lstrcmpi(text, "top_right") == 0)
        return BUTTON_GRID_GEAR_CORNER_TOP_RIGHT;

    if (lstrcmpi(text, "bottomLeft") == 0 ||
        lstrcmpi(text, "bottom_left") == 0)
        return BUTTON_GRID_GEAR_CORNER_BOTTOM_LEFT;

    if (lstrcmpi(text, "bottomRight") == 0 ||
        lstrcmpi(text, "bottom_right") == 0)
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

    if (lstrcmpi(text, "topLeft") == 0 || lstrcmpi(text, "top_left") == 0)
        return BUTTON_GRID_ALIGN_TOP_LEFT;

    if (lstrcmpi(text, "top") == 0)
        return BUTTON_GRID_ALIGN_TOP;

    if (lstrcmpi(text, "topRight") == 0 || lstrcmpi(text, "top_right") == 0)
        return BUTTON_GRID_ALIGN_TOP_RIGHT;

    if (lstrcmpi(text, "left") == 0)
        return BUTTON_GRID_ALIGN_LEFT;

    if (lstrcmpi(text, "center") == 0)
        return BUTTON_GRID_ALIGN_CENTER;

    if (lstrcmpi(text, "right") == 0)
        return BUTTON_GRID_ALIGN_RIGHT;

    if (lstrcmpi(text, "bottomLeft") == 0 || lstrcmpi(text, "bottom_left") == 0)
        return BUTTON_GRID_ALIGN_BOTTOM_LEFT;

    if (lstrcmpi(text, "bottom") == 0)
        return BUTTON_GRID_ALIGN_BOTTOM;

    if (lstrcmpi(text, "bottomRight") == 0 || lstrcmpi(text, "bottom_right") == 0)
        return BUTTON_GRID_ALIGN_BOTTOM_RIGHT;

    if (lstrcmpi(text, "xy") == 0 || lstrcmpi(text, "x/y") == 0)
        return BUTTON_GRID_ALIGN_XY;

    if (lstrcmpi(text, "percent") == 0 || lstrcmpi(text, "percentage") == 0)
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

    if (lstrcmpi(text, "toggle") == 0)
        return BUTTON_GRID_BUTTON_TOGGLE;

    if (lstrcmpi(text, "radio") == 0)
        return BUTTON_GRID_BUTTON_RADIO;

    if (lstrcmpi(text, "disabled") == 0)
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

    if (lstrcmpi(text, "default") == 0 ||
        lstrcmpi(text, "useDefault") == 0)
        return BUTTON_GRID_TEXT_USE_DEFAULT;

    if (lstrcmpi(text, "show") == 0 ||
        lstrcmpi(text, "on") == 0 ||
        lstrcmpi(text, "true") == 0)
        return BUTTON_GRID_TEXT_SHOW;

    if (lstrcmpi(text, "hide") == 0 ||
        lstrcmpi(text, "off") == 0 ||
        lstrcmpi(text, "false") == 0)
        return BUTTON_GRID_TEXT_HIDE;

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