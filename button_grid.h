#ifndef BUTTON_GRID_H
#define BUTTON_GRID_H

#include <windows.h>

#define BUTTON_GRID_DEFAULT_BUTTON_COUNT 16

#define BUTTON_GRID_DEFAULT_BUTTON_WIDTH 90
#define BUTTON_GRID_DEFAULT_BUTTON_HEIGHT 90

#define BUTTON_GRID_DEFAULT_HORIZONTAL_SPACING 10
#define BUTTON_GRID_DEFAULT_VERTICAL_SPACING 10

#define BUTTON_GRID_LAYOUT_HORIZONTAL 0
#define BUTTON_GRID_LAYOUT_VERTICAL 1

#define BUTTON_GRID_DEFAULT_LAYOUT BUTTON_GRID_LAYOUT_HORIZONTAL

#define BUTTON_GRID_DEFAULT_NAME_PREFIX "mybutton_"
#define BUTTON_GRID_DEFAULT_TEXT_FORMAT "%d"
#define BUTTON_GRID_DEFAULT_CLICK_IDENTIFIER_FORMAT "%s"

#define BUTTON_GRID_DEFAULT_BACK_COLOR RGB(192, 192, 192)
#define BUTTON_GRID_DEFAULT_FORE_COLOR RGB(0, 0, 0)

#define BUTTON_GRID_DEFAULT_ID_BASE 1000
#define BUTTON_GRID_DEFAULT_FIRST_INDEX 1

#define BUTTON_GRID_DEFAULT_USE_PICTURES 1
#define BUTTON_GRID_DEFAULT_TOGGLE_ON_CLICK 1
#define BUTTON_GRID_DEFAULT_STATE 0
#define BUTTON_GRID_DEFAULT_STRETCH_PICTURES 1

#define BUTTON_GRID_DEFAULT_OFF_PICTURE_COLOR RGB(150, 150, 150)
#define BUTTON_GRID_DEFAULT_ON_PICTURE_COLOR RGB(80, 190, 80)

typedef void (*ButtonGridClickCallback)(const char *controlName);

typedef struct ButtonGridConfig
{
    int buttonCount;

    int buttonWidth;
    int buttonHeight;

    int horizontalSpacing;
    int verticalSpacing;

    int layout;

    int idBase;
    int firstIndex;

    const char *namePrefix;
    const char *textFormat;
    const char *clickIdentifierFormat;

    COLORREF backColor;
    COLORREF foreColor;

    int usePictures;
    int toggleOnClick;
    int defaultState;
    int stretchPictures;

    HBITMAP pictureOff;
    HBITMAP pictureOn;

    int pictureOffLoadFailed;
    int pictureOnLoadFailed;

    COLORREF generatedOffPictureColor;
    COLORREF generatedOnPictureColor;
} ButtonGridConfig;

void ButtonGrid_GetDefaultConfig(ButtonGridConfig *config);

BOOL ButtonGrid_RegisterClass(HINSTANCE hInstance);

HWND ButtonGrid_CreateEx(
    HWND parent,
    HINSTANCE hInstance,
    int x,
    int y,
    int width,
    int height,
    const ButtonGridConfig *config,
    ButtonGridClickCallback onClick
);

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

void ButtonGrid_SetPictures(
    HWND gridHwnd,
    HBITMAP pictureOff,
    HBITMAP pictureOn,
    int stretchPictures
);

void ButtonGrid_SetButtonStateByNumber(
    HWND gridHwnd,
    int buttonNumber,
    int isOn
);

int ButtonGrid_GetButtonStateByNumber(
    HWND gridHwnd,
    int buttonNumber
);

void ButtonGrid_ToggleButtonStateByNumber(
    HWND gridHwnd,
    int buttonNumber
);

#endif