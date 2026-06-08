#include "ui_drawing.h"

void Ui_FillSolidRect(
    HDC hdc,
    const RECT *rc,
    COLORREF color
)
{
    HBRUSH brush;

    if (!hdc || !rc)
        return;

    brush = CreateSolidBrush(color);

    if (!brush)
        return;

    FillRect(hdc, rc, brush);
    DeleteObject(brush);
}

void Ui_DrawLine(
    HDC hdc,
    int x1,
    int y1,
    int x2,
    int y2,
    COLORREF color
)
{
    HPEN pen;
    HGDIOBJ oldPen;

    if (!hdc)
        return;

    pen = CreatePen(PS_SOLID, 1, color);

    if (!pen)
        return;

    oldPen = SelectObject(hdc, pen);

    MoveToEx(hdc, x1, y1, NULL);
    LineTo(hdc, x2, y2);

    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}

void Ui_DrawRectBorder(
    HDC hdc,
    const RECT *rc,
    COLORREF color
)
{
    HPEN pen;
    HGDIOBJ oldPen;
    HGDIOBJ oldBrush;

    if (!hdc || !rc)
        return;

    pen = CreatePen(PS_SOLID, 1, color);

    if (!pen)
        return;

    oldPen = SelectObject(hdc, pen);
    oldBrush = SelectObject(hdc, GetStockObject(HOLLOW_BRUSH));

    Rectangle(
        hdc,
        rc->left,
        rc->top,
        rc->right,
        rc->bottom
    );

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}

void Ui_DrawRectBorderThickness(
    HDC hdc,
    const RECT *rc,
    COLORREF color,
    int thickness
)
{
    RECT r;
    int i;

    if (!hdc || !rc)
        return;

    if (thickness < 1)
        thickness = 1;

    r = *rc;

    for (i = 0; i < thickness; i++)
    {
        Ui_DrawRectBorder(hdc, &r, color);
        InflateRect(&r, -1, -1);

        if (r.right <= r.left || r.bottom <= r.top)
            break;
    }
}

void Ui_DrawInsetBorder(
    HDC hdc,
    const RECT *rc,
    COLORREF lightColor,
    COLORREF shadowColor,
    int sunken
)
{
    COLORREF topLeft;
    COLORREF bottomRight;

    if (!hdc || !rc)
        return;

    if (sunken)
    {
        topLeft = shadowColor;
        bottomRight = lightColor;
    }
    else
    {
        topLeft = lightColor;
        bottomRight = shadowColor;
    }

    Ui_DrawLine(hdc, rc->left, rc->top, rc->right - 1, rc->top, topLeft);
    Ui_DrawLine(hdc, rc->left, rc->top, rc->left, rc->bottom - 1, topLeft);

    Ui_DrawLine(hdc, rc->left, rc->bottom - 1, rc->right - 1, rc->bottom - 1, bottomRight);
    Ui_DrawLine(hdc, rc->right - 1, rc->top, rc->right - 1, rc->bottom - 1, bottomRight);
}

void Ui_DrawCenteredText(
    HDC hdc,
    HFONT font,
    const char *text,
    const RECT *rc,
    COLORREF color,
    UINT format
)
{
    HGDIOBJ oldFont;
    int oldBkMode;
    RECT drawRc;

    if (!hdc || !rc || !text)
        return;

    drawRc = *rc;

    oldFont = NULL;

    if (font)
        oldFont = SelectObject(hdc, font);

    oldBkMode = SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, color);

    DrawText(
        hdc,
        text,
        -1,
        &drawRc,
        format
    );

    SetBkMode(hdc, oldBkMode);

    if (oldFont)
        SelectObject(hdc, oldFont);
}

HFONT Ui_CreateFontSimple(
    const char *faceName,
    int height,
    int weight,
    int fixedPitch
)
{
    DWORD pitch;

    if (!faceName)
        faceName = "Segoe UI";

    if (height == 0)
        height = -17;

    if (weight == 0)
        weight = FW_NORMAL;

    pitch = fixedPitch ?
        (FIXED_PITCH | FF_MODERN) :
        (DEFAULT_PITCH | FF_DONTCARE);

    return CreateFont(
        height,
        0,
        0,
        0,
        weight,
        FALSE,
        FALSE,
        FALSE,
        ANSI_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY,
        pitch,
        faceName
    );
}

void Ui_DeleteFontIfSet(
    HFONT *font
)
{
    if (!font)
        return;

    if (*font)
        DeleteObject(*font);

    *font = NULL;
}

int Ui_DrawToMemoryThenBlit(
    HDC targetDc,
    const RECT *targetRc,
    void *context,
    UiBufferedDrawProc drawProc
)
{
    HDC memoryDc;
    HBITMAP bitmap;
    HGDIOBJ oldBitmap;
    RECT memoryRc;
    int width;
    int height;

    if (!targetDc || !targetRc || !drawProc)
        return 0;

    width = targetRc->right - targetRc->left;
    height = targetRc->bottom - targetRc->top;

    if (width <= 0 || height <= 0)
        return 0;

    memoryDc = CreateCompatibleDC(targetDc);

    if (!memoryDc)
        return 0;

    bitmap = CreateCompatibleBitmap(targetDc, width, height);

    if (!bitmap)
    {
        DeleteDC(memoryDc);
        return 0;
    }

    oldBitmap = SelectObject(memoryDc, bitmap);

    SetRect(&memoryRc, 0, 0, width, height);

    drawProc(context, memoryDc, &memoryRc);

    BitBlt(
        targetDc,
        targetRc->left,
        targetRc->top,
        width,
        height,
        memoryDc,
        0,
        0,
        SRCCOPY
    );

    SelectObject(memoryDc, oldBitmap);
    DeleteObject(bitmap);
    DeleteDC(memoryDc);

    return 1;
}

void Ui_DoubleBufferedPaint(
    HWND hwnd,
    void *context,
    UiBufferedDrawProc drawProc
)
{
    PAINTSTRUCT ps;
    HDC hdc;
    RECT rc;

    if (!hwnd)
        return;

    hdc = BeginPaint(hwnd, &ps);

    GetClientRect(hwnd, &rc);

    if (!drawProc)
    {
        EndPaint(hwnd, &ps);
        return;
    }

    if (!Ui_DrawToMemoryThenBlit(hdc, &rc, context, drawProc))
        drawProc(context, hdc, &rc);

    EndPaint(hwnd, &ps);
}