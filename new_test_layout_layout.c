#include "new_test_layout_window_internal.h"

void NewTestLayout_Layout(HWND hwnd)
{
    NewTestLayoutVisibility visibility;
    NewTestLayoutLayoutState layout;

    if (!hwnd)
        return;

    ZeroMemory(&visibility, sizeof(visibility));
    ZeroMemory(&layout, sizeof(layout));

    GetClientRect(hwnd, &layout.clientRect);

    layout.width = layout.clientRect.right - layout.clientRect.left;
    layout.height = layout.clientRect.bottom - layout.clientRect.top;
    layout.modeGridHeight = NewTestLayout_GetSingleRowModeGridHeight();

    NewTestLayout_ComputeVisibility(
        layout.width,
        layout.height,
        &visibility
    );

    NewTestLayout_LayoutCombosAndUtility(
        &visibility,
        &layout
    );

    NewTestLayout_LayoutModeGrid(
        &visibility,
        &layout
    );

    NewTestLayout_LayoutGroups(
        &visibility,
        &layout
    );

    if (g_ntl_settingsPanel)
    {
        NewTestLayoutSettings_Layout(g_ntl_settingsPanel, &layout.clientRect);

        if (NewTestLayoutSettings_IsVisible(g_ntl_settingsPanel))
            NewTestLayoutSettings_Show(g_ntl_settingsPanel, 1);
    }

    RedrawWindow(
        hwnd,
        NULL,
        NULL,
        RDW_INVALIDATE | RDW_NOCHILDREN
    );

    NewTestLayout_RedrawImportantControls();
}