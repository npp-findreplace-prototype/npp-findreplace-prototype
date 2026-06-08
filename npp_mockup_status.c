#include "npp_mockup_internal.h"

void Npp_UpdateStatus(void)
{
    NppMockupTextInfo info;
    NppPane *pane;
    NppDocument *doc;
    char status[512];

    if (!g_status)
        return;

    if (!NppMockupWindow_GetActiveTextInfo(&info))
    {
        SetWindowText(g_status, "");
        return;
    }

    pane = &g_panes[g_activePane];

    doc = NULL;

    if (pane->activeDoc >= 0 && pane->activeDoc < pane->docCount)
        doc = &pane->docs[pane->activeDoc];

    if (info.hasSelection)
    {
        wsprintf(
            status,
            "Pane: %s | File: %s | length: %d | lines: %d | Ln: %d | Col: %d | Pos: %d | Sel: %d chars, %d lines | SelStart: %d/Ln %d | SelEnd: %d/Ln %d",
            g_activePane == 0 ? "left" : "right",
            doc ? doc->title : "",
            info.characterCount,
            info.lineCount,
            info.caretLine,
            info.caretColumn,
            info.caretCharacter,
            info.selectedCharacterCount,
            info.selectedLineCount,
            info.selectionStartCharacter,
            info.selectionStartLine,
            info.selectionEndCharacter,
            info.selectionEndLine
        );
    }
    else
    {
        wsprintf(
            status,
            "Pane: %s | File: %s | length: %d | lines: %d | Ln: %d | Col: %d | Pos: %d",
            g_activePane == 0 ? "left" : "right",
            doc ? doc->title : "",
            info.characterCount,
            info.lineCount,
            info.caretLine,
            info.caretColumn,
            info.caretCharacter
        );
    }

    SetWindowText(g_status, status);
}