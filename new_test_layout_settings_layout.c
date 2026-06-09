#include "new_test_layout_settings_internal.h"

static void Settings_ClampScroll(
    NewTestLayoutSettingsPanel *panel,
    int visibleContentHeight
)
{
    int maxScroll;

    if (!panel)
        return;

    maxScroll = panel->contentHeight - visibleContentHeight;

    if (maxScroll < 0)
        maxScroll = 0;

    if (panel->scrollY < 0)
        panel->scrollY = 0;

    if (panel->scrollY > maxScroll)
        panel->scrollY = maxScroll;
}

void NewTestLayoutSettings_Layout(
    NewTestLayoutSettingsPanel *panel,
    const RECT *parentClientRect
)
{
    int margin;
    int gap;
    int rowH;
    int x;
    int y;
    int contentTop;
    int w;
    int h;
    int labelW;
    int editW;
    int saveW;
    int resetW;
    int closeW;
    int buttonX;
    int visibleContentHeight;
    int i;

    if (!panel || !panel->container || !parentClientRect)
        return;

    margin = panel->config.overlayMargin;
    gap = panel->config.gap;
    rowH = panel->config.rowHeight;

    if (margin < 4)
        margin = 4;

    if (gap < 2)
        gap = 2;

    if (rowH < 20)
        rowH = 20;

    w = panel->config.overlayWidth;

    if (w < 320)
        w = 320;

    if (w > parentClientRect->right - parentClientRect->left - margin * 2)
        w = parentClientRect->right - parentClientRect->left - margin * 2;

    if (w < 160)
        w = 160;

    h = parentClientRect->bottom - parentClientRect->top - margin * 2;

    if (h < 140)
        h = 140;

    x = parentClientRect->right - margin - w;
    y = parentClientRect->top + margin;

    MoveWindow(panel->container, x, y, w, h, TRUE);

    x = margin;
    y = margin;

    saveW = 94;
    resetW = 56;
    closeW = 58;

    buttonX = w - margin - closeW;

    MoveWindow(
        panel->closeButton,
        buttonX,
        y,
        closeW,
        rowH,
        TRUE
    );

    buttonX -= gap + resetW;

    MoveWindow(
        panel->resetDefaultsButton,
        buttonX,
        y,
        resetW,
        rowH,
        TRUE
    );

    buttonX -= gap + saveW;

    MoveWindow(
        panel->saveCloseButton,
        buttonX,
        y,
        saveW,
        rowH,
        TRUE
    );

    MoveWindow(
        panel->titleLabel,
        x,
        y,
        buttonX - gap - x,
        rowH,
        TRUE
    );

    contentTop = margin + rowH + gap * 2;
    visibleContentHeight = h - contentTop - margin;

    if (visibleContentHeight < 1)
        visibleContentHeight = 1;

    panel->contentHeight =
        NTL_SETTINGS_INT_COUNT * (rowH + gap) +
        gap +
        NTL_SETTINGS_BOOL_COUNT * (rowH + gap);

    Settings_ClampScroll(panel, visibleContentHeight);

    y = contentTop - panel->scrollY;

    labelW = (w - margin * 2 - gap) / 2;
    editW = w - margin * 2 - labelW - gap;

    if (labelW < 120)
        labelW = 120;

    if (editW < 60)
        editW = 60;

    for (i = 0; i < NTL_SETTINGS_INT_COUNT; i++)
    {
        MoveWindow(
            panel->intLabels[i],
            margin,
            y,
            labelW,
            rowH,
            TRUE
        );

        MoveWindow(
            panel->intEdits[i],
            margin + labelW + gap,
            y,
            editW,
            rowH,
            TRUE
        );

        y += rowH + gap;
    }

    y += gap;

    for (i = 0; i < NTL_SETTINGS_BOOL_COUNT; i++)
    {
        MoveWindow(
            panel->boolChecks[i],
            margin,
            y,
            w - margin * 2,
            rowH,
            TRUE
        );

        y += rowH + gap;
    }

    InvalidateRect(panel->container, NULL, TRUE);
}