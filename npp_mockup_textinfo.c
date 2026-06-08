#include "npp_mockup_internal.h"

HWND NppMockupWindow_GetActiveRichEdit(void)
{
    if (g_activePane < 0 || g_activePane > 1)
        g_activePane = 0;

    return g_panes[g_activePane].edit;
}

int NppMockupWindow_GetActiveTextInfo(NppMockupTextInfo *info)
{
    HWND edit;
    DWORD selStart;
    DWORD selEnd;
    int caret;
    int lineStart;
    int selectionEndForLine;
    int selStartLine;
    int selEndLine;

    if (!info)
        return 0;

    ZeroMemory(info, sizeof(*info));

    edit = NppMockupWindow_GetActiveRichEdit();

    if (!edit)
        return 0;

    Npp_GetEditSelection(edit, &selStart, &selEnd);

    caret = (int)selEnd;

    info->characterCount = GetWindowTextLength(edit);
    info->lineCount = Npp_GetLineCount(edit);

    info->caretCharacter = caret;
    info->caretLine = Npp_GetLineFromChar(edit, caret) + 1;

    lineStart = Npp_GetLineIndex(edit, info->caretLine - 1);

    if (lineStart < 0)
        lineStart = 0;

    info->caretColumn = caret - lineStart + 1;

    info->selectionStartCharacter = (int)selStart;
    info->selectionEndCharacter = (int)selEnd;
    info->selectedCharacterCount = (int)(selEnd - selStart);
    info->hasSelection = info->selectedCharacterCount > 0;

    if (info->hasSelection)
    {
        selectionEndForLine = (int)selEnd;

        if (selectionEndForLine > (int)selStart)
            selectionEndForLine--;

        selStartLine = Npp_GetLineFromChar(edit, (int)selStart) + 1;
        selEndLine = Npp_GetLineFromChar(edit, selectionEndForLine) + 1;

        info->selectionStartLine = selStartLine;
        info->selectionEndLine = selEndLine;
        info->selectedLineCount = selEndLine - selStartLine + 1;

        if (info->selectedLineCount < 1)
            info->selectedLineCount = 1;
    }
    else
    {
        info->selectionStartLine = info->caretLine;
        info->selectionEndLine = info->caretLine;
        info->selectedLineCount = 0;
    }

    return 1;
}

char *NppMockupWindow_AllocActiveText(int *length)
{
    HWND edit;
    int len;
    char *text;

    if (length)
        *length = 0;

    edit = NppMockupWindow_GetActiveRichEdit();

    if (!edit)
        return NULL;

    len = GetWindowTextLength(edit);

    text = (char *)malloc(len + 1);

    if (!text)
        return NULL;

    GetWindowText(edit, text, len + 1);
    text[len] = '\0';

    if (length)
        *length = len;

    return text;
}

void NppMockupWindow_FreeText(char *text)
{
    if (text)
        free(text);
}