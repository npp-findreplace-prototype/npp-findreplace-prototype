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