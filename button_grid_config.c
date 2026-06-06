#include "button_grid_internal.h"

void ButtonGrid_GetDefaultConfig(ButtonGridConfig *config)
{
    if (!config)
        return;

    config->buttonCount = BUTTON_GRID_DEFAULT_BUTTON_COUNT;
    config->items = NULL;

    config->buttonWidth = BUTTON_GRID_DEFAULT_BUTTON_WIDTH;
    config->buttonHeight = BUTTON_GRID_DEFAULT_BUTTON_HEIGHT;

    config->horizontalSpacing = BUTTON_GRID_DEFAULT_HORIZONTAL_SPACING;
    config->verticalSpacing = BUTTON_GRID_DEFAULT_VERTICAL_SPACING;

    config->layout = BUTTON_GRID_DEFAULT_LAYOUT;
    config->sizeMode = BUTTON_GRID_DEFAULT_SIZE_MODE;

    config->showText = BUTTON_GRID_DEFAULT_SHOW_TEXT;
    config->hidePartialButtons = BUTTON_GRID_DEFAULT_HIDE_PARTIAL_BUTTONS;
    config->resizeInLayoutSteps = BUTTON_GRID_DEFAULT_RESIZE_IN_LAYOUT_STEPS;
    config->settingsWheelScrub = BUTTON_GRID_DEFAULT_SETTINGS_WHEEL_SCRUB;

    config->dpiScaleEnabled = BUTTON_GRID_DEFAULT_DPI_SCALE_ENABLED;

    config->themeName = BUTTON_GRID_DEFAULT_THEME_NAME;
    config->allowThemeSelection = BUTTON_GRID_DEFAULT_ALLOW_THEME_SELECTION;

    config->showBorder = BUTTON_GRID_DEFAULT_SHOW_BORDER;
    config->borderTitle = BUTTON_GRID_DEFAULT_BORDER_TITLE;
    config->borderPadding = BUTTON_GRID_DEFAULT_BORDER_PADDING;
    config->borderTitleHeight = BUTTON_GRID_DEFAULT_BORDER_TITLE_HEIGHT;
    config->borderStyle = BUTTON_GRID_DEFAULT_BORDER_STYLE;
    config->borderThickness = BUTTON_GRID_DEFAULT_BORDER_THICKNESS;
    config->borderCornerRadius = BUTTON_GRID_DEFAULT_BORDER_CORNER_RADIUS;
    config->borderColor = BUTTON_GRID_DEFAULT_BORDER_COLOR;
    config->borderLightColor = BUTTON_GRID_DEFAULT_BORDER_LIGHT_COLOR;
    config->borderShadowColor = BUTTON_GRID_DEFAULT_BORDER_SHADOW_COLOR;
    config->borderTitleColor = BUTTON_GRID_DEFAULT_BORDER_TITLE_COLOR;
    config->borderTitleBackColor = BUTTON_GRID_DEFAULT_BORDER_TITLE_BACK_COLOR;

    config->showGearIcon = BUTTON_GRID_DEFAULT_SHOW_GEAR_ICON;
    config->gearCorner = BUTTON_GRID_DEFAULT_GEAR_CORNER;
    config->gearSize = BUTTON_GRID_DEFAULT_GEAR_SIZE;
    config->gearMargin = BUTTON_GRID_DEFAULT_GEAR_MARGIN;
    config->gearColor = BUTTON_GRID_DEFAULT_GEAR_COLOR;
    config->gearBackColor = BUTTON_GRID_DEFAULT_GEAR_BACK_COLOR;
    config->gearBorderColor = BUTTON_GRID_DEFAULT_GEAR_BORDER_COLOR;

    config->idBase = BUTTON_GRID_DEFAULT_ID_BASE;
    config->firstIndex = BUTTON_GRID_DEFAULT_FIRST_INDEX;

    config->namePrefix = BUTTON_GRID_DEFAULT_NAME_PREFIX;
    config->actionPrefix = BUTTON_GRID_DEFAULT_ACTION_PREFIX;
    config->textFormat = BUTTON_GRID_DEFAULT_TEXT_FORMAT;
    config->clickIdentifierFormat = BUTTON_GRID_DEFAULT_CLICK_IDENTIFIER_FORMAT;

    config->backColor = BUTTON_GRID_DEFAULT_BACK_COLOR;
    config->foreColor = BUTTON_GRID_DEFAULT_FORE_COLOR;

    config->usePictures = BUTTON_GRID_DEFAULT_USE_PICTURES;
    config->toggleOnClick = BUTTON_GRID_DEFAULT_TOGGLE_ON_CLICK;
    config->defaultState = BUTTON_GRID_DEFAULT_STATE;
    config->stretchPictures = BUTTON_GRID_DEFAULT_STRETCH_PICTURES;

    config->generatedOffPictureColor = BUTTON_GRID_DEFAULT_OFF_PICTURE_COLOR;
    config->generatedOnPictureColor = BUTTON_GRID_DEFAULT_ON_PICTURE_COLOR;
    config->generatedErrorPictureColor = BUTTON_GRID_DEFAULT_ERROR_PICTURE_COLOR;
}

void ButtonGrid_NormalizeConfig(ButtonGridConfig *config)
{
    if (!config)
        return;

    if (config->buttonCount < 1)
        config->buttonCount = 1;

    if (config->buttonWidth < 1)
        config->buttonWidth = 1;

    if (config->buttonHeight < 1)
        config->buttonHeight = 1;

    if (config->horizontalSpacing < 0)
        config->horizontalSpacing = 0;

    if (config->verticalSpacing < 0)
        config->verticalSpacing = 0;

    if (config->layout != BUTTON_GRID_LAYOUT_HORIZONTAL &&
        config->layout != BUTTON_GRID_LAYOUT_VERTICAL)
    {
        config->layout = BUTTON_GRID_LAYOUT_HORIZONTAL;
    }

    config->sizeMode = ButtonGrid_NormalizeSizeMode(config->sizeMode);

    config->showText = config->showText ? 1 : 0;
    config->hidePartialButtons = config->hidePartialButtons ? 1 : 0;
    config->resizeInLayoutSteps = config->resizeInLayoutSteps ? 1 : 0;
    config->settingsWheelScrub = config->settingsWheelScrub ? 1 : 0;

    config->dpiScaleEnabled = config->dpiScaleEnabled ? 1 : 0;

    if (!config->themeName)
        config->themeName = BUTTON_GRID_DEFAULT_THEME_NAME;

    config->allowThemeSelection = config->allowThemeSelection ? 1 : 0;

    config->showBorder = config->showBorder ? 1 : 0;

    if (config->borderStyle != BUTTON_GRID_BORDER_STYLE_NONE &&
        config->borderStyle != BUTTON_GRID_BORDER_STYLE_SIMPLE &&
        config->borderStyle != BUTTON_GRID_BORDER_STYLE_ETCHED &&
        config->borderStyle != BUTTON_GRID_BORDER_STYLE_ROUNDED)
    {
        config->borderStyle = BUTTON_GRID_DEFAULT_BORDER_STYLE;
    }

    if (config->borderPadding < 0)
        config->borderPadding = 0;

    if (config->borderTitleHeight < 0)
        config->borderTitleHeight = 0;

    if (config->borderThickness < 1)
        config->borderThickness = 1;

    if (config->borderThickness > 8)
        config->borderThickness = 8;

    if (config->borderCornerRadius < 0)
        config->borderCornerRadius = 0;

    if (!config->borderTitle)
        config->borderTitle = "";

    config->showGearIcon = config->showGearIcon ? 1 : 0;

    if (config->gearCorner < BUTTON_GRID_GEAR_CORNER_TOP_LEFT ||
        config->gearCorner > BUTTON_GRID_GEAR_CORNER_BOTTOM_RIGHT)
    {
        config->gearCorner = BUTTON_GRID_DEFAULT_GEAR_CORNER;
    }

    if (config->gearSize < 8)
        config->gearSize = 8;

    if (config->gearMargin < 0)
        config->gearMargin = 0;

    if (!config->namePrefix)
        config->namePrefix = BUTTON_GRID_DEFAULT_NAME_PREFIX;

    if (!config->actionPrefix)
        config->actionPrefix = BUTTON_GRID_DEFAULT_ACTION_PREFIX;

    if (!config->textFormat)
        config->textFormat = BUTTON_GRID_DEFAULT_TEXT_FORMAT;

    if (!config->clickIdentifierFormat)
        config->clickIdentifierFormat = BUTTON_GRID_DEFAULT_CLICK_IDENTIFIER_FORMAT;

    config->defaultState = config->defaultState ? 1 : 0;
    config->usePictures = config->usePictures ? 1 : 0;
    config->toggleOnClick = config->toggleOnClick ? 1 : 0;
    config->stretchPictures = config->stretchPictures ? 1 : 0;
}

void ButtonGrid_ApplyConfig(ButtonGrid *grid, const ButtonGridConfig *config)
{
    if (!grid || !config)
        return;

    grid->buttonCount = config->buttonCount;

    grid->configuredItems = config->items;
    grid->configuredItemCount = config->buttonCount;

    grid->buttonWidth = config->buttonWidth;
    grid->buttonHeight = config->buttonHeight;

    grid->horizontalSpacing = config->horizontalSpacing;
    grid->verticalSpacing = config->verticalSpacing;

    grid->layout = config->layout;
    grid->sizeMode = config->sizeMode;

    grid->showText = config->showText;
    grid->hidePartialButtons = config->hidePartialButtons;
    grid->resizeInLayoutSteps = config->resizeInLayoutSteps;
    grid->settingsWheelScrub = config->settingsWheelScrub;

    grid->dpiScaleEnabled = config->dpiScaleEnabled;
    ButtonGrid_UpdateDpi(grid);

    ButtonGrid_CopyText(
        grid->themeName,
        BUTTON_GRID_THEME_NAME_SIZE,
        config->themeName
    );

    grid->allowThemeSelection = config->allowThemeSelection;

    grid->showBorder = config->showBorder;
    ButtonGrid_CopyText(grid->borderTitle, BUTTON_GRID_TITLE_SIZE, config->borderTitle);
    grid->borderPadding = config->borderPadding;
    grid->borderTitleHeight = config->borderTitleHeight;
    grid->borderStyle = config->borderStyle;
    grid->borderThickness = config->borderThickness;
    grid->borderCornerRadius = config->borderCornerRadius;
    grid->borderColor = config->borderColor;
    grid->borderLightColor = config->borderLightColor;
    grid->borderShadowColor = config->borderShadowColor;
    grid->borderTitleColor = config->borderTitleColor;
    grid->borderTitleBackColor = config->borderTitleBackColor;

    grid->showGearIcon = config->showGearIcon;
    grid->gearCorner = config->gearCorner;
    grid->gearSize = config->gearSize;
    grid->gearMargin = config->gearMargin;
    grid->gearColor = config->gearColor;
    grid->gearBackColor = config->gearBackColor;
    grid->gearBorderColor = config->gearBorderColor;

    grid->settingsPageHwnd = NULL;
    grid->settingsPageVisible = 0;

    grid->idBase = config->idBase;
    grid->firstIndex = config->firstIndex;

    ButtonGrid_CopyText(grid->namePrefix, BUTTON_GRID_FORMAT_SIZE, config->namePrefix);
    ButtonGrid_CopyText(grid->actionPrefix, BUTTON_GRID_FORMAT_SIZE, config->actionPrefix);
    ButtonGrid_CopyText(grid->textFormat, BUTTON_GRID_FORMAT_SIZE, config->textFormat);
    ButtonGrid_CopyText(grid->clickIdentifierFormat, BUTTON_GRID_FORMAT_SIZE, config->clickIdentifierFormat);

    grid->backColor = config->backColor;
    grid->foreColor = config->foreColor;

    grid->usePictures = config->usePictures;
    grid->toggleOnClick = config->toggleOnClick;
    grid->defaultState = config->defaultState;
    grid->stretchPictures = config->stretchPictures;

    grid->generatedOffPictureColor = config->generatedOffPictureColor;
    grid->generatedOnPictureColor = config->generatedOnPictureColor;
    grid->generatedErrorPictureColor = config->generatedErrorPictureColor;

    grid->buttonBrush = CreateSolidBrush(grid->backColor);
}