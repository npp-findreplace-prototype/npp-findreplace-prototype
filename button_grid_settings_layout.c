#include <windows.h>

#include "button_grid_internal.h"
#include "button_grid_settings.h"
#include "button_grid_settings_internal.h"

static char ButtonGrid_SettingsLowerAscii(char c)
{
    if (c >= 'A' && c <= 'Z')
        return (char)(c - 'A' + 'a');

    return c;
}

static int ButtonGrid_SettingsTextContainsNoCase(
    const char *text,
    const char *find
)
{
    int i;
    int j;

    if (!find || !find[0])
        return 1;

    if (!text)
        return 0;

    for (i = 0; text[i]; i++)
    {
        j = 0;

        while (find[j] &&
               text[i + j] &&
               ButtonGrid_SettingsLowerAscii(text[i + j]) ==
               ButtonGrid_SettingsLowerAscii(find[j]))
        {
            j++;
        }

        if (!find[j])
            return 1;
    }

    return 0;
}

static void ButtonGrid_SettingsGetFilterText(
    HWND pageHwnd,
    char *buffer,
    int bufferSize
)
{
    HWND edit;

    if (!buffer || bufferSize <= 0)
        return;

    buffer[0] = '\0';

    edit = GetDlgItem(pageHwnd, BG_SETTINGS_ID_FILTER);

    if (edit)
        GetWindowText(edit, buffer, bufferSize);
}

static int ButtonGrid_SettingMatchesFilter(
    const ButtonGridSettingDefinition *def,
    const char *filterText
)
{
    if (!filterText || !filterText[0])
        return 1;

    if (ButtonGrid_SettingsTextContainsNoCase(def->label, filterText))
        return 1;

    if (ButtonGrid_SettingsTextContainsNoCase(def->key, filterText))
        return 1;

    return 0;
}

void ButtonGrid_SettingsSetScrollPos(HWND pageHwnd, int pos)
{
    SCROLLINFO si;

    ZeroMemory(&si, sizeof(si));

    si.cbSize = sizeof(si);
    si.fMask = SIF_POS;
    si.nPos = pos;

    SetScrollInfo(pageHwnd, SB_VERT, &si, TRUE);
}

void ButtonGrid_LayoutSettingsPage(ButtonGrid *grid)
{
    if (!grid || !grid->settingsPageHwnd)
        return;

    ButtonGrid_SettingsLayoutControls(grid->settingsPageHwnd);
}

static void ButtonGrid_SettingsShowSettingControls(
    HWND pageHwnd,
    int index,
    int show
)
{
    HWND label;
    HWND primary;
    HWND raw;

    label = ButtonGrid_SettingsGetLabelControl(pageHwnd, index);
    primary = ButtonGrid_SettingsGetPrimaryControl(pageHwnd, index);
    raw = ButtonGrid_SettingsGetRawControl(pageHwnd, index);

    if (label)
        ShowWindow(label, show ? SW_SHOW : SW_HIDE);

    if (primary)
        ShowWindow(primary, show ? SW_SHOW : SW_HIDE);

    if (raw)
        ShowWindow(raw, show ? SW_SHOW : SW_HIDE);
}

static int ButtonGrid_SettingsGetVisibleCount(const char *filterText)
{
    int i;
    int count;
    int settingCount;

    count = 0;
    settingCount = ButtonGrid_SettingsGetCount();

    for (i = 0; i < settingCount; i++)
    {
        if (ButtonGrid_SettingMatchesFilter(
                ButtonGrid_SettingsGetDefinition(i),
                filterText
            ))
        {
            count++;
        }
    }

    return count;
}

static int ButtonGrid_SettingsGetClampedScrollY(
    HWND pageHwnd,
    int contentH,
    int clientH
)
{
    SCROLLINFO si;
    int maxScroll;
    int scrollY;

    maxScroll = contentH - clientH;

    if (maxScroll < 0)
        maxScroll = 0;

    ZeroMemory(&si, sizeof(si));

    si.cbSize = sizeof(si);
    si.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;

    GetScrollInfo(pageHwnd, SB_VERT, &si);

    scrollY = si.nPos;

    if (scrollY > maxScroll)
        scrollY = maxScroll;

    if (scrollY < 0)
        scrollY = 0;

    return scrollY;
}

static void ButtonGrid_SettingsUpdateScrollBar(
    HWND pageHwnd,
    int contentH,
    int clientH,
    int scrollY
)
{
    SCROLLINFO si;

    ZeroMemory(&si, sizeof(si));

    si.cbSize = sizeof(si);
    si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
    si.nMin = 0;
    si.nMax = contentH;
    si.nPage = clientH;
    si.nPos = scrollY;

    SetScrollInfo(pageHwnd, SB_VERT, &si, TRUE);
}

static void ButtonGrid_SettingsGetAdaptiveColumns(
    int clientW,
    int *labelX,
    int *labelW,
    int *primaryX,
    int *primaryW,
    int *rawX,
    int *rawW
)
{
    int rightMargin;
    int availableW;

    rightMargin = BG_SETTINGS_LEFT;

    *labelX = BG_SETTINGS_LEFT;
    *rawW = BG_SETTINGS_RAW_WIDTH;

    availableW = clientW - BG_SETTINGS_LEFT - rightMargin;

    if (availableW < 620)
        *rawW = 150;

    if (availableW < 500)
        *rawW = BG_SETTINGS_MIN_RAW_WIDTH;

    if (availableW < 390)
        *rawW = 90;

    if (*rawW < BG_SETTINGS_MIN_RAW_WIDTH)
        *rawW = BG_SETTINGS_MIN_RAW_WIDTH;

    *rawX = clientW - rightMargin - *rawW;

    if (*rawX <
        BG_SETTINGS_LEFT +
        BG_SETTINGS_MIN_LABEL_WIDTH +
        BG_SETTINGS_GAP +
        BG_SETTINGS_MIN_PRIMARY_WIDTH +
        BG_SETTINGS_GAP)
    {
        *rawX =
            BG_SETTINGS_LEFT +
            BG_SETTINGS_MIN_LABEL_WIDTH +
            BG_SETTINGS_GAP +
            BG_SETTINGS_MIN_PRIMARY_WIDTH +
            BG_SETTINGS_GAP;
    }

    *labelW = BG_SETTINGS_LABEL_WIDTH;
    *primaryX = *labelX + *labelW + BG_SETTINGS_GAP;

    if (*primaryX + BG_SETTINGS_MIN_PRIMARY_WIDTH + BG_SETTINGS_GAP > *rawX)
    {
        *labelW =
            *rawX -
            *labelX -
            BG_SETTINGS_GAP -
            BG_SETTINGS_MIN_PRIMARY_WIDTH -
            BG_SETTINGS_GAP;

        if (*labelW < BG_SETTINGS_MIN_LABEL_WIDTH)
            *labelW = BG_SETTINGS_MIN_LABEL_WIDTH;

        *primaryX = *labelX + *labelW + BG_SETTINGS_GAP;
    }

    *primaryW = *rawX - BG_SETTINGS_GAP - *primaryX;

    if (*primaryW < BG_SETTINGS_MIN_PRIMARY_WIDTH)
        *primaryW = BG_SETTINGS_MIN_PRIMARY_WIDTH;
}

static void ButtonGrid_SettingsLayoutHeader(
    HWND pageHwnd,
    int clientW,
    const char *filterText
)
{
    int filterX;
    int filterClearX;
    int filterW;

    MoveWindow(
        GetDlgItem(pageHwnd, BG_SETTINGS_ID_CLOSE),
        clientW - 84,
        8,
        72,
        24,
        TRUE
    );

    filterX = BG_SETTINGS_LEFT + 54;
    filterClearX = clientW - BG_SETTINGS_LEFT - 24;
    filterW = filterClearX - BG_SETTINGS_GAP - filterX;

    if (filterW < 80)
        filterW = 80;

    MoveWindow(
        GetDlgItem(pageHwnd, BG_SETTINGS_ID_FILTER_LABEL),
        BG_SETTINGS_LEFT,
        43,
        50,
        22,
        TRUE
    );

    MoveWindow(
        GetDlgItem(pageHwnd, BG_SETTINGS_ID_FILTER),
        filterX,
        38,
        filterW,
        24,
        TRUE
    );

    MoveWindow(
        GetDlgItem(pageHwnd, BG_SETTINGS_ID_FILTER_CLEAR),
        filterClearX,
        38,
        24,
        24,
        TRUE
    );

    ShowWindow(
        GetDlgItem(pageHwnd, BG_SETTINGS_ID_FILTER_CLEAR),
        filterText[0] ? SW_SHOW : SW_HIDE
    );
}

static void ButtonGrid_SettingsLayoutOneRow(
    HWND pageHwnd,
    int index,
    int rowY,
    int labelX,
    int labelW,
    int primaryX,
    int primaryW,
    int rawX,
    int rawW
)
{
    const ButtonGridSettingDefinition *def;
    HWND primary;

    def = ButtonGrid_SettingsGetDefinition(index);
    primary = ButtonGrid_SettingsGetPrimaryControl(pageHwnd, index);

    MoveWindow(
        ButtonGrid_SettingsGetLabelControl(pageHwnd, index),
        labelX,
        rowY + 5,
        labelW,
        22,
        TRUE
    );

    if (primary)
    {
        MoveWindow(
            primary,
            primaryX,
            rowY,
            primaryW,
            def->type == BG_SETTING_ENUM ? 220 : 26,
            TRUE
        );
    }

    MoveWindow(
        ButtonGrid_SettingsGetRawControl(pageHwnd, index),
        rawX,
        rowY,
        rawW,
        24,
        TRUE
    );
}

void ButtonGrid_SettingsLayoutControls(HWND pageHwnd)
{
    RECT rc;
    char filterText[128];

    int clientW;
    int clientH;
    int contentH;
    int scrollY;

    int labelX;
    int labelW;
    int primaryX;
    int primaryW;
    int rawX;
    int rawW;

    int i;
    int rowY;
    int visibleIndex;
    int visibleCount;
    int settingCount;
    int matches;

    if (!ButtonGrid_SettingsGetGrid(pageHwnd))
        return;

    GetClientRect(pageHwnd, &rc);

    clientW = rc.right - rc.left;
    clientH = rc.bottom - rc.top;

    ButtonGrid_SettingsGetFilterText(
        pageHwnd,
        filterText,
        sizeof(filterText)
    );

    visibleCount = ButtonGrid_SettingsGetVisibleCount(filterText);

    contentH =
        BG_SETTINGS_TOP +
        visibleCount * BG_SETTINGS_ROW_HEIGHT +
        BG_SETTINGS_BOTTOM_PADDING;

    if (contentH < clientH)
        contentH = clientH;

    scrollY = ButtonGrid_SettingsGetClampedScrollY(
        pageHwnd,
        contentH,
        clientH
    );

    ButtonGrid_SettingsUpdateScrollBar(
        pageHwnd,
        contentH,
        clientH,
        scrollY
    );

    ButtonGrid_SettingsLayoutHeader(pageHwnd, clientW, filterText);

    ButtonGrid_SettingsGetAdaptiveColumns(
        clientW,
        &labelX,
        &labelW,
        &primaryX,
        &primaryW,
        &rawX,
        &rawW
    );

    visibleIndex = 0;
    settingCount = ButtonGrid_SettingsGetCount();

    for (i = 0; i < settingCount; i++)
    {
        matches = ButtonGrid_SettingMatchesFilter(
            ButtonGrid_SettingsGetDefinition(i),
            filterText
        );

        ButtonGrid_SettingsShowSettingControls(pageHwnd, i, matches);

        if (!matches)
            continue;

        rowY =
            BG_SETTINGS_TOP +
            visibleIndex * BG_SETTINGS_ROW_HEIGHT -
            scrollY;

        ButtonGrid_SettingsLayoutOneRow(
            pageHwnd,
            i,
            rowY,
            labelX,
            labelW,
            primaryX,
            primaryW,
            rawX,
            rawW
        );

        visibleIndex++;
    }
}

void ButtonGrid_SettingsScroll(HWND pageHwnd, int request, int wheelDelta)
{
    SCROLLINFO si;
    int oldPos;
    int newPos;

    ZeroMemory(&si, sizeof(si));

    si.cbSize = sizeof(si);
    si.fMask = SIF_ALL;

    GetScrollInfo(pageHwnd, SB_VERT, &si);

    oldPos = si.nPos;
    newPos = oldPos;

    if (request == SB_LINEUP)
        newPos -= BG_SETTINGS_ROW_HEIGHT;
    else if (request == SB_LINEDOWN)
        newPos += BG_SETTINGS_ROW_HEIGHT;
    else if (request == SB_PAGEUP)
        newPos -= (int)si.nPage;
    else if (request == SB_PAGEDOWN)
        newPos += (int)si.nPage;
    else if (request == SB_THUMBTRACK)
        newPos = si.nTrackPos;
    else if (request == -1)
        newPos -= wheelDelta / 3;

    if (newPos < si.nMin)
        newPos = si.nMin;

    if (newPos > si.nMax - (int)si.nPage + 1)
        newPos = si.nMax - (int)si.nPage + 1;

    if (newPos < 0)
        newPos = 0;

    si.fMask = SIF_POS;
    si.nPos = newPos;

    SetScrollInfo(pageHwnd, SB_VERT, &si, TRUE);

    if (newPos != oldPos)
        ButtonGrid_SettingsLayoutControls(pageHwnd);
}