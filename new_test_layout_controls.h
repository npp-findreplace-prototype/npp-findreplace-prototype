#ifndef NEW_TEST_LAYOUT_CONTROLS_H
#define NEW_TEST_LAYOUT_CONTROLS_H

#include <windows.h>

#define NTL_CONTROLS_MAX_TEXT 512
#define NTL_CONTROLS_MAX_RECENT_ITEMS 64
#define NTL_CONTROLS_MAX_ACTION_BUTTONS 4

#define NTL_FCN_TEXT_CHANGED 1
#define NTL_FCN_DROPDOWN_SHOWN 2
#define NTL_FCN_DROPDOWN_HIDDEN 3
#define NTL_FCN_RECENT_SELECTED 4
#define NTL_FCN_SET_FOCUS 5
#define NTL_FCN_KILL_FOCUS 6

#define NTL_ACTION_BN_CLICKED 1

typedef struct NewTestLayoutTheme
{
    COLORREF windowBackColor;
    COLORREF panelBackColor;
    COLORREF panelBorderColor;

    COLORREF editBackColor;
    COLORREF editBorderColor;
    COLORREF editTextColor;
    COLORREF placeholderColor;
    COLORREF arrowColor;

    COLORREF buttonBackColor;
    COLORREF buttonBorderColor;
    COLORREF buttonTextColor;
    COLORREF countTextColor;

    COLORREF groupBackColor;
    COLORREF groupBorderColor;
    COLORREF groupTitleColor;

    COLORREF gearColor;
    COLORREF gearBackColor;
    COLORREF gearBorderColor;

    HFONT normalFont;
    HFONT monoFont;
    HFONT placeholderFont;
    HFONT buttonFont;
    HFONT titleFont;
} NewTestLayoutTheme;

typedef struct NewTestLayoutFauxCombo NewTestLayoutFauxCombo;
typedef struct NewTestLayoutActionButton NewTestLayoutActionButton;
typedef struct NewTestLayoutActionGroup NewTestLayoutActionGroup;
typedef struct NewTestLayoutGearButton NewTestLayoutGearButton;

typedef struct NewTestLayoutActionButtonDef
{
    int id;
    const char *label;
    int count;
    int hasCount;
} NewTestLayoutActionButtonDef;

void NewTestLayoutTheme_GetDefault(
    NewTestLayoutTheme *theme
);

void NewTestLayoutTheme_CreateDefaultFonts(
    NewTestLayoutTheme *theme
);

void NewTestLayoutTheme_DeleteFonts(
    NewTestLayoutTheme *theme
);

NewTestLayoutFauxCombo *NewTestLayoutFauxCombo_Create(
    HWND parent,
    HINSTANCE hInstance,
    int id,
    const char *placeholder,
    const NewTestLayoutTheme *theme
);

void NewTestLayoutFauxCombo_Destroy(
    NewTestLayoutFauxCombo *combo
);

HWND NewTestLayoutFauxCombo_GetHwnd(
    NewTestLayoutFauxCombo *combo
);

HWND NewTestLayoutFauxCombo_GetEditHwnd(
    NewTestLayoutFauxCombo *combo
);

void NewTestLayoutFauxCombo_SetTheme(
    NewTestLayoutFauxCombo *combo,
    const NewTestLayoutTheme *theme
);

void NewTestLayoutFauxCombo_SetRect(
    NewTestLayoutFauxCombo *combo,
    const RECT *rect
);

void NewTestLayoutFauxCombo_Show(
    NewTestLayoutFauxCombo *combo,
    int show
);

int NewTestLayoutFauxCombo_IsVisible(
    NewTestLayoutFauxCombo *combo
);

void NewTestLayoutFauxCombo_SetText(
    NewTestLayoutFauxCombo *combo,
    const char *text
);

void NewTestLayoutFauxCombo_GetText(
    NewTestLayoutFauxCombo *combo,
    char *buffer,
    int bufferSize
);

int NewTestLayoutFauxCombo_GetTextLength(
    NewTestLayoutFauxCombo *combo
);

void NewTestLayoutFauxCombo_SetPlaceholder(
    NewTestLayoutFauxCombo *combo,
    const char *placeholder
);

void NewTestLayoutFauxCombo_SetPlaceholderLarge(
    NewTestLayoutFauxCombo *combo,
    int large
);

void NewTestLayoutFauxCombo_SetRecentItems(
    NewTestLayoutFauxCombo *combo,
    const char **items,
    int itemCount
);

void NewTestLayoutFauxCombo_ShowDropdown(
    NewTestLayoutFauxCombo *combo,
    int show
);

void NewTestLayoutFauxCombo_ToggleDropdown(
    NewTestLayoutFauxCombo *combo
);

int NewTestLayoutFauxCombo_IsDropdownVisible(
    NewTestLayoutFauxCombo *combo
);

void NewTestLayoutFauxCombo_HandleParentCommand(
    NewTestLayoutFauxCombo *combo,
    WPARAM wParam,
    LPARAM lParam
);

int NewTestLayoutFauxCombo_IsChildWindow(
    NewTestLayoutFauxCombo *combo,
    HWND hwnd
);

NewTestLayoutActionButton *NewTestLayoutActionButton_Create(
    HWND parent,
    HINSTANCE hInstance,
    int id,
    const char *label,
    const NewTestLayoutTheme *theme
);

void NewTestLayoutActionButton_Destroy(
    NewTestLayoutActionButton *button
);

HWND NewTestLayoutActionButton_GetHwnd(
    NewTestLayoutActionButton *button
);

void NewTestLayoutActionButton_SetTheme(
    NewTestLayoutActionButton *button,
    const NewTestLayoutTheme *theme
);

void NewTestLayoutActionButton_SetRect(
    NewTestLayoutActionButton *button,
    const RECT *rect
);

void NewTestLayoutActionButton_Show(
    NewTestLayoutActionButton *button,
    int show
);

int NewTestLayoutActionButton_IsVisible(
    NewTestLayoutActionButton *button
);

void NewTestLayoutActionButton_SetText(
    NewTestLayoutActionButton *button,
    const char *label
);

void NewTestLayoutActionButton_SetCount(
    NewTestLayoutActionButton *button,
    int count,
    int hasCount
);

void NewTestLayoutActionButton_SetCountOptions(
    NewTestLayoutActionButton *button,
    int showCounts,
    int showZeroCounts,
    int countInParentheses,
    int colorCountText
);

void NewTestLayoutActionButton_SetEnabled(
    NewTestLayoutActionButton *button,
    int enabled
);

NewTestLayoutActionGroup *NewTestLayoutActionGroup_Create(
    HWND parent,
    HINSTANCE hInstance,
    int id,
    const char *title,
    const NewTestLayoutActionButtonDef *buttons,
    int buttonCount,
    const NewTestLayoutTheme *theme
);

void NewTestLayoutActionGroup_Destroy(
    NewTestLayoutActionGroup *group
);

HWND NewTestLayoutActionGroup_GetHwnd(
    NewTestLayoutActionGroup *group
);

void NewTestLayoutActionGroup_SetTheme(
    NewTestLayoutActionGroup *group,
    const NewTestLayoutTheme *theme
);

void NewTestLayoutActionGroup_SetRect(
    NewTestLayoutActionGroup *group,
    const RECT *rect
);

void NewTestLayoutActionGroup_Show(
    NewTestLayoutActionGroup *group,
    int show
);

int NewTestLayoutActionGroup_IsVisible(
    NewTestLayoutActionGroup *group
);

void NewTestLayoutActionGroup_SetTitle(
    NewTestLayoutActionGroup *group,
    const char *title
);

void NewTestLayoutActionGroup_SetBorderVisible(
    NewTestLayoutActionGroup *group,
    int visible
);

void NewTestLayoutActionGroup_SetPadding(
    NewTestLayoutActionGroup *group,
    int padding
);

void NewTestLayoutActionGroup_SetButtonCount(
    NewTestLayoutActionGroup *group,
    int buttonIndex,
    int count,
    int hasCount
);

void NewTestLayoutActionGroup_SetCountOptions(
    NewTestLayoutActionGroup *group,
    int showCounts,
    int showZeroCounts,
    int countInParentheses,
    int colorCountText
);

void NewTestLayoutActionGroup_LayoutButtons(
    NewTestLayoutActionGroup *group
);

NewTestLayoutGearButton *NewTestLayoutGearButton_Create(
    HWND parent,
    HINSTANCE hInstance,
    int id,
    const NewTestLayoutTheme *theme
);

void NewTestLayoutGearButton_Destroy(
    NewTestLayoutGearButton *gear
);

HWND NewTestLayoutGearButton_GetHwnd(
    NewTestLayoutGearButton *gear
);

void NewTestLayoutGearButton_SetTheme(
    NewTestLayoutGearButton *gear,
    const NewTestLayoutTheme *theme
);

void NewTestLayoutGearButton_SetRect(
    NewTestLayoutGearButton *gear,
    const RECT *rect
);

void NewTestLayoutGearButton_Show(
    NewTestLayoutGearButton *gear,
    int show
);

int NewTestLayoutGearButton_IsVisible(
    NewTestLayoutGearButton *gear
);

int NewTestLayoutControls_RegisterClasses(
    HINSTANCE hInstance
);

void NewTestLayoutControls_UnregisterClasses(
    HINSTANCE hInstance
);

#endif