#include "new_test_layout_window_internal.h"

static void NewTestLayout_InvalidateChild(HWND hwnd)
{
    if (!hwnd)
        return;

    InvalidateRect(hwnd, NULL, FALSE);
}

void NewTestLayout_RedrawUtilityButtons(void)
{
    NewTestLayout_InvalidateChild(
        NewTestLayoutActionButton_GetHwnd(g_ntl_copyToReplaceButton)
    );

    NewTestLayout_InvalidateChild(
        NewTestLayoutActionButton_GetHwnd(g_ntl_swapFindReplaceButton)
    );

    NewTestLayout_InvalidateChild(
        NewTestLayoutActionButton_GetHwnd(g_ntl_copyToFindButton)
    );
}

void NewTestLayout_RedrawImportantControls(void)
{
    NewTestLayout_InvalidateChild(
        NewTestLayoutFauxCombo_GetHwnd(g_ntl_findCombo)
    );

    NewTestLayout_InvalidateChild(
        NewTestLayoutFauxCombo_GetHwnd(g_ntl_replaceCombo)
    );

    NewTestLayout_InvalidateChild(g_ntl_modeGrid);

    NewTestLayout_RedrawUtilityButtons();
}