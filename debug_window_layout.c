#include "debug_window_internal.h"

void Debug_LayoutControls(HWND hwnd)
{
    RECT rc;
    int margin;
    int gap;
    int buttonH;
    int filterH;
    int topH;
    int bottomY;
    int bottomH;
    int leftW;
    int rightX;
    int rightW;
    int y;
    int x;
    int dumpTop;
    int logTop;
    int updateW;
    int loggingW;
    int clearW;
    int rightButtonH;
    int rightButtonW;
    int rightButtonY;
    int rightListTop;

    if (!hwnd)
        return;

    GetClientRect(hwnd, &rc);

    margin = 8;
    gap = 6;
    buttonH = 28;
    filterH = 24;

    updateW = 130;
    loggingW = 145;
    clearW = 100;

    topH = ((rc.bottom - rc.top) * 52) / 100;
    bottomY = topH + gap;
    bottomH = rc.bottom - bottomY - margin;

    leftW = ((rc.right - rc.left) - margin * 3) / 2;
    rightX = margin + leftW + margin;
    rightW = rc.right - rightX - margin;

    if (leftW < 260)
        leftW = 260;

    if (rightW < 180)
        rightW = 180;

    x = margin;
    y = margin;

    SetWindowPos(g_updateButton, NULL, x, y, updateW, buttonH, SWP_NOZORDER | SWP_NOACTIVATE);

    x += updateW + gap;
    SetWindowPos(g_loggingButton, NULL, x, y, loggingW, buttonH, SWP_NOZORDER | SWP_NOACTIVATE);

    x += loggingW + gap;
    SetWindowPos(g_clearLogButton, NULL, x, y, clearW, buttonH, SWP_NOZORDER | SWP_NOACTIVATE);

    y += buttonH + gap;

    SetWindowPos(
        g_dumpFilterEdit,
        NULL,
        margin,
        y,
        leftW,
        filterH,
        SWP_NOZORDER | SWP_NOACTIVATE
    );

    dumpTop = y + filterH + gap;

    SetWindowPos(
        g_dumpEdit,
        NULL,
        margin,
        dumpTop,
        leftW,
        topH - dumpTop - margin,
        SWP_NOZORDER | SWP_NOACTIVATE
    );

    rightButtonH = 28;
    rightButtonW = (rightW - gap) / 2;
    rightButtonY = margin;

    SetWindowPos(
        g_addVariableButton,
        NULL,
        rightX,
        rightButtonY,
        rightButtonW,
        rightButtonH,
        SWP_NOZORDER | SWP_NOACTIVATE
    );

    SetWindowPos(
        g_removeVariableButton,
        NULL,
        rightX + rightButtonW + gap,
        rightButtonY,
        rightButtonW,
        rightButtonH,
        SWP_NOZORDER | SWP_NOACTIVATE
    );

    rightButtonY += rightButtonH + gap;

    SetWindowPos(
        g_showAllVariablesButton,
        NULL,
        rightX,
        rightButtonY,
        rightButtonW,
        rightButtonH,
        SWP_NOZORDER | SWP_NOACTIVATE
    );

    SetWindowPos(
        g_hideAllVariablesButton,
        NULL,
        rightX + rightButtonW + gap,
        rightButtonY,
        rightButtonW,
        rightButtonH,
        SWP_NOZORDER | SWP_NOACTIVATE
    );

    rightListTop = rightButtonY + rightButtonH + gap;

    SetWindowPos(
        g_availableList,
        NULL,
        rightX,
        rightListTop,
        rightW,
        topH - rightListTop - margin,
        SWP_NOZORDER | SWP_NOACTIVATE
    );

    SetWindowPos(
        g_logFilterEdit,
        NULL,
        margin,
        bottomY,
        rc.right - rc.left - margin * 2,
        filterH,
        SWP_NOZORDER | SWP_NOACTIVATE
    );

    logTop = bottomY + filterH + gap;

    SetWindowPos(
        g_logEdit,
        NULL,
        margin,
        logTop,
        rc.right - rc.left - margin * 2,
        bottomH - filterH - gap,
        SWP_NOZORDER | SWP_NOACTIVATE
    );
}