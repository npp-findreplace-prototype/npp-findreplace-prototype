#include "button_grid_internal.h"

static int ButtonGrid_NormalizeContentAlignment(int alignment)
{
    if (alignment < BUTTON_GRID_ALIGN_TOP_LEFT ||
        alignment > BUTTON_GRID_ALIGN_PERCENT)
    {
        return BUTTON_GRID_DEFAULT_CONTENT_ALIGNMENT;
    }

    return alignment;
}

static int ButtonGrid_NormalizeBorderStyle(int borderStyle)
{
    if (borderStyle == BUTTON_GRID_BORDER_STYLE_NONE ||
        borderStyle == BUTTON_GRID_BORDER_STYLE_SIMPLE ||
        borderStyle == BUTTON_GRID_BORDER_STYLE_ETCHED ||
        borderStyle == BUTTON_GRID_BORDER_STYLE_ROUNDED ||
        borderStyle == BUTTON_GRID_BORDER_STYLE_ETCHED_ROUNDED ||
        borderStyle == BUTTON_GRID_BORDER_STYLE_CONTAINER ||
        borderStyle == BUTTON_GRID_BORDER_STYLE_SUNKEN ||
        borderStyle == BUTTON_GRID_BORDER_STYLE_RAISED ||
        borderStyle == BUTTON_GRID_BORDER_STYLE_DOUBLE)
    {
        return borderStyle;
    }

    return BUTTON_GRID_DEFAULT_BORDER_STYLE;
}

static int ButtonGrid_NormalizeButtonBackMode(int mode)
{
    if (mode == BUTTON_GRID_BUTTON_BACK_OPAQUE ||
        mode == BUTTON_GRID_BUTTON_BACK_TRANSPARENT)
    {
        return mode;
    }

    return BUTTON_GRID_DEFAULT_BUTTON_BACK_MODE;
}

static int ButtonGrid_ClampInt(
    int value,
    int minValue,
    int maxValue
)
{
    if (value < minValue)
        return minValue;

    if (value > maxValue)
        return maxValue;

    return value;
}

static int ButtonGrid_NormalizeBool(int value)
{
    return value ? 1 : 0;
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

    config->showText = ButtonGrid_NormalizeBool(config->showText);
    config->hidePartialButtons = ButtonGrid_NormalizeBool(config->hidePartialButtons);
    config->resizeInLayoutSteps = ButtonGrid_NormalizeBool(config->resizeInLayoutSteps);
    config->settingsWheelScrub = ButtonGrid_NormalizeBool(config->settingsWheelScrub);
    config->dpiScaleEnabled = ButtonGrid_NormalizeBool(config->dpiScaleEnabled);

    config->contentAlignment = ButtonGrid_NormalizeContentAlignment(
        config->contentAlignment
    );

    config->contentAlignPercentX = ButtonGrid_ClampInt(
        config->contentAlignPercentX,
        0,
        100
    );

    config->contentAlignPercentY = ButtonGrid_ClampInt(
        config->contentAlignPercentY,
        0,
        100
    );

    if (!config->themeName)
        config->themeName = BUTTON_GRID_DEFAULT_THEME_NAME;

    config->allowThemeSelection = ButtonGrid_NormalizeBool(config->allowThemeSelection);

    config->showBorder = ButtonGrid_NormalizeBool(config->showBorder);
    config->showBorderTitle = ButtonGrid_NormalizeBool(config->showBorderTitle);
    config->borderTitleTransparent = ButtonGrid_NormalizeBool(config->borderTitleTransparent);
    config->borderTitleAutoBackColor = ButtonGrid_NormalizeBool(config->borderTitleAutoBackColor);

    config->borderStyle = ButtonGrid_NormalizeBorderStyle(config->borderStyle);

    if (config->borderPadding < 0)
        config->borderPadding = 0;

    if (config->borderTitleHeight < 0)
        config->borderTitleHeight = 0;

    if (config->borderTitlePadding < 0)
        config->borderTitlePadding = 0;

    config->borderTitleFontSize = ButtonGrid_ClampInt(
        config->borderTitleFontSize,
        0,
        96
    );

    config->borderThickness = ButtonGrid_ClampInt(
        config->borderThickness,
        1,
        12
    );

    if (config->borderCornerRadius < 0)
        config->borderCornerRadius = 0;

    if (!config->borderTitle)
        config->borderTitle = "";

    config->showGearIcon = ButtonGrid_NormalizeBool(config->showGearIcon);

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

    config->buttonBackMode = ButtonGrid_NormalizeButtonBackMode(
        config->buttonBackMode
    );

    config->showButtonBorder = ButtonGrid_NormalizeBool(config->showButtonBorder);

    config->buttonBorderThickness = ButtonGrid_ClampInt(
        config->buttonBorderThickness,
        1,
        12
    );

    config->defaultState = ButtonGrid_NormalizeBool(config->defaultState);
    config->usePictures = ButtonGrid_NormalizeBool(config->usePictures);
    config->toggleOnClick = ButtonGrid_NormalizeBool(config->toggleOnClick);
    config->stretchPictures = ButtonGrid_NormalizeBool(config->stretchPictures);
}