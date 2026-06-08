#ifndef NEW_TEST_LAYOUT_WINDOW_INTERNAL_H
#define NEW_TEST_LAYOUT_WINDOW_INTERNAL_H

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

extern HINSTANCE g_hInstance;
extern HWND g_window;

extern NewTestLayoutWindowClosedCallback g_onClosed;

extern NewTestLayoutTheme g_theme;
extern NewTestLayoutSettingsConfig g_settingsConfig;
extern NewTestLayoutSettingsPanel *g_settingsPanel;

extern NewTestLayoutFauxCombo *g_findCombo;
extern NewTestLayoutFauxCombo *g_replaceCombo;

extern NewTestLayoutActionButton *g_copyToReplaceButton;
extern NewTestLayoutActionButton *g_swapFindReplaceButton;
extern NewTestLayoutActionButton *g_copyToFindButton;

extern NewTestLayoutActionGroup *g_findGroup;
extern NewTestLayoutActionGroup *g_replaceGroup;
extern NewTestLayoutActionGroup *g_selectionGroup;

extern HWND g_modeGrid;
extern ButtonGridConfig g_modeGridConfig;
extern ButtonGridItemConfig g_modeGridItems[12];

extern char g_lastFindText[512];
extern NewTestLayoutCounts g_counts;

extern HBRUSH g_backBrush;

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

void NewTestLayout_UpdateWindowTitle(void);
void NewTestLayout_ToggleSettings(void);

void NewTestLayout_GetFindText(
    char *buffer,
    int bufferSize
);

void NewTestLayout_GetReplaceText(
    char *buffer,
    int bufferSize
);

void NewTestLayout_ApplyCountOptions(void);
void NewTestLayout_ApplyCounts(void);

void NewTestLayout_CreateModeGrid(HWND hwnd);
void NewTestLayout_CreateControls(HWND hwnd);
void NewTestLayout_DestroyControls(void);

void NewTestLayout_Layout(HWND hwnd);

int NewTestLayout_HandleCommand(
    WPARAM wParam,
    LPARAM lParam
);

#endif