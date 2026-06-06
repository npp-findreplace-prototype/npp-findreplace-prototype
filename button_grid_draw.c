#include "button_grid_internal.h"

static int ButtonGrid_HasVisibleBorder(ButtonGrid *grid)
{
    if (!grid)
        return 0;

    if (!grid->showBorder)
        return 0;

    if (grid->borderStyle == BUTTON_GRID_BORDER_STYLE_NONE)
        return 0;

    return 1;
}

static void ButtonGrid_DrawLine(
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

    pen = CreatePen(PS_SOLID, 1, color);
    oldPen = SelectObject(hdc, pen);

    MoveToEx(hdc, x1, y1, NULL);
    LineTo(hdc, x2, y2);

    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}

static void ButtonGrid_DrawSimpleBorder(ButtonGrid *grid, HDC hdc, RECT *rc)
{
    HPEN pen;
    HGDIOBJ oldPen;
    HGDIOBJ oldBrush;

    pen = CreatePen(PS_SOLID, grid->borderThickness, grid->borderColor);
    oldPen = SelectObject(hdc, pen);
    oldBrush = SelectObject(hdc, GetStockObject(HOLLOW_BRUSH));

    Rectangle(hdc, rc->left, rc->top, rc->right, rc->bottom);

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}

static void ButtonGrid_DrawEtchedBorder(ButtonGrid *grid, HDC hdc, RECT *rc)
{
    ButtonGrid_DrawLine(hdc, rc->left, rc->top, rc->right, rc->top, grid->borderLightColor);
    ButtonGrid_DrawLine(hdc, rc->left, rc->top, rc->left, rc->bottom, grid->borderLightColor);

    ButtonGrid_DrawLine(hdc, rc->left, rc->bottom, rc->right, rc->bottom, grid->borderShadowColor);
    ButtonGrid_DrawLine(hdc, rc->right, rc->top, rc->right, rc->bottom, grid->borderShadowColor);

    rc->left += 1;
    rc->top += 1;
    rc->right -= 1;
    rc->bottom -= 1;

    ButtonGrid_DrawLine(hdc, rc->left, rc->top, rc->right, rc->top, grid->borderColor);
    ButtonGrid_DrawLine(hdc, rc->left, rc->top, rc->left, rc->bottom, grid->borderColor);
    ButtonGrid_DrawLine(hdc, rc->left, rc->bottom, rc->right, rc->bottom, grid->borderColor);
    ButtonGrid_DrawLine(hdc, rc->right, rc->top, rc->right, rc->bottom, grid->borderColor);
}

static void ButtonGrid_DrawRoundedBorder(ButtonGrid *grid, HDC hdc, RECT *rc)
{
    HPEN pen;
    HGDIOBJ oldPen;
    HGDIOBJ oldBrush;
    int radius;

    radius = grid->borderCornerRadius;

    if (radius < 1)
        radius = 1;

    pen = CreatePen(PS_SOLID, grid->borderThickness, grid->borderColor);
    oldPen = SelectObject(hdc, pen);
    oldBrush = SelectObject(hdc, GetStockObject(HOLLOW_BRUSH));

    RoundRect(
        hdc,
        rc->left,
        rc->top,
        rc->right,
        rc->bottom,
        radius,
        radius
    );

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}

static void ButtonGrid_DrawBorderTitle(ButtonGrid *grid, HDC hdc, RECT *borderRc)
{
    RECT titleRc;
    HBRUSH brush;

    if (!grid->borderTitle[0])
        return;

    titleRc.left = borderRc->left + 12;
    titleRc.top = 0;
    titleRc.right = borderRc->right - 12;
    titleRc.bottom = grid->borderTitleHeight;

    brush = CreateSolidBrush(grid->borderTitleBackColor);
    FillRect(hdc, &titleRc, brush);
    DeleteObject(brush);

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

static void ButtonGrid_DrawBorder(ButtonGrid *grid, HDC hdc)
{
    RECT rc;

    if (!ButtonGrid_HasVisibleBorder(grid))
        return;

    GetClientRect(grid->hwnd, &rc);

    if (rc.right <= rc.left || rc.bottom <= rc.top)
        return;

    rc.left += 2;
    rc.top += 8;
    rc.right -= 2;
    rc.bottom -= 2;

    if (grid->borderStyle == BUTTON_GRID_BORDER_STYLE_SIMPLE)
        ButtonGrid_DrawSimpleBorder(grid, hdc, &rc);
    else if (grid->borderStyle == BUTTON_GRID_BORDER_STYLE_ROUNDED)
        ButtonGrid_DrawRoundedBorder(grid, hdc, &rc);
    else
        ButtonGrid_DrawEtchedBorder(grid, hdc, &rc);

    ButtonGrid_DrawBorderTitle(grid, hdc, &rc);
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

static int ButtonGrid_ButtonShouldShowText(ButtonGrid *grid, ButtonItem *button)
{
    if (!grid || !button)
        return 0;

    if (button->showTextOverride == BUTTON_GRID_TEXT_SHOW)
        return 1;

    if (button->showTextOverride == BUTTON_GRID_TEXT_HIDE)
        return 0;

    return grid->showText ? 1 : 0;
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

    if (ButtonGrid_ButtonShouldShowText(grid, button))
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