#include "button_grid_internal.h"
#include "button_grid_settings.h"

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

static int ButtonGrid_ScaledBorderThickness(ButtonGrid *grid)
{
    return ButtonGrid_DpiScaleMin(grid, grid->borderThickness, 1);
}

static int ButtonGrid_ScaledBorderRadius(ButtonGrid *grid)
{
    return ButtonGrid_DpiScale(grid, grid->borderCornerRadius);
}

static int ButtonGrid_ScaledBorderTitleHeight(ButtonGrid *grid)
{
    if (!grid || !grid->borderTitle[0])
        return 0;

    return ButtonGrid_DpiScale(grid, grid->borderTitleHeight);
}

static int ButtonGrid_ScaledSmallMargin(ButtonGrid *grid, int value)
{
    return ButtonGrid_DpiScaleMin(grid, value, 1);
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

    pen = CreatePen(
        PS_SOLID,
        ButtonGrid_ScaledBorderThickness(grid),
        grid->borderColor
    );

    oldPen = SelectObject(hdc, pen);
    oldBrush = SelectObject(hdc, GetStockObject(HOLLOW_BRUSH));

    Rectangle(hdc, rc->left, rc->top, rc->right, rc->bottom);

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}

static void ButtonGrid_DrawEtchedBorder(ButtonGrid *grid, HDC hdc, RECT *rc)
{
    int inset;

    inset = ButtonGrid_DpiScaleMin(grid, 1, 1);

    ButtonGrid_DrawLine(hdc, rc->left, rc->top, rc->right, rc->top, grid->borderLightColor);
    ButtonGrid_DrawLine(hdc, rc->left, rc->top, rc->left, rc->bottom, grid->borderLightColor);

    ButtonGrid_DrawLine(hdc, rc->left, rc->bottom, rc->right, rc->bottom, grid->borderShadowColor);
    ButtonGrid_DrawLine(hdc, rc->right, rc->top, rc->right, rc->bottom, grid->borderShadowColor);

    rc->left += inset;
    rc->top += inset;
    rc->right -= inset;
    rc->bottom -= inset;

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

    radius = ButtonGrid_ScaledBorderRadius(grid);

    if (radius < 1)
        radius = 1;

    pen = CreatePen(
        PS_SOLID,
        ButtonGrid_ScaledBorderThickness(grid),
        grid->borderColor
    );

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
    int titleHeight;
    int titleInset;

    if (!grid->borderTitle[0])
        return;

    titleHeight = ButtonGrid_ScaledBorderTitleHeight(grid);
    titleInset = ButtonGrid_DpiScale(grid, 12);

    if (titleHeight < 1)
        titleHeight = 1;

    titleRc.left = borderRc->left + titleInset;
    titleRc.top = 0;
    titleRc.right = borderRc->right - titleInset;
    titleRc.bottom = titleHeight;

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

static void ButtonGrid_GetBorderDrawRect(ButtonGrid *grid, RECT *rc)
{
    int sideMargin;
    int topMargin;
    int bottomMargin;

    GetClientRect(grid->hwnd, rc);

    sideMargin = ButtonGrid_ScaledSmallMargin(grid, 2);
    topMargin = ButtonGrid_ScaledSmallMargin(grid, 8);
    bottomMargin = ButtonGrid_ScaledSmallMargin(grid, 2);

    rc->left += sideMargin;
    rc->top += topMargin;
    rc->right -= sideMargin;
    rc->bottom -= bottomMargin;

    if (rc->right < rc->left)
        rc->right = rc->left;

    if (rc->bottom < rc->top)
        rc->bottom = rc->top;
}

static void ButtonGrid_DrawBorder(ButtonGrid *grid, HDC hdc)
{
    RECT rc;

    if (!ButtonGrid_HasVisibleBorder(grid))
        return;

    ButtonGrid_GetBorderDrawRect(grid, &rc);

    if (rc.right <= rc.left || rc.bottom <= rc.top)
        return;

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

    if (grid)
    {
        ButtonGrid_UpdateDpi(grid);
        ButtonGrid_DrawBorder(grid, hdc);
        ButtonGrid_DrawGearIcon(grid, hdc);
    }

    EndPaint(hwnd, &ps);

    return 0;
}

static void ButtonGrid_DrawGeneratedFallback(
    ButtonGrid *grid,
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
    int penWidth;

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

    penWidth = ButtonGrid_DpiScaleMin(grid, 5, 1);

    pen = CreatePen(PS_SOLID, penWidth, markColor);
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

static void ButtonGrid_DrawButtonFrame(ButtonGrid *grid, HDC hdc, RECT *rc)
{
    HPEN pen;
    HGDIOBJ oldPen;
    HGDIOBJ oldBrush;
    int thickness;
    int i;

    thickness = ButtonGrid_DpiScaleMin(grid, 1, 1);

    pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
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
    HBRUSH backBrush;

    if (!grid || !draw)
        return;

    index = (int)draw->CtlID - grid->idBase;

    if (index < 0 || index >= grid->buttonCount)
        return;

    button = &grid->buttons[index];

    hdc = draw->hDC;
    rc = draw->rcItem;

    backBrush = CreateSolidBrush(grid->backColor);
    FillRect(hdc, &rc, backBrush);
    DeleteObject(backBrush);

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
                grid,
                hdc,
                &rc,
                fallbackColor,
                pictureType
            );
        }
    }

    if (ButtonGrid_ButtonShouldShowText(grid, button))
    {
        ButtonGrid_DrawCenteredText(
            grid,
            hdc,
            button->text,
            &rc,
            grid->foreColor
        );
    }

    ButtonGrid_DrawButtonFrame(grid, hdc, &rc);

    if (draw->itemState & ODS_FOCUS)
        DrawFocusRect(hdc, &rc);
}

LRESULT ButtonGrid_HandleDrawItem(ButtonGrid *grid, LPARAM lParam)
{
    DRAWITEMSTRUCT *draw;

    draw = (DRAWITEMSTRUCT *)lParam;

    if (!grid || !draw)
        return 0;

    ButtonGrid_UpdateDpi(grid);
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