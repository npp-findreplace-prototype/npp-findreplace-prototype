#include "new_test_layout_window_internal.h"

static int NewTestLayout_ClampPercent(
    int percent,
    int defaultPercent
)
{
    if (percent <= 0)
        percent = defaultPercent;

    if (percent < 1)
        percent = 1;

    if (percent > 400)
        percent = 400;

    return percent;
}

static int NewTestLayout_HeightFromPercent(
    int percent,
    int defaultPercent
)
{
    percent = NewTestLayout_ClampPercent(percent, defaultPercent);

    return MulDiv(
        NEW_TEST_LAYOUT_SETTINGS_DESIGN_HEIGHT,
        percent,
        100
    );
}

static int NewTestLayout_WidthFromPercent(
    int percent,
    int defaultPercent
)
{
    percent = NewTestLayout_ClampPercent(percent, defaultPercent);

    return MulDiv(
        NEW_TEST_LAYOUT_SETTINGS_DESIGN_WIDTH,
        percent,
        100
    );
}

void NewTestLayout_ComputeVisibility(
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
    int minimumLeftPanelWindowWidth;
    int fullHeightNeeded;
    int modeGridHeight;
    int groupStackHeight;

    int replaceBoxHeight;
    int utilityButtonsWidth;
    int modeGridHeightThreshold;
    int findGridHeight;
    int replaceGridHeight;
    int selectionGridHeight;

    if (!visibility)
        return;

    ZeroMemory(visibility, sizeof(*visibility));

    autoLayout = g_ntl_settingsConfig.autoLayoutEnabled;
    modeGridHeight = NewTestLayout_GetSingleRowModeGridHeight();

    replaceBoxHeight = NewTestLayout_HeightFromPercent(
        g_ntl_settingsConfig.replaceBoxMinHeightPercent,
        NEW_TEST_LAYOUT_SETTINGS_DEFAULT_REPLACE_BOX_MIN_HEIGHT_PERCENT
    );

    utilityButtonsWidth = NewTestLayout_WidthFromPercent(
        g_ntl_settingsConfig.utilityButtonsMinWidthPercent,
        NEW_TEST_LAYOUT_SETTINGS_DEFAULT_UTILITY_BUTTONS_MIN_WIDTH_PERCENT
    );

    modeGridHeightThreshold = NewTestLayout_HeightFromPercent(
        g_ntl_settingsConfig.modeGridMinHeightPercent,
        NEW_TEST_LAYOUT_SETTINGS_DEFAULT_MODE_GRID_MIN_HEIGHT_PERCENT
    );

    findGridHeight = NewTestLayout_HeightFromPercent(
        g_ntl_settingsConfig.findDocumentGridMinHeightPercent,
        NEW_TEST_LAYOUT_SETTINGS_DEFAULT_FIND_GRID_MIN_HEIGHT_PERCENT
    );

    replaceGridHeight = NewTestLayout_HeightFromPercent(
        g_ntl_settingsConfig.replaceDocumentGridMinHeightPercent,
        NEW_TEST_LAYOUT_SETTINGS_DEFAULT_REPLACE_GRID_MIN_HEIGHT_PERCENT
    );

    selectionGridHeight = NewTestLayout_HeightFromPercent(
        g_ntl_settingsConfig.replaceSelectionGridMinHeightPercent,
        NEW_TEST_LAYOUT_SETTINGS_DEFAULT_SELECTION_GRID_MIN_HEIGHT_PERCENT
    );

    visibility->showReplaceBox =
        g_ntl_settingsConfig.showReplaceBox &&
        (!autoLayout || height >= replaceBoxHeight);

    visibility->showUtilityButtons =
        g_ntl_settingsConfig.showUtilityButtons &&
        visibility->showReplaceBox &&
        (!autoLayout || width >= utilityButtonsWidth);

    visibility->showModeGrid =
        g_ntl_settingsConfig.showModeGrid &&
        (!autoLayout || height >= modeGridHeightThreshold);

    visibility->showFindGroup =
        g_ntl_settingsConfig.showFindDocumentGrid &&
        (!autoLayout || height >= findGridHeight);

    visibility->showReplaceGroup =
        g_ntl_settingsConfig.showReplaceDocumentGrid &&
        (!autoLayout || height >= replaceGridHeight);

    visibility->showSelectionGroup =
        g_ntl_settingsConfig.showReplaceSelectionGrid &&
        (!autoLayout || height >= selectionGridHeight);

    allGroupsVisible =
        visibility->showFindGroup &&
        visibility->showReplaceGroup &&
        visibility->showSelectionGroup;

    groupStackHeight =
        NTL_ACTION_GROUP_HEIGHT * 3 +
        NTL_GAP * 2;

    leftPanelWidth = NewTestLayout_GetLeftModeGridWidth();

    minimumLeftPanelWindowWidth = NewTestLayout_WidthFromPercent(
        g_ntl_settingsConfig.leftModePanelMinWidthPercent,
        NEW_TEST_LAYOUT_SETTINGS_DEFAULT_LEFT_MODE_PANEL_MIN_WIDTH_PERCENT
    );

    minimumGroupWidth = NewTestLayout_WidthFromPercent(
        g_ntl_settingsConfig.leftModePanelMinGroupWidthPercent,
        NEW_TEST_LAYOUT_SETTINGS_DEFAULT_LEFT_MODE_PANEL_MIN_GROUP_WIDTH_PERCENT
    );

    availableWidth = width - NTL_MARGIN * 2;

    visibility->useLeftModePanel =
        allGroupsVisible &&
        visibility->showModeGrid &&
        g_ntl_settingsConfig.enableLeftModePanel &&
        width >= minimumLeftPanelWindowWidth &&
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
        g_ntl_settingsConfig.enableGrowingBorder &&
        height > fullHeightNeeded + 20;

    visibility->groupPadding = visibility->showGroupBorder ? 8 : 4;
}