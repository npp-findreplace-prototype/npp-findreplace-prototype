#include "npp_mockup_internal.h"

void Npp_FreeDocument(NppDocument *doc)
{
    if (!doc)
        return;

    if (doc->text)
        free(doc->text);

    ZeroMemory(doc, sizeof(*doc));
}

void Npp_FreePane(NppPane *pane)
{
    int i;

    if (!pane)
        return;

    for (i = 0; i < pane->docCount; i++)
        Npp_FreeDocument(&pane->docs[i]);

    pane->docCount = 0;
    pane->activeDoc = -1;
}

HWND Npp_CreateTabControl(HWND parent, int id)
{
    HWND hwnd;

    hwnd = CreateWindowEx(
        0,
        WC_TABCONTROL,
        "",
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | TCS_TABS,
        0,
        0,
        100,
        24,
        parent,
        (HMENU)id,
        g_hInstance,
        NULL
    );

    SendMessage(hwnd, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);

    return hwnd;
}

void Npp_PaneSaveActiveDocument(NppPane *pane)
{
    NppDocument *doc;
    int len;
    char *text;

    if (!pane || !pane->edit)
        return;

    if (pane->activeDoc < 0 || pane->activeDoc >= pane->docCount)
        return;

    doc = &pane->docs[pane->activeDoc];

    len = GetWindowTextLength(pane->edit);

    text = (char *)malloc(len + 1);

    if (!text)
        return;

    GetWindowText(pane->edit, text, len + 1);
    text[len] = '\0';

    if (doc->text)
        free(doc->text);

    doc->text = text;

    Npp_GetEditSelection(pane->edit, &doc->selStart, &doc->selEnd);
    doc->firstVisibleLine = (int)SendMessage(pane->edit, EM_GETFIRSTVISIBLELINE, 0, 0);
}

void Npp_PaneLoadActiveDocument(NppPane *pane)
{
    NppDocument *doc;
    int currentFirstLine;

    if (!pane || !pane->edit)
        return;

    if (pane->activeDoc < 0 || pane->activeDoc >= pane->docCount)
    {
        SetWindowText(pane->edit, "");
        return;
    }

    doc = &pane->docs[pane->activeDoc];

    SendMessage(pane->edit, WM_SETREDRAW, FALSE, 0);

    SetWindowText(pane->edit, doc->text ? doc->text : "");
    Npp_SetEditTheme(pane->edit);
    Npp_SetEditSelection(pane->edit, doc->selStart, doc->selEnd);

    currentFirstLine = (int)SendMessage(pane->edit, EM_GETFIRSTVISIBLELINE, 0, 0);

    if (doc->firstVisibleLine != currentFirstLine)
    {
        SendMessage(
            pane->edit,
            EM_LINESCROLL,
            0,
            doc->firstVisibleLine - currentFirstLine
        );
    }

    SendMessage(pane->edit, WM_SETREDRAW, TRUE, 0);
    InvalidateRect(pane->edit, NULL, FALSE);
}

int Npp_PaneAddDocumentFromText(
    NppPane *pane,
    const char *title,
    const char *path,
    const char *text,
    int selectNew
)
{
    NppDocument *doc;
    TCITEM item;
    int index;

    if (!pane)
        return 0;

    if (pane->docCount >= NPP_MAX_DOCS)
        return 0;

    if (pane->activeDoc >= 0)
        Npp_PaneSaveActiveDocument(pane);

    index = pane->docCount;
    doc = &pane->docs[index];

    ZeroMemory(doc, sizeof(*doc));

    Npp_CopyText(doc->title, sizeof(doc->title), title ? title : "new");
    Npp_CopyText(doc->path, sizeof(doc->path), path ? path : "");

    doc->text = Npp_DuplicateText(text ? text : "");
    doc->selStart = 0;
    doc->selEnd = 0;
    doc->firstVisibleLine = 0;

    ZeroMemory(&item, sizeof(item));
    item.mask = TCIF_TEXT;
    item.pszText = doc->title;

    SendMessage(pane->tab, TCM_INSERTITEM, index, (LPARAM)&item);

    pane->docCount++;

    if (selectNew || pane->activeDoc < 0)
    {
        pane->activeDoc = index;
        SendMessage(pane->tab, TCM_SETCURSEL, index, 0);
        Npp_PaneLoadActiveDocument(pane);
    }

    return 1;
}

void Npp_PaneSelectDocument(NppPane *pane, int index)
{
    if (!pane)
        return;

    if (index < 0 || index >= pane->docCount)
        return;

    if (pane->activeDoc == index)
        return;

    Npp_PaneSaveActiveDocument(pane);

    pane->activeDoc = index;

    SendMessage(pane->tab, TCM_SETCURSEL, index, 0);
    Npp_PaneLoadActiveDocument(pane);
}

void Npp_HandleTabNotify(NMHDR *hdr)
{
    NppPane *pane;
    int index;

    if (!hdr)
        return;

    pane = NULL;

    if (hdr->idFrom == ID_NPP_LEFT_TAB)
        pane = &g_panes[0];
    else if (hdr->idFrom == ID_NPP_RIGHT_TAB)
        pane = &g_panes[1];

    if (!pane)
        return;

    if (hdr->code == TCN_SELCHANGING)
    {
        Npp_PaneSaveActiveDocument(pane);
        return;
    }

    if (hdr->code == TCN_SELCHANGE)
    {
        index = (int)SendMessage(pane->tab, TCM_GETCURSEL, 0, 0);

        if (index >= 0 && index < pane->docCount)
        {
            pane->activeDoc = index;
            g_activePane = pane->paneIndex;
            Npp_PaneLoadActiveDocument(pane);
            SetFocus(pane->edit);
            Npp_UpdateStatus();
        }
    }
}

void Npp_CreateInitialDocuments(void)
{
    const char *leftSample =
        "3540    Find: (?im)^(set-cookie|authorization|token|api_key)\\s*[:=].*$  | Replace: [REDACTED]\r\n"
        "3541    Find: (?im)^\\s*password\\s*[:=].*$                         | Replace: [REDACTED]\r\n"
        "3542    Find: (?m)^\\s*$                                             | Replace:\r\n"
        "\r\n"
        "47. Secrets, credentials, and PII\r\n"
        "48. Database connection strings\r\n"
        "49. Licensing / copyright / SPDX\r\n"
        "50. Additional programming languages\r\n";

    const char *rightSample =
        "LiteralSearch\r\n"
        "EscapedLiteralSearch\r\n"
        "RegExSearch\r\n"
        "SemanticSearch\r\n"
        "CaseSensitive\r\n"
        "DiacriticSensitive\r\n"
        "DotIncludesNewline\r\n"
        "FuzzyLogicSearch\r\n"
        "WrapAround\r\n"
        "WholeWord\r\n"
        "BooleanSearch\r\n"
        "Settings\r\n"
        "\r\n"
        "the tooltip for each will be the names above but with spaces and normal case\r\n"
        "\r\n"
        "These buttons are radio buttons with each other.\r\n"
        "The rest of the buttons are toggle buttons.\r\n";

    Npp_PaneAddDocumentFromText(&g_panes[0], "new 167", "", leftSample, 1);
    Npp_PaneAddDocumentFromText(&g_panes[0], "new 176", "", "", 0);
    Npp_PaneAddDocumentFromText(&g_panes[0], "new 177", "", "", 0);

    Npp_PaneAddDocumentFromText(&g_panes[1], "new 173", "", rightSample, 1);
    Npp_PaneAddDocumentFromText(&g_panes[1], "new 172", "", "", 0);
    Npp_PaneAddDocumentFromText(&g_panes[1], "new 174", "", "", 0);
    Npp_PaneAddDocumentFromText(&g_panes[1], "new 175", "", "", 0);

    g_activePane = 1;
    SetFocus(g_panes[1].edit);
}