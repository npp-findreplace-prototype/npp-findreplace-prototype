#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "new_test_layout_window.h"
#include "new_test_layout_controls.h"
#include "new_test_layout_settings.h"

#include "button_grid.h"
#include "npp_mockup_window.h"
#include "debug_window.h"

#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0
#endif

#define NEW_TEST_LAYOUT_WINDOW_CLASS_NAME "NewTestLayoutWindowClass"
#define NEW_TEST_LAYOUT_WINDOW_TITLE "New Test Layout"

#define NTL_TIMER_ID 1
#define NTL_TIMER_MS 300

#define NTL_MARGIN 12
#define NTL_GAP 8

#define NTL_FAUX_COMBO_HEIGHT 48
#define NTL_MODE_GRID_ROW_HEIGHT 52
#define NTL_ACTION_GROUP_HEIGHT 84
#define NTL_UTILITY_BUTTON_WIDTH 132
#define NTL_UTILITY_BUTTON_HEIGHT 30
#define NTL_GEAR_SIZE 32

#define ID_NTL_FIND_COMBO 7001
#define ID_NTL_REPLACE_COMBO 7002

#define ID_NTL_GEAR_BUTTON 7003

#define ID_NTL_COPY_TO_REPLACE_BUTTON 7010
#define ID_NTL_SWAP_FIND_REPLACE_BUTTON 7011
#define ID_NTL_COPY_TO_FIND_BUTTON 7012

#define ID_NTL_FIND_GROUP 7100
#define ID_NTL_REPLACE_GROUP 7200
#define ID_NTL_SELECTION_GROUP 7300

#define ID_NTL_FIND_PREVIOUS_BUTTON 7110
#define ID_NTL_FIND_ALL_BUTTON 7111
#define ID_NTL_FIND_ALL_DOCUMENTS_BUTTON 7112
#define ID_NTL_FIND_NEXT_BUTTON 7113

#define ID_NTL_REPLACE_PREVIOUS_BUTTON 7210
#define ID_NTL_REPLACE_ALL_BUTTON 7211
#define ID_NTL_REPLACE_NEXT_BUTTON 7212

#define ID_NTL_SELECTION_REPLACE_PREVIOUS_BUTTON 7310
#define ID_NTL_SELECTION_REPLACE_ALL_BUTTON 7311
#define ID_NTL_SELECTION_REPLACE_NEXT_BUTTON 7312

#define ID_NTL_MODE_GRID_BASE 7400

typedef struct NewTestLayoutCounts
{
    int hasCounts;

    int findPrevious;
    int findAll;
    int findNext;

    int replacePrevious;
    int replaceAll;
    int replaceNext;

    int hasSelectionCounts;
    int selectionReplacePrevious;
    int selectionReplaceAll;
    int selectionReplaceNext;
} NewTestLayoutCounts;

typedef struct NewTestLayoutVisibility
{
    int showReplaceBox;
    int showUtilityButtons;
    int showModeGrid;
    int showFindGroup;
    int showReplaceGroup;
    int showSelectionGroup;

    int useLeftModePanel;
    int showGroupBorder;
    int groupPadding;
} NewTestLayoutVisibility;

static HINSTANCE g_hInstance = NULL;
static HWND g_window = NULL;

static NewTestLayoutWindowClosedCallback g_onClosed = NULL;

static NewTestLayoutTheme g_theme;
static NewTestLayoutSettingsConfig g_settingsConfig;
static NewTestLayoutSettingsPanel *g_settingsPanel = NULL;

static NewTestLayoutFauxCombo *g_findCombo = NULL;
static NewTestLayoutFauxCombo *g_replaceCombo = NULL;

static NewTestLayoutGearButton *g_gearButton = NULL;

static NewTestLayoutActionButton *g_copyToReplaceButton = NULL;
static NewTestLayoutActionButton *g_swapFindReplaceButton = NULL;
static NewTestLayoutActionButton *g_copyToFindButton = NULL;

static NewTestLayoutActionGroup *g_findGroup = NULL;
static NewTestLayoutActionGroup *g_replaceGroup = NULL;
static NewTestLayoutActionGroup *g_selectionGroup = NULL;

static HWND g_modeGrid = NULL;
static ButtonGridConfig g_modeGridConfig;
static ButtonGridItemConfig g_modeGridItems[12];

static char g_lastFindText[512];
static NewTestLayoutCounts g_counts;

static HBRUSH g_backBrush = NULL;

static void NewTestLayout_CopyText(char *dest, int destSize, const char *src)
{
    if (!dest || destSize <= 0)
        return;

    if (!src)
        src = "";

    lstrcpyn(dest, src, destSize);
    dest[destSize - 1] = '\0';
}

static int NewTestLayout_MinInt(int a, int b)
{
    return a < b ? a : b;
}

static int NewTestLayout_MaxInt(int a, int b)
{
    return a > b ? a : b;
}

static int NewTestLayout_ClampInt(int value, int minValue, int maxValue)
{
    if (value < minValue)
        return minValue;

    if (value > maxValue)
        return maxValue;

    return value;
}

static void NewTestLayout_SetRect(
    RECT *rc,
    int left,
    int top,
    int right,
    int bottom
)
{
    if (!rc)
        return;

    rc->left = left;
    rc->top = top;
    rc->right = right;
    rc->bottom = bottom;
}

static int NewTestLayout_TextStartsWith(
    const char *text,
    const char *needle,
    int needleLength
)
{
    int i;

    if (!text || !needle || needleLength <= 0)
        return 0;

    for (i = 0; i < needleLength; i++)
    {
        if (text[i] != needle[i])
            return 0;
    }

    return 1;
}

static int NewTestLayout_CountLiteralInRange(
    const char *text,
    int textLength,
    const char *needle,
    int start,
    int end
)
{
    int count;
    int needleLength;
    int i;

    if (!text || !needle)
        return 0;

    needleLength = lstrlen(needle);

    if (needleLength <= 0)
        return 0;

    if (textLength < 0)
        textLength = lstrlen(text);

    start = NewTestLayout_ClampInt(start, 0, textLength);
    end = NewTestLayout_ClampInt(end, 0, textLength);

    if (end < start)
        end = start;

    count = 0;
    i = start;

    while (i + needleLength <= end)
    {
        if (NewTestLayout_TextStartsWith(text + i, needle, needleLength))
        {
            count++;
            i += needleLength;
        }
        else
        {
            i++;
        }
    }

    return count;
}

static void NewTestLayout_UpdateWindowTitle(void)
{
    char title[256];

    wsprintf(
        title,
        "%s%s",
        NEW_TEST_LAYOUT_WINDOW_TITLE,
        g_settingsPanel && NewTestLayoutSettings_IsVisible(g_settingsPanel)
            ? " - settings"
            : ""
    );

    if (g_window)
        SetWindowText(g_window, title);
}

static void NewTestLayout_GetFindText(char *buffer, int bufferSize)
{
    if (!buffer || bufferSize <= 0)
        return;

    buffer[0] = '\0';

    if (g_findCombo)
        NewTestLayoutFauxCombo_GetText(g_findCombo, buffer, bufferSize);
}

static void NewTestLayout_GetReplaceText(char *buffer, int bufferSize)
{
    if (!buffer || bufferSize <= 0)
        return;

    buffer[0] = '\0';

    if (g_replaceCombo)
        NewTestLayoutFauxCombo_GetText(g_replaceCombo, buffer, bufferSize);
}

static int NewTestLayout_ComputeCounts(NewTestLayoutCounts *counts)
{
    char findText[512];
    char *text;
    int textLength;
    NppMockupTextInfo info;

    if (!counts)
        return 0;

    ZeroMemory(counts, sizeof(*counts));

    NewTestLayout_GetFindText(findText, sizeof(findText));

    if (!findText[0])
        return 0;

    text = NppMockupWindow_AllocActiveText(&textLength);

    if (!text)
        return 0;

    if (!NppMockupWindow_GetActiveTextInfo(&info))
    {
        NppMockupWindow_FreeText(text);
        return 0;
    }

    counts->hasCounts = 1;

    counts->findPrevious = NewTestLayout_CountLiteralInRange(
        text,
        textLength,
        findText,
        0,
        info.caretCharacter
    );

    counts->findAll = NewTestLayout_CountLiteralInRange(
        text,
        textLength,
        findText,
        0,
        textLength
    );

    counts->findNext = NewTestLayout_CountLiteralInRange(
        text,
        textLength,
        findText,
        info.caretCharacter,
        textLength
    );

    counts->replacePrevious = counts->findPrevious;
    counts->replaceAll = counts->findAll;
    counts->replaceNext = counts->findNext;

    if (info.hasSelection)
    {
        counts->hasSelectionCounts = 1;

        counts->selectionReplacePrevious = NewTestLayout_CountLiteralInRange(
            text,
            textLength,
            findText,
            info.selectionStartCharacter,
            info.caretCharacter
        );

        counts->selectionReplaceAll = NewTestLayout_CountLiteralInRange(
            text,
            textLength,
            findText,
            info.selectionStartCharacter,
            info.selectionEndCharacter
        );

        counts->selectionReplaceNext = NewTestLayout_CountLiteralInRange(
            text,
            textLength,
            findText,
            info.caretCharacter,
            info.selectionEndCharacter
        );
    }

    NppMockupWindow_FreeText(text);

    return 1;
}

static void NewTestLayout_ApplyCountOptions(void)
{
    if (g_findGroup)
    {
        NewTestLayoutActionGroup_SetCountOptions(
            g_findGroup,
            g_settingsConfig.showCounts,
            g_settingsConfig.showZeroCounts,
            g_settingsConfig.countInParentheses,
            g_settingsConfig.actionButtonCountColorEnabled
        );
    }

    if (g_replaceGroup)
    {
        NewTestLayoutActionGroup_SetCountOptions(
            g_replaceGroup,
            g_settingsConfig.showCounts,
            g_settingsConfig.showZeroCounts,
            g_settingsConfig.countInParentheses,
            g_settingsConfig.actionButtonCountColorEnabled
        );
    }

    if (g_selectionGroup)
    {
        NewTestLayoutActionGroup_SetCountOptions(
            g_selectionGroup,
            g_settingsConfig.showCounts,
            g_settingsConfig.showZeroCounts,
            g_settingsConfig.countInParentheses,
            g_settingsConfig.actionButtonCountColorEnabled
        );
    }
}

static void NewTestLayout_ApplyCounts(void)
{
    NewTestLayoutCounts counts;
    char currentFind[512];

    ZeroMemory(&counts, sizeof(counts));

    NewTestLayout_ComputeCounts(&counts);
    g_counts = counts;

    if (g_findGroup)
    {
        NewTestLayoutActionGroup_SetButtonCount(
            g_findGroup,
            0,
            counts.findPrevious,
            counts.hasCounts
        );

        NewTestLayoutActionGroup_SetButtonCount(
            g_findGroup,
            1,
            counts.findAll,
            counts.hasCounts
        );

        NewTestLayoutActionGroup_SetButtonCount(
            g_findGroup,
            2,
            0,
            0
        );

        NewTestLayoutActionGroup_SetButtonCount(
            g_findGroup,
            3,
            counts.findNext,
            counts.hasCounts
        );
    }

    if (g_replaceGroup)
    {
        NewTestLayoutActionGroup_SetButtonCount(
            g_replaceGroup,
            0,
            counts.replacePrevious,
            counts.hasCounts
        );

        NewTestLayoutActionGroup_SetButtonCount(
            g_replaceGroup,
            1,
            counts.replaceAll,
            counts.hasCounts
        );

        NewTestLayoutActionGroup_SetButtonCount(
            g_replaceGroup,
            2,
            counts.replaceNext,
            counts.hasCounts
        );
    }

    if (g_selectionGroup)
    {
        NewTestLayoutActionGroup_SetButtonCount(
            g_selectionGroup,
            0,
            counts.selectionReplacePrevious,
            counts.hasSelectionCounts
        );

        NewTestLayoutActionGroup_SetButtonCount(
            g_selectionGroup,
            1,
            counts.selectionReplaceAll,
            counts.hasSelectionCounts
        );

        NewTestLayoutActionGroup_SetButtonCount(
            g_selectionGroup,
            2,
            counts.selectionReplaceNext,
            counts.hasSelectionCounts
        );
    }

    NewTestLayout_GetFindText(currentFind, sizeof(currentFind));

    if (lstrcmp(currentFind, g_lastFindText) != 0)
    {
        NewTestLayout_CopyText(g_lastFindText, sizeof(g_lastFindText), currentFind);

        Debug_Log(
            "NewLayout",
            "FindTextChanged",
            "Find text='%s'",
            g_lastFindText
        );
    }
}

static void NewTestLayout_ModeGridClicked(const char *actionName)
{
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

    g_modeGridConfig.buttonWidth = 38;
    g_modeGridConfig.buttonHeight = 38;
    g_modeGridConfig.horizontalSpacing = 8;
    g_modeGridConfig.verticalSpacing = 8;

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

    g_modeGridConfig.themeName = BUTTON_GRID_DEFAULT_THEME_NAME;
    g_modeGridConfig.idBase = ID_NTL_MODE_GRID_BASE;
}

static void NewTestLayout_CreateModeGrid(HWND hwnd)
{
    NewTestLayout_InitModeGridConfig();

    g_modeGrid = ButtonGrid_CreateEx(
        hwnd,
        g_hInstance,
        0,
        0,
        100,
        40,
        &g_modeGridConfig,
        NewTestLayout_ModeGridClicked
    );
}

static void NewTestLayout_CreateFauxCombos(HWND hwnd)
{
    static const char *recentFindItems[] =
    {
        "LiteralSearch",
        "button_grid",
        "Find What",
        "Replace With",
        "Search Options",
        "[A-Za-z_][A-Za-z0-9_]*",
        "TODO",
        "Debug_Log"
    };

    static const char *recentReplaceItems[] =
    {
        "",
        "[REPLACED]",
        "ButtonGrid",
        "NewTestLayout",
        "Debug_Log"
    };

    g_findCombo = NewTestLayoutFauxCombo_Create(
        hwnd,
        g_hInstance,
        ID_NTL_FIND_COMBO,
        "FIND WHAT",
        &g_theme
    );

    g_replaceCombo = NewTestLayoutFauxCombo_Create(
        hwnd,
        g_hInstance,
        ID_NTL_REPLACE_COMBO,
        "REPLACE WITH",
        &g_theme
    );

    if (g_findCombo)
    {
        NewTestLayoutFauxCombo_SetPlaceholderLarge(
            g_findCombo,
            g_settingsConfig.fauxComboPlaceholderLarge
        );

        NewTestLayoutFauxCombo_SetRecentItems(
            g_findCombo,
            recentFindItems,
            (int)(sizeof(recentFindItems) / sizeof(recentFindItems[0]))
        );
    }

    if (g_replaceCombo)
    {
        NewTestLayoutFauxCombo_SetPlaceholderLarge(
            g_replaceCombo,
            g_settingsConfig.fauxComboPlaceholderLarge
        );

        NewTestLayoutFauxCombo_SetRecentItems(
            g_replaceCombo,
            recentReplaceItems,
            (int)(sizeof(recentReplaceItems) / sizeof(recentReplaceItems[0]))
        );
    }
}

static void NewTestLayout_CreateUtilityButtons(HWND hwnd)
{
    g_copyToReplaceButton = NewTestLayoutActionButton_Create(
        hwnd,
        g_hInstance,
        ID_NTL_COPY_TO_REPLACE_BUTTON,
        "Copy -> Replace",
        &g_theme
    );

    g_swapFindReplaceButton = NewTestLayoutActionButton_Create(
        hwnd,
        g_hInstance,
        ID_NTL_SWAP_FIND_REPLACE_BUTTON,
        "Swap",
        &g_theme
    );

    g_copyToFindButton = NewTestLayoutActionButton_Create(
        hwnd,
        g_hInstance,
        ID_NTL_COPY_TO_FIND_BUTTON,
        "Copy -> Find",
        &g_theme
    );

    if (g_copyToReplaceButton)
        NewTestLayoutActionButton_SetCountOptions(g_copyToReplaceButton, 0, 0, 0, 0);

    if (g_swapFindReplaceButton)
        NewTestLayoutActionButton_SetCountOptions(g_swapFindReplaceButton, 0, 0, 0, 0);

    if (g_copyToFindButton)
        NewTestLayoutActionButton_SetCountOptions(g_copyToFindButton, 0, 0, 0, 0);
}

static void NewTestLayout_CreateActionGroups(HWND hwnd)
{
    NewTestLayoutActionButtonDef findButtons[4];
    NewTestLayoutActionButtonDef replaceButtons[3];
    NewTestLayoutActionButtonDef selectionButtons[3];

    findButtons[0].id = ID_NTL_FIND_PREVIOUS_BUTTON;
    findButtons[0].label = "Find Previous";
    findButtons[0].count = 0;
    findButtons[0].hasCount = 0;

    findButtons[1].id = ID_NTL_FIND_ALL_BUTTON;
    findButtons[1].label = "Find All";
    findButtons[1].count = 0;
    findButtons[1].hasCount = 0;

    findButtons[2].id = ID_NTL_FIND_ALL_DOCUMENTS_BUTTON;
    findButtons[2].label = "Find In All Documents";
    findButtons[2].count = 0;
    findButtons[2].hasCount = 0;

    findButtons[3].id = ID_NTL_FIND_NEXT_BUTTON;
    findButtons[3].label = "Find Next";
    findButtons[3].count = 0;
    findButtons[3].hasCount = 0;

    replaceButtons[0].id = ID_NTL_REPLACE_PREVIOUS_BUTTON;
    replaceButtons[0].label = "Replace Previous";
    replaceButtons[0].count = 0;
    replaceButtons[0].hasCount = 0;

    replaceButtons[1].id = ID_NTL_REPLACE_ALL_BUTTON;
    replaceButtons[1].label = "Replace All";
    replaceButtons[1].count = 0;
    replaceButtons[1].hasCount = 0;

    replaceButtons[2].id = ID_NTL_REPLACE_NEXT_BUTTON;
    replaceButtons[2].label = "Replace Next";
    replaceButtons[2].count = 0;
    replaceButtons[2].hasCount = 0;

    selectionButtons[0].id = ID_NTL_SELECTION_REPLACE_PREVIOUS_BUTTON;
    selectionButtons[0].label = "Replace Previous";
    selectionButtons[0].count = 0;
    selectionButtons[0].hasCount = 0;

    selectionButtons[1].id = ID_NTL_SELECTION_REPLACE_ALL_BUTTON;
    selectionButtons[1].label = "Replace All";
    selectionButtons[1].count = 0;
    selectionButtons[1].hasCount = 0;

    selectionButtons[2].id = ID_NTL_SELECTION_REPLACE_NEXT_BUTTON;
    selectionButtons[2].label = "Replace Next";
    selectionButtons[2].count = 0;
    selectionButtons[2].hasCount = 0;

    g_findGroup = NewTestLayoutActionGroup_Create(
        hwnd,
        g_hInstance,
        ID_NTL_FIND_GROUP,
        "Find In Document",
        findButtons,
        4,
        &g_theme
    );

    g_replaceGroup = NewTestLayoutActionGroup_Create(
        hwnd,
        g_hInstance,
        ID_NTL_REPLACE_GROUP,
        "Replace In Document",
        replaceButtons,
        3,
        &g_theme
    );

    g_selectionGroup = NewTestLayoutActionGroup_Create(
        hwnd,
        g_hInstance,
        ID_NTL_SELECTION_GROUP,
        "Replace In Selection",
        selectionButtons,
        3,
        &g_theme
    );

    NewTestLayout_ApplyCountOptions();
}

static void NewTestLayout_SettingsChanged(void *userData)
{
    RECT rc;

    (void)userData;

    if (!g_window || !g_settingsPanel)
        return;

    NewTestLayoutSettings_GetConfig(g_settingsPanel, &g_settingsConfig);

    if (g_findCombo)
        NewTestLayoutFauxCombo_SetPlaceholderLarge(
            g_findCombo,
            g_settingsConfig.fauxComboPlaceholderLarge
        );

    if (g_replaceCombo)
        NewTestLayoutFauxCombo_SetPlaceholderLarge(
            g_replaceCombo,
            g_settingsConfig.fauxComboPlaceholderLarge
        );

    NewTestLayout_ApplyCountOptions();
    NewTestLayout_ApplyCounts();

    GetClientRect(g_window, &rc);
    NewTestLayoutSettings_Layout(g_settingsPanel, &rc);

    InvalidateRect(g_window, NULL, TRUE);
    SendMessage(g_window, WM_SIZE, 0, 0);
}

static void NewTestLayout_CreateControls(HWND hwnd)
{
    NewTestLayoutTheme_GetDefault(&g_theme);
    NewTestLayoutTheme_CreateDefaultFonts(&g_theme);

    NewTestLayoutSettings_GetDefaultConfig(&g_settingsConfig);

    if (!g_backBrush)
        g_backBrush = CreateSolidBrush(g_theme.windowBackColor);

    NewTestLayoutControls_RegisterClasses(g_hInstance);

    NewTestLayout_CreateFauxCombos(hwnd);
    NewTestLayout_CreateUtilityButtons(hwnd);
    NewTestLayout_CreateActionGroups(hwnd);
    NewTestLayout_CreateModeGrid(hwnd);

    g_gearButton = NewTestLayoutGearButton_Create(
        hwnd,
        g_hInstance,
        ID_NTL_GEAR_BUTTON,
        &g_theme
    );

    g_settingsPanel = NewTestLayoutSettings_Create(
        hwnd,
        g_hInstance,
        &g_settingsConfig,
        NewTestLayout_SettingsChanged,
        NULL
    );
}

static void NewTestLayout_DestroyControls(void)
{
    if (g_settingsPanel)
    {
        NewTestLayoutSettings_Destroy(g_settingsPanel);
        g_settingsPanel = NULL;
    }

    if (g_findCombo)
    {
        NewTestLayoutFauxCombo_Destroy(g_findCombo);
        g_findCombo = NULL;
    }

    if (g_replaceCombo)
    {
        NewTestLayoutFauxCombo_Destroy(g_replaceCombo);
        g_replaceCombo = NULL;
    }

    if (g_copyToReplaceButton)
    {
        NewTestLayoutActionButton_Destroy(g_copyToReplaceButton);
        g_copyToReplaceButton = NULL;
    }

    if (g_swapFindReplaceButton)
    {
        NewTestLayoutActionButton_Destroy(g_swapFindReplaceButton);
        g_swapFindReplaceButton = NULL;
    }

    if (g_copyToFindButton)
    {
        NewTestLayoutActionButton_Destroy(g_copyToFindButton);
        g_copyToFindButton = NULL;
    }

    if (g_findGroup)
    {
        NewTestLayoutActionGroup_Destroy(g_findGroup);
        g_findGroup = NULL;
    }

    if (g_replaceGroup)
    {
        NewTestLayoutActionGroup_Destroy(g_replaceGroup);
        g_replaceGroup = NULL;
    }

    if (g_selectionGroup)
    {
        NewTestLayoutActionGroup_Destroy(g_selectionGroup);
        g_selectionGroup = NULL;
    }

    if (g_gearButton)
    {
        NewTestLayoutGearButton_Destroy(g_gearButton);
        g_gearButton = NULL;
    }

    g_modeGrid = NULL;

    NewTestLayoutTheme_DeleteFonts(&g_theme);

    if (g_backBrush)
    {
        DeleteObject(g_backBrush);
        g_backBrush = NULL;
    }
}

static void NewTestLayout_ComputeVisibility(
    int width,
    int height,
    NewTestLayoutVisibility *visibility
)
{
    int autoLayout;
    int allGroupsVisible;
    int availableWidth;
    int groupStackHeight;
    int minimumGroupWidth;
    int extraHeight;
    int fullHeightNeeded;

    if (!visibility)
        return;

    ZeroMemory(visibility, sizeof(*visibility));

    autoLayout = g_settingsConfig.autoLayoutEnabled;

    visibility->showReplaceBox =
        g_settingsConfig.showReplaceBox &&
        (!autoLayout || height >= 118);

    visibility->showUtilityButtons =
        g_settingsConfig.showUtilityButtons &&
        visibility->showReplaceBox &&
        (!autoLayout || width >= 620);

    visibility->showModeGrid =
        g_settingsConfig.showModeGrid &&
        (!autoLayout || height >= 178);

    visibility->showFindGroup =
        g_settingsConfig.showFindDocumentGrid &&
        (!autoLayout || height >= 268);

    visibility->showReplaceGroup =
        g_settingsConfig.showReplaceDocumentGrid &&
        (!autoLayout || height >= 360);

    visibility->showSelectionGroup =
        g_settingsConfig.showReplaceSelectionGrid &&
        (!autoLayout || height >= 452);

    allGroupsVisible =
        visibility->showFindGroup &&
        visibility->showReplaceGroup &&
        visibility->showSelectionGroup;

    groupStackHeight =
        NTL_ACTION_GROUP_HEIGHT * 3 +
        NTL_GAP * 2;

    availableWidth = width - NTL_MARGIN * 2;
    minimumGroupWidth = 420;

    visibility->useLeftModePanel =
        allGroupsVisible &&
        visibility->showModeGrid &&
        g_settingsConfig.enableLeftModePanel &&
        availableWidth >= groupStackHeight + NTL_GAP + minimumGroupWidth;

    fullHeightNeeded =
        NTL_MARGIN +
        NTL_FAUX_COMBO_HEIGHT +
        NTL_GAP +
        NTL_FAUX_COMBO_HEIGHT +
        NTL_GAP +
        NTL_MODE_GRID_ROW_HEIGHT +
        NTL_GAP +
        groupStackHeight +
        NTL_MARGIN;

    extraHeight = height - fullHeightNeeded;

    visibility->showGroupBorder =
        allGroupsVisible &&
        g_settingsConfig.enableGrowingBorder &&
        extraHeight > 20;

    if (visibility->showGroupBorder)
    {
        visibility->groupPadding = 8 + NewTestLayout_MinInt(extraHeight / 14, 18);
    }
    else
    {
        visibility->groupPadding = 4;
    }
}

static void NewTestLayout_ShowUtilityButtons(int show)
{
    NewTestLayoutActionButton_Show(g_copyToReplaceButton, show);
    NewTestLayoutActionButton_Show(g_swapFindReplaceButton, show);
    NewTestLayoutActionButton_Show(g_copyToFindButton, show);
}

static void NewTestLayout_ShowGroups(const NewTestLayoutVisibility *visibility)
{
    if (!visibility)
        return;

    NewTestLayoutActionGroup_Show(g_findGroup, visibility->showFindGroup);
    NewTestLayoutActionGroup_Show(g_replaceGroup, visibility->showReplaceGroup);
    NewTestLayoutActionGroup_Show(g_selectionGroup, visibility->showSelectionGroup);

    NewTestLayoutActionGroup_SetBorderVisible(g_findGroup, visibility->showGroupBorder);
    NewTestLayoutActionGroup_SetBorderVisible(g_replaceGroup, visibility->showGroupBorder);
    NewTestLayoutActionGroup_SetBorderVisible(g_selectionGroup, visibility->showGroupBorder);

    NewTestLayoutActionGroup_SetPadding(g_findGroup, visibility->groupPadding);
    NewTestLayoutActionGroup_SetPadding(g_replaceGroup, visibility->groupPadding);
    NewTestLayoutActionGroup_SetPadding(g_selectionGroup, visibility->groupPadding);
}

static void NewTestLayout_Layout(HWND hwnd)
{
    RECT rc;
    RECT r;
    NewTestLayoutVisibility visibility;

    int width;
    int height;

    int x;
    int y;
    int right;
    int comboRight;
    int utilityX;
    int gearX;

    int groupX;
    int groupY;
    int groupWidth;
    int groupStackHeight;
    int leftPanelSize;

    GetClientRect(hwnd, &rc);

    width = rc.right - rc.left;
    height = rc.bottom - rc.top;

    NewTestLayout_ComputeVisibility(width, height, &visibility);

    x = NTL_MARGIN;
    y = NTL_MARGIN;
    right = width - NTL_MARGIN;

    gearX = right - NTL_GEAR_SIZE;

    NewTestLayout_SetRect(
        &r,
        gearX,
        y,
        gearX + NTL_GEAR_SIZE,
        y + NTL_GEAR_SIZE
    );

    NewTestLayoutGearButton_SetRect(g_gearButton, &r);

    if (visibility.showUtilityButtons)
    {
        utilityX = gearX - NTL_GAP - NTL_UTILITY_BUTTON_WIDTH;
        comboRight = utilityX - NTL_GAP;
    }
    else
    {
        utilityX = right;
        comboRight = gearX - NTL_GAP;
    }

    if (comboRight < x + 160)
        comboRight = right;

    NewTestLayout_SetRect(
        &r,
        x,
        y,
        comboRight,
        y + NTL_FAUX_COMBO_HEIGHT
    );

    NewTestLayoutFauxCombo_SetRect(g_findCombo, &r);
    NewTestLayoutFauxCombo_Show(g_findCombo, 1);

    if (visibility.showReplaceBox)
    {
        y += NTL_FAUX_COMBO_HEIGHT + NTL_GAP;

        NewTestLayout_SetRect(
            &r,
            x,
            y,
            comboRight,
            y + NTL_FAUX_COMBO_HEIGHT
        );

        NewTestLayoutFauxCombo_SetRect(g_replaceCombo, &r);
        NewTestLayoutFauxCombo_Show(g_replaceCombo, 1);
    }
    else
    {
        NewTestLayoutFauxCombo_Show(g_replaceCombo, 0);
    }

    if (visibility.showUtilityButtons)
    {
        NewTestLayout_SetRect(
            &r,
            utilityX,
            NTL_MARGIN,
            utilityX + NTL_UTILITY_BUTTON_WIDTH,
            NTL_MARGIN + NTL_UTILITY_BUTTON_HEIGHT
        );

        NewTestLayoutActionButton_SetRect(g_copyToReplaceButton, &r);

        NewTestLayout_SetRect(
            &r,
            utilityX,
            NTL_MARGIN + NTL_UTILITY_BUTTON_HEIGHT + 5,
            utilityX + NTL_UTILITY_BUTTON_WIDTH,
            NTL_MARGIN + NTL_UTILITY_BUTTON_HEIGHT * 2 + 5
        );

        NewTestLayoutActionButton_SetRect(g_swapFindReplaceButton, &r);

        NewTestLayout_SetRect(
            &r,
            utilityX,
            NTL_MARGIN + (NTL_UTILITY_BUTTON_HEIGHT + 5) * 2,
            utilityX + NTL_UTILITY_BUTTON_WIDTH,
            NTL_MARGIN + NTL_UTILITY_BUTTON_HEIGHT * 3 + 10
        );

        NewTestLayoutActionButton_SetRect(g_copyToFindButton, &r);
    }

    NewTestLayout_ShowUtilityButtons(visibility.showUtilityButtons);

    y = NTL_MARGIN + NTL_FAUX_COMBO_HEIGHT;

    if (visibility.showReplaceBox)
        y += NTL_GAP + NTL_FAUX_COMBO_HEIGHT;

    groupY = y;

    if (visibility.showModeGrid && !visibility.useLeftModePanel)
    {
        groupY += NTL_GAP + NTL_MODE_GRID_ROW_HEIGHT;

        NewTestLayout_SetRect(
            &r,
            NTL_MARGIN,
            y + NTL_GAP,
            width - NTL_MARGIN,
            y + NTL_GAP + NTL_MODE_GRID_ROW_HEIGHT
        );

        if (g_modeGrid)
        {
            ShowWindow(g_modeGrid, SW_SHOW);
            ButtonGrid_SetRect(
                g_modeGrid,
                r.left,
                r.top,
                r.right - r.left,
                r.bottom - r.top
            );
        }
    }
    else if (visibility.showModeGrid && visibility.useLeftModePanel)
    {
        if (g_modeGrid)
            ShowWindow(g_modeGrid, SW_SHOW);
    }
    else
    {
        if (g_modeGrid)
            ShowWindow(g_modeGrid, SW_HIDE);
    }

    groupStackHeight =
        NTL_ACTION_GROUP_HEIGHT * 3 +
        NTL_GAP * 2;

    groupX = NTL_MARGIN;
    groupWidth = width - NTL_MARGIN * 2;

    if (visibility.useLeftModePanel)
    {
        leftPanelSize = groupStackHeight;

        if (leftPanelSize > height - groupY - NTL_MARGIN)
            leftPanelSize = height - groupY - NTL_MARGIN;

        if (leftPanelSize < 120)
            leftPanelSize = 120;

        NewTestLayout_SetRect(
            &r,
            NTL_MARGIN,
            groupY,
            NTL_MARGIN + leftPanelSize,
            groupY + leftPanelSize
        );

        if (g_modeGrid)
        {
            ButtonGrid_SetRect(
                g_modeGrid,
                r.left,
                r.top,
                r.right - r.left,
                r.bottom - r.top
            );
        }

        groupX = r.right + NTL_GAP;
        groupWidth = width - groupX - NTL_MARGIN;
    }

    if (groupWidth < 180)
        groupWidth = 180;

    if (visibility.showFindGroup)
    {
        NewTestLayout_SetRect(
            &r,
            groupX,
            groupY,
            groupX + groupWidth,
            groupY + NTL_ACTION_GROUP_HEIGHT
        );

        NewTestLayoutActionGroup_SetRect(g_findGroup, &r);
        groupY += NTL_ACTION_GROUP_HEIGHT + NTL_GAP;
    }

    if (visibility.showReplaceGroup)
    {
        NewTestLayout_SetRect(
            &r,
            groupX,
            groupY,
            groupX + groupWidth,
            groupY + NTL_ACTION_GROUP_HEIGHT
        );

        NewTestLayoutActionGroup_SetRect(g_replaceGroup, &r);
        groupY += NTL_ACTION_GROUP_HEIGHT + NTL_GAP;
    }

    if (visibility.showSelectionGroup)
    {
        NewTestLayout_SetRect(
            &r,
            groupX,
            groupY,
            groupX + groupWidth,
            groupY + NTL_ACTION_GROUP_HEIGHT
        );

        NewTestLayoutActionGroup_SetRect(g_selectionGroup, &r);
    }

    NewTestLayout_ShowGroups(&visibility);

    if (g_settingsPanel)
    {
        NewTestLayoutSettings_Layout(g_settingsPanel, &rc);

        if (NewTestLayoutSettings_IsVisible(g_settingsPanel))
            NewTestLayoutSettings_Show(g_settingsPanel, 1);
    }

    InvalidateRect(hwnd, NULL, FALSE);
}

static void NewTestLayout_CopyFindToReplace(void)
{
    char text[512];

    NewTestLayout_GetFindText(text, sizeof(text));

    if (g_replaceCombo)
        NewTestLayoutFauxCombo_SetText(g_replaceCombo, text);

    Debug_Log("NewLayout", "CopyFindToReplace", "%s", text);
}

static void NewTestLayout_CopyReplaceToFind(void)
{
    char text[512];

    NewTestLayout_GetReplaceText(text, sizeof(text));

    if (g_findCombo)
        NewTestLayoutFauxCombo_SetText(g_findCombo, text);

    Debug_Log("NewLayout", "CopyReplaceToFind", "%s", text);

    NewTestLayout_ApplyCounts();
}

static void NewTestLayout_SwapFindReplace(void)
{
    char findText[512];
    char replaceText[512];

    NewTestLayout_GetFindText(findText, sizeof(findText));
    NewTestLayout_GetReplaceText(replaceText, sizeof(replaceText));

    if (g_findCombo)
        NewTestLayoutFauxCombo_SetText(g_findCombo, replaceText);

    if (g_replaceCombo)
        NewTestLayoutFauxCombo_SetText(g_replaceCombo, findText);

    Debug_Log(
        "NewLayout",
        "SwapFindReplace",
        "find='%s' replace='%s'",
        replaceText,
        findText
    );

    NewTestLayout_ApplyCounts();
}

static void NewTestLayout_LogActionButton(int id)
{
    const char *name;

    name = "Unknown";

    switch (id)
    {
        case ID_NTL_FIND_PREVIOUS_BUTTON:
            name = "Find Previous";
            break;

        case ID_NTL_FIND_ALL_BUTTON:
            name = "Find All";
            break;

        case ID_NTL_FIND_ALL_DOCUMENTS_BUTTON:
            name = "Find In All Documents";
            break;

        case ID_NTL_FIND_NEXT_BUTTON:
            name = "Find Next";
            break;

        case ID_NTL_REPLACE_PREVIOUS_BUTTON:
            name = "Replace Previous";
            break;

        case ID_NTL_REPLACE_ALL_BUTTON:
            name = "Replace All";
            break;

        case ID_NTL_REPLACE_NEXT_BUTTON:
            name = "Replace Next";
            break;

        case ID_NTL_SELECTION_REPLACE_PREVIOUS_BUTTON:
            name = "Selection Replace Previous";
            break;

        case ID_NTL_SELECTION_REPLACE_ALL_BUTTON:
            name = "Selection Replace All";
            break;

        case ID_NTL_SELECTION_REPLACE_NEXT_BUTTON:
            name = "Selection Replace Next";
            break;
    }

    Debug_Log("NewLayout", "ActionButton", "%s", name);
}

static int NewTestLayout_HandleCommand(WPARAM wParam, LPARAM lParam)
{
    int id;
    int notifyCode;

    if (g_settingsPanel &&
        NewTestLayoutSettings_HandleCommand(g_settingsPanel, wParam, lParam))
    {
        NewTestLayout_UpdateWindowTitle();
        return 1;
    }

    if (g_findCombo)
        NewTestLayoutFauxCombo_HandleParentCommand(g_findCombo, wParam, lParam);

    if (g_replaceCombo)
        NewTestLayoutFauxCombo_HandleParentCommand(g_replaceCombo, wParam, lParam);

    id = LOWORD(wParam);
    notifyCode = HIWORD(wParam);

    if ((HWND)lParam == NewTestLayoutFauxCombo_GetHwnd(g_findCombo))
    {
        if (notifyCode == NTL_FCN_TEXT_CHANGED ||
            notifyCode == NTL_FCN_RECENT_SELECTED)
        {
            NewTestLayout_ApplyCounts();
            return 1;
        }

        return 1;
    }

    if ((HWND)lParam == NewTestLayoutFauxCombo_GetHwnd(g_replaceCombo))
    {
        return 1;
    }

    if (id == ID_NTL_GEAR_BUTTON && notifyCode == BN_CLICKED)
    {
        if (g_settingsPanel)
        {
            NewTestLayoutSettings_Toggle(g_settingsPanel);
            NewTestLayout_UpdateWindowTitle();
            NewTestLayout_Layout(g_window);
        }

        return 1;
    }

    if (notifyCode == NTL_ACTION_BN_CLICKED)
    {
        if (id == ID_NTL_COPY_TO_REPLACE_BUTTON)
        {
            NewTestLayout_CopyFindToReplace();
            return 1;
        }

        if (id == ID_NTL_SWAP_FIND_REPLACE_BUTTON)
        {
            NewTestLayout_SwapFindReplace();
            return 1;
        }

        if (id == ID_NTL_COPY_TO_FIND_BUTTON)
        {
            NewTestLayout_CopyReplaceToFind();
            return 1;
        }

        NewTestLayout_LogActionButton(id);
        return 1;
    }

    return 0;
}

static LRESULT CALLBACK NewTestLayoutWindowProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam
)
{
    switch (msg)
    {
        case WM_CREATE:
        {
            CREATESTRUCT *cs;

            cs = (CREATESTRUCT *)lParam;

            g_hInstance = cs->hInstance;
            g_window = hwnd;

            NewTestLayout_CreateControls(hwnd);
            NewTestLayout_Layout(hwnd);

            SetTimer(hwnd, NTL_TIMER_ID, NTL_TIMER_MS, NULL);

            Debug_Log(
                "NewLayout",
                "WindowCreate",
                "New test layout window created."
            );

            return 0;
        }

        case WM_SIZE:
        {
            NewTestLayout_Layout(hwnd);
            return 0;
        }

        case WM_DPICHANGED:
        {
            RECT *suggested;

            suggested = (RECT *)lParam;

            if (suggested)
            {
                SetWindowPos(
                    hwnd,
                    NULL,
                    suggested->left,
                    suggested->top,
                    suggested->right - suggested->left,
                    suggested->bottom - suggested->top,
                    SWP_NOZORDER | SWP_NOACTIVATE
                );
            }

            NewTestLayout_Layout(hwnd);
            return 0;
        }

        case WM_COMMAND:
        {
            if (NewTestLayout_HandleCommand(wParam, lParam))
                return 0;

            break;
        }

        case WM_TIMER:
        {
            if (wParam == NTL_TIMER_ID)
            {
                NewTestLayout_ApplyCounts();
                return 0;
            }

            break;
        }

        case WM_CTLCOLOREDIT:
        {
            HDC hdc;

            hdc = (HDC)wParam;

            SetBkColor(hdc, g_theme.editBackColor);
            SetTextColor(hdc, g_theme.editTextColor);

            return (LRESULT)GetStockObject(NULL_BRUSH);
        }

        case WM_CTLCOLORSTATIC:
        {
            LRESULT result;

            result = 0;

            if (g_settingsPanel &&
                NewTestLayoutSettings_HandleCtlColorStatic(
                    g_settingsPanel,
                    (HDC)wParam,
                    (HWND)lParam,
                    &result
                ))
            {
                return result;
            }

            SetBkColor((HDC)wParam, g_theme.windowBackColor);
            SetTextColor((HDC)wParam, g_theme.buttonTextColor);
            return (LRESULT)g_backBrush;
        }

        case WM_ERASEBKGND:
        {
            return 1;
        }

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc;
            RECT rc;

            hdc = BeginPaint(hwnd, &ps);

            GetClientRect(hwnd, &rc);

            if (g_backBrush)
                FillRect(hdc, &rc, g_backBrush);
            else
                FillRect(hdc, &rc, (HBRUSH)(COLOR_WINDOW + 1));

            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_CLOSE:
        {
            DestroyWindow(hwnd);
            return 0;
        }

        case WM_NCDESTROY:
        {
            NewTestLayoutWindowClosedCallback callback;

            KillTimer(hwnd, NTL_TIMER_ID);

            Debug_Log(
                "NewLayout",
                "WindowDestroy",
                "New test layout window destroyed."
            );

            NewTestLayout_DestroyControls();

            g_window = NULL;
            g_modeGrid = NULL;

            ZeroMemory(&g_counts, sizeof(g_counts));
            g_lastFindText[0] = '\0';

            callback = g_onClosed;
            g_onClosed = NULL;

            if (callback)
                callback();

            return 0;
        }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

static BOOL NewTestLayoutWindow_RegisterClass(HINSTANCE hInstance)
{
    static int registered = 0;
    WNDCLASS wc;

    if (registered)
        return TRUE;

    ZeroMemory(&wc, sizeof(wc));

    wc.lpfnWndProc = NewTestLayoutWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = NEW_TEST_LAYOUT_WINDOW_CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;

    if (!RegisterClass(&wc))
        return FALSE;

    registered = 1;
    return TRUE;
}

HWND NewTestLayoutWindow_Show(
    HINSTANCE hInstance,
    NewTestLayoutWindowClosedCallback onClosed
)
{
    g_hInstance = hInstance;
    g_onClosed = onClosed;

    if (g_window && IsWindow(g_window))
    {
        ShowWindow(g_window, SW_SHOW);
        SetForegroundWindow(g_window);
        return g_window;
    }

    if (!NewTestLayoutWindow_RegisterClass(hInstance))
        return NULL;

    g_window = CreateWindowEx(
        0,
        NEW_TEST_LAYOUT_WINDOW_CLASS_NAME,
        NEW_TEST_LAYOUT_WINDOW_TITLE,
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        860,
        520,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!g_window)
        return NULL;

    ShowWindow(g_window, SW_SHOW);
    UpdateWindow(g_window);

    return g_window;
}

void NewTestLayoutWindow_Close(void)
{
    if (g_window && IsWindow(g_window))
        DestroyWindow(g_window);
}

HWND NewTestLayoutWindow_GetHwnd(void)
{
    return g_window;
}

int NewTestLayoutWindow_IsOpen(void)
{
    return g_window && IsWindow(g_window);
}
