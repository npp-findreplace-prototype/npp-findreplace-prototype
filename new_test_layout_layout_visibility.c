#include "new_test_layout_window_internal.h"

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
    int fullHeightNeeded;
    int modeGridHeight;
    int groupStackHeight;

    if (!visibility)
        return;

    ZeroMemory(visibility, sizeof(*visibility));

    autoLayout = g_ntl_settingsConfig.autoLayoutEnabled;
    modeGridHeight = NewTestLayout_GetSingleRowModeGridHeight();

    visibility->showReplaceBox =
        g_ntl_settingsConfig.showReplaceBox &&
        (!autoLayout || height >= 118);

    visibility->showUtilityButtons =
        g_ntl_settingsConfig.showUtilityButtons &&
        visibility->showReplaceBox &&
        (!autoLayout || width >= 430);

    visibility->showModeGrid =
        g_ntl_settingsConfig.showModeGrid &&
        (!autoLayout || height >= 178);

    visibility->showFindGroup =
        g_ntl_settingsConfig.showFindDocumentGrid &&
        (!autoLayout || height >= 268 + (modeGridHeight - 52));

    visibility->showReplaceGroup =
        g_ntl_settingsConfig.showReplaceDocumentGrid &&
        (!autoLayout || height >= 360 + (modeGridHeight - 52));

    visibility->showSelectionGroup =
        g_ntl_settingsConfig.showReplaceSelectionGrid &&
        (!autoLayout || height >= 452 + (modeGridHeight - 52));

    allGroupsVisible =
        visibility->showFindGroup &&
        visibility->showReplaceGroup &&
        visibility->showSelectionGroup;

    groupStackHeight =
        NTL_ACTION_GROUP_HEIGHT * 3 +
        NTL_GAP * 2;

    leftPanelWidth = NewTestLayout_GetLeftModeGridWidth();

    /*
        Do not move the mode grid to the left until there is enough width for
        the full-height 4x3 icon grid and a comfortable action-button area.
        This prevents the small floating 4x3 grid and avoids clipping labels.
    */
    minimumGroupWidth = 620;
    availableWidth = width - NTL_MARGIN * 2;

    visibility->useLeftModePanel =
        allGroupsVisible &&
        visibility->showModeGrid &&
        g_ntl_settingsConfig.enableLeftModePanel &&
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