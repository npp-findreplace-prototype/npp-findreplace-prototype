#include "grid_tester_window_internal.h"

typedef struct SearchGridButtonDefinition
{
    const char *name;
    const char *action;
    const char *text;
    const char *tooltip;
    const char *iconBaseName;

    int behavior;
    int radioGroup;
    int defaultState;

    int widthOverride;
    int heightOverride;
    int sizeModeOverride;

    int showTextOverride;
} SearchGridButtonDefinition;

#define SEARCH_GRID_BUTTON(name, action, text, tooltip, iconBaseName, behavior, radioGroup, defaultState, widthOverride, heightOverride, sizeModeOverride, showTextOverride) \
    { name, action, text, tooltip, iconBaseName, behavior, radioGroup, defaultState, widthOverride, heightOverride, sizeModeOverride, showTextOverride }

static const SearchGridButtonDefinition SEARCH_GRID_BUTTONS[] =
{
    SEARCH_GRID_BUTTON("LiteralSearch",        NULL, "Literal",   "Literal Search",         NULL, BUTTON_GRID_BUTTON_RADIO,  SEARCH_MODE_RADIO_GROUP, 1, 0, 0, BUTTON_GRID_SIZE_USE_DEFAULT, BUTTON_GRID_TEXT_USE_DEFAULT),
    SEARCH_GRID_BUTTON("EscapedLiteralSearch", NULL, "Escaped",   "Escaped Literal Search", NULL, BUTTON_GRID_BUTTON_RADIO,  SEARCH_MODE_RADIO_GROUP, 0, 0, 0, BUTTON_GRID_SIZE_USE_DEFAULT, BUTTON_GRID_TEXT_USE_DEFAULT),
    SEARCH_GRID_BUTTON("RegExSearch",          NULL, "Regex",     "Regex Search",           NULL, BUTTON_GRID_BUTTON_RADIO,  SEARCH_MODE_RADIO_GROUP, 0, 0, 0, BUTTON_GRID_SIZE_USE_DEFAULT, BUTTON_GRID_TEXT_USE_DEFAULT),
    SEARCH_GRID_BUTTON("SemanticSearch",       NULL, "Semantic",  "Semantic Search",        NULL, BUTTON_GRID_BUTTON_RADIO,  SEARCH_MODE_RADIO_GROUP, 0, 0, 0, BUTTON_GRID_SIZE_USE_DEFAULT, BUTTON_GRID_TEXT_USE_DEFAULT),

    SEARCH_GRID_BUTTON("CaseSensitive",        NULL, "Case",      "Case Sensitive",         NULL, BUTTON_GRID_BUTTON_TOGGLE, 0, 0, 0, 0, BUTTON_GRID_SIZE_USE_DEFAULT, BUTTON_GRID_TEXT_USE_DEFAULT),
    SEARCH_GRID_BUTTON("DiacriticSensitive",   NULL, "Diacritic", "Diacritic Sensitive",    NULL, BUTTON_GRID_BUTTON_TOGGLE, 0, 0, 0, 0, BUTTON_GRID_SIZE_USE_DEFAULT, BUTTON_GRID_TEXT_USE_DEFAULT),
    SEARCH_GRID_BUTTON("DotIncludesNewline",   NULL, "Dot NL",    "Dot Includes Newline",   NULL, BUTTON_GRID_BUTTON_TOGGLE, 0, 0, 0, 0, BUTTON_GRID_SIZE_USE_DEFAULT, BUTTON_GRID_TEXT_USE_DEFAULT),
    SEARCH_GRID_BUTTON("FuzzyLogicSearch",     NULL, "Fuzzy",     "Fuzzy Logic Search",     NULL, BUTTON_GRID_BUTTON_TOGGLE, 0, 0, 0, 0, BUTTON_GRID_SIZE_USE_DEFAULT, BUTTON_GRID_TEXT_USE_DEFAULT),

    SEARCH_GRID_BUTTON("WrapAround",           NULL, "Wrap",      "Wrap Around",            NULL, BUTTON_GRID_BUTTON_TOGGLE, 0, 0, 0, 0, BUTTON_GRID_SIZE_USE_DEFAULT, BUTTON_GRID_TEXT_USE_DEFAULT),
    SEARCH_GRID_BUTTON("WholeWord",            NULL, "Word",      "Whole Word",             NULL, BUTTON_GRID_BUTTON_TOGGLE, 0, 0, 0, 0, BUTTON_GRID_SIZE_USE_DEFAULT, BUTTON_GRID_TEXT_USE_DEFAULT),
    SEARCH_GRID_BUTTON("BooleanSearch",        NULL, "Boolean",   "Boolean Search",         NULL, BUTTON_GRID_BUTTON_TOGGLE, 0, 0, 0, 0, BUTTON_GRID_SIZE_USE_DEFAULT, BUTTON_GRID_TEXT_USE_DEFAULT),
    SEARCH_GRID_BUTTON("Settings",             NULL, "Settings",  "Settings",               NULL, BUTTON_GRID_BUTTON_TOGGLE, 0, 0, 0, 0, BUTTON_GRID_SIZE_USE_DEFAULT, BUTTON_GRID_TEXT_USE_DEFAULT)
};

#define SEARCH_GRID_BUTTON_COUNT (sizeof(SEARCH_GRID_BUTTONS) / sizeof(SEARCH_GRID_BUTTONS[0]))

static ButtonGridItemConfig g_searchGridItems[SEARCH_GRID_BUTTON_COUNT];

static void GridTester_PrepareSearchGridItems(void)
{
    int i;

    ZeroMemory(g_searchGridItems, sizeof(g_searchGridItems));

    for (i = 0; i < (int)SEARCH_GRID_BUTTON_COUNT; i++)
    {
        const char *iconBaseName;

        iconBaseName = SEARCH_GRID_BUTTONS[i].iconBaseName;

        if (!iconBaseName)
            iconBaseName = SEARCH_GRID_BUTTONS[i].name;

        g_searchGridItems[i].name = SEARCH_GRID_BUTTONS[i].name;
        g_searchGridItems[i].action = SEARCH_GRID_BUTTONS[i].action;
        g_searchGridItems[i].text = SEARCH_GRID_BUTTONS[i].text;
        g_searchGridItems[i].tooltip = SEARCH_GRID_BUTTONS[i].tooltip;
        g_searchGridItems[i].iconBaseName = iconBaseName;

        g_searchGridItems[i].behavior = SEARCH_GRID_BUTTONS[i].behavior;
        g_searchGridItems[i].radioGroup = SEARCH_GRID_BUTTONS[i].radioGroup;
        g_searchGridItems[i].defaultState = SEARCH_GRID_BUTTONS[i].defaultState;

        g_searchGridItems[i].widthOverride = SEARCH_GRID_BUTTONS[i].widthOverride;
        g_searchGridItems[i].heightOverride = SEARCH_GRID_BUTTONS[i].heightOverride;
        g_searchGridItems[i].sizeModeOverride = SEARCH_GRID_BUTTONS[i].sizeModeOverride;
        g_searchGridItems[i].showTextOverride = SEARCH_GRID_BUTTONS[i].showTextOverride;

        g_searchGridItems[i].pictureOff = NULL;
        g_searchGridItems[i].pictureOn = NULL;

        g_searchGridItems[i].ownsPictureOff = 0;
        g_searchGridItems[i].ownsPictureOn = 0;

        g_searchGridItems[i].pictureOffLoadFailed = 0;
        g_searchGridItems[i].pictureOnLoadFailed = 0;
    }
}

static void GridTester_ConfigureButtonGrid(ButtonGridConfig *config)
{
    ButtonGrid_GetDefaultConfig(config);

    GridTester_PrepareSearchGridItems();

    config->buttonCount = (int)SEARCH_GRID_BUTTON_COUNT;
    config->items = g_searchGridItems;

    config->buttonWidth = 90;
    config->buttonHeight = 90;

    config->horizontalSpacing = 10;
    config->verticalSpacing = 10;

    config->layout = BUTTON_GRID_LAYOUT_HORIZONTAL;
    config->sizeMode = BUTTON_GRID_SIZE_MATCH_IMAGE_ASPECT_BY_LAYOUT;

    config->showText = 0;
    config->hidePartialButtons = 1;
    config->resizeInLayoutSteps = 0;
    config->settingsWheelScrub = 0;

    config->dpiScaleEnabled = 1;

    config->contentAlignment = BUTTON_GRID_ALIGN_TOP_LEFT;
    config->contentAlignX = 0;
    config->contentAlignY = 0;
    config->contentAlignPercentX = 50;
    config->contentAlignPercentY = 50;

    config->themeName = BUTTON_GRID_DEFAULT_THEME_NAME;
    config->allowThemeSelection = 1;

    config->showBorder = 1;
    config->showBorderTitle = 1;
    config->borderTitle = "Search Options";
    config->borderPadding = 12;
    config->borderTitleHeight = 22;
    config->borderTitlePadding = 6;
    config->borderTitleFontSize = 0;
    config->borderTitleTransparent = 1;
    config->borderTitleAutoBackColor = 1;
    config->borderStyle = BUTTON_GRID_BORDER_STYLE_ROUNDED;
    config->borderThickness = 1;
    config->borderCornerRadius = 10;
    config->borderColor = RGB(96, 96, 96);
    config->borderLightColor = RGB(255, 255, 255);
    config->borderShadowColor = RGB(128, 128, 128);
    config->borderTitleColor = RGB(0, 0, 0);
    config->borderTitleBackColor = RGB(240, 240, 240);

    config->showGearIcon = 1;
    config->gearCorner = BUTTON_GRID_GEAR_CORNER_TOP_RIGHT;
    config->gearSize = 24;
    config->gearMargin = 8;
    config->gearColor = RGB(60, 60, 60);
    config->gearBackColor = RGB(245, 245, 245);
    config->gearBorderColor = RGB(120, 120, 120);

    config->buttonBackMode = BUTTON_GRID_BUTTON_BACK_TRANSPARENT;
    config->showButtonBorder = 0;
    config->buttonBorderThickness = 1;
    config->buttonBorderColor = RGB(0, 0, 0);

    config->backColor = RGB(192, 192, 192);
    config->foreColor = RGB(0, 0, 0);

    config->usePictures = 1;
    config->toggleOnClick = 1;
    config->defaultState = 0;
    config->stretchPictures = 1;

    config->generatedOffPictureColor = RGB(150, 150, 150);
    config->generatedOnPictureColor = RGB(80, 190, 80);
    config->generatedErrorPictureColor = RGB(190, 100, 100);
}

static void GridTester_OnGridButtonClicked(const char *actionName)
{
    int isOn;

    if (!g_gridTesterButtonGrid || !actionName)
        return;

    isOn = ButtonGrid_GetButtonStateByAction(
        g_gridTesterButtonGrid,
        actionName
    );

    printf(
        "Grid tester clicked: %s is now %s\n",
        actionName,
        isOn ? "ON" : "OFF"
    );
}

int GridTester_CreateSearchGrid(HWND hwnd)
{
    ButtonGridConfig gridConfig;

    GridTester_ConfigureButtonGrid(&gridConfig);

    g_gridTesterSquareSize = gridConfig.buttonWidth;

    g_gridTesterButtonGrid = ButtonGrid_CreateEx(
        hwnd,
        g_gridTesterHInstance,
        GRID_MARGIN,
        GRID_MARGIN,
        400,
        300,
        &gridConfig,
        GridTester_OnGridButtonClicked
    );

    if (!g_gridTesterButtonGrid)
        return 0;

    GridTester_LayoutWindow(hwnd);
    GridTester_SetWindowTitle(hwnd);

    return 1;
}