#ifndef BUTTON_GRID_H
#define BUTTON_GRID_H

#include <windows.h>

#define BUTTON_COUNT 16
#define BUTTON_GRID_COUNT BUTTON_COUNT

#define BUTTON_GRID_DEFAULT_BUTTON_WIDTH 90
#define BUTTON_GRID_DEFAULT_BUTTON_HEIGHT 90

#define BUTTON_GRID_DEFAULT_HORIZONTAL_SPACING 10
#define BUTTON_GRID_DEFAULT_VERTICAL_SPACING 10

#define BUTTON_GRID_LAYOUT_HORIZONTAL 0
#define BUTTON_GRID_LAYOUT_VERTICAL 1

#define horizontal BUTTON_GRID_LAYOUT_HORIZONTAL
#define vertical BUTTON_GRID_LAYOUT_VERTICAL
#define button_layout horizontal

#define BUTTON_GRID_DEFAULT_LAYOUT button_layout

#define BUTTON_GRID_NAME_PREFIX "mybutton_"
#define BUTTON_GRID_TEXT_FORMAT "%d"
#define BUTTON_GRID_CLICK_IDENTIFIER_FORMAT "%s"

#define BUTTON_GRID_BACK_COLOR RGB(192, 192, 192)
#define BUTTON_GRID_FORE_COLOR RGB(0, 0, 0)

#define ismetafile 0
#define BUTTON_GRID_DEFAULT_IS_METAFILE ismetafile

#define BUTTON_ID_BASE 1000
#define BUTTON_FIRST_INDEX 1

typedef void (*ButtonGridClickCallback)(const char *controlName);

BOOL ButtonGrid_RegisterClass(HINSTANCE hInstance);

HWND ButtonGrid_Create(
    HWND parent,
    HINSTANCE hInstance,
    int x,
    int y,
    int width,
    int height,
    ButtonGridClickCallback onClick
);

void ButtonGrid_SetRect(
    HWND gridHwnd,
    int x,
    int y,
    int width,
    int height
);

void ButtonGrid_SetButtonSize(
    HWND gridHwnd,
    int buttonWidth,
    int buttonHeight
);

void ButtonGrid_SetSpacing(
    HWND gridHwnd,
    int horizontalSpacing,
    int verticalSpacing
);

void ButtonGrid_SetLayout(
    HWND gridHwnd,
    int layout
);

void ButtonGrid_Relayout(HWND gridHwnd);

#endif