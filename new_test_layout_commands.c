#include "new_test_layout_window_internal.h"

void NewTestLayout_UpdateWindowTitle(void)
{
    char title[256];

    wsprintf(
        title,
        "%s%s",
        NEW_TEST_LAYOUT_WINDOW_TITLE,
        g_settingsPanel && NewTestLayoutSettings_IsVisible(g_settingsPanel)
            ? " - settings"
            : ""
    );

    if (g_window)
        SetWindowText(g_window, title);
}

void NewTestLayout_ToggleSettings(void)
{
    if (!g_settingsPanel)
        return;

    NewTestLayoutSettings_Toggle(g_settingsPanel);
    NewTestLayout_UpdateWindowTitle();

    if (g_window)
        NewTestLayout_Layout(g_window);
}

static void NewTestLayout_CopyFindToReplace(void)
{
    char text[512];

    NewTestLayout_GetFindText(text, sizeof(text));

    if (g_replaceCombo)
        NewTestLayoutFauxCombo_SetText(g_replaceCombo, text);

    Debug_Log("NewLayout", "CopyFindToReplace", "%s", text);
}

static void NewTestLayout_CopyReplaceToFind(void)
{
    char text[512];

    NewTestLayout_GetReplaceText(text, sizeof(text));

    if (g_findCombo)
        NewTestLayoutFauxCombo_SetText(g_findCombo, text);

    Debug_Log("NewLayout", "CopyReplaceToFind", "%s", text);

    NewTestLayout_ApplyCounts();
}

static void NewTestLayout_SwapFindReplace(void)
{
    char findText[512];
    char replaceText[512];

    NewTestLayout_GetFindText(findText, sizeof(findText));
    NewTestLayout_GetReplaceText(replaceText, sizeof(replaceText));

    if (g_findCombo)
        NewTestLayoutFauxCombo_SetText(g_findCombo, replaceText);

    if (g_replaceCombo)
        NewTestLayoutFauxCombo_SetText(g_replaceCombo, findText);

    Debug_Log(
        "NewLayout",
        "SwapFindReplace",
        "find='%s' replace='%s'",
        replaceText,
        findText
    );

    NewTestLayout_ApplyCounts();
}

static void NewTestLayout_LogActionButton(int id)
{
    const char *name;

    name = "Unknown";

    switch (id)
    {
        case ID_NTL_FIND_PREVIOUS_BUTTON:
            name = "Find Previous";
            break;

        case ID_NTL_FIND_ALL_BUTTON:
            name = "Find All";
            break;

        case ID_NTL_FIND_ALL_DOCUMENTS_BUTTON:
            name = "Find In All Documents";
            break;

        case ID_NTL_FIND_NEXT_BUTTON:
            name = "Find Next";
            break;

        case ID_NTL_REPLACE_PREVIOUS_BUTTON:
            name = "Replace Previous";
            break;

        case ID_NTL_REPLACE_ALL_BUTTON:
            name = "Replace All";
            break;

        case ID_NTL_REPLACE_NEXT_BUTTON:
            name = "Replace Next";
            break;

        case ID_NTL_SELECTION_REPLACE_PREVIOUS_BUTTON:
            name = "Selection Replace Previous";
            break;

        case ID_NTL_SELECTION_REPLACE_ALL_BUTTON:
            name = "Selection Replace All";
            break;

        case ID_NTL_SELECTION_REPLACE_NEXT_BUTTON:
            name = "Selection Replace Next";
            break;
    }

    Debug_Log("NewLayout", "ActionButton", "%s", name);
}

int NewTestLayout_HandleCommand(WPARAM wParam, LPARAM lParam)
{
    int id;
    int notifyCode;

    if (g_settingsPanel &&
        NewTestLayoutSettings_HandleCommand(g_settingsPanel, wParam, lParam))
    {
        NewTestLayout_UpdateWindowTitle();
        return 1;
    }

    if (g_findCombo)
        NewTestLayoutFauxCombo_HandleParentCommand(g_findCombo, wParam, lParam);

    if (g_replaceCombo)
        NewTestLayoutFauxCombo_HandleParentCommand(g_replaceCombo, wParam, lParam);

    id = LOWORD(wParam);
    notifyCode = HIWORD(wParam);

    if ((HWND)lParam == NewTestLayoutFauxCombo_GetHwnd(g_findCombo))
    {
        if (notifyCode == NTL_FCN_TEXT_CHANGED ||
            notifyCode == NTL_FCN_RECENT_SELECTED)
        {
            NewTestLayout_ApplyCounts();
            return 1;
        }

        return 1;
    }

    if ((HWND)lParam == NewTestLayoutFauxCombo_GetHwnd(g_replaceCombo))
        return 1;

    if (notifyCode == NTL_ACTION_BN_CLICKED)
    {
        if (id == ID_NTL_COPY_TO_REPLACE_BUTTON)
        {
            NewTestLayout_CopyFindToReplace();
            return 1;
        }

        if (id == ID_NTL_SWAP_FIND_REPLACE_BUTTON)
        {
            NewTestLayout_SwapFindReplace();
            return 1;
        }

        if (id == ID_NTL_COPY_TO_FIND_BUTTON)
        {
            NewTestLayout_CopyReplaceToFind();
            return 1;
        }

        NewTestLayout_LogActionButton(id);
        return 1;
    }

    return 0;
}