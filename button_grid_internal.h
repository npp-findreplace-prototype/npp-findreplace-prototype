#ifndef BUTTON_GRID_INTERNAL_H
#define BUTTON_GRID_INTERNAL_H

#include <windows.h>
#include <commctrl.h>
#include <stdlib.h>

#include "button_grid.h"
#include "image_loader.h"

#ifndef SS_OWNERDRAW
#define SS_OWNERDRAW 0x0000000D
#endif

#ifndef SWP_NOCOPYBITS
#define SWP_NOCOPYBITS 0x0100
#endif

#ifndef TTS_ALWAYSTIP
#define TTS_ALWAYSTIP 0x01
#endif

#ifndef TTS_NOPREFIX
#define TTS_NOPREFIX 0x02
#endif

#ifndef TTF_IDISHWND
#define TTF_IDISHWND 0x0001
#endif

#ifndef TTF_SUBCLASS
#define TTF_SUBCLASS 0x0010
#endif

#ifndef TTM_ADDTOOLA
#define TTM_ADDTOOLA (WM_USER + 4)
#endif

#ifndef TOOLTIPS_CLASSA
#define TOOLTIPS_CLASSA "tooltips_class32"
#endif

#define BUTTON_GRID_CLASS_NAME "ButtonGridChildClass"
#define BUTTON_GRID_PROP_NAME "ButtonGridData"

#define BUTTON_GRID_NAME_SIZE 64
#define BUTTON_GRID_ACTION_SIZE 64
#define BUTTON_GRID_TEXT_SIZE 64
#define BUTTON_GRID_TOOLTIP_SIZE 128
#define BUTTON_GRID_FORMAT_SIZE 64
#define BUTTON_GRID_TITLE_SIZE 128

#define BUTTON_GRID_PICTURE_TYPE_OFF 0
#define BUTTON_GRID_PICTURE_TYPE_ON 1
#define BUTTON_GRID_PICTURE_TYPE_ERROR 2

typedef struct ButtonItem
{
    HWND hwnd;

    char name[BUTTON_GRID_NAME_SIZE];
    char action[BUTTON_GRID_ACTION_SIZE];
    char text[BUTTON_GRID_TEXT_SIZE];
    char tooltip[BUTTON_GRID_TOOLTIP_SIZE];
    char iconBaseName[BUTTON_GRID_NAME_SIZE];

    int behavior;
    int radioGroup;
    int isOn;

    int widthOverride;
    int heightOverride;
    int sizeModeOverride;

    int showTextOverride;

    int width;
    int height;

    AppImage *pictureOff;
    AppImage *pictureOn;

    int ownsPictureOff;
    int ownsPictureOn;

    int pictureOffLoadFailed;
    int pictureOnLoadFailed;
} ButtonItem;

typedef struct GridPosition
{
    int x;
    int y;
} GridPosition;

typedef struct GridSize
{
    int width;
    int height;
} GridSize;

typedef struct ButtonGrid
{
    HWND hwnd;
    HWND tooltipHwnd;

    HINSTANCE hInstance;

    ButtonGridClickCallback onClick;

    ButtonItem *buttons;
    int buttonCount;

    const ButtonGridItemConfig *configuredItems;
    int configuredItemCount;

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
    int currentDpi;

    int contentAlignment;
    int contentAlignX;
    int contentAlignY;
    int contentAlignPercentX;
    int contentAlignPercentY;

    char themeName[BUTTON_GRID_THEME_NAME_SIZE];
    int allowThemeSelection;

    int showBorder;
    int showBorderTitle;
    char borderTitle[BUTTON_GRID_TITLE_SIZE];
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

    HWND settingsPageHwnd;
    int settingsPageVisible;

    int idBase;
    int firstIndex;

    char namePrefix[BUTTON_GRID_FORMAT_SIZE];
    char actionPrefix[BUTTON_GRID_FORMAT_SIZE];
    char textFormat[BUTTON_GRID_FORMAT_SIZE];
    char clickIdentifierFormat[BUTTON_GRID_FORMAT_SIZE];

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

    DWORD debugLastLogTick;

    int debugSetRectCalls;
    int debugSetRectSameCalls;
    int debugSetRectSizeChangedCalls;
    int debugSetWindowPosCalls;

    int debugSizeMessages;
    int debugWindowPosChangedMessages;

    int debugRelayoutCalls;
    int debugDpiRelayoutCalls;

    int debugGridRedrawCalls;
    int debugGridRedrawUpdateNowCalls;
    int debugGridRedrawAllChildrenCalls;

    int debugPaintCalls;
    int debugDrawItemCalls;
    int debugEraseCalls;

    int debugButtonRedrawEstimated;

    int debugLastWidth;
    int debugLastHeight;

    HBRUSH buttonBrush;
} ButtonGrid;

typedef struct ButtonGridCreateParams
{
    ButtonGridConfig config;
    ButtonGridClickCallback onClick;
} ButtonGridCreateParams;

ButtonGrid *ButtonGrid_Get(HWND hwnd);

void ButtonGrid_CopyText(char *dest, int destSize, const char *src);
int ButtonGrid_SameText(const char *a, const char *b);

void ButtonGrid_NormalizeConfig(ButtonGridConfig *config);
void ButtonGrid_ApplyConfig(ButtonGrid *grid, const ButtonGridConfig *config);

int ButtonGrid_NormalizeSizeMode(int sizeMode);

int ButtonGrid_GetWindowDpi(HWND hwnd);
void ButtonGrid_UpdateDpi(ButtonGrid *grid);

int ButtonGrid_DpiScale(ButtonGrid *grid, int value);
int ButtonGrid_DpiScaleMin(ButtonGrid *grid, int value, int minValue);

void ButtonGrid_RedrawContainer(HWND hwnd);
void ButtonGrid_AdjustRectToLayoutSteps(ButtonGrid *grid, int *width, int *height);
void ButtonGrid_ResolveButtonSize(ButtonGrid *grid, ButtonItem *button);
void ButtonGrid_UpdateAllButtonSizes(ButtonGrid *grid);
void ButtonGrid_Layout(ButtonGrid *grid);

LRESULT ButtonGrid_HandlePaint(HWND hwnd);
LRESULT ButtonGrid_HandleDrawItem(ButtonGrid *grid, LPARAM lParam);
LRESULT ButtonGrid_HandleEraseBackground(HWND hwnd, WPARAM wParam);

void ButtonGrid_CreateTooltipWindow(ButtonGrid *grid);
void ButtonGrid_AddTooltip(ButtonGrid *grid, int buttonIndex);

int ButtonGrid_CreateButtons(ButtonGrid *grid);
void ButtonGrid_Free(ButtonGrid *grid);
void ButtonGrid_FreeButtonImages(ButtonItem *button);

void ButtonGrid_ReloadThemeImages(ButtonGrid *grid);

int ButtonGrid_FindButtonIndexByName(ButtonGrid *grid, const char *name);
int ButtonGrid_FindButtonIndexByAction(ButtonGrid *grid, const char *action);
int ButtonGrid_FindButtonIndexByHwnd(ButtonGrid *grid, HWND hwnd);

void ButtonGrid_RedrawButton(ButtonGrid *grid, int index);
void ButtonGrid_RedrawAllButtons(ButtonGrid *grid);

void ButtonGrid_FocusButtonByIndex(ButtonGrid *grid, int index);
void ButtonGrid_FocusNextButton(ButtonGrid *grid, int currentIndex, int direction);
void ButtonGrid_ActivateButtonByIndex(ButtonGrid *grid, int index);

void ButtonGrid_HandleStaticClick(ButtonGrid *grid, int controlId);

#endif