#ifndef BUTTON_GRID_SETTINGS_INTERNAL_H
#define BUTTON_GRID_SETTINGS_INTERNAL_H

#include <windows.h>
#include <commctrl.h>
#include <stddef.h>

#include "button_grid_internal.h"
#include "button_grid_settings.h"

#ifndef GA_ROOT
#define GA_ROOT 2
#endif

#ifndef TRACKBAR_CLASSA
#define TRACKBAR_CLASSA "msctls_trackbar32"
#endif

#ifndef TBM_SETRANGE
#define TBM_SETRANGE (WM_USER + 6)
#endif

#ifndef TBM_SETPOS
#define TBM_SETPOS (WM_USER + 5)
#endif

#ifndef TBM_GETPOS
#define TBM_GETPOS (WM_USER)
#endif

#ifndef TBS_AUTOTICKS
#define TBS_AUTOTICKS 0x0001
#endif

#ifndef TBS_TOOLTIPS
#define TBS_TOOLTIPS 0x0100
#endif

#define BUTTON_GRID_SETTINGS_CLASS_NAME "ButtonGridSettingsWindowClass"
#define BUTTON_GRID_SETTINGS_PROP_NAME "ButtonGridSettingsGrid"
#define BUTTON_GRID_SETTINGS_OLDPROC_PROP "ButtonGridSettingsOldProc"

#define BG_SETTINGS_ID_CLOSE 5001
#define BG_SETTINGS_ID_FILTER_LABEL 5002
#define BG_SETTINGS_ID_FILTER 5003
#define BG_SETTINGS_ID_FILTER_CLEAR 5004

#define BG_SETTINGS_ID_BASE 6000
#define BG_SETTINGS_ID_STEP 10

#define BG_SETTINGS_PART_LABEL 0
#define BG_SETTINGS_PART_PRIMARY 1
#define BG_SETTINGS_PART_RAW 2

#define BG_SETTINGS_WINDOW_WIDTH 760
#define BG_SETTINGS_WINDOW_HEIGHT 520
#define BG_SETTINGS_WINDOW_GAP 12

#define BG_SETTINGS_TOP 76
#define BG_SETTINGS_LEFT 12
#define BG_SETTINGS_ROW_HEIGHT 34
#define BG_SETTINGS_LABEL_WIDTH 170
#define BG_SETTINGS_MIN_LABEL_WIDTH 95
#define BG_SETTINGS_PRIMARY_WIDTH 180
#define BG_SETTINGS_MIN_PRIMARY_WIDTH 70
#define BG_SETTINGS_RAW_WIDTH 180
#define BG_SETTINGS_MIN_RAW_WIDTH 105
#define BG_SETTINGS_GAP 8
#define BG_SETTINGS_BOTTOM_PADDING 16

#define BG_SETTING_BOOL 1
#define BG_SETTING_INT 2
#define BG_SETTING_ENUM 3
#define BG_SETTING_COLOR 4
#define BG_SETTING_TEXT 5
#define BG_SETTING_THEME 6

typedef struct ButtonGridSettingOption
{
    const char *label;
    int value;
} ButtonGridSettingOption;

typedef struct ButtonGridSettingDefinition
{
    const char *key;
    const char *label;
    int type;
    size_t offset;
    int minValue;
    int maxValue;
    const ButtonGridSettingOption *options;
} ButtonGridSettingDefinition;

/* schema */
const ButtonGridSettingDefinition *ButtonGrid_SettingsGetDefinition(int index);
int ButtonGrid_SettingsGetCount(void);

/* window */
ButtonGrid *ButtonGrid_SettingsGetGrid(HWND hwnd);
void ButtonGrid_SettingsSetGrid(HWND hwnd, ButtonGrid *grid);

/* control ids / lookup */
int ButtonGrid_SettingsGetLabelId(int index);
int ButtonGrid_SettingsGetPrimaryId(int index);
int ButtonGrid_SettingsGetRawId(int index);
int ButtonGrid_SettingsIdToIndex(int id);
int ButtonGrid_SettingsIdToPart(int id);

HWND ButtonGrid_SettingsGetLabelControl(HWND pageHwnd, int index);
HWND ButtonGrid_SettingsGetPrimaryControl(HWND pageHwnd, int index);
HWND ButtonGrid_SettingsGetRawControl(HWND pageHwnd, int index);

/* controls */
void ButtonGrid_SettingsCreateControls(HWND pageHwnd, ButtonGrid *grid);

void ButtonGrid_SettingsRefreshOne(HWND pageHwnd, int index, ButtonGrid *grid);
void ButtonGrid_SettingsRefreshAll(HWND pageHwnd);

void ButtonGrid_SettingsApplyRawEdit(HWND pageHwnd, int index);
void ButtonGrid_SettingsApplyPrimary(HWND pageHwnd, int index);

int ButtonGrid_SettingsHandleCommand(HWND hwnd, WPARAM wParam, LPARAM lParam);
int ButtonGrid_SettingsHandleHScroll(HWND hwnd, WPARAM wParam, LPARAM lParam);

int ButtonGrid_SettingsControlsAreUpdating(void);
void ButtonGrid_SettingsBeginControlUpdate(void);
void ButtonGrid_SettingsEndControlUpdate(void);

/* layout / filtering / scrolling */
void ButtonGrid_SettingsLayoutControls(HWND pageHwnd);
void ButtonGrid_SettingsSetScrollPos(HWND pageHwnd, int pos);
void ButtonGrid_SettingsScroll(HWND pageHwnd, int request, int wheelDelta);

/* values */
int ButtonGrid_SettingsClampInt(int value, int minValue, int maxValue);

int ButtonGrid_SettingsGetIntField(
    ButtonGrid *grid,
    const ButtonGridSettingDefinition *def
);

void ButtonGrid_SettingsSetIntField(
    ButtonGrid *grid,
    const ButtonGridSettingDefinition *def,
    int value
);

COLORREF ButtonGrid_SettingsGetColorField(
    ButtonGrid *grid,
    const ButtonGridSettingDefinition *def
);

void ButtonGrid_SettingsSetColorField(
    ButtonGrid *grid,
    const ButtonGridSettingDefinition *def,
    COLORREF value
);

char *ButtonGrid_SettingsGetTextField(
    ButtonGrid *grid,
    const ButtonGridSettingDefinition *def
);

void ButtonGrid_SettingsWriteRawText(HWND pageHwnd, int index, ButtonGrid *grid);

void ButtonGrid_SettingsApplyGridChange(ButtonGrid *grid);

void ButtonGrid_SettingsApplyValue(
    HWND pageHwnd,
    int index,
    const char *textValue,
    int valueFromPrimary,
    int usePrimary
);

/* child-control subclassing */
void ButtonGrid_SettingsSubclassWheelControl(HWND hwnd);

#endif