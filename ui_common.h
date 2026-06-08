#ifndef UI_COMMON_H
#define UI_COMMON_H

#include "win_compat.h"

void Ui_CopyText(
    char *dest,
    int destSize,
    const char *src
);

void Ui_AppendText(
    char *dest,
    int destSize,
    const char *src
);

int Ui_SameText(
    const char *a,
    const char *b
);

int Ui_SameTextI(
    const char *a,
    const char *b
);

int Ui_MinInt(
    int a,
    int b
);

int Ui_MaxInt(
    int a,
    int b
);

int Ui_ClampInt(
    int value,
    int minValue,
    int maxValue
);

void Ui_SetRect(
    RECT *rc,
    int left,
    int top,
    int right,
    int bottom
);

int Ui_RectWidth(
    const RECT *rc
);

int Ui_RectHeight(
    const RECT *rc
);

int Ui_GetWindowRectInParent(
    HWND hwnd,
    RECT *rc
);

int Ui_WindowRectMatches(
    HWND hwnd,
    int x,
    int y,
    int width,
    int height
);

int Ui_SetWindowPosIfChanged(
    HWND hwnd,
    HWND insertAfter,
    int x,
    int y,
    int width,
    int height,
    UINT flags
);

int Ui_SetWindowRectIfChanged(
    HWND hwnd,
    const RECT *rc,
    UINT flags
);

void Ui_ShowWindowIfChanged(
    HWND hwnd,
    int show
);

void Ui_EnableWindowIfChanged(
    HWND hwnd,
    int enabled
);

void Ui_InvalidateNoErase(
    HWND hwnd
);

void Ui_RedrawNoErase(
    HWND hwnd,
    int updateNow,
    int includeChildren
);

void Ui_BringWindowToTopNoActivate(
    HWND hwnd
);

#endif