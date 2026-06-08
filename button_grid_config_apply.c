#include "button_grid_internal.h"

static void ButtonGrid_ApplyButtonSizingConfig(
    ButtonGrid *grid,
    const ButtonGridConfig *config
)
{
    grid->buttonCount = config->buttonCount;

    grid->configuredItems = config->items;
    grid->configuredItemCount = config->buttonCount;

    grid->buttonWidth = config->buttonWidth;
    grid->buttonHeight = config->buttonHeight;

    grid->horizontalSpacing = config->horizontalSpacing;
    grid->verticalSpacing = config->verticalSpacing;

    grid->layout = config->layout;
    grid->sizeMode = config->sizeMode;
}

static void ButtonGrid_ApplyLayoutConfig(
    ButtonGrid *grid,
    const ButtonGridConfig *config
)
{
    grid->showText = config->showText;
    grid->hidePartialButtons = config->hidePartialButtons;
    grid->resizeInLayoutSteps = config->resizeInLayoutSteps;
    grid->settingsWheelScrub = config->settingsWheelScrub;

    grid->dpiScaleEnabled = config->dpiScaleEnabled;
    ButtonGrid_UpdateDpi(grid);

    grid->contentAlignment = config->contentAlignment;
    grid->contentAlignX = config->contentAlignX;
    grid->contentAlignY = config->contentAlignY;
    grid->contentAlignPercentX = config->contentAlignPercentX;
    grid->contentAlignPercentY = config->contentAlignPercentY;
}

static void ButtonGrid_ApplyThemeConfig(
    ButtonGrid *grid,
    const ButtonGridConfig *config
)
{
    ButtonGrid_CopyText(
        grid->themeName,
        BUTTON_GRID_THEME_NAME_SIZE,
        config->themeName
    );

    grid->allowThemeSelection = config->allowThemeSelection;
}

static void ButtonGrid_ApplyBorderConfig(
    ButtonGrid *grid,
    const ButtonGridConfig *config
)
{
    grid->showBorder = config->showBorder;
    grid->showBorderTitle = config->showBorderTitle;

    ButtonGrid_CopyText(
        grid->borderTitle,
        BUTTON_GRID_TITLE_SIZE,
        config->borderTitle
    );

    grid->borderPadding = config->borderPadding;
    grid->borderTitleHeight = config->borderTitleHeight;
    grid->borderTitlePadding = config->borderTitlePadding;
    grid->borderTitleFontSize = config->borderTitleFontSize;
    grid->borderTitleTransparent = config->borderTitleTransparent;
    grid->borderTitleAutoBackColor = config->borderTitleAutoBackColor;
    grid->borderStyle = config->borderStyle;
    grid->borderThickness = config->borderThickness;
    grid->borderCornerRadius = config->borderCornerRadius;
    grid->borderColor = config->borderColor;
    grid->borderLightColor = config->borderLightColor;
    grid->borderShadowColor = config->borderShadowColor;
    grid->borderTitleColor = config->borderTitleColor;
    grid->borderTitleBackColor = config->borderTitleBackColor;
}

static void ButtonGrid_ApplyGearConfig(
    ButtonGrid *grid,
    const ButtonGridConfig *config
)
{
    grid->showGearIcon = config->showGearIcon;
    grid->gearCorner = config->gearCorner;
    grid->gearSize = config->gearSize;
    grid->gearMargin = config->gearMargin;
    grid->gearColor = config->gearColor;
    grid->gearBackColor = config->gearBackColor;
    grid->gearBorderColor = config->gearBorderColor;
}

static void ButtonGrid_ApplyIdAndFormatConfig(
    ButtonGrid *grid,
    const ButtonGridConfig *config
)
{
    grid->idBase = config->idBase;
    grid->firstIndex = config->firstIndex;

    ButtonGrid_CopyText(
        grid->namePrefix,
        BUTTON_GRID_FORMAT_SIZE,
        config->namePrefix
    );

    ButtonGrid_CopyText(
        grid->actionPrefix,
        BUTTON_GRID_FORMAT_SIZE,
        config->actionPrefix
    );

    ButtonGrid_CopyText(
        grid->textFormat,
        BUTTON_GRID_FORMAT_SIZE,
        config->textFormat
    );

    ButtonGrid_CopyText(
        grid->clickIdentifierFormat,
        BUTTON_GRID_FORMAT_SIZE,
        config->clickIdentifierFormat
    );
}

static void ButtonGrid_ApplyButtonAppearanceConfig(
    ButtonGrid *grid,
    const ButtonGridConfig *config
)
{
    grid->buttonBackMode = config->buttonBackMode;
    grid->showButtonBorder = config->showButtonBorder;
    grid->buttonBorderThickness = config->buttonBorderThickness;
    grid->buttonBorderColor = config->buttonBorderColor;
    grid->backColor = config->backColor;
    grid->foreColor = config->foreColor;
}

static void ButtonGrid_ApplyPictureConfig(
    ButtonGrid *grid,
    const ButtonGridConfig *config
)
{
    grid->usePictures = config->usePictures;
    grid->toggleOnClick = config->toggleOnClick;
    grid->defaultState = config->defaultState;
    grid->stretchPictures = config->stretchPictures;

    grid->generatedOffPictureColor = config->generatedOffPictureColor;
    grid->generatedOnPictureColor = config->generatedOnPictureColor;
    grid->generatedErrorPictureColor = config->generatedErrorPictureColor;
}

static void ButtonGrid_RecreateButtonBrush(ButtonGrid *grid)
{
    if (!grid)
        return;

    if (grid->buttonBrush)
        DeleteObject(grid->buttonBrush);

    grid->buttonBrush = CreateSolidBrush(grid->backColor);
}

void ButtonGrid_ApplyConfig(ButtonGrid *grid, const ButtonGridConfig *config)
{
    if (!grid || !config)
        return;

    ButtonGrid_ApplyButtonSizingConfig(grid, config);
    ButtonGrid_ApplyLayoutConfig(grid, config);
    ButtonGrid_ApplyThemeConfig(grid, config);
    ButtonGrid_ApplyBorderConfig(grid, config);
    ButtonGrid_ApplyGearConfig(grid, config);

    grid->settingsPageHwnd = NULL;
    grid->settingsPageVisible = 0;

    ButtonGrid_ApplyIdAndFormatConfig(grid, config);
    ButtonGrid_ApplyButtonAppearanceConfig(grid, config);
    ButtonGrid_ApplyPictureConfig(grid, config);

    ButtonGrid_RecreateButtonBrush(grid);
}