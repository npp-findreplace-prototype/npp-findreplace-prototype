#ifndef BUTTON_GRID_H
#define BUTTON_GRID_H

#include <windows.h>

typedef struct AppImage AppImage;

#define BUTTON_GRID_DEFAULT_BUTTON_COUNT 16

#define BUTTON_GRID_DEFAULT_BUTTON_WIDTH 90
#define BUTTON_GRID_DEFAULT_BUTTON_HEIGHT 90

#define BUTTON_GRID_DEFAULT_HORIZONTAL_SPACING 10
#define BUTTON_GRID_DEFAULT_VERTICAL_SPACING 10

#define BUTTON_GRID_LAYOUT_HORIZONTAL 0
#define BUTTON_GRID_LAYOUT_VERTICAL 1

#define BUTTON_GRID_DEFAULT_LAYOUT BUTTON_GRID_LAYOUT_HORIZONTAL

#define BUTTON_GRID_BUTTON_TOGGLE 0
#define BUTTON_GRID_BUTTON_RADIO 1
#define BUTTON_GRID_BUTTON_DISABLED 2

#define BUTTON_GRID_SIZE_USE_DEFAULT -1
#define BUTTON_GRID_SIZE_FIXED 0
#define BUTTON_GRID_SIZE_MATCH_IMAGE_SIZE 1
#define BUTTON_GRID_SIZE_MATCH_IMAGE_ASPECT_HORIZONTAL 2
#define BUTTON_GRID_SIZE_MATCH_IMAGE_ASPECT_VERTICAL 3
#define BUTTON_GRID_SIZE_MATCH_IMAGE_ASPECT_BY_LAYOUT 4

#define BUTTON_GRID_TEXT_USE_DEFAULT -1
#define BUTTON_GRID_TEXT_HIDE 0
#define BUTTON_GRID_TEXT_SHOW 1

#define BUTTON_GRID_BUTTON_BACK_OPAQUE 0
#define BUTTON_GRID_BUTTON_BACK_TRANSPARENT 1
#define BUTTON_GRID_DEFAULT_BUTTON_BACK_MODE BUTTON_GRID_BUTTON_BACK_TRANSPARENT

#define BUTTON_GRID_DEFAULT_SHOW_BUTTON_BORDER 0
#define BUTTON_GRID_DEFAULT_BUTTON_BORDER_THICKNESS 1
#define BUTTON_GRID_DEFAULT_BUTTON_BORDER_COLOR RGB(0, 0, 0)

#define BUTTON_GRID_BORDER_STYLE_NONE 0
#define BUTTON_GRID_BORDER_STYLE_SIMPLE 1
#define BUTTON_GRID_BORDER_STYLE_ETCHED 2
#define BUTTON_GRID_BORDER_STYLE_ROUNDED 3
#define BUTTON_GRID_BORDER_STYLE_ETCHED_ROUNDED 4
#define BUTTON_GRID_BORDER_STYLE_CONTAINER 5
#define BUTTON_GRID_BORDER_STYLE_SUNKEN 6
#define BUTTON_GRID_BORDER_STYLE_RAISED 7
#define BUTTON_GRID_BORDER_STYLE_DOUBLE 8

#define BUTTON_GRID_GEAR_CORNER_TOP_LEFT 0
#define BUTTON_GRID_GEAR_CORNER_TOP_RIGHT 1
#define BUTTON_GRID_GEAR_CORNER_BOTTOM_LEFT 2
#define BUTTON_GRID_GEAR_CORNER_BOTTOM_RIGHT 3

#define BUTTON_GRID_ALIGN_TOP_LEFT 0
#define BUTTON_GRID_ALIGN_TOP 1
#define BUTTON_GRID_ALIGN_TOP_RIGHT 2
#define BUTTON_GRID_ALIGN_LEFT 3
#define BUTTON_GRID_ALIGN_CENTER 4
#define BUTTON_GRID_ALIGN_RIGHT 5
#define BUTTON_GRID_ALIGN_BOTTOM_LEFT 6
#define BUTTON_GRID_ALIGN_BOTTOM 7
#define BUTTON_GRID_ALIGN_BOTTOM_RIGHT 8
#define BUTTON_GRID_ALIGN_XY 9
#define BUTTON_GRID_ALIGN_PERCENT 10

#define BUTTON_GRID_DEFAULT_CONTENT_ALIGNMENT BUTTON_GRID_ALIGN_TOP_LEFT
#define BUTTON_GRID_DEFAULT_CONTENT_ALIGN_X 0
#define BUTTON_GRID_DEFAULT_CONTENT_ALIGN_Y 0
#define BUTTON_GRID_DEFAULT_CONTENT_ALIGN_PERCENT_X 50
#define BUTTON_GRID_DEFAULT_CONTENT_ALIGN_PERCENT_Y 50

#define BUTTON_GRID_THEME_NAME_SIZE 128
#define BUTTON_GRID_DEFAULT_THEME_NAME "darkmetalred"
#define BUTTON_GRID_DEFAULT_ALLOW_THEME_SELECTION 1

#define BUTTON_GRID_DEFAULT_DPI_SCALE_ENABLED 1
#define BUTTON_GRID_DEFAULT_DPI_BASE 96

#define BUTTON_GRID_DEFAULT_NAME_PREFIX "mybutton_"
#define BUTTON_GRID_DEFAULT_ACTION_PREFIX "mybutton_"
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

#define BUTTON_GRID_DEFAULT_SIZE_MODE BUTTON_GRID_SIZE_MATCH_IMAGE_ASPECT_BY_LAYOUT

#define BUTTON_GRID_DEFAULT_SHOW_TEXT 1
#define BUTTON_GRID_DEFAULT_HIDE_PARTIAL_BUTTONS 0
#define BUTTON_GRID_DEFAULT_RESIZE_IN_LAYOUT_STEPS 0
#define BUTTON_GRID_DEFAULT_SETTINGS_WHEEL_SCRUB 0

#define BUTTON_GRID_DEFAULT_SHOW_BORDER 0
#define BUTTON_GRID_DEFAULT_SHOW_BORDER_TITLE 1
#define BUTTON_GRID_DEFAULT_BORDER_TITLE ""
#define BUTTON_GRID_DEFAULT_BORDER_PADDING 8
#define BUTTON_GRID_DEFAULT_BORDER_TITLE_HEIGHT 20
#define BUTTON_GRID_DEFAULT_BORDER_TITLE_PADDING 6
#define BUTTON_GRID_DEFAULT_BORDER_TITLE_FONT_SIZE 0
#define BUTTON_GRID_DEFAULT_BORDER_TITLE_TRANSPARENT 1
#define BUTTON_GRID_DEFAULT_BORDER_TITLE_AUTO_BACK_COLOR 1
#define BUTTON_GRID_DEFAULT_BORDER_STYLE BUTTON_GRID_BORDER_STYLE_ETCHED
#define BUTTON_GRID_DEFAULT_BORDER_THICKNESS 1
#define BUTTON_GRID_DEFAULT_BORDER_CORNER_RADIUS 8
#define BUTTON_GRID_DEFAULT_BORDER_COLOR RGB(96, 96, 96)
#define BUTTON_GRID_DEFAULT_BORDER_LIGHT_COLOR RGB(255, 255, 255)
#define BUTTON_GRID_DEFAULT_BORDER_SHADOW_COLOR RGB(128, 128, 128)
#define BUTTON_GRID_DEFAULT_BORDER_TITLE_COLOR RGB(0, 0, 0)
#define BUTTON_GRID_DEFAULT_BORDER_TITLE_BACK_COLOR RGB(240, 240, 240)

#define BUTTON_GRID_DEFAULT_SHOW_GEAR_ICON 1
#define BUTTON_GRID_DEFAULT_GEAR_CORNER BUTTON_GRID_GEAR_CORNER_TOP_RIGHT
#define BUTTON_GRID_DEFAULT_GEAR_SIZE 22
#define BUTTON_GRID_DEFAULT_GEAR_MARGIN 8
#define BUTTON_GRID_DEFAULT_GEAR_COLOR RGB(60, 60, 60)
#define BUTTON_GRID_DEFAULT_GEAR_BACK_COLOR RGB(245, 245, 245)
#define BUTTON_GRID_DEFAULT_GEAR_BORDER_COLOR RGB(120, 120, 120)

#define BUTTON_GRID_DEFAULT_OFF_PICTURE_COLOR RGB(150, 150, 150)
#define BUTTON_GRID_DEFAULT_ON_PICTURE_COLOR RGB(80, 190, 80)
#define BUTTON_GRID_DEFAULT_ERROR_PICTURE_COLOR RGB(190, 100, 100)

typedef void (*ButtonGridClickCallback)(const char *actionName);

typedef struct ButtonGridItemConfig
{
    const char *name;
    const char *action;
    const char *text;
    const char *tooltip;
    const char *iconBaseName;

    int behavior;
    int radioGroup;
    int defaultState;

    int widthOverride;
    int heightOverride;
    int sizeModeOverride;

    int showTextOverride;

    AppImage *pictureOff;
    AppImage *pictureOn;

    int ownsPictureOff;
    int ownsPictureOn;

    int pictureOffLoadFailed;
    int pictureOnLoadFailed;
} ButtonGridItemConfig;

typedef struct ButtonGridConfig
{
    int buttonCount;

    const ButtonGridItemConfig *items;

    int buttonWidth;
    int buttonHeight;

    int horizontalSpacing;
    int verticalSpacing;

    int layout;
    int sizeMode;

    int showText;
    int hidePartialButtons;
    int resizeInLayoutSteps;
    int settingsWheelScrub;

    int dpiScaleEnabled;

    int contentAlignment;
    int contentAlignX;
    int contentAlignY;
    int contentAlignPercentX;
    int contentAlignPercentY;

    const char *themeName;
    int allowThemeSelection;

    int showBorder;
    int showBorderTitle;
    const char *borderTitle;
    int borderPadding;
    int borderTitleHeight;
    int borderTitlePadding;
    int borderTitleFontSize;
    int borderTitleTransparent;
    int borderTitleAutoBackColor;
    int borderStyle;
    int borderThickness;
    int borderCornerRadius;
    COLORREF borderColor;
    COLORREF borderLightColor;
    COLORREF borderShadowColor;
    COLORREF borderTitleColor;
    COLORREF borderTitleBackColor;

    int showGearIcon;
    int gearCorner;
    int gearSize;
    int gearMargin;
    COLORREF gearColor;
    COLORREF gearBackColor;
    COLORREF gearBorderColor;

    int idBase;
    int firstIndex;

    const char *namePrefix;
    const char *actionPrefix;
    const char *textFormat;
    const char *clickIdentifierFormat;

    int buttonBackMode;
    int showButtonBorder;
    int buttonBorderThickness;
    COLORREF buttonBorderColor;
    COLORREF backColor;
    COLORREF foreColor;

    int usePictures;
    int toggleOnClick;
    int defaultState;
    int stretchPictures;

    COLORREF generatedOffPictureColor;
    COLORREF generatedOnPictureColor;
    COLORREF generatedErrorPictureColor;
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

void ButtonGrid_SetClickCallback(
    HWND gridHwnd,
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

void ButtonGrid_SetSizeMode(
    HWND gridHwnd,
    int sizeMode
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
    AppImage *pictureOff,
    AppImage *pictureOn,
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

void ButtonGrid_SetButtonStateByName(
    HWND gridHwnd,
    const char *name,
    int isOn
);

int ButtonGrid_GetButtonStateByName(
    HWND gridHwnd,
    const char *name
);

void ButtonGrid_ToggleButtonStateByName(
    HWND gridHwnd,
    const char *name
);

void ButtonGrid_SetButtonStateByAction(
    HWND gridHwnd,
    const char *action,
    int isOn
);

int ButtonGrid_GetButtonStateByAction(
    HWND gridHwnd,
    const char *action
);

void ButtonGrid_ToggleButtonStateByAction(
    HWND gridHwnd,
    const char *action
);

#endif