#include "button_grid_draw_internal.h"

static void ButtonGridDraw_FillButtonBackground(
    ButtonGrid *grid,
    HDC hdc,
    const RECT *rc
)
{
    ButtonGridDraw_FillSolid(
        hdc,
        rc,
        ButtonGridDraw_GetButtonBackgroundColor(grid)
    );
}

static void ButtonGridDraw_DrawGeneratedFallback(
    ButtonGrid *grid,
    HDC hdc,
    const RECT *rc,
    COLORREF color,
    int pictureType
)
{
    COLORREF markColor;
    int width;
    int height;
    int penWidth;
    HPEN pen;
    HGDIOBJ oldPen;

    ButtonGridDraw_FillSolid(hdc, rc, color);

    width = rc->right - rc->left;
    height = rc->bottom - rc->top;

    if (pictureType == BUTTON_GRID_PICTURE_TYPE_OFF)
        return;

    if (pictureType == BUTTON_GRID_PICTURE_TYPE_ON)
        markColor = RGB(0, 100, 0);
    else
        markColor = RGB(160, 0, 0);

    penWidth = ButtonGrid_DpiScaleMin(grid, 5, 1);

    pen = CreatePen(PS_SOLID, penWidth, markColor);

    if (!pen)
        return;

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

static int ButtonGridDraw_ButtonShouldShowText(ButtonGrid *grid, ButtonItem *button)
{
    if (!grid || !button)
        return 0;

    if (button->showTextOverride == BUTTON_GRID_TEXT_SHOW)
        return 1;

    if (button->showTextOverride == BUTTON_GRID_TEXT_HIDE)
        return 0;

    return grid->showText ? 1 : 0;
}

static void ButtonGridDraw_DrawCenteredButtonText(
    ButtonGrid *grid,
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
    int padding;

    if (!text || !text[0])
        return;

    padding = ButtonGrid_DpiScale(grid, 4);

    if (padding < 1)
        padding = 1;

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, color);

    calcRect = *rc;
    InflateRect(&calcRect, -padding, -padding);

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
    InflateRect(&drawRect, -padding, -padding);

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

static void ButtonGridDraw_DrawButtonFrame(ButtonGrid *grid, HDC hdc, RECT *rc)
{
    HPEN pen;
    HGDIOBJ oldPen;
    HGDIOBJ oldBrush;
    int thickness;
    int i;

    if (!grid)
        return;

    if (!grid->showButtonBorder)
        return;

    thickness = ButtonGrid_DpiScaleMin(grid, grid->buttonBorderThickness, 1);

    if (thickness < 1)
        thickness = 1;

    pen = CreatePen(PS_SOLID, 1, grid->buttonBorderColor);

    if (!pen)
        return;

    oldPen = SelectObject(hdc, pen);
    oldBrush = SelectObject(hdc, GetStockObject(HOLLOW_BRUSH));

    for (i = 0; i < thickness; i++)
    {
        Rectangle(
            hdc,
            rc->left + i,
            rc->top + i,
            rc->right - i,
            rc->bottom - i
        );
    }

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}

static void ButtonGridDraw_DrawButtonContents(
    ButtonGrid *grid,
    HDC hdc,
    RECT *rc,
    ButtonItem *button
)
{
    AppImage *picture;
    int failed;
    int pictureType;
    COLORREF fallbackColor;

    ButtonGridDraw_FillButtonBackground(grid, hdc, rc);

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
                rc,
                grid->stretchPictures
            );
        }
        else
        {
            ButtonGridDraw_DrawGeneratedFallback(
                grid,
                hdc,
                rc,
                fallbackColor,
                pictureType
            );
        }
    }

    if (ButtonGridDraw_ButtonShouldShowText(grid, button))
    {
        ButtonGridDraw_DrawCenteredButtonText(
            grid,
            hdc,
            button->text,
            rc,
            grid->foreColor
        );
    }

    ButtonGridDraw_DrawButtonFrame(grid, hdc, rc);
    ButtonGridDraw_DrawKeyboardFocus(grid, hdc, rc, button);
}

static void ButtonGridDraw_DrawButtonDirect(
    ButtonGrid *grid,
    DRAWITEMSTRUCT *draw,
    ButtonItem *button
)
{
    RECT rc;

    rc = draw->rcItem;

    ButtonGridDraw_DrawButtonContents(
        grid,
        draw->hDC,
        &rc,
        button
    );
}

static void ButtonGridDraw_DrawButtonBuffered(
    ButtonGrid *grid,
    DRAWITEMSTRUCT *draw,
    ButtonItem *button
)
{
    HDC screenDc;
    HDC memoryDc;
    HBITMAP bitmap;
    HGDIOBJ oldBitmap;
    RECT sourceRc;
    int width;
    int height;

    screenDc = draw->hDC;

    width = draw->rcItem.right - draw->rcItem.left;
    height = draw->rcItem.bottom - draw->rcItem.top;

    if (width < 1 || height < 1)
        return;

    memoryDc = CreateCompatibleDC(screenDc);

    if (!memoryDc)
    {
        ButtonGridDraw_DrawButtonDirect(grid, draw, button);
        return;
    }

    bitmap = CreateCompatibleBitmap(screenDc, width, height);

    if (!bitmap)
    {
        DeleteDC(memoryDc);
        ButtonGridDraw_DrawButtonDirect(grid, draw, button);
        return;
    }

    oldBitmap = SelectObject(memoryDc, bitmap);

    SetRect(&sourceRc, 0, 0, width, height);

    ButtonGridDraw_DrawButtonContents(
        grid,
        memoryDc,
        &sourceRc,
        button
    );

    BitBlt(
        screenDc,
        draw->rcItem.left,
        draw->rcItem.top,
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
}

void ButtonGridDraw_DrawButton(ButtonGrid *grid, DRAWITEMSTRUCT *draw)
{
    int index;
    ButtonItem *button;

    if (!grid || !draw)
        return;

    index = (int)draw->CtlID - grid->idBase;

    if (index < 0 || index >= grid->buttonCount)
        return;

    button = &grid->buttons[index];

    ButtonGridDraw_DrawButtonBuffered(
        grid,
        draw,
        button
    );
}