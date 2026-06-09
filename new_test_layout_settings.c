#include "new_test_layout_settings_internal.h"

void NewTestLayoutSettings_GetDefaultConfig(
    NewTestLayoutSettingsConfig *config
)
{
    if (!config)
        return;

    ZeroMemory(config, sizeof(*config));

    config->overlayWidth = NEW_TEST_LAYOUT_SETTINGS_DEFAULT_OVERLAY_WIDTH;
    config->overlayMargin = NEW_TEST_LAYOUT_SETTINGS_DEFAULT_MARGIN;
    config->rowHeight = NEW_TEST_LAYOUT_SETTINGS_DEFAULT_ROW_HEIGHT;
    config->gap = NEW_TEST_LAYOUT_SETTINGS_DEFAULT_GAP;
    config->singleRowModeGridHeight = NEW_TEST_LAYOUT_SETTINGS_DEFAULT_SINGLE_ROW_GRID_HEIGHT;

    config->replaceBoxMinHeightPercent = NEW_TEST_LAYOUT_SETTINGS_DEFAULT_REPLACE_BOX_MIN_HEIGHT_PERCENT;
    config->utilityButtonsMinWidthPercent = NEW_TEST_LAYOUT_SETTINGS_DEFAULT_UTILITY_BUTTONS_MIN_WIDTH_PERCENT;
    config->modeGridMinHeightPercent = NEW_TEST_LAYOUT_SETTINGS_DEFAULT_MODE_GRID_MIN_HEIGHT_PERCENT;
    config->findDocumentGridMinHeightPercent = NEW_TEST_LAYOUT_SETTINGS_DEFAULT_FIND_GRID_MIN_HEIGHT_PERCENT;
    config->replaceDocumentGridMinHeightPercent = NEW_TEST_LAYOUT_SETTINGS_DEFAULT_REPLACE_GRID_MIN_HEIGHT_PERCENT;
    config->replaceSelectionGridMinHeightPercent = NEW_TEST_LAYOUT_SETTINGS_DEFAULT_SELECTION_GRID_MIN_HEIGHT_PERCENT;
    config->leftModePanelMinWidthPercent = NEW_TEST_LAYOUT_SETTINGS_DEFAULT_LEFT_MODE_PANEL_MIN_WIDTH_PERCENT;
    config->leftModePanelMinGroupWidthPercent = NEW_TEST_LAYOUT_SETTINGS_DEFAULT_LEFT_MODE_PANEL_MIN_GROUP_WIDTH_PERCENT;

    config->autoLayoutEnabled = 1;
    config->showReplaceBox = 1;
    config->showUtilityButtons = 1;
    config->showModeGrid = 1;
    config->showFindDocumentGrid = 1;
    config->showReplaceDocumentGrid = 1;
    config->showReplaceSelectionGrid = 1;

    config->showCounts = 1;
    config->showZeroCounts = 0;
    config->countInParentheses = 1;

    config->enableLeftModePanel = 1;
    config->enableGrowingBorder = 1;
    config->enableRecentFindDropdown = 1;
    config->enableRecentReplaceDropdown = 1;

    config->fauxComboPlaceholderLarge = 1;
    config->actionButtonCountColorEnabled = 1;

    config->overlayBackColor = RGB(22, 22, 22);
    config->overlayBorderColor = RGB(86, 86, 86);
    config->overlayTitleColor = RGB(230, 230, 230);
    config->labelColor = RGB(190, 190, 190);
    config->textColor = RGB(230, 230, 230);
    config->mutedTextColor = RGB(130, 130, 130);
    config->accentColor = RGB(0, 105, 210);
    config->buttonBackColor = RGB(28, 28, 28);
    config->buttonBorderColor = RGB(90, 90, 90);
    config->buttonTextColor = RGB(220, 220, 220);
    config->countTextColor = RGB(0, 105, 210);
}

NewTestLayoutSettingsPanel *NewTestLayoutSettings_Create(
    HWND parent,
    HINSTANCE hInstance,
    const NewTestLayoutSettingsConfig *initialConfig,
    NewTestLayoutSettingsChangedCallback changedCallback,
    void *changedUserData
)
{
    NewTestLayoutSettingsPanel *panel;

    panel = (NewTestLayoutSettingsPanel *)malloc(sizeof(*panel));

    if (!panel)
        return NULL;

    ZeroMemory(panel, sizeof(*panel));

    panel->parent = parent;
    panel->hInstance = hInstance;
    panel->changedCallback = changedCallback;
    panel->changedUserData = changedUserData;
    panel->visible = 0;
    panel->scrollY = 0;
    panel->contentHeight = 0;

    Settings_CopyConfig(&panel->config, initialConfig);
    Settings_RecreateBrushes(panel);

    panel->container = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        "STATIC",
        "",
        WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
        0,
        0,
        panel->config.overlayWidth,
        300,
        parent,
        NULL,
        hInstance,
        NULL
    );

    if (!panel->container)
    {
        NewTestLayoutSettings_Destroy(panel);
        return NULL;
    }

    Settings_CreateChildControls(panel);

    NewTestLayoutSettings_RequestRefresh(panel);
    ShowWindow(panel->container, SW_HIDE);

    return panel;
}

void NewTestLayoutSettings_Destroy(
    NewTestLayoutSettingsPanel *panel
)
{
    if (!panel)
        return;

    if (panel->container)
        DestroyWindow(panel->container);

    if (panel->backBrush)
        DeleteObject(panel->backBrush);

    if (panel->editBrush)
        DeleteObject(panel->editBrush);

    free(panel);
}

void NewTestLayoutSettings_SetConfig(
    NewTestLayoutSettingsPanel *panel,
    const NewTestLayoutSettingsConfig *config
)
{
    if (!panel)
        return;

    Settings_CopyConfig(&panel->config, config);
    Settings_RecreateBrushes(panel);
    NewTestLayoutSettings_RequestRefresh(panel);
}

void NewTestLayoutSettings_GetConfig(
    NewTestLayoutSettingsPanel *panel,
    NewTestLayoutSettingsConfig *config
)
{
    if (!panel || !config)
        return;

    CopyMemory(config, &panel->config, sizeof(*config));
}

void NewTestLayoutSettings_Show(
    NewTestLayoutSettingsPanel *panel,
    int show
)
{
    if (!panel || !panel->container)
        return;

    panel->visible = show ? 1 : 0;

    ShowWindow(panel->container, panel->visible ? SW_SHOW : SW_HIDE);

    if (panel->visible)
    {
        SetWindowPos(
            panel->container,
            HWND_TOP,
            0,
            0,
            0,
            0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE
        );
    }
}

void NewTestLayoutSettings_Toggle(
    NewTestLayoutSettingsPanel *panel
)
{
    if (!panel)
        return;

    NewTestLayoutSettings_Show(panel, !panel->visible);
}

int NewTestLayoutSettings_IsVisible(
    NewTestLayoutSettingsPanel *panel
)
{
    if (!panel)
        return 0;

    return panel->visible;
}