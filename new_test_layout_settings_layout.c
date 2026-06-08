#include "new_test_layout_settings_internal.h"

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
    int w;
    int h;
    int labelW;
    int editW;
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

    if (w < 220)
        w = 220;

    if (w > parentClientRect->right - parentClientRect->left - margin * 2)
        w = parentClientRect->right - parentClientRect->left - margin * 2;

    if (w < 120)
        w = 120;

    h = parentClientRect->bottom - parentClientRect->top - margin * 2;

    if (h < 120)
        h = 120;

    x = parentClientRect->right - margin - w;
    y = parentClientRect->top + margin;

    MoveWindow(panel->container, x, y, w, h, TRUE);

    x = margin;
    y = margin;

    MoveWindow(
        panel->titleLabel,
        x,
        y,
        w - margin * 2 - 36,
        rowH,
        TRUE
    );

    MoveWindow(
        panel->closeButton,
        w - margin - 30,
        y,
        30,
        rowH,
        TRUE
    );

    y += rowH + gap * 2;

    labelW = (w - margin * 2 - gap) / 2;
    editW = w - margin * 2 - labelW - gap;

    if (labelW < 90)
        labelW = 90;

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