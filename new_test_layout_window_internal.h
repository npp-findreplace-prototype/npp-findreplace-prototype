#ifndef NEW_TEST_LAYOUT_WINDOW_INTERNAL_H
#define NEW_TEST_LAYOUT_WINDOW_INTERNAL_H

#include "win_compat.h"
#include "ui_common.h"
#include "ui_drawing.h"

#include <stdio.h>
#include <stdlib.h>

#include "new_test_layout_window.h"
#include "new_test_layout_controls.h"
#include "new_test_layout_settings.h"

#include "button_grid.h"
#include "npp_mockup_window.h"
#include "debug_window.h"

#define NEW_TEST_LAYOUT_WINDOW_CLASS_NAME "NewTestLayoutWindowClass"
#define NEW_TEST_LAYOUT_WINDOW_TITLE "New Test Layout"

#define NTL_TIMER_ID 1
#define NTL_TIMER_MS 300

#define NTL_MARGIN 12
#define NTL_GAP 8

#define NTL_FAUX_COMBO_HEIGHT 48
#define NTL_ACTION_GROUP_HEIGHT 84

#define NTL_UTILITY_BUTTON_SQUARE_MIN 28
#define NTL_UTILITY_BUTTON_SQUARE_MAX 42
#define NTL_UTILITY_BUTTON_GAP 4

#define NTL_MODE_SINGLE_BUTTON_SIZE 30
#define NTL_MODE_SINGLE_SPACING 4

#define NTL_MODE_LEFT_COLUMNS 4
#define NTL_MODE_LEFT_ROWS 3
#define NTL_MODE_LEFT_BUTTON_SIZE 30
#define NTL_MODE_LEFT_SPACING 4

#define ID_NTL_FIND_COMBO 7001
#define ID_NTL_REPLACE_COMBO 7002

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

typedef struct NewTestLayoutLayoutState
{
    RECT clientRect;

    int width;
    int height;
    int modeGridHeight;

    int showUtilityButtons;
    int comboRight;

    int groupX;
    int groupY;
    int groupWidth;
} NewTestLayoutLayoutState;

/* shared window state */
extern HINSTANCE g_ntl_hInstance;
extern HWND g_ntl_window;
extern NewTestLayoutWindowClosedCallback g_ntl_onClosed;

/* shared theme/settings state */
extern NewTestLayoutTheme g_ntl_theme;
extern NewTestLayoutSettingsConfig g_ntl_settingsConfig;
extern NewTestLayoutSettingsPanel *g_ntl_settingsPanel;

/* shared top input controls */
extern NewTestLayoutFauxCombo *g_ntl_findCombo;
extern NewTestLayoutFauxCombo *g_ntl_replaceCombo;

/* shared utility buttons */
extern NewTestLayoutActionButton *g_ntl_copyToReplaceButton;
extern NewTestLayoutActionButton *g_ntl_swapFindReplaceButton;
extern NewTestLayoutActionButton *g_ntl_copyToFindButton;

/* shared action groups */
extern NewTestLayoutActionGroup *g_ntl_findGroup;
extern NewTestLayoutActionGroup *g_ntl_replaceGroup;
extern NewTestLayoutActionGroup *g_ntl_selectionGroup;

/* shared mode grid */
extern HWND g_ntl_modeGrid;
extern ButtonGridConfig g_ntl_modeGridConfig;
extern ButtonGridItemConfig g_ntl_modeGridItems[12];

/* shared dynamic counts/text */
extern char g_ntl_lastFindText[512];
extern NewTestLayoutCounts g_ntl_counts;

/* shared painting resources */
extern HBRUSH g_ntl_backBrush;

void NewTestLayout_CopyText(
    char *dest,
    int destSize,
    const char *src
);

int NewTestLayout_MinInt(
    int a,
    int b
);

int NewTestLayout_ClampInt(
    int value,
    int minValue,
    int maxValue
);

void NewTestLayout_SetRect(
    RECT *rc,
    int left,
    int top,
    int right,
    int bottom
);

int NewTestLayout_GetSingleRowModeGridHeight(void);
int NewTestLayout_GetLeftModeGridWidth(void);
int NewTestLayout_GetLeftModeGridHeight(void);

void NewTestLayout_ToggleSettings(void);

void NewTestLayout_CreateModeGrid(
    HWND hwnd
);

void NewTestLayout_GetFindText(
    char *buffer,
    int bufferSize
);

void NewTestLayout_GetReplaceText(
    char *buffer,
    int bufferSize
);

void NewTestLayout_UpdateWindowTitle(void);

void NewTestLayout_ApplyCounts(void);
void NewTestLayout_ApplyCountOptions(void);

void NewTestLayout_CreateControls(
    HWND hwnd
);

void NewTestLayout_DestroyControls(void);

void NewTestLayout_ComputeVisibility(
    int width,
    int height,
    NewTestLayoutVisibility *visibility
);

void NewTestLayout_LayoutCombosAndUtility(
    const NewTestLayoutVisibility *visibility,
    NewTestLayoutLayoutState *layout
);

void NewTestLayout_LayoutModeGrid(
    const NewTestLayoutVisibility *visibility,
    NewTestLayoutLayoutState *layout
);

void NewTestLayout_LayoutGroups(
    const NewTestLayoutVisibility *visibility,
    NewTestLayoutLayoutState *layout
);

void NewTestLayout_ShowUtilityButtons(
    int show
);

void NewTestLayout_ShowGroups(
    const NewTestLayoutVisibility *visibility
);

void NewTestLayout_RedrawUtilityButtons(void);
void NewTestLayout_RedrawImportantControls(void);

void NewTestLayout_Layout(
    HWND hwnd
);

int NewTestLayout_HandleCommand(
    WPARAM wParam,
    LPARAM lParam
);

#endif