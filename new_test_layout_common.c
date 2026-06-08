#include "new_test_layout_controls_internal.h"

void Ntl_CopyText(char *dest, int destSize, const char *src)
{
    Ui_CopyText(dest, destSize, src);
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
        Controls own their own font handles. Do not copy borrowed font
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
    Ui_FillSolidRect(hdc, rc, color);
}

void Ntl_DrawBorder(HDC hdc, const RECT *rc, COLORREF color)
{
    Ui_DrawRectBorder(hdc, rc, color);
}

void Ntl_DoubleBufferedPaint(
    HWND hwnd,
    void *context,
    NtlBufferedDrawProc drawProc
)
{
    Ui_DoubleBufferedPaint(hwnd, context, drawProc);
}

int Ntl_SetWindowRectIfChanged(
    HWND hwnd,
    const RECT *rc,
    UINT flags
)
{
    return Ui_SetWindowRectIfChanged(hwnd, rc, flags);
}

void Ntl_InvalidateNoErase(HWND hwnd)
{
    Ui_InvalidateNoErase(hwnd);
}

void Ntl_RedrawNoErase(
    HWND hwnd,
    int updateNow,
    int includeChildren
)
{
    Ui_RedrawNoErase(hwnd, updateNow, includeChildren);
}