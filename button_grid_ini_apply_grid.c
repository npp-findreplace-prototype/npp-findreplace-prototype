#include "button_grid_ini_internal.h"

void Ini_ReadGridSection(
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