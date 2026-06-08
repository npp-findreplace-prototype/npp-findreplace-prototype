#include "new_test_layout_window_internal.h"

static int NewTestLayout_TextStartsWith(
    const char *text,
    const char *needle,
    int needleLength
)
{
    int i;

    if (!text || !needle || needleLength <= 0)
        return 0;

    for (i = 0; i < needleLength; i++)
    {
        if (text[i] != needle[i])
            return 0;
    }

    return 1;
}

static int NewTestLayout_CountLiteralInRange(
    const char *text,
    int textLength,
    const char *needle,
    int start,
    int end
)
{
    int count;
    int needleLength;
    int i;

    if (!text || !needle)
        return 0;

    needleLength = lstrlen(needle);

    if (needleLength <= 0)
        return 0;

    if (textLength < 0)
        textLength = lstrlen(text);

    start = NewTestLayout_ClampInt(start, 0, textLength);
    end = NewTestLayout_ClampInt(end, 0, textLength);

    if (end < start)
        end = start;

    count = 0;
    i = start;

    while (i + needleLength <= end)
    {
        if (NewTestLayout_TextStartsWith(text + i, needle, needleLength))
        {
            count++;
            i += needleLength;
        }
        else
        {
            i++;
        }
    }

    return count;
}

void NewTestLayout_GetFindText(char *buffer, int bufferSize)
{
    if (!buffer || bufferSize <= 0)
        return;

    buffer[0] = '\0';

    if (g_findCombo)
        NewTestLayoutFauxCombo_GetText(g_findCombo, buffer, bufferSize);
}

void NewTestLayout_GetReplaceText(char *buffer, int bufferSize)
{
    if (!buffer || bufferSize <= 0)
        return;

    buffer[0] = '\0';

    if (g_replaceCombo)
        NewTestLayoutFauxCombo_GetText(g_replaceCombo, buffer, bufferSize);
}

static int NewTestLayout_ComputeCounts(NewTestLayoutCounts *counts)
{
    char findText[512];
    char *text;
    int textLength;
    NppMockupTextInfo info;

    if (!counts)
        return 0;

    ZeroMemory(counts, sizeof(*counts));

    NewTestLayout_GetFindText(findText, sizeof(findText));

    if (!findText[0])
        return 0;

    text = NppMockupWindow_AllocActiveText(&textLength);

    if (!text)
        return 0;

    if (!NppMockupWindow_GetActiveTextInfo(&info))
    {
        NppMockupWindow_FreeText(text);
        return 0;
    }

    counts->hasCounts = 1;

    counts->findPrevious = NewTestLayout_CountLiteralInRange(
        text,
        textLength,
        findText,
        0,
        info.caretCharacter
    );

    counts->findAll = NewTestLayout_CountLiteralInRange(
        text,
        textLength,
        findText,
        0,
        textLength
    );

    counts->findNext = NewTestLayout_CountLiteralInRange(
        text,
        textLength,
        findText,
        info.caretCharacter,
        textLength
    );

    counts->replacePrevious = counts->findPrevious;
    counts->replaceAll = counts->findAll;
    counts->replaceNext = counts->findNext;

    if (info.hasSelection)
    {
        counts->hasSelectionCounts = 1;

        counts->selectionReplacePrevious = NewTestLayout_CountLiteralInRange(
            text,
            textLength,
            findText,
            info.selectionStartCharacter,
            info.caretCharacter
        );

        counts->selectionReplaceAll = NewTestLayout_CountLiteralInRange(
            text,
            textLength,
            findText,
            info.selectionStartCharacter,
            info.selectionEndCharacter
        );

        counts->selectionReplaceNext = NewTestLayout_CountLiteralInRange(
            text,
            textLength,
            findText,
            info.caretCharacter,
            info.selectionEndCharacter
        );
    }

    NppMockupWindow_FreeText(text);

    return 1;
}

void NewTestLayout_ApplyCountOptions(void)
{
    if (g_findGroup)
    {
        NewTestLayoutActionGroup_SetCountOptions(
            g_findGroup,
            g_settingsConfig.showCounts,
            g_settingsConfig.showZeroCounts,
            g_settingsConfig.countInParentheses,
            g_settingsConfig.actionButtonCountColorEnabled
        );
    }

    if (g_replaceGroup)
    {
        NewTestLayoutActionGroup_SetCountOptions(
            g_replaceGroup,
            g_settingsConfig.showCounts,
            g_settingsConfig.showZeroCounts,
            g_settingsConfig.countInParentheses,
            g_settingsConfig.actionButtonCountColorEnabled
        );
    }

    if (g_selectionGroup)
    {
        NewTestLayoutActionGroup_SetCountOptions(
            g_selectionGroup,
            g_settingsConfig.showCounts,
            g_settingsConfig.showZeroCounts,
            g_settingsConfig.countInParentheses,
            g_settingsConfig.actionButtonCountColorEnabled
        );
    }
}

void NewTestLayout_ApplyCounts(void)
{
    NewTestLayoutCounts counts;
    char currentFind[512];

    ZeroMemory(&counts, sizeof(counts));

    NewTestLayout_ComputeCounts(&counts);
    g_counts = counts;

    if (g_findGroup)
    {
        NewTestLayoutActionGroup_SetButtonCount(g_findGroup, 0, counts.findPrevious, counts.hasCounts);
        NewTestLayoutActionGroup_SetButtonCount(g_findGroup, 1, counts.findAll, counts.hasCounts);
        NewTestLayoutActionGroup_SetButtonCount(g_findGroup, 2, 0, 0);
        NewTestLayoutActionGroup_SetButtonCount(g_findGroup, 3, counts.findNext, counts.hasCounts);
    }

    if (g_replaceGroup)
    {
        NewTestLayoutActionGroup_SetButtonCount(g_replaceGroup, 0, counts.replacePrevious, counts.hasCounts);
        NewTestLayoutActionGroup_SetButtonCount(g_replaceGroup, 1, counts.replaceAll, counts.hasCounts);
        NewTestLayoutActionGroup_SetButtonCount(g_replaceGroup, 2, counts.replaceNext, counts.hasCounts);
    }

    if (g_selectionGroup)
    {
        NewTestLayoutActionGroup_SetButtonCount(g_selectionGroup, 0, counts.selectionReplacePrevious, counts.hasSelectionCounts);
        NewTestLayoutActionGroup_SetButtonCount(g_selectionGroup, 1, counts.selectionReplaceAll, counts.hasSelectionCounts);
        NewTestLayoutActionGroup_SetButtonCount(g_selectionGroup, 2, counts.selectionReplaceNext, counts.hasSelectionCounts);
    }

    NewTestLayout_GetFindText(currentFind, sizeof(currentFind));

    if (lstrcmp(currentFind, g_lastFindText) != 0)
    {
        NewTestLayout_CopyText(g_lastFindText, sizeof(g_lastFindText), currentFind);

        Debug_Log(
            "NewLayout",
            "FindTextChanged",
            "Find text='%s'",
            g_lastFindText
        );
    }
}