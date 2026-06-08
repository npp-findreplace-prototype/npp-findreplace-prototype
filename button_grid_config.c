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

    config->contentAlignment = BUTTON_GRID_DEFAULT_CONTENT_ALIGNMENT;
    config->contentAlignX = BUTTON_GRID_DEFAULT_CONTENT_ALIGN_X;
    config->contentAlignY = BUTTON_GRID_DEFAULT_CONTENT_ALIGN_Y;
    config->contentAlignPercentX = BUTTON_GRID_DEFAULT_CONTENT_ALIGN_PERCENT_X;
    config->contentAlignPercentY = BUTTON_GRID_DEFAULT_CONTENT_ALIGN_PERCENT_Y;

    config->themeName = BUTTON_GRID_DEFAULT_THEME_NAME;
    config->allowThemeSelection = BUTTON_GRID_DEFAULT_ALLOW_THEME_SELECTION;

    config->showBorder = BUTTON_GRID_DEFAULT_SHOW_BORDER;
    config->showBorderTitle = BUTTON_GRID_DEFAULT_SHOW_BORDER_TITLE;
    config->borderTitle = BUTTON_GRID_DEFAULT_BORDER_TITLE;
    config->borderPadding = BUTTON_GRID_DEFAULT_BORDER_PADDING;
    config->borderTitleHeight = BUTTON_GRID_DEFAULT_BORDER_TITLE_HEIGHT;
    config->borderTitlePadding = BUTTON_GRID_DEFAULT_BORDER_TITLE_PADDING;
    config->borderTitleFontSize = BUTTON_GRID_DEFAULT_BORDER_TITLE_FONT_SIZE;
    config->borderTitleTransparent = BUTTON_GRID_DEFAULT_BORDER_TITLE_TRANSPARENT;
    config->borderTitleAutoBackColor = BUTTON_GRID_DEFAULT_BORDER_TITLE_AUTO_BACK_COLOR;
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

    config->buttonBackMode = BUTTON_GRID_DEFAULT_BUTTON_BACK_MODE;
    config->showButtonBorder = BUTTON_GRID_DEFAULT_SHOW_BUTTON_BORDER;
    config->buttonBorderThickness = BUTTON_GRID_DEFAULT_BUTTON_BORDER_THICKNESS;
    config->buttonBorderColor = BUTTON_GRID_DEFAULT_BUTTON_BORDER_COLOR;
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