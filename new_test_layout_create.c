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

    g_ntl_findCombo = NewTestLayoutFauxCombo_Create(
        hwnd,
        g_ntl_hInstance,
        ID_NTL_FIND_COMBO,
        "FIND WHAT",
        &g_ntl_theme
    );

    g_ntl_replaceCombo = NewTestLayoutFauxCombo_Create(
        hwnd,
        g_ntl_hInstance,
        ID_NTL_REPLACE_COMBO,
        "REPLACE WITH",
        &g_ntl_theme
    );

    if (g_ntl_findCombo)
    {
        NewTestLayoutFauxCombo_SetPlaceholderLarge(
            g_ntl_findCombo,
            g_ntl_settingsConfig.fauxComboPlaceholderLarge
        );

        NewTestLayoutFauxCombo_SetRecentItems(
            g_ntl_findCombo,
            recentFindItems,
            (int)(sizeof(recentFindItems) / sizeof(recentFindItems[0]))
        );
    }

    if (g_ntl_replaceCombo)
    {
        NewTestLayoutFauxCombo_SetPlaceholderLarge(
            g_ntl_replaceCombo,
            g_ntl_settingsConfig.fauxComboPlaceholderLarge
        );

        NewTestLayoutFauxCombo_SetRecentItems(
            g_ntl_replaceCombo,
            recentReplaceItems,
            (int)(sizeof(recentReplaceItems) / sizeof(recentReplaceItems[0]))
        );
    }
}

static void NewTestLayout_CreateUtilityButtons(HWND hwnd)
{
    g_ntl_copyToReplaceButton = NewTestLayoutActionButton_Create(
        hwnd,
        g_ntl_hInstance,
        ID_NTL_COPY_TO_REPLACE_BUTTON,
        "R",
        &g_ntl_theme
    );

    g_ntl_swapFindReplaceButton = NewTestLayoutActionButton_Create(
        hwnd,
        g_ntl_hInstance,
        ID_NTL_SWAP_FIND_REPLACE_BUTTON,
        "<>",
        &g_ntl_theme
    );

    g_ntl_copyToFindButton = NewTestLayoutActionButton_Create(
        hwnd,
        g_ntl_hInstance,
        ID_NTL_COPY_TO_FIND_BUTTON,
        "F",
        &g_ntl_theme
    );

    if (g_ntl_copyToReplaceButton)
        NewTestLayoutActionButton_SetCountOptions(g_ntl_copyToReplaceButton, 0, 0, 0, 0);

    if (g_ntl_swapFindReplaceButton)
        NewTestLayoutActionButton_SetCountOptions(g_ntl_swapFindReplaceButton, 0, 0, 0, 0);

    if (g_ntl_copyToFindButton)
        NewTestLayoutActionButton_SetCountOptions(g_ntl_copyToFindButton, 0, 0, 0, 0);
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

    g_ntl_findGroup = NewTestLayoutActionGroup_Create(
        hwnd,
        g_ntl_hInstance,
        ID_NTL_FIND_GROUP,
        "Find In Document",
        findButtons,
        4,
        &g_ntl_theme
    );

    g_ntl_replaceGroup = NewTestLayoutActionGroup_Create(
        hwnd,
        g_ntl_hInstance,
        ID_NTL_REPLACE_GROUP,
        "Replace In Document",
        replaceButtons,
        3,
        &g_ntl_theme
    );

    g_ntl_selectionGroup = NewTestLayoutActionGroup_Create(
        hwnd,
        g_ntl_hInstance,
        ID_NTL_SELECTION_GROUP,
        "Replace In Selection",
        selectionButtons,
        3,
        &g_ntl_theme
    );

    NewTestLayout_ApplyCountOptions();
}

static void NewTestLayout_SettingsChanged(void *userData)
{
    RECT rc;

    (void)userData;

    if (!g_ntl_window || !g_ntl_settingsPanel)
        return;

    NewTestLayoutSettings_GetConfig(g_ntl_settingsPanel, &g_ntl_settingsConfig);

    if (g_ntl_findCombo)
    {
        NewTestLayoutFauxCombo_SetPlaceholderLarge(
            g_ntl_findCombo,
            g_ntl_settingsConfig.fauxComboPlaceholderLarge
        );
    }

    if (g_ntl_replaceCombo)
    {
        NewTestLayoutFauxCombo_SetPlaceholderLarge(
            g_ntl_replaceCombo,
            g_ntl_settingsConfig.fauxComboPlaceholderLarge
        );
    }

    NewTestLayout_ApplyCountOptions();
    NewTestLayout_ApplyCounts();

    GetClientRect(g_ntl_window, &rc);
    NewTestLayoutSettings_Layout(g_ntl_settingsPanel, &rc);

    NewTestLayout_Layout(g_ntl_window);
}

void NewTestLayout_CreateControls(HWND hwnd)
{
    NewTestLayoutTheme_GetDefault(&g_ntl_theme);
    NewTestLayoutTheme_CreateDefaultFonts(&g_ntl_theme);

    NewTestLayoutSettings_LoadConfig(&g_ntl_settingsConfig);

    if (!g_ntl_backBrush)
        g_ntl_backBrush = CreateSolidBrush(g_ntl_theme.windowBackColor);

    NewTestLayoutControls_RegisterClasses(g_ntl_hInstance);

    NewTestLayout_CreateFauxCombos(hwnd);
    NewTestLayout_CreateUtilityButtons(hwnd);
    NewTestLayout_CreateActionGroups(hwnd);
    NewTestLayout_CreateModeGrid(hwnd);

    g_ntl_settingsPanel = NewTestLayoutSettings_Create(
        hwnd,
        g_ntl_hInstance,
        &g_ntl_settingsConfig,
        NewTestLayout_SettingsChanged,
        NULL
    );
}

void NewTestLayout_DestroyControls(void)
{
    if (g_ntl_settingsPanel)
    {
        NewTestLayoutSettings_Destroy(g_ntl_settingsPanel);
        g_ntl_settingsPanel = NULL;
    }

    if (g_ntl_findCombo)
    {
        NewTestLayoutFauxCombo_Destroy(g_ntl_findCombo);
        g_ntl_findCombo = NULL;
    }

    if (g_ntl_replaceCombo)
    {
        NewTestLayoutFauxCombo_Destroy(g_ntl_replaceCombo);
        g_ntl_replaceCombo = NULL;
    }

    if (g_ntl_copyToReplaceButton)
    {
        NewTestLayoutActionButton_Destroy(g_ntl_copyToReplaceButton);
        g_ntl_copyToReplaceButton = NULL;
    }

    if (g_ntl_swapFindReplaceButton)
    {
        NewTestLayoutActionButton_Destroy(g_ntl_swapFindReplaceButton);
        g_ntl_swapFindReplaceButton = NULL;
    }

    if (g_ntl_copyToFindButton)
    {
        NewTestLayoutActionButton_Destroy(g_ntl_copyToFindButton);
        g_ntl_copyToFindButton = NULL;
    }

    if (g_ntl_findGroup)
    {
        NewTestLayoutActionGroup_Destroy(g_ntl_findGroup);
        g_ntl_findGroup = NULL;
    }

    if (g_ntl_replaceGroup)
    {
        NewTestLayoutActionGroup_Destroy(g_ntl_replaceGroup);
        g_ntl_replaceGroup = NULL;
    }

    if (g_ntl_selectionGroup)
    {
        NewTestLayoutActionGroup_Destroy(g_ntl_selectionGroup);
        g_ntl_selectionGroup = NULL;
    }

    g_ntl_modeGrid = NULL;

    NewTestLayoutTheme_DeleteFonts(&g_ntl_theme);

    if (g_ntl_backBrush)
    {
        DeleteObject(g_ntl_backBrush);
        g_ntl_backBrush = NULL;
    }
}