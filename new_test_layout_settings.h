#ifndef NEW_TEST_LAYOUT_SETTINGS_H
#define NEW_TEST_LAYOUT_SETTINGS_H

#include <windows.h>

#define NEW_TEST_LAYOUT_SETTINGS_DEFAULT_OVERLAY_WIDTH 360
#define NEW_TEST_LAYOUT_SETTINGS_DEFAULT_MARGIN 12
#define NEW_TEST_LAYOUT_SETTINGS_DEFAULT_ROW_HEIGHT 28
#define NEW_TEST_LAYOUT_SETTINGS_DEFAULT_GAP 8
#define NEW_TEST_LAYOUT_SETTINGS_DEFAULT_SINGLE_ROW_GRID_HEIGHT 68

#define NEW_TEST_LAYOUT_SETTINGS_DESIGN_WIDTH 860
#define NEW_TEST_LAYOUT_SETTINGS_DESIGN_HEIGHT 520

#define NEW_TEST_LAYOUT_SETTINGS_DEFAULT_REPLACE_BOX_MIN_HEIGHT_PERCENT 23
#define NEW_TEST_LAYOUT_SETTINGS_DEFAULT_UTILITY_BUTTONS_MIN_WIDTH_PERCENT 50
#define NEW_TEST_LAYOUT_SETTINGS_DEFAULT_MODE_GRID_MIN_HEIGHT_PERCENT 34
#define NEW_TEST_LAYOUT_SETTINGS_DEFAULT_FIND_GRID_MIN_HEIGHT_PERCENT 55
#define NEW_TEST_LAYOUT_SETTINGS_DEFAULT_REPLACE_GRID_MIN_HEIGHT_PERCENT 72
#define NEW_TEST_LAYOUT_SETTINGS_DEFAULT_SELECTION_GRID_MIN_HEIGHT_PERCENT 90
#define NEW_TEST_LAYOUT_SETTINGS_DEFAULT_LEFT_MODE_PANEL_MIN_WIDTH_PERCENT 116
#define NEW_TEST_LAYOUT_SETTINGS_DEFAULT_LEFT_MODE_PANEL_MIN_GROUP_WIDTH_PERCENT 72

typedef struct NewTestLayoutSettingsPanel NewTestLayoutSettingsPanel;

typedef void (*NewTestLayoutSettingsChangedCallback)(
    void *userData
);

typedef struct NewTestLayoutSettingsConfig
{
    int overlayWidth;
    int overlayMargin;
    int rowHeight;
    int gap;
    int singleRowModeGridHeight;

    int replaceBoxMinHeightPercent;
    int utilityButtonsMinWidthPercent;
    int modeGridMinHeightPercent;
    int findDocumentGridMinHeightPercent;
    int replaceDocumentGridMinHeightPercent;
    int replaceSelectionGridMinHeightPercent;
    int leftModePanelMinWidthPercent;
    int leftModePanelMinGroupWidthPercent;

    int autoLayoutEnabled;
    int showReplaceBox;
    int showUtilityButtons;
    int showModeGrid;
    int showFindDocumentGrid;
    int showReplaceDocumentGrid;
    int showReplaceSelectionGrid;

    int showCounts;
    int showZeroCounts;
    int countInParentheses;

    int enableLeftModePanel;
    int enableGrowingBorder;
    int enableRecentFindDropdown;
    int enableRecentReplaceDropdown;

    int fauxComboPlaceholderLarge;
    int actionButtonCountColorEnabled;

    COLORREF overlayBackColor;
    COLORREF overlayBorderColor;
    COLORREF overlayTitleColor;
    COLORREF labelColor;
    COLORREF textColor;
    COLORREF mutedTextColor;
    COLORREF accentColor;
    COLORREF buttonBackColor;
    COLORREF buttonBorderColor;
    COLORREF buttonTextColor;
    COLORREF countTextColor;
} NewTestLayoutSettingsConfig;

void NewTestLayoutSettings_GetDefaultConfig(
    NewTestLayoutSettingsConfig *config
);

NewTestLayoutSettingsPanel *NewTestLayoutSettings_Create(
    HWND parent,
    HINSTANCE hInstance,
    const NewTestLayoutSettingsConfig *initialConfig,
    NewTestLayoutSettingsChangedCallback changedCallback,
    void *changedUserData
);

void NewTestLayoutSettings_Destroy(
    NewTestLayoutSettingsPanel *panel
);

void NewTestLayoutSettings_SetConfig(
    NewTestLayoutSettingsPanel *panel,
    const NewTestLayoutSettingsConfig *config
);

void NewTestLayoutSettings_GetConfig(
    NewTestLayoutSettingsPanel *panel,
    NewTestLayoutSettingsConfig *config
);

void NewTestLayoutSettings_Show(
    NewTestLayoutSettingsPanel *panel,
    int show
);

void NewTestLayoutSettings_Toggle(
    NewTestLayoutSettingsPanel *panel
);

int NewTestLayoutSettings_IsVisible(
    NewTestLayoutSettingsPanel *panel
);

void NewTestLayoutSettings_Layout(
    NewTestLayoutSettingsPanel *panel,
    const RECT *parentClientRect
);

int NewTestLayoutSettings_HandleCommand(
    NewTestLayoutSettingsPanel *panel,
    WPARAM wParam,
    LPARAM lParam
);

int NewTestLayoutSettings_HandleNotify(
    NewTestLayoutSettingsPanel *panel,
    WPARAM wParam,
    LPARAM lParam
);

int NewTestLayoutSettings_HandleMouseWheel(
    NewTestLayoutSettingsPanel *panel,
    WPARAM wParam,
    LPARAM lParam
);

int NewTestLayoutSettings_HandleCtlColorStatic(
    NewTestLayoutSettingsPanel *panel,
    HDC hdc,
    HWND hwnd,
    LRESULT *result
);

void NewTestLayoutSettings_RequestRefresh(
    NewTestLayoutSettingsPanel *panel
);

#endif