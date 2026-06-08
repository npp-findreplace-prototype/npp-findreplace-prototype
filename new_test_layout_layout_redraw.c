#include "new_test_layout_window_internal.h"

void NewTestLayout_RedrawUtilityButtons(void)
{
    HWND hwnd;

    hwnd = NewTestLayoutActionButton_GetHwnd(g_ntl_copyToReplaceButton);

    if (hwnd)
    {
        SetWindowPos(
            hwnd,
            HWND_TOP,
            0,
            0,
            0,
            0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE
        );

        RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    }

    hwnd = NewTestLayoutActionButton_GetHwnd(g_ntl_swapFindReplaceButton);

    if (hwnd)
    {
        SetWindowPos(
            hwnd,
            HWND_TOP,
            0,
            0,
            0,
            0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE
        );

        RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    }

    hwnd = NewTestLayoutActionButton_GetHwnd(g_ntl_copyToFindButton);

    if (hwnd)
    {
        SetWindowPos(
            hwnd,
            HWND_TOP,
            0,
            0,
            0,
            0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE
        );

        RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    }
}

void NewTestLayout_RedrawImportantControls(void)
{
    HWND hwnd;

    hwnd = NewTestLayoutFauxCombo_GetHwnd(g_ntl_findCombo);

    if (hwnd)
    {
        RedrawWindow(
            hwnd,
            NULL,
            NULL,
            RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN
        );
    }

    hwnd = NewTestLayoutFauxCombo_GetHwnd(g_ntl_replaceCombo);

    if (hwnd)
    {
        RedrawWindow(
            hwnd,
            NULL,
            NULL,
            RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN
        );
    }

    if (g_ntl_modeGrid)
    {
        RedrawWindow(
            g_ntl_modeGrid,
            NULL,
            NULL,
            RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN
        );
    }

    NewTestLayout_RedrawUtilityButtons();
}