#ifndef NEW_TEST_LAYOUT_CONTROLS_INTERNAL_H
#define NEW_TEST_LAYOUT_CONTROLS_INTERNAL_H

#include <windows.h>
#include <stdlib.h>

#include "new_test_layout_controls.h"

#ifndef GWLP_USERDATA
#define GWLP_USERDATA (-21)
#endif

#ifndef GWLP_WNDPROC
#define GWLP_WNDPROC (-4)
#endif

#ifndef IDC_HAND
#define IDC_HAND MAKEINTRESOURCE(32649)
#endif

#ifndef EM_SETRECT
#define EM_SETRECT 0x00B3
#endif

#ifndef VK_RETURN
#define VK_RETURN 0x0D
#endif

#ifndef VK_SPACE
#define VK_SPACE 0x20
#endif

#define NTL_FAUX_COMBO_CLASS_NAME "NewTestLayoutFauxComboClass"
#define NTL_ACTION_BUTTON_CLASS_NAME "NewTestLayoutActionButtonClass"
#define NTL_ACTION_GROUP_CLASS_NAME "NewTestLayoutActionGroupClass"
#define NTL_GEAR_BUTTON_CLASS_NAME "NewTestLayoutGearButtonClass"

#define NTL_FAUX_COMBO_ARROW_WIDTH 34
#define NTL_FAUX_COMBO_DROP_HEIGHT 160
#define NTL_FAUX_COMBO_EDIT_MARGIN 8
#define NTL_FAUX_COMBO_LIST_ID_OFFSET 20000

typedef void (*NtlBufferedDrawProc)(void *context, HDC hdc, RECT *rc);

struct NewTestLayoutFauxCombo
{
    HWND parent;
    HWND hwnd;
    HWND edit;
    HWND list;

    WNDPROC oldEditProc;

    HINSTANCE hInstance;
    int id;

    char placeholder[NTL_CONTROLS_MAX_TEXT];
    char recentItems[NTL_CONTROLS_MAX_RECENT_ITEMS][NTL_CONTROLS_MAX_TEXT];
    int recentItemCount;

    NewTestLayoutTheme theme;

    HBRUSH editBrush;

    int visible;
    int dropdownVisible;
    int placeholderLarge;
};

struct NewTestLayoutActionButton
{
    HWND parent;
    HWND hwnd;

    HINSTANCE hInstance;
    int id;

    char label[NTL_CONTROLS_MAX_TEXT];

    int count;
    int hasCount;
    int showCounts;
    int showZeroCounts;
    int countInParentheses;
    int colorCountText;

    int visible;
    int enabled;
    int hot;
    int pressed;

    NewTestLayoutTheme theme;
};

struct NewTestLayoutActionGroup
{
    HWND parent;
    HWND hwnd;

    HINSTANCE hInstance;
    int id;

    char title[NTL_CONTROLS_MAX_TEXT];

    NewTestLayoutActionButton *buttons[NTL_CONTROLS_MAX_ACTION_BUTTONS];
    int buttonCount;

    int visible;
    int borderVisible;
    int padding;

    NewTestLayoutTheme theme;
};

struct NewTestLayoutGearButton
{
    HWND parent;
    HWND hwnd;

    HINSTANCE hInstance;
    int id;

    int visible;
    int hot;
    int pressed;

    NewTestLayoutTheme theme;
};

void Ntl_CopyText(
    char *dest,
    int destSize,
    const char *src
);

void Ntl_CopyTheme(
    NewTestLayoutTheme *dest,
    const NewTestLayoutTheme *src
);

void Ntl_SendCommand(
    HWND parent,
    HWND hwnd,
    int id,
    int notifyCode
);

void Ntl_FillRect(
    HDC hdc,
    const RECT *rc,
    COLORREF color
);

void Ntl_DrawBorder(
    HDC hdc,
    const RECT *rc,
    COLORREF color
);

void Ntl_DoubleBufferedPaint(
    HWND hwnd,
    void *context,
    NtlBufferedDrawProc drawProc
);

LRESULT CALLBACK NewTestLayoutFauxCombo_WndProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam
);

LRESULT CALLBACK NewTestLayoutActionButton_WndProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam
);

LRESULT CALLBACK NewTestLayoutActionGroup_WndProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam
);

LRESULT CALLBACK NewTestLayoutGearButton_WndProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam
);

#endif