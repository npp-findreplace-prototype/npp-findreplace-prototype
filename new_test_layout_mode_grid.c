#include "new_test_layout_window_internal.h"

static void NewTestLayout_ModeGridClicked(const char *actionName)
{
    if (actionName && lstrcmpi(actionName, "Settings") == 0)
    {
        NewTestLayout_ToggleSettings();
        return;
    }

    Debug_Log(
        "NewLayout",
        "ModeGridClick",
        "%s",
        actionName ? actionName : ""
    );
}

static void NewTestLayout_InitModeGridItems(void)
{
    static const char *names[12] =
    {
        "LiteralSearch",
        "EscapedLiteralSearch",
        "RegExSearch",
        "SemanticSearch",
        "CaseSensitive",
        "DiacriticSensitive",
        "DotIncludesNewline",
        "FuzzyLogicSearch",
        "WrapAround",
        "WholeWord",
        "BooleanSearch",
        "Settings"
    };

    static const char *texts[12] =
    {
        "Literal",
        "Escaped",
        "Regex",
        "Semantic",
        "Case",
        "Diacritic",
        "Dot NL",
        "Fuzzy",
        "Wrap",
        "Word",
        "Boolean",
        "Settings"
    };

    int i;

    ZeroMemory(g_modeGridItems, sizeof(g_modeGridItems));

    for (i = 0; i < 12; i++)
    {
        g_modeGridItems[i].name = names[i];
        g_modeGridItems[i].action = names[i];
        g_modeGridItems[i].text = texts[i];
        g_modeGridItems[i].tooltip = names[i];
        g_modeGridItems[i].iconBaseName = names[i];

        if (i < 4)
        {
            g_modeGridItems[i].behavior = BUTTON_GRID_BUTTON_RADIO;
            g_modeGridItems[i].radioGroup = 1;
            g_modeGridItems[i].defaultState = i == 0 ? 1 : 0;
        }
        else
        {
            g_modeGridItems[i].behavior = BUTTON_GRID_BUTTON_TOGGLE;
            g_modeGridItems[i].radioGroup = 0;
            g_modeGridItems[i].defaultState = 0;
        }
    }
}

static void NewTestLayout_InitModeGridConfig(void)
{
    ButtonGrid_GetDefaultConfig(&g_modeGridConfig);

    NewTestLayout_InitModeGridItems();

    g_modeGridConfig.buttonCount = 12;
    g_modeGridConfig.items = g_modeGridItems;

    g_modeGridConfig.buttonWidth = NTL_MODE_SINGLE_BUTTON_SIZE;
    g_modeGridConfig.buttonHeight = NTL_MODE_SINGLE_BUTTON_SIZE;
    g_modeGridConfig.horizontalSpacing = NTL_MODE_SINGLE_SPACING;
    g_modeGridConfig.verticalSpacing = NTL_MODE_SINGLE_SPACING;

    g_modeGridConfig.layout = BUTTON_GRID_LAYOUT_HORIZONTAL;
    g_modeGridConfig.sizeMode = BUTTON_GRID_SIZE_FIXED;

    g_modeGridConfig.showText = 0;
    g_modeGridConfig.usePictures = 1;
    g_modeGridConfig.toggleOnClick = 1;
    g_modeGridConfig.defaultState = 0;
    g_modeGridConfig.stretchPictures = 1;

    g_modeGridConfig.hidePartialButtons = 1;
    g_modeGridConfig.resizeInLayoutSteps = 0;

    g_modeGridConfig.showBorder = 0;
    g_modeGridConfig.showBorderTitle = 0;
    g_modeGridConfig.showGearIcon = 0;

    g_modeGridConfig.buttonBackMode = BUTTON_GRID_BUTTON_BACK_TRANSPARENT;
    g_modeGridConfig.showButtonBorder = 0;

    g_modeGridConfig.backColor = RGB(8, 8, 8);
    g_modeGridConfig.foreColor = RGB(230, 230, 230);

    g_modeGridConfig.themeName = BUTTON_GRID_DEFAULT_THEME_NAME;
    g_modeGridConfig.idBase = ID_NTL_MODE_GRID_BASE;
}

void NewTestLayout_CreateModeGrid(HWND hwnd)
{
    NewTestLayout_InitModeGridConfig();

    g_modeGrid = ButtonGrid_CreateEx(
        hwnd,
        g_hInstance,
        0,
        0,
        100,
        60,
        &g_modeGridConfig,
        NewTestLayout_ModeGridClicked
    );
}