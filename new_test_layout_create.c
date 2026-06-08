#include "new_test_layout_window_internal.h"

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
        "R",
        &g_theme
    );

    g_swapFindReplaceButton = NewTestLayoutActionButton_Create(
        hwnd,
        g_hInstance,
        ID_NTL_SWAP_FIND_REPLACE_BUTTON,
        "<>",
        &g_theme
    );

    g_copyToFindButton = NewTestLayoutActionButton_Create(
        hwnd,
        g_hInstance,
        ID_NTL_COPY_TO_FIND_BUTTON,
        "F",
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
    {
        NewTestLayoutFauxCombo_SetPlaceholderLarge(
            g_findCombo,
            g_settingsConfig.fauxComboPlaceholderLarge
        );
    }

    if (g_replaceCombo)
    {
        NewTestLayoutFauxCombo_SetPlaceholderLarge(
            g_replaceCombo,
            g_settingsConfig.fauxComboPlaceholderLarge
        );
    }

    NewTestLayout_ApplyCountOptions();
    NewTestLayout_ApplyCounts();

    GetClientRect(g_window, &rc);
    NewTestLayoutSettings_Layout(g_settingsPanel, &rc);

    NewTestLayout_Layout(g_window);
}

void NewTestLayout_CreateControls(HWND hwnd)
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

    g_settingsPanel = NewTestLayoutSettings_Create(
        hwnd,
        g_hInstance,
        &g_settingsConfig,
        NewTestLayout_SettingsChanged,
        NULL
    );
}

void NewTestLayout_DestroyControls(void)
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

    g_modeGrid = NULL;

    NewTestLayoutTheme_DeleteFonts(&g_theme);

    if (g_backBrush)
    {
        DeleteObject(g_backBrush);
        g_backBrush = NULL;
    }
}