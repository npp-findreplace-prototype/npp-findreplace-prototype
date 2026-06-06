#include "button_grid_internal.h"

static void ButtonGrid_DrawBorder(ButtonGrid *grid, HDC hdc)
{
    RECT rc;
    RECT titleRc;
    HPEN pen;
    HGDIOBJ oldPen;
    HBRUSH backgroundBrush;

    if (!grid || !grid->showBorder)
        return;

    GetClientRect(grid->hwnd, &rc);

    if (rc.right <= rc.left || rc.bottom <= rc.top)
        return;

    rc.left += 2;
    rc.top += 8;
    rc.right -= 2;
    rc.bottom -= 2;

    pen = CreatePen(PS_SOLID, 1, grid->borderColor);
    oldPen = SelectObject(hdc, pen);

    MoveToEx(hdc, rc.left, rc.top, NULL);
    LineTo(hdc, rc.right, rc.top);
    LineTo(hdc, rc.right, rc.bottom);
    LineTo(hdc, rc.left, rc.bottom);
    LineTo(hdc, rc.left, rc.top);

    SelectObject(hdc, oldPen);
    DeleteObject(pen);

    if (grid->borderTitle[0])
    {
        titleRc.left = rc.left + 10;
        titleRc.top = 0;
        titleRc.right = rc.right - 10;
        titleRc.bottom = grid->borderTitleHeight;

        backgroundBrush = (HBRUSH)(COLOR_WINDOW + 1);
        FillRect(hdc, &titleRc, backgroundBrush);

        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, grid->borderTitleColor);

        DrawText(
            hdc,
            grid->borderTitle,
            -1,
            &titleRc,
            DT_LEFT | DT_VCENTER | DT_SINGLELINE
        );
    }
}

LRESULT ButtonGrid_HandlePaint(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc;
    ButtonGrid *grid;

    grid = ButtonGrid_Get(hwnd);

    hdc = BeginPaint(hwnd, &ps);

    ButtonGrid_DrawBorder(grid, hdc);

    EndPaint(hwnd, &ps);

    return 0;
}

static void ButtonGrid_DrawGeneratedFallback(
    HDC hdc,
    const RECT *rc,
    COLORREF color,
    int pictureType
)
{
    HBRUSH brush;
    HPEN pen;
    HGDIOBJ oldPen;
    COLORREF markColor;
    int width;
    int height;

    brush = CreateSolidBrush(color);
    FillRect(hdc, rc, brush);
    DeleteObject(brush);

    width = rc->right - rc->left;
    height = rc->bottom - rc->top;

    if (pictureType == BUTTON_GRID_PICTURE_TYPE_OFF)
        return;

    if (pictureType == BUTTON_GRID_PICTURE_TYPE_ON)
        markColor = RGB(0, 100, 0);
    else
        markColor = RGB(160, 0, 0);

    pen = CreatePen(PS_SOLID, 5, markColor);
    oldPen = SelectObject(hdc, pen);

    if (pictureType == BUTTON_GRID_PICTURE_TYPE_ON)
    {
        MoveToEx(hdc, rc->left + width / 5, rc->top + height / 2, NULL);
        LineTo(hdc, rc->left + (width * 2) / 5, rc->top + (height * 3) / 4);
        LineTo(hdc, rc->left + (width * 4) / 5, rc->top + height / 4);
    }
    else
    {
        MoveToEx(hdc, rc->left + width / 4, rc->top + height / 4, NULL);
        LineTo(hdc, rc->left + (width * 3) / 4, rc->top + (height * 3) / 4);

        MoveToEx(hdc, rc->left + (width * 3) / 4, rc->top + height / 4, NULL);
        LineTo(hdc, rc->left + width / 4, rc->top + (height * 3) / 4);
    }

    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}

static void ButtonGrid_DrawCenteredText(
    HDC hdc,
    const char *text,
    RECT *rc,
    COLORREF color
)
{
    RECT calcRect;
    RECT drawRect;
    int textHeight;
    int rectHeight;

    if (!text || !text[0])
        return;

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, color);

    calcRect = *rc;
    InflateRect(&calcRect, -4, -4);

    DrawText(
        hdc,
        text,
        -1,
        &calcRect,
        DT_CENTER | DT_WORDBREAK | DT_CALCRECT
    );

    textHeight = calcRect.bottom - calcRect.top;
    rectHeight = rc->bottom - rc->top;

    drawRect = *rc;
    InflateRect(&drawRect, -4, -4);

    if (textHeight < rectHeight)
    {
        drawRect.top = rc->top + (rectHeight - textHeight) / 2;
        drawRect.bottom = drawRect.top + textHeight;
    }

    DrawText(
        hdc,
        text,
        -1,
        &drawRect,
        DT_CENTER | DT_WORDBREAK
    );
}

static void ButtonGrid_DrawButton(ButtonGrid *grid, DRAWITEMSTRUCT *draw)
{
    int index;
    RECT rc;
    HDC hdc;
    ButtonItem *button;
    AppImage *picture;
    int failed;
    int pictureType;
    COLORREF fallbackColor;

    if (!grid || !draw)
        return;

    index = (int)draw->CtlID - grid->idBase;

    if (index < 0 || index >= grid->buttonCount)
        return;

    button = &grid->buttons[index];

    hdc = draw->hDC;
    rc = draw->rcItem;

    if (grid->buttonBrush)
        FillRect(hdc, &rc, grid->buttonBrush);
    else
        FillRect(hdc, &rc, (HBRUSH)(COLOR_WINDOW + 1));

    if (grid->usePictures)
    {
        if (button->isOn)
        {
            picture = button->pictureOn;
            failed = button->pictureOnLoadFailed;
            pictureType = failed ? BUTTON_GRID_PICTURE_TYPE_ERROR : BUTTON_GRID_PICTURE_TYPE_ON;
            fallbackColor = failed ? grid->generatedErrorPictureColor : grid->generatedOnPictureColor;
        }
        else
        {
            picture = button->pictureOff;
            failed = button->pictureOffLoadFailed;
            pictureType = failed ? BUTTON_GRID_PICTURE_TYPE_ERROR : BUTTON_GRID_PICTURE_TYPE_OFF;
            fallbackColor = failed ? grid->generatedErrorPictureColor : grid->generatedOffPictureColor;
        }

        if (picture)
        {
            ImageLoader_Draw(
                hdc,
                picture,
                &rc,
                grid->stretchPictures
            );
        }
        else
        {
            ButtonGrid_DrawGeneratedFallback(
                hdc,
                &rc,
                fallbackColor,
                pictureType
            );
        }
    }

    ButtonGrid_DrawCenteredText(hdc, button->text, &rc, grid->foreColor);

    FrameRect(hdc, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));

    if (draw->itemState & ODS_FOCUS)
        DrawFocusRect(hdc, &rc);
}

LRESULT ButtonGrid_HandleDrawItem(ButtonGrid *grid, LPARAM lParam)
{
    DRAWITEMSTRUCT *draw;

    draw = (DRAWITEMSTRUCT *)lParam;

    if (!grid || !draw)
        return 0;

    ButtonGrid_DrawButton(grid, draw);

    return TRUE;
}

LRESULT ButtonGrid_HandleEraseBackground(HWND hwnd, WPARAM wParam)
{
    RECT rc;
    HDC hdc;

    hdc = (HDC)wParam;

    GetClientRect(hwnd, &rc);
    FillRect(hdc, &rc, (HBRUSH)(COLOR_WINDOW + 1));

    return 1;
}