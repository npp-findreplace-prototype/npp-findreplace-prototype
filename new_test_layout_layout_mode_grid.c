#include "new_test_layout_window_internal.h"

static void NewTestLayout_SetModeGridSingleRowStyle(void)
{
    if (!g_ntl_modeGrid)
        return;

    ButtonGrid_SetLayout(g_ntl_modeGrid, BUTTON_GRID_LAYOUT_HORIZONTAL);

    ButtonGrid_SetButtonSize(
        g_ntl_modeGrid,
        NTL_MODE_SINGLE_BUTTON_SIZE,
        NTL_MODE_SINGLE_BUTTON_SIZE
    );

    ButtonGrid_SetSpacing(
        g_ntl_modeGrid,
        NTL_MODE_SINGLE_SPACING,
        NTL_MODE_SINGLE_SPACING
    );
}

static void NewTestLayout_SetModeGridLeftPanelStyle(void)
{
    int buttonSize;

    if (!g_ntl_modeGrid)
        return;

    buttonSize = NewTestLayout_GetLeftModeGridButtonSize();

    ButtonGrid_SetLayout(g_ntl_modeGrid, BUTTON_GRID_LAYOUT_HORIZONTAL);

    ButtonGrid_SetButtonSize(
        g_ntl_modeGrid,
        buttonSize,
        buttonSize
    );

    ButtonGrid_SetSpacing(
        g_ntl_modeGrid,
        NTL_MODE_LEFT_SPACING,
        NTL_MODE_LEFT_SPACING
    );
}

void NewTestLayout_LayoutModeGrid(
    const NewTestLayoutVisibility *visibility,
    NewTestLayoutLayoutState *layout
)
{
    RECT r;

    int groupStackHeight;
    int leftPanelWidth;
    int leftPanelHeight;
    int leftPanelTop;

    if (!visibility || !layout)
        return;

    if (visibility->showModeGrid && !visibility->useLeftModePanel)
    {
        NewTestLayout_SetModeGridSingleRowStyle();

        NewTestLayout_SetRect(
            &r,
            NTL_MARGIN,
            layout->groupY + NTL_GAP,
            layout->width - NTL_MARGIN,
            layout->groupY + NTL_GAP + layout->modeGridHeight
        );

        layout->groupY += NTL_GAP + layout->modeGridHeight;

        if (g_ntl_modeGrid)
        {
            ShowWindow(g_ntl_modeGrid, SW_SHOW);

            ButtonGrid_SetRect(
                g_ntl_modeGrid,
                r.left,
                r.top,
                r.right - r.left,
                r.bottom - r.top
            );
        }
    }
    else if (visibility->showModeGrid && visibility->useLeftModePanel)
    {
        NewTestLayout_SetModeGridLeftPanelStyle();

        if (g_ntl_modeGrid)
            ShowWindow(g_ntl_modeGrid, SW_SHOW);
    }
    else
    {
        if (g_ntl_modeGrid)
            ShowWindow(g_ntl_modeGrid, SW_HIDE);
    }

    groupStackHeight =
        NTL_ACTION_GROUP_HEIGHT * 3 +
        NTL_GAP * 2;

    layout->groupX = NTL_MARGIN;
    layout->groupWidth = layout->width - NTL_MARGIN * 2;

    if (visibility->useLeftModePanel)
    {
        leftPanelWidth = NewTestLayout_GetLeftModeGridWidth();
        leftPanelHeight = NewTestLayout_GetLeftModeGridHeight();

        leftPanelTop = layout->groupY;

        if (groupStackHeight > leftPanelHeight)
            leftPanelTop = layout->groupY + (groupStackHeight - leftPanelHeight) / 2;

        NewTestLayout_SetRect(
            &r,
            NTL_MARGIN,
            leftPanelTop,
            NTL_MARGIN + leftPanelWidth,
            leftPanelTop + leftPanelHeight
        );

        if (g_ntl_modeGrid)
        {
            ButtonGrid_SetRect(
                g_ntl_modeGrid,
                r.left,
                r.top,
                r.right - r.left,
                r.bottom - r.top
            );
        }

        layout->groupX = NTL_MARGIN + leftPanelWidth + NTL_GAP;
        layout->groupWidth = layout->width - layout->groupX - NTL_MARGIN;
    }

    if (layout->groupWidth < 180)
        layout->groupWidth = 180;
}