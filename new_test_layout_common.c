#include "new_test_layout_controls_internal.h"

void Ntl_CopyText(char *dest, int destSize, const char *src)
{
    if (!dest || destSize <= 0)
        return;

    if (!src)
        src = "";

    lstrcpyn(dest, src, destSize);
    dest[destSize - 1] = '\0';
}

void Ntl_CopyTheme(NewTestLayoutTheme *dest, const NewTestLayoutTheme *src)
{
    if (!dest)
        return;

    if (src)
        CopyMemory(dest, src, sizeof(*dest));
    else
        NewTestLayoutTheme_GetDefault(dest);

    /*
        Controls own their own font handles.  Do not copy borrowed font
        handles from the parent theme, because each control later calls
        NewTestLayoutTheme_DeleteFonts().
    */
    dest->normalFont = NULL;
    dest->monoFont = NULL;
    dest->placeholderFont = NULL;
    dest->buttonFont = NULL;
    dest->titleFont = NULL;
}

void Ntl_SendCommand(HWND parent, HWND hwnd, int id, int notifyCode)
{
    if (!parent)
        return;

    SendMessage(
        parent,
        WM_COMMAND,
        MAKEWPARAM(id, notifyCode),
        (LPARAM)hwnd
    );
}

void Ntl_FillRect(HDC hdc, const RECT *rc, COLORREF color)
{
    HBRUSH brush;

    if (!hdc || !rc)
        return;

    brush = CreateSolidBrush(color);

    if (brush)
    {
        FillRect(hdc, rc, brush);
        DeleteObject(brush);
    }
}

void Ntl_DrawBorder(HDC hdc, const RECT *rc, COLORREF color)
{
    HPEN pen;
    HPEN oldPen;
    HBRUSH oldBrush;

    if (!hdc || !rc)
        return;

    pen = CreatePen(PS_SOLID, 1, color);

    if (!pen)
        return;

    oldPen = (HPEN)SelectObject(hdc, pen);
    oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));

    Rectangle(hdc, rc->left, rc->top, rc->right, rc->bottom);

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}

void Ntl_DoubleBufferedPaint(
    HWND hwnd,
    void *context,
    NtlBufferedDrawProc drawProc
)
{
    PAINTSTRUCT ps;
    HDC hdc;
    HDC memDc;
    HBITMAP bitmap;
    HBITMAP oldBitmap;
    RECT rc;
    int width;
    int height;

    hdc = BeginPaint(hwnd, &ps);

    GetClientRect(hwnd, &rc);

    width = rc.right - rc.left;
    height = rc.bottom - rc.top;

    if (width <= 0 || height <= 0 || !drawProc)
    {
        EndPaint(hwnd, &ps);
        return;
    }

    memDc = CreateCompatibleDC(hdc);

    if (!memDc)
    {
        drawProc(context, hdc, &rc);
        EndPaint(hwnd, &ps);
        return;
    }

    bitmap = CreateCompatibleBitmap(hdc, width, height);

    if (!bitmap)
    {
        DeleteDC(memDc);
        drawProc(context, hdc, &rc);
        EndPaint(hwnd, &ps);
        return;
    }

    oldBitmap = (HBITMAP)SelectObject(memDc, bitmap);

    drawProc(context, memDc, &rc);

    BitBlt(
        hdc,
        0,
        0,
        width,
        height,
        memDc,
        0,
        0,
        SRCCOPY
    );

    SelectObject(memDc, oldBitmap);
    DeleteObject(bitmap);
    DeleteDC(memDc);

    EndPaint(hwnd, &ps);
}