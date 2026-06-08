#include "new_test_layout_settings_internal.h"

const NewTestLayoutSettingsIntDef g_ntlSettingsIntDefs[NTL_SETTINGS_INT_COUNT] =
{
    { "Overlay width", offsetof(NewTestLayoutSettingsConfig, overlayWidth) },
    { "Overlay margin", offsetof(NewTestLayoutSettingsConfig, overlayMargin) },
    { "Row height", offsetof(NewTestLayoutSettingsConfig, rowHeight) },
    { "Gap", offsetof(NewTestLayoutSettingsConfig, gap) },
    { "Single row icon grid height", offsetof(NewTestLayoutSettingsConfig, singleRowModeGridHeight) },

    { "Replace box min height %", offsetof(NewTestLayoutSettingsConfig, replaceBoxMinHeightPercent) },
    { "Utility buttons min width %", offsetof(NewTestLayoutSettingsConfig, utilityButtonsMinWidthPercent) },
    { "Icon grid min height %", offsetof(NewTestLayoutSettingsConfig, modeGridMinHeightPercent) },
    { "Find grid min height %", offsetof(NewTestLayoutSettingsConfig, findDocumentGridMinHeightPercent) },
    { "Replace grid min height %", offsetof(NewTestLayoutSettingsConfig, replaceDocumentGridMinHeightPercent) },
    { "Selection grid min height %", offsetof(NewTestLayoutSettingsConfig, replaceSelectionGridMinHeightPercent) },
    { "Left icon panel min width %", offsetof(NewTestLayoutSettingsConfig, leftModePanelMinWidthPercent) },
    { "Left panel group min width %", offsetof(NewTestLayoutSettingsConfig, leftModePanelMinGroupWidthPercent) }
};

const NewTestLayoutSettingsBoolDef g_ntlSettingsBoolDefs[NTL_SETTINGS_BOOL_COUNT] =
{
    { "Auto layout enabled", offsetof(NewTestLayoutSettingsConfig, autoLayoutEnabled) },
    { "Show replace box", offsetof(NewTestLayoutSettingsConfig, showReplaceBox) },
    { "Show utility buttons", offsetof(NewTestLayoutSettingsConfig, showUtilityButtons) },
    { "Show mode icon grid", offsetof(NewTestLayoutSettingsConfig, showModeGrid) },
    { "Show Find In Document grid", offsetof(NewTestLayoutSettingsConfig, showFindDocumentGrid) },
    { "Show Replace In Document grid", offsetof(NewTestLayoutSettingsConfig, showReplaceDocumentGrid) },
    { "Show Replace In Selection grid", offsetof(NewTestLayoutSettingsConfig, showReplaceSelectionGrid) },
    { "Show counts", offsetof(NewTestLayoutSettingsConfig, showCounts) },
    { "Show zero counts", offsetof(NewTestLayoutSettingsConfig, showZeroCounts) },
    { "Counts in parentheses", offsetof(NewTestLayoutSettingsConfig, countInParentheses) },
    { "Enable left mode panel", offsetof(NewTestLayoutSettingsConfig, enableLeftModePanel) },
    { "Enable group borders", offsetof(NewTestLayoutSettingsConfig, enableGrowingBorder) },
    { "Recent find dropdown", offsetof(NewTestLayoutSettingsConfig, enableRecentFindDropdown) },
    { "Recent replace dropdown", offsetof(NewTestLayoutSettingsConfig, enableRecentReplaceDropdown) },
    { "Large placeholder text", offsetof(NewTestLayoutSettingsConfig, fauxComboPlaceholderLarge) },
    { "Blue count text", offsetof(NewTestLayoutSettingsConfig, actionButtonCountColorEnabled) }
};

int *Settings_IntField(
    NewTestLayoutSettingsConfig *config,
    size_t offset
)
{
    return (int *)((BYTE *)config + offset);
}

void Settings_CopyConfig(
    NewTestLayoutSettingsConfig *dest,
    const NewTestLayoutSettingsConfig *src
)
{
    if (!dest)
        return;

    if (src)
        CopyMemory(dest, src, sizeof(*dest));
    else
        NewTestLayoutSettings_GetDefaultConfig(dest);
}

void Settings_NotifyChanged(
    NewTestLayoutSettingsPanel *panel
)
{
    if (!panel)
        return;

    if (panel->changedCallback)
        panel->changedCallback(panel->changedUserData);
}