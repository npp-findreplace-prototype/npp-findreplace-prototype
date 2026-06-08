#include "new_test_layout_window_internal.h"

void NewTestLayout_ShowGroups(const NewTestLayoutVisibility *visibility)
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

void NewTestLayout_LayoutGroups(
    const NewTestLayoutVisibility *visibility,
    NewTestLayoutLayoutState *layout
)
{
    RECT r;
    int groupY;

    if (!visibility || !layout)
        return;

    groupY = layout->groupY;

    if (visibility->showFindGroup)
    {
        NewTestLayout_SetRect(
            &r,
            layout->groupX,
            groupY,
            layout->groupX + layout->groupWidth,
            groupY + NTL_ACTION_GROUP_HEIGHT
        );

        NewTestLayoutActionGroup_SetRect(g_findGroup, &r);
        groupY += NTL_ACTION_GROUP_HEIGHT + NTL_GAP;
    }

    if (visibility->showReplaceGroup)
    {
        NewTestLayout_SetRect(
            &r,
            layout->groupX,
            groupY,
            layout->groupX + layout->groupWidth,
            groupY + NTL_ACTION_GROUP_HEIGHT
        );

        NewTestLayoutActionGroup_SetRect(g_replaceGroup, &r);
        groupY += NTL_ACTION_GROUP_HEIGHT + NTL_GAP;
    }

    if (visibility->showSelectionGroup)
    {
        NewTestLayout_SetRect(
            &r,
            layout->groupX,
            groupY,
            layout->groupX + layout->groupWidth,
            groupY + NTL_ACTION_GROUP_HEIGHT
        );

        NewTestLayoutActionGroup_SetRect(g_selectionGroup, &r);
    }

    layout->groupY = groupY;

    NewTestLayout_ShowGroups(visibility);
}