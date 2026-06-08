#ifndef UI_DRAWING_H
#define UI_DRAWING_H

#include "win_compat.h"

typedef void (*UiBufferedDrawProc)(
    void *context,
    HDC hdc,
    RECT *rc
);

void Ui_FillSolidRect(
    HDC hdc,
    const RECT *rc,
    COLORREF color
);

void Ui_DrawLine(
    HDC hdc,
    int x1,
    int y1,
    int x2,
    int y2,
    COLORREF color
);

void Ui_DrawRectBorder(
    HDC hdc,
    const RECT *rc,
    COLORREF color
);

void Ui_DrawRectBorderThickness(
    HDC hdc,
    const RECT *rc,
    COLORREF color,
    int thickness
);

void Ui_DrawInsetBorder(
    HDC hdc,
    const RECT *rc,
    COLORREF lightColor,
    COLORREF shadowColor,
    int sunken
);

void Ui_DrawCenteredText(
    HDC hdc,
    HFONT font,
    const char *text,
    const RECT *rc,
    COLORREF color,
    UINT format
);

HFONT Ui_CreateFontSimple(
    const char *faceName,
    int height,
    int weight,
    int fixedPitch
);

void Ui_DeleteFontIfSet(
    HFONT *font
);

void Ui_DoubleBufferedPaint(
    HWND hwnd,
    void *context,
    UiBufferedDrawProc drawProc
);

int Ui_DrawToMemoryThenBlit(
    HDC targetDc,
    const RECT *targetRc,
    void *context,
    UiBufferedDrawProc drawProc
);

#endif