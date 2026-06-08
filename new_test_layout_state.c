#include "new_test_layout_window_internal.h"

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

    h = g_ntl_settingsConfig.singleRowModeGridHeight;

    if (h < 44)
        h = 44;

    if (h > 140)
        h = 140;

    return h;
}

int NewTestLayout_GetLeftModeGridWidth(void)
{
    return
        NTL_MODE_LEFT_COLUMNS * NTL_MODE_LEFT_BUTTON_SIZE +
        (NTL_MODE_LEFT_COLUMNS - 1) * NTL_MODE_LEFT_SPACING +
        4;
}

int NewTestLayout_GetLeftModeGridHeight(void)
{
    return
        NTL_MODE_LEFT_ROWS * NTL_MODE_LEFT_BUTTON_SIZE +
        (NTL_MODE_LEFT_ROWS - 1) * NTL_MODE_LEFT_SPACING +
        4;
}