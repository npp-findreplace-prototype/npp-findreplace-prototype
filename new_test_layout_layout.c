#include "new_test_layout_window_internal.h"

static void NewTestLayout_ComputeVisibility(
    int width,
    int height,
    NewTestLayoutVisibility *visibility
)
{
    int autoLayout;
    int allGroupsVisible;
    int availableWidth;
    int leftPanelWidth;
    int minimumGroupWidth;
    int fullHeightNeeded;
    int modeGridHeight;
    int groupStackHeight;

    if (!visibility)
        return;

    ZeroMemory(visibility, sizeof(*visibility));

    autoLayout = g_settingsConfig.autoLayoutEnabled;
    modeGridHeight = NewTestLayout_GetSingleRowModeGridHeight();

    visibility->showReplaceBox =
        g_settingsConfig.showReplaceBox &&
        (!autoLayout || height >= 118);

    visibility->showUtilityButtons =
        g_settingsConfig.showUtilityButtons &&
        visibility->showReplaceBox &&
        (!autoLayout || width >= 430);

    visibility->showModeGrid =
        g_settingsConfig.showModeGrid &&
        (!autoLayout || height >= 178);

    visibility->showFindGroup =
        g_settingsConfig.showFindDocumentGrid &&
        (!autoLayout || height >= 268 + (modeGridHeight - 52));

    visibility->showReplaceGroup =
        g_settingsConfig.showReplaceDocumentGrid &&
        (!autoLayout || height >= 360 + (modeGridHeight - 52));

    visibility->showSelectionGroup =
        g_settingsConfig.showReplaceSelectionGrid &&
        (!autoLayout || height >= 452 + (modeGridHeight - 52));

    allGroupsVisible =
        visibility->showFindGroup &&
        visibility->showReplaceGroup &&
        visibility->showSelectionGroup;

    groupStackHeight =
        NTL_ACTION_GROUP_HEIGHT * 3 +
        NTL_GAP * 2;

    leftPanelWidth = NewTestLayout_GetLeftModeGridWidth();
    minimumGroupWidth = 420;
    availableWidth = width - NTL_MARGIN * 2;

    visibility->useLeftModePanel =
        allGroupsVisible &&
        visibility->showModeGrid &&
        g_settingsConfig.enableLeftModePanel &&
        availableWidth >= leftPanelWidth + NTL_GAP + minimumGroupWidth;

    fullHeightNeeded =
        NTL_MARGIN +
        NTL_FAUX_COMBO_HEIGHT +
        NTL_GAP +
        NTL_FAUX_COMBO_HEIGHT +
        NTL_GAP +
        modeGridHeight +
        NTL_GAP +
        groupStackHeight +
        NTL_MARGIN;

    visibility->showGroupBorder =
        allGroupsVisible &&
        g_settingsConfig.enableGrowingBorder &&
        height > fullHeightNeeded + 20;

    visibility->groupPadding = visibility->showGroupBorder ? 8 : 4;
}

static void NewTestLayout_ShowUtilityButtons(int show)
{
    NewTestLayoutActionButton_Show(g_copyToReplaceButton, show);
    NewTestLayoutActionButton_Show(g_swapFindReplaceButton, show);
    NewTestLayoutActionButton_Show(g_copyToFindButton, show);
}

static void NewTestLayout_ShowGroups(const NewTestLayoutVisibility *visibility)
{
    if (!visibility)
        return;

    NewTestLayoutActionGroup_Show(g_findGroup, visibility->showFindGroup);
    NewTestLayoutActionGroup_Show(g_replaceGroup, visibility->showReplaceGroup);
    NewTestLayoutActionGroup_Show(g_selectionGroup, visibility->showSelectionGroup);

    NewTestLayoutActionGroup_SetBorderVisible(g_findGroup, visibility->showGroupBorder);
    NewTestLayoutActionGroup_SetBorderVisible(g_replaceGroup, visibility->showGroupBorder);
    NewTestLayoutActionGroup_SetBorderVisible(g_selectionGroup, visibility->showGroupBorder);

    NewTestLayoutActionGroup_SetPadding(g_findGroup, visibility->groupPadding);
    NewTestLayoutActionGroup_SetPadding(g_replaceGroup, visibility->groupPadding);
    NewTestLayoutActionGroup_SetPadding(g_selectionGroup, visibility->groupPadding);
}

static void NewTestLayout_RedrawUtilityButtons(void)
{
    HWND hwnd;

    hwnd = NewTestLayoutActionButton_GetHwnd(g_copyToReplaceButton);

    if (hwnd)
    {
        SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    }

    hwnd = NewTestLayoutActionButton_GetHwnd(g_swapFindReplaceButton);

    if (hwnd)
    {
        SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    }

    hwnd = NewTestLayoutActionButton_GetHwnd(g_copyToFindButton);

    if (hwnd)
    {
        SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    }
}

static void NewTestLayout_RedrawImportantControls(void)
{
    HWND hwnd;

    hwnd = NewTestLayoutFauxCombo_GetHwnd(g_findCombo);
    if (hwnd)
        RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);

    hwnd = NewTestLayoutFauxCombo_GetHwnd(g_replaceCombo);
    if (hwnd)
        RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);

    if (g_modeGrid)
        RedrawWindow(g_modeGrid, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);

    NewTestLayout_RedrawUtilityButtons();
}

void NewTestLayout_Layout(HWND hwnd)
{
    RECT rc;
    RECT r;
    NewTestLayoutVisibility visibility;

    int width;
    int height;

    int x;
    int y;
    int right;
    int comboRight;
    int utilityX;

    int groupX;
    int groupY;
    int groupWidth;
    int groupStackHeight;
    int modeGridHeight;

    int leftPanelWidth;
    int leftPanelHeight;
    int leftPanelTop;

    int showUtilityButtons;
    int utilityTop;
    int utilityBottom;
    int utilitySquare;
    int utilityGap;

    GetClientRect(hwnd, &rc);

    width = rc.right - rc.left;
    height = rc.bottom - rc.top;

    modeGridHeight = NewTestLayout_GetSingleRowModeGridHeight();

    NewTestLayout_ComputeVisibility(width, height, &visibility);

    x = NTL_MARGIN;
    y = NTL_MARGIN;
    right = width - NTL_MARGIN;

    showUtilityButtons = visibility.showUtilityButtons;

    utilityGap = NTL_UTILITY_BUTTON_GAP;
    utilitySquare = 0;

    if (showUtilityButtons)
    {
        utilityTop = NTL_MARGIN;
        utilityBottom =
            NTL_MARGIN +
            NTL_FAUX_COMBO_HEIGHT +
            NTL_GAP +
            NTL_FAUX_COMBO_HEIGHT;

        utilitySquare =
            (utilityBottom - utilityTop - utilityGap * 2) / 3;

        if (utilitySquare < NTL_UTILITY_BUTTON_SQUARE_MIN)
            utilitySquare = NTL_UTILITY_BUTTON_SQUARE_MIN;

        if (utilitySquare > NTL_UTILITY_BUTTON_SQUARE_MAX)
            utilitySquare = NTL_UTILITY_BUTTON_SQUARE_MAX;

        utilityX = right - utilitySquare;
        comboRight = utilityX - NTL_GAP;

        if (comboRight < x + 200)
        {
            showUtilityButtons = 0;
            utilityX = right;
            comboRight = right;
        }
    }
    else
    {
        utilityX = right;
        comboRight = right;
    }

    NewTestLayout_SetRect(
        &r,
        x,
        y,
        comboRight,
        y + NTL_FAUX_COMBO_HEIGHT
    );

    NewTestLayoutFauxCombo_SetRect(g_findCombo, &r);
    NewTestLayoutFauxCombo_Show(g_findCombo, 1);

    if (visibility.showReplaceBox)
    {
        y += NTL_FAUX_COMBO_HEIGHT + NTL_GAP;

        NewTestLayout_SetRect(
            &r,
            x,
            y,
            comboRight,
            y + NTL_FAUX_COMBO_HEIGHT
        );

        NewTestLayoutFauxCombo_SetRect(g_replaceCombo, &r);
        NewTestLayoutFauxCombo_Show(g_replaceCombo, 1);
    }
    else
    {
        NewTestLayoutFauxCombo_Show(g_replaceCombo, 0);
    }

    if (showUtilityButtons)
    {
        utilityTop = NTL_MARGIN;

        NewTestLayout_SetRect(
            &r,
            utilityX,
            utilityTop,
            utilityX + utilitySquare,
            utilityTop + utilitySquare
        );

        NewTestLayoutActionButton_SetRect(g_copyToReplaceButton, &r);

        utilityTop += utilitySquare + utilityGap;

        NewTestLayout_SetRect(
            &r,
            utilityX,
            utilityTop,
            utilityX + utilitySquare,
            utilityTop + utilitySquare
        );

        NewTestLayoutActionButton_SetRect(g_swapFindReplaceButton, &r);

        utilityTop += utilitySquare + utilityGap;

        NewTestLayout_SetRect(
            &r,
            utilityX,
            utilityTop,
            utilityX + utilitySquare,
            utilityTop + utilitySquare
        );

        NewTestLayoutActionButton_SetRect(g_copyToFindButton, &r);
    }

    NewTestLayout_ShowUtilityButtons(showUtilityButtons);

    y = NTL_MARGIN + NTL_FAUX_COMBO_HEIGHT;

    if (visibility.showReplaceBox)
        y += NTL_GAP + NTL_FAUX_COMBO_HEIGHT;

    groupY = y;

    if (visibility.showModeGrid && !visibility.useLeftModePanel)
    {
        NewTestLayout_SetRect(
            &r,
            NTL_MARGIN,
            y + NTL_GAP,
            width - NTL_MARGIN,
            y + NTL_GAP + modeGridHeight
        );

        groupY += NTL_GAP + modeGridHeight;

        if (g_modeGrid)
        {
            ShowWindow(g_modeGrid, SW_SHOW);

            ButtonGrid_SetRect(
                g_modeGrid,
                r.left,
                r.top,
                r.right - r.left,
                r.bottom - r.top
            );
        }
    }
    else if (visibility.showModeGrid && visibility.useLeftModePanel)
    {
        if (g_modeGrid)
            ShowWindow(g_modeGrid, SW_SHOW);
    }
    else
    {
        if (g_modeGrid)
            ShowWindow(g_modeGrid, SW_HIDE);
    }

    groupStackHeight =
        NTL_ACTION_GROUP_HEIGHT * 3 +
        NTL_GAP * 2;

    groupX = NTL_MARGIN;
    groupWidth = width - NTL_MARGIN * 2;

    if (visibility.useLeftModePanel)
    {
        leftPanelWidth = NewTestLayout_GetLeftModeGridWidth();
        leftPanelHeight = NewTestLayout_GetLeftModeGridHeight();

        leftPanelTop = groupY;

        if (groupStackHeight > leftPanelHeight)
            leftPanelTop = groupY + (groupStackHeight - leftPanelHeight) / 2;

        NewTestLayout_SetRect(
            &r,
            NTL_MARGIN,
            leftPanelTop,
            NTL_MARGIN + leftPanelWidth,
            leftPanelTop + leftPanelHeight
        );

        if (g_modeGrid)
        {
            ButtonGrid_SetRect(
                g_modeGrid,
                r.left,
                r.top,
                r.right - r.left,
                r.bottom - r.top
            );
        }

        groupX = NTL_MARGIN + leftPanelWidth + NTL_GAP;
        groupWidth = width - groupX - NTL_MARGIN;
    }

    if (groupWidth < 180)
        groupWidth = 180;

    if (visibility.showFindGroup)
    {
        NewTestLayout_SetRect(
            &r,
            groupX,
            groupY,
            groupX + groupWidth,
            groupY + NTL_ACTION_GROUP_HEIGHT
        );

        NewTestLayoutActionGroup_SetRect(g_findGroup, &r);
        groupY += NTL_ACTION_GROUP_HEIGHT + NTL_GAP;
    }

    if (visibility.showReplaceGroup)
    {
        NewTestLayout_SetRect(
            &r,
            groupX,
            groupY,
            groupX + groupWidth,
            groupY + NTL_ACTION_GROUP_HEIGHT
        );

        NewTestLayoutActionGroup_SetRect(g_replaceGroup, &r);
        groupY += NTL_ACTION_GROUP_HEIGHT + NTL_GAP;
    }

    if (visibility.showSelectionGroup)
    {
        NewTestLayout_SetRect(
            &r,
            groupX,
            groupY,
            groupX + groupWidth,
            groupY + NTL_ACTION_GROUP_HEIGHT
        );

        NewTestLayoutActionGroup_SetRect(g_selectionGroup, &r);
    }

    NewTestLayout_ShowGroups(&visibility);

    if (g_settingsPanel)
    {
        NewTestLayoutSettings_Layout(g_settingsPanel, &rc);

        if (NewTestLayoutSettings_IsVisible(g_settingsPanel))
            NewTestLayoutSettings_Show(g_settingsPanel, 1);
    }

    RedrawWindow(
        hwnd,
        NULL,
        NULL,
        RDW_INVALIDATE | RDW_NOCHILDREN
    );

    NewTestLayout_RedrawImportantControls();
}