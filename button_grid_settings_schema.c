#include "button_grid_settings_internal.h"

static const ButtonGridSettingOption g_boolOptions[] =
{
    { "Off", 0 },
    { "On", 1 },
    { NULL, 0 }
};

static const ButtonGridSettingOption g_layoutOptions[] =
{
    { "Horizontal", BUTTON_GRID_LAYOUT_HORIZONTAL },
    { "Vertical", BUTTON_GRID_LAYOUT_VERTICAL },
    { NULL, 0 }
};

static const ButtonGridSettingOption g_sizeModeOptions[] =
{
    { "Fixed", BUTTON_GRID_SIZE_FIXED },
    { "Match image size", BUTTON_GRID_SIZE_MATCH_IMAGE_SIZE },
    { "Aspect: horizontal", BUTTON_GRID_SIZE_MATCH_IMAGE_ASPECT_HORIZONTAL },
    { "Aspect: vertical", BUTTON_GRID_SIZE_MATCH_IMAGE_ASPECT_VERTICAL },
    { "Aspect: by layout", BUTTON_GRID_SIZE_MATCH_IMAGE_ASPECT_BY_LAYOUT },
    { NULL, 0 }
};

static const ButtonGridSettingOption g_alignmentOptions[] =
{
    { "Top left", BUTTON_GRID_ALIGN_TOP_LEFT },
    { "Top", BUTTON_GRID_ALIGN_TOP },
    { "Top right", BUTTON_GRID_ALIGN_TOP_RIGHT },
    { "Left", BUTTON_GRID_ALIGN_LEFT },
    { "Center", BUTTON_GRID_ALIGN_CENTER },
    { "Right", BUTTON_GRID_ALIGN_RIGHT },
    { "Bottom left", BUTTON_GRID_ALIGN_BOTTOM_LEFT },
    { "Bottom", BUTTON_GRID_ALIGN_BOTTOM },
    { "Bottom right", BUTTON_GRID_ALIGN_BOTTOM_RIGHT },
    { "X/Y offset", BUTTON_GRID_ALIGN_XY },
    { "Percent", BUTTON_GRID_ALIGN_PERCENT },
    { NULL, 0 }
};

static const ButtonGridSettingOption g_borderStyleOptions[] =
{
    { "None", BUTTON_GRID_BORDER_STYLE_NONE },
    { "Simple", BUTTON_GRID_BORDER_STYLE_SIMPLE },
    { "Etched", BUTTON_GRID_BORDER_STYLE_ETCHED },
    { "Rounded", BUTTON_GRID_BORDER_STYLE_ROUNDED },
    { NULL, 0 }
};

static const ButtonGridSettingOption g_gearCornerOptions[] =
{
    { "Top left", BUTTON_GRID_GEAR_CORNER_TOP_LEFT },
    { "Top right", BUTTON_GRID_GEAR_CORNER_TOP_RIGHT },
    { "Bottom left", BUTTON_GRID_GEAR_CORNER_BOTTOM_LEFT },
    { "Bottom right", BUTTON_GRID_GEAR_CORNER_BOTTOM_RIGHT },
    { NULL, 0 }
};

static const ButtonGridSettingDefinition g_settings[] =
{
    {
        "themeName",
        "Theme",
        BG_SETTING_THEME,
        offsetof(ButtonGrid, themeName),
        0,
        0,
        NULL
    },

    {
        "allowThemeSelection",
        "Allow theme selector",
        BG_SETTING_BOOL,
        offsetof(ButtonGrid, allowThemeSelection),
        0,
        1,
        g_boolOptions
    },

    {
        "dpiScaleEnabled",
        "DPI scale grid",
        BG_SETTING_BOOL,
        offsetof(ButtonGrid, dpiScaleEnabled),
        0,
        1,
        g_boolOptions
    },

    {
        "contentAlignment",
        "Grid alignment",
        BG_SETTING_ENUM,
        offsetof(ButtonGrid, contentAlignment),
        0,
        0,
        g_alignmentOptions
    },

    {
        "contentAlignX",
        "Grid align X",
        BG_SETTING_INT,
        offsetof(ButtonGrid, contentAlignX),
        -2000,
        2000,
        NULL
    },

    {
        "contentAlignY",
        "Grid align Y",
        BG_SETTING_INT,
        offsetof(ButtonGrid, contentAlignY),
        -2000,
        2000,
        NULL
    },

    {
        "contentAlignPercentX",
        "Grid align percent X",
        BG_SETTING_INT,
        offsetof(ButtonGrid, contentAlignPercentX),
        0,
        100,
        NULL
    },

    {
        "contentAlignPercentY",
        "Grid align percent Y",
        BG_SETTING_INT,
        offsetof(ButtonGrid, contentAlignPercentY),
        0,
        100,
        NULL
    },

    {
        "buttonWidth",
        "Button width",
        BG_SETTING_INT,
        offsetof(ButtonGrid, buttonWidth),
        10,
        400,
        NULL
    },

    {
        "buttonHeight",
        "Button height",
        BG_SETTING_INT,
        offsetof(ButtonGrid, buttonHeight),
        10,
        400,
        NULL
    },

    {
        "horizontalSpacing",
        "Horizontal spacing",
        BG_SETTING_INT,
        offsetof(ButtonGrid, horizontalSpacing),
        0,
        100,
        NULL
    },

    {
        "verticalSpacing",
        "Vertical spacing",
        BG_SETTING_INT,
        offsetof(ButtonGrid, verticalSpacing),
        0,
        100,
        NULL
    },

    {
        "layout",
        "Layout",
        BG_SETTING_ENUM,
        offsetof(ButtonGrid, layout),
        0,
        0,
        g_layoutOptions
    },

    {
        "sizeMode",
        "Size mode",
        BG_SETTING_ENUM,
        offsetof(ButtonGrid, sizeMode),
        0,
        0,
        g_sizeModeOptions
    },

    {
        "showText",
        "Show button text",
        BG_SETTING_BOOL,
        offsetof(ButtonGrid, showText),
        0,
        1,
        g_boolOptions
    },

    {
        "hidePartialButtons",
        "Hide partial buttons",
        BG_SETTING_BOOL,
        offsetof(ButtonGrid, hidePartialButtons),
        0,
        1,
        g_boolOptions
    },

    {
        "resizeInLayoutSteps",
        "Resize in layout steps",
        BG_SETTING_BOOL,
        offsetof(ButtonGrid, resizeInLayoutSteps),
        0,
        1,
        g_boolOptions
    },

    {
        "settingsWheelScrub",
        "Wheel scrub values",
        BG_SETTING_BOOL,
        offsetof(ButtonGrid, settingsWheelScrub),
        0,
        1,
        g_boolOptions
    },

    {
        "showBorder",
        "Show border",
        BG_SETTING_BOOL,
        offsetof(ButtonGrid, showBorder),
        0,
        1,
        g_boolOptions
    },

    {
        "borderTitle",
        "Border title",
        BG_SETTING_TEXT,
        offsetof(ButtonGrid, borderTitle),
        0,
        0,
        NULL
    },

    {
        "borderPadding",
        "Border padding",
        BG_SETTING_INT,
        offsetof(ButtonGrid, borderPadding),
        0,
        100,
        NULL
    },

    {
        "borderTitleHeight",
        "Border title height",
        BG_SETTING_INT,
        offsetof(ButtonGrid, borderTitleHeight),
        0,
        80,
        NULL
    },

    {
        "borderStyle",
        "Border style",
        BG_SETTING_ENUM,
        offsetof(ButtonGrid, borderStyle),
        0,
        0,
        g_borderStyleOptions
    },

    {
        "borderThickness",
        "Border thickness",
        BG_SETTING_INT,
        offsetof(ButtonGrid, borderThickness),
        1,
        8,
        NULL
    },

    {
        "borderCornerRadius",
        "Border corner radius",
        BG_SETTING_INT,
        offsetof(ButtonGrid, borderCornerRadius),
        0,
        50,
        NULL
    },

    {
        "borderColor",
        "Border color",
        BG_SETTING_COLOR,
        offsetof(ButtonGrid, borderColor),
        0,
        0,
        NULL
    },

    {
        "borderLightColor",
        "Border light color",
        BG_SETTING_COLOR,
        offsetof(ButtonGrid, borderLightColor),
        0,
        0,
        NULL
    },

    {
        "borderShadowColor",
        "Border shadow color",
        BG_SETTING_COLOR,
        offsetof(ButtonGrid, borderShadowColor),
        0,
        0,
        NULL
    },

    {
        "borderTitleColor",
        "Border title color",
        BG_SETTING_COLOR,
        offsetof(ButtonGrid, borderTitleColor),
        0,
        0,
        NULL
    },

    {
        "borderTitleBackColor",
        "Border title back color",
        BG_SETTING_COLOR,
        offsetof(ButtonGrid, borderTitleBackColor),
        0,
        0,
        NULL
    },

    {
        "showGearIcon",
        "Show gear icon",
        BG_SETTING_BOOL,
        offsetof(ButtonGrid, showGearIcon),
        0,
        1,
        g_boolOptions
    },

    {
        "gearCorner",
        "Gear corner",
        BG_SETTING_ENUM,
        offsetof(ButtonGrid, gearCorner),
        0,
        0,
        g_gearCornerOptions
    },

    {
        "gearSize",
        "Gear size",
        BG_SETTING_INT,
        offsetof(ButtonGrid, gearSize),
        10,
        80,
        NULL
    },

    {
        "gearMargin",
        "Gear margin",
        BG_SETTING_INT,
        offsetof(ButtonGrid, gearMargin),
        0,
        80,
        NULL
    },

    {
        "gearColor",
        "Gear color",
        BG_SETTING_COLOR,
        offsetof(ButtonGrid, gearColor),
        0,
        0,
        NULL
    },

    {
        "gearBackColor",
        "Gear background color",
        BG_SETTING_COLOR,
        offsetof(ButtonGrid, gearBackColor),
        0,
        0,
        NULL
    },

    {
        "gearBorderColor",
        "Gear border color",
        BG_SETTING_COLOR,
        offsetof(ButtonGrid, gearBorderColor),
        0,
        0,
        NULL
    },

    {
        "backColor",
        "Button back color",
        BG_SETTING_COLOR,
        offsetof(ButtonGrid, backColor),
        0,
        0,
        NULL
    },

    {
        "foreColor",
        "Button text color",
        BG_SETTING_COLOR,
        offsetof(ButtonGrid, foreColor),
        0,
        0,
        NULL
    },

    {
        "generatedOffPictureColor",
        "Fallback OFF color",
        BG_SETTING_COLOR,
        offsetof(ButtonGrid, generatedOffPictureColor),
        0,
        0,
        NULL
    },

    {
        "generatedOnPictureColor",
        "Fallback ON color",
        BG_SETTING_COLOR,
        offsetof(ButtonGrid, generatedOnPictureColor),
        0,
        0,
        NULL
    },

    {
        "generatedErrorPictureColor",
        "Fallback error color",
        BG_SETTING_COLOR,
        offsetof(ButtonGrid, generatedErrorPictureColor),
        0,
        0,
        NULL
    }
};

int ButtonGrid_SettingsGetCount(void)
{
    return (int)(sizeof(g_settings) / sizeof(g_settings[0]));
}

const ButtonGridSettingDefinition *ButtonGrid_SettingsGetDefinition(int index)
{
    if (index < 0 || index >= ButtonGrid_SettingsGetCount())
        return NULL;

    return &g_settings[index];
}