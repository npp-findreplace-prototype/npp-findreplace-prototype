#include "new_test_layout_window_internal.h"

#ifndef USER_DEFAULT_SCREEN_DPI
#define USER_DEFAULT_SCREEN_DPI 96
#endif

#ifndef LOGPIXELSX
#define LOGPIXELSX 88
#endif

HINSTANCE g_ntl_hInstance = NULL;
HWND g_ntl_window = NULL;

NewTestLayoutWindowClosedCallback g_ntl_onClosed = NULL;

NewTestLayoutTheme g_ntl_theme;
NewTestLayoutSettingsConfig g_ntl_settingsConfig;
NewTestLayoutSettingsPanel *g_ntl_settingsPanel = NULL;

NewTestLayoutFauxCombo *g_ntl_findCombo = NULL;
NewTestLayoutFauxCombo *g_ntl_replaceCombo = NULL;

NewTestLayoutActionButton *g_ntl_copyToReplaceButton = NULL;
NewTestLayoutActionButton *g_ntl_swapFindReplaceButton = NULL;
NewTestLayoutActionButton *g_ntl_copyToFindButton = NULL;

NewTestLayoutActionGroup *g_ntl_findGroup = NULL;
NewTestLayoutActionGroup *g_ntl_replaceGroup = NULL;
NewTestLayoutActionGroup *g_ntl_selectionGroup = NULL;

HWND g_ntl_modeGrid = NULL;
ButtonGridConfig g_ntl_modeGridConfig;
ButtonGridItemConfig g_ntl_modeGridItems[12];

char g_ntl_lastFindText[512];
NewTestLayoutCounts g_ntl_counts;

HBRUSH g_ntl_backBrush = NULL;

static int NewTestLayout_GetLayoutDpi(void)
{
    HDC hdc;
    HWND hwnd;
    int dpi;

    dpi = USER_DEFAULT_SCREEN_DPI;
    hwnd = g_ntl_window;

    hdc = GetDC(hwnd);

    if (hdc)
    {
        dpi = GetDeviceCaps(hdc, LOGPIXELSX);
        ReleaseDC(hwnd, hdc);
    }

    if (dpi < 48)
        dpi = USER_DEFAULT_SCREEN_DPI;

    if (dpi > 384)
        dpi = 384;

    return dpi;
}

static int NewTestLayout_ScaleModeGridMetric(int value)
{
    int dpi;
    int scaled;

    if (value <= 0)
        return value;

    if (!g_ntl_modeGridConfig.dpiScaleEnabled)
        return value;

    dpi = NewTestLayout_GetLayoutDpi();
    scaled = MulDiv(value, dpi, USER_DEFAULT_SCREEN_DPI);

    if (scaled < 1)
        scaled = 1;

    return scaled;
}

static int NewTestLayout_UnscaleModeGridMetric(int value)
{
    int dpi;
    int unscaled;

    if (value <= 0)
        return value;

    if (!g_ntl_modeGridConfig.dpiScaleEnabled)
        return value;

    dpi = NewTestLayout_GetLayoutDpi();
    unscaled = MulDiv(value, USER_DEFAULT_SCREEN_DPI, dpi);

    if (unscaled < 1)
        unscaled = 1;

    return unscaled;
}

static int NewTestLayout_GetActionGroupStackHeight(void)
{
    return
        NTL_ACTION_GROUP_HEIGHT * 3 +
        NTL_GAP * 2;
}

void NewTestLayout_CopyText(char *dest, int destSize, const char *src)
{
    if (!dest || destSize <= 0)
        return;

    if (!src)
        src = "";

    lstrcpyn(dest, src, destSize);
    dest[destSize - 1] = '\0';
}

int NewTestLayout_MinInt(int a, int b)
{
    return a < b ? a : b;
}

int NewTestLayout_ClampInt(int value, int minValue, int maxValue)
{
    if (value < minValue)
        return minValue;

    if (value > maxValue)
        return maxValue;

    return value;
}

void NewTestLayout_SetRect(
    RECT *rc,
    int left,
    int top,
    int right,
    int bottom
)
{
    if (!rc)
        return;

    rc->left = left;
    rc->top = top;
    rc->right = right;
    rc->bottom = bottom;
}

int NewTestLayout_GetSingleRowModeGridHeight(void)
{
    int h;
    int minH;

    h = g_ntl_settingsConfig.singleRowModeGridHeight;

    minH =
        NewTestLayout_ScaleModeGridMetric(NTL_MODE_SINGLE_BUTTON_SIZE) +
        NewTestLayout_ScaleModeGridMetric(8);

    if (minH < 44)
        minH = 44;

    if (h < minH)
        h = minH;

    if (h > 140)
        h = 140;

    return h;
}

int NewTestLayout_GetLeftModeGridButtonSize(void)
{
    int targetHeight;
    int spacing;
    int padding;
    int buttonPixels;
    int buttonSize;

    targetHeight = NewTestLayout_GetActionGroupStackHeight();

    spacing = NewTestLayout_ScaleModeGridMetric(NTL_MODE_LEFT_SPACING);
    padding = NewTestLayout_ScaleModeGridMetric(4);

    buttonPixels =
        targetHeight -
        (NTL_MODE_LEFT_ROWS - 1) * spacing -
        padding;

    buttonPixels = buttonPixels / NTL_MODE_LEFT_ROWS;

    buttonSize = NewTestLayout_UnscaleModeGridMetric(buttonPixels);

    if (buttonSize < NTL_MODE_LEFT_BUTTON_SIZE)
        buttonSize = NTL_MODE_LEFT_BUTTON_SIZE;

    return buttonSize;
}

int NewTestLayout_GetLeftModeGridWidth(void)
{
    int buttonSize;
    int buttonPixels;
    int spacing;
    int padding;

    buttonSize = NewTestLayout_GetLeftModeGridButtonSize();

    buttonPixels = NewTestLayout_ScaleModeGridMetric(buttonSize);
    spacing = NewTestLayout_ScaleModeGridMetric(NTL_MODE_LEFT_SPACING);
    padding = NewTestLayout_ScaleModeGridMetric(4);

    return
        NTL_MODE_LEFT_COLUMNS * buttonPixels +
        (NTL_MODE_LEFT_COLUMNS - 1) * spacing +
        padding;
}

int NewTestLayout_GetLeftModeGridHeight(void)
{
    int buttonSize;
    int buttonPixels;
    int spacing;
    int padding;

    buttonSize = NewTestLayout_GetLeftModeGridButtonSize();

    buttonPixels = NewTestLayout_ScaleModeGridMetric(buttonSize);
    spacing = NewTestLayout_ScaleModeGridMetric(NTL_MODE_LEFT_SPACING);
    padding = NewTestLayout_ScaleModeGridMetric(4);

    return
        NTL_MODE_LEFT_ROWS * buttonPixels +
        (NTL_MODE_LEFT_ROWS - 1) * spacing +
        padding;
}