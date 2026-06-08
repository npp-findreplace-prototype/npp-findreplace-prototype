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

static int ButtonGrid_ShouldShowBorderTitle(ButtonGrid *grid)
{
    if (!ButtonGrid_HasVisibleBorder(grid))
        return 0;

    if (!grid->showBorderTitle)
        return 0;

    if (!grid->borderTitle[0])
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

static int ButtonGrid_ScaledSmallMargin(ButtonGrid *grid, int value)
{
    return ButtonGrid_DpiScaleMin(grid, value, 1);
}

static COLORREF ButtonGrid_GetFallbackBackColor(void)
{
    return GetSysColor(COLOR_WINDOW);
}

static COLORREF ButtonGrid_GetGridBackColor(ButtonGrid *grid)
{
    if (!grid)
        return ButtonGrid_GetFallbackBackColor();

    return grid->backColor;
}

static COLORREF ButtonGrid_GetAutoTitleBackColor(ButtonGrid *grid)
{
    return ButtonGrid_GetGridBackColor(grid);
}

static COLORREF ButtonGrid_GetButtonBackgroundColor(ButtonGrid *grid)
{
    if (!grid)
        return ButtonGrid_GetFallbackBackColor();

    if (grid->buttonBackMode == BUTTON_GRID_BUTTON_BACK_TRANSPARENT)
        return ButtonGrid_GetGridBackColor(grid);

    return grid->backColor;
}

static HPEN ButtonGrid_CreateScaledPen(ButtonGrid *grid, COLORREF color)
{
    return CreatePen(
        PS_SOLID,
        ButtonGrid_ScaledBorderThickness(grid),
        color
    );
}

static void ButtonGrid_FillSolid(HDC hdc, const RECT *rc, COLORREF color)
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

    if (!pen)
        return;

    oldPen = SelectObject(hdc, pen);

    MoveToEx(hdc, x1, y1, NULL);
    LineTo(hdc, x2, y2);

    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}

static void ButtonGrid_DrawTopLineWithGap(
    HDC hdc,
    int left,
    int y,
    int right,
    int gapLeft,
    int gapRight,
    COLORREF color
)
{
    if (gapRight <= gapLeft)
    {
        ButtonGrid_DrawLine(hdc, left, y, right, y, color);
        return;
    }

    if (gapLeft > left)
        ButtonGrid_DrawLine(hdc, left, y, gapLeft, y, color);

    if (gapRight < right)
        ButtonGrid_DrawLine(hdc, gapRight, y, right, y, color);
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

static HFONT ButtonGrid_CreateBorderTitleFont(ButtonGrid *grid)
{
    HFONT baseFont;
    LOGFONT lf;
    int fontSize;

    fontSize = grid->borderTitleFontSize;

    if (fontSize <= 0)
        return NULL;

    baseFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

    if (!baseFont)
        return NULL;

    ZeroMemory(&lf, sizeof(lf));

    if (!GetObject(baseFont, sizeof(lf), &lf))
        return NULL;

    lf.lfHeight = -ButtonGrid_DpiScale(grid, fontSize);
    lf.lfWeight = FW_NORMAL;

    return CreateFontIndirect(&lf);
}

static void ButtonGrid_GetBorderTitleMetrics(
    ButtonGrid *grid,
    HDC hdc,
    RECT *borderRc,
    RECT *gapRc,
    RECT *textRc,
    HFONT *createdFont
)
{
    RECT calcRc;
    HFONT titleFont;
    HGDIOBJ oldFont;
    int textW;
    int textH;
    int pad;
    int titleX;
    int centerY;

    SetRectEmpty(gapRc);
    SetRectEmpty(textRc);

    if (createdFont)
        *createdFont = NULL;

    if (!ButtonGrid_ShouldShowBorderTitle(grid))
        return;

    titleFont = ButtonGrid_CreateBorderTitleFont(grid);

    oldFont = NULL;

    if (titleFont)
    {
        oldFont = SelectObject(hdc, titleFont);

        if (createdFont)
            *createdFont = titleFont;
    }

    SetRect(&calcRc, 0, 0, 10000, 10000);

    DrawText(
        hdc,
        grid->borderTitle,
        -1,
        &calcRc,
        DT_SINGLELINE | DT_LEFT | DT_TOP | DT_CALCRECT
    );

    textW = calcRc.right - calcRc.left;
    textH = calcRc.bottom - calcRc.top;

    if (textW < 1)
        textW = 1;

    if (textH < 1)
        textH = ButtonGrid_DpiScaleMin(grid, 12, 1);

    pad = ButtonGrid_DpiScale(grid, grid->borderTitlePadding);

    if (pad < 0)
        pad = 0;

    titleX = borderRc->left + ButtonGrid_DpiScaleMin(grid, 12, 1);
    centerY = borderRc->top;

    gapRc->left = titleX - pad;
    gapRc->right = titleX + textW + pad;
    gapRc->top = centerY - textH / 2 - 1;
    gapRc->bottom = centerY + (textH + 1) / 2 + 1;

    textRc->left = titleX;
    textRc->right = titleX + textW;
    textRc->top = centerY - textH / 2;
    textRc->bottom = centerY + (textH + 1) / 2;

    if (gapRc->left < borderRc->left + 2)
        gapRc->left = borderRc->left + 2;

    if (gapRc->right > borderRc->right - 2)
        gapRc->right = borderRc->right - 2;

    if (oldFont)
        SelectObject(hdc, oldFont);
}

static void ButtonGrid_FillTitleGap(
    ButtonGrid *grid,
    HDC hdc,
    RECT *gapRc
)
{
    COLORREF color;

    if (gapRc->right <= gapRc->left || gapRc->bottom <= gapRc->top)
        return;

    if (grid->borderTitleAutoBackColor)
        color = ButtonGrid_GetAutoTitleBackColor(grid);
    else
        color = grid->borderTitleBackColor;

    ButtonGrid_FillSolid(hdc, gapRc, color);
}

static void ButtonGrid_DrawBorderTitleText(
    ButtonGrid *grid,
    HDC hdc,
    RECT *textRc,
    HFONT titleFont
)
{
    HGDIOBJ oldFont;

    if (!ButtonGrid_ShouldShowBorderTitle(grid))
        return;

    if (textRc->right <= textRc->left || textRc->bottom <= textRc->top)
        return;

    oldFont = NULL;

    if (titleFont)
        oldFont = SelectObject(hdc, titleFont);

    if (grid->borderTitleTransparent)
        SetBkMode(hdc, TRANSPARENT);
    else
        SetBkMode(hdc, OPAQUE);

    if (!grid->borderTitleTransparent)
    {
        if (grid->borderTitleAutoBackColor)
            SetBkColor(hdc, ButtonGrid_GetAutoTitleBackColor(grid));
        else
            SetBkColor(hdc, grid->borderTitleBackColor);
    }

    SetTextColor(hdc, grid->borderTitleColor);

    DrawText(
        hdc,
        grid->borderTitle,
        -1,
        textRc,
        DT_SINGLELINE | DT_LEFT | DT_VCENTER
    );

    if (oldFont)
        SelectObject(hdc, oldFont);
}

static void ButtonGrid_DrawSimpleBorder(
    ButtonGrid *grid,
    HDC hdc,
    RECT *rc,
    RECT *gapRc
)
{
    COLORREF color;

    color = grid->borderColor;

    ButtonGrid_DrawTopLineWithGap(hdc, rc->left, rc->top, rc->right, gapRc->left, gapRc->right, color);
    ButtonGrid_DrawLine(hdc, rc->left, rc->top, rc->left, rc->bottom, color);
    ButtonGrid_DrawLine(hdc, rc->right, rc->top, rc->right, rc->bottom, color);
    ButtonGrid_DrawLine(hdc, rc->left, rc->bottom, rc->right, rc->bottom, color);
}

static void ButtonGrid_DrawDoubleBorder(
    ButtonGrid *grid,
    HDC hdc,
    RECT *rc,
    RECT *gapRc
)
{
    RECT inner;
    int inset;

    inset = ButtonGrid_DpiScaleMin(grid, 3, 2);

    ButtonGrid_DrawSimpleBorder(grid, hdc, rc, gapRc);

    inner = *rc;
    InflateRect(&inner, -inset, -inset);

    if (inner.right > inner.left && inner.bottom > inner.top)
        ButtonGrid_DrawSimpleBorder(grid, hdc, &inner, gapRc);
}

static void ButtonGrid_DrawEtchedBorder(
    ButtonGrid *grid,
    HDC hdc,
    RECT *rc,
    RECT *gapRc
)
{
    int inset;
    RECT inner;

    inset = ButtonGrid_DpiScaleMin(grid, 1, 1);

    ButtonGrid_DrawTopLineWithGap(hdc, rc->left, rc->top, rc->right, gapRc->left, gapRc->right, grid->borderLightColor);
    ButtonGrid_DrawLine(hdc, rc->left, rc->top, rc->left, rc->bottom, grid->borderLightColor);
    ButtonGrid_DrawLine(hdc, rc->left, rc->bottom, rc->right, rc->bottom, grid->borderShadowColor);
    ButtonGrid_DrawLine(hdc, rc->right, rc->top, rc->right, rc->bottom, grid->borderShadowColor);

    inner = *rc;
    inner.left += inset;
    inner.top += inset;
    inner.right -= inset;
    inner.bottom -= inset;

    ButtonGrid_DrawTopLineWithGap(hdc, inner.left, inner.top, inner.right, gapRc->left, gapRc->right, grid->borderColor);
    ButtonGrid_DrawLine(hdc, inner.left, inner.top, inner.left, inner.bottom, grid->borderColor);
    ButtonGrid_DrawLine(hdc, inner.left, inner.bottom, inner.right, inner.bottom, grid->borderColor);
    ButtonGrid_DrawLine(hdc, inner.right, inner.top, inner.right, inner.bottom, grid->borderColor);
}

static void ButtonGrid_DrawRaisedBorder(
    ButtonGrid *grid,
    HDC hdc,
    RECT *rc,
    RECT *gapRc
)
{
    ButtonGrid_DrawTopLineWithGap(hdc, rc->left, rc->top, rc->right, gapRc->left, gapRc->right, grid->borderLightColor);
    ButtonGrid_DrawLine(hdc, rc->left, rc->top, rc->left, rc->bottom, grid->borderLightColor);
    ButtonGrid_DrawLine(hdc, rc->left, rc->bottom, rc->right, rc->bottom, grid->borderShadowColor);
    ButtonGrid_DrawLine(hdc, rc->right, rc->top, rc->right, rc->bottom, grid->borderShadowColor);
}

static void ButtonGrid_DrawSunkenBorder(
    ButtonGrid *grid,
    HDC hdc,
    RECT *rc,
    RECT *gapRc
)
{
    ButtonGrid_DrawTopLineWithGap(hdc, rc->left, rc->top, rc->right, gapRc->left, gapRc->right, grid->borderShadowColor);
    ButtonGrid_DrawLine(hdc, rc->left, rc->top, rc->left, rc->bottom, grid->borderShadowColor);
    ButtonGrid_DrawLine(hdc, rc->left, rc->bottom, rc->right, rc->bottom, grid->borderLightColor);
    ButtonGrid_DrawLine(hdc, rc->right, rc->top, rc->right, rc->bottom, grid->borderLightColor);
}

static void ButtonGrid_DrawContainerBorder(
    ButtonGrid *grid,
    HDC hdc,
    RECT *rc,
    RECT *gapRc
)
{
    COLORREF light;
    COLORREF shadow;

    light = GetSysColor(COLOR_3DHILIGHT);
    shadow = GetSysColor(COLOR_3DSHADOW);

    ButtonGrid_DrawTopLineWithGap(hdc, rc->left, rc->top, rc->right, gapRc->left, gapRc->right, shadow);
    ButtonGrid_DrawLine(hdc, rc->left, rc->top, rc->left, rc->bottom, shadow);
    ButtonGrid_DrawLine(hdc, rc->left, rc->bottom, rc->right, rc->bottom, light);
    ButtonGrid_DrawLine(hdc, rc->right, rc->top, rc->right, rc->bottom, light);
}

static void ButtonGrid_DrawRoundedBorder(
    ButtonGrid *grid,
    HDC hdc,
    RECT *rc,
    RECT *gapRc,
    int etched
)
{
    HPEN pen;
    HGDIOBJ oldPen;
    HGDIOBJ oldBrush;
    int radius;
    RECT gap;

    radius = ButtonGrid_ScaledBorderRadius(grid);

    if (radius < 1)
        radius = 1;

    pen = ButtonGrid_CreateScaledPen(grid, etched ? grid->borderShadowColor : grid->borderColor);

    if (!pen)
        return;

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

    if (etched)
    {
        RECT inner;

        inner = *rc;
        InflateRect(&inner, -ButtonGrid_DpiScaleMin(grid, 1, 1), -ButtonGrid_DpiScaleMin(grid, 1, 1));

        pen = CreatePen(PS_SOLID, 1, grid->borderLightColor);

        if (pen)
        {
            oldPen = SelectObject(hdc, pen);
            oldBrush = SelectObject(hdc, GetStockObject(HOLLOW_BRUSH));

            RoundRect(
                hdc,
                inner.left,
                inner.top,
                inner.right,
                inner.bottom,
                radius,
                radius
            );

            SelectObject(hdc, oldBrush);
            SelectObject(hdc, oldPen);
            DeleteObject(pen);
        }
    }

    gap = *gapRc;

    if (gap.right > gap.left && gap.bottom > gap.top)
        ButtonGrid_FillTitleGap(grid, hdc, &gap);
}

static void ButtonGrid_DrawBorder(ButtonGrid *grid, HDC hdc)
{
    RECT rc;
    RECT gapRc;
    RECT textRc;
    HFONT titleFont;

    if (!ButtonGrid_HasVisibleBorder(grid))
        return;

    ButtonGrid_GetBorderDrawRect(grid, &rc);

    if (rc.right <= rc.left || rc.bottom <= rc.top)
        return;

    SetRectEmpty(&gapRc);
    SetRectEmpty(&textRc);
    titleFont = NULL;

    ButtonGrid_GetBorderTitleMetrics(grid, hdc, &rc, &gapRc, &textRc, &titleFont);

    if (ButtonGrid_ShouldShowBorderTitle(grid))
        ButtonGrid_FillTitleGap(grid, hdc, &gapRc);

    if (grid->borderStyle == BUTTON_GRID_BORDER_STYLE_SIMPLE)
        ButtonGrid_DrawSimpleBorder(grid, hdc, &rc, &gapRc);
    else if (grid->borderStyle == BUTTON_GRID_BORDER_STYLE_ROUNDED)
        ButtonGrid_DrawRoundedBorder(grid, hdc, &rc, &gapRc, 0);
    else if (grid->borderStyle == BUTTON_GRID_BORDER_STYLE_ETCHED_ROUNDED)
        ButtonGrid_DrawRoundedBorder(grid, hdc, &rc, &gapRc, 1);
    else if (grid->borderStyle == BUTTON_GRID_BORDER_STYLE_CONTAINER)
        ButtonGrid_DrawContainerBorder(grid, hdc, &rc, &gapRc);
    else if (grid->borderStyle == BUTTON_GRID_BORDER_STYLE_SUNKEN)
        ButtonGrid_DrawSunkenBorder(grid, hdc, &rc, &gapRc);
    else if (grid->borderStyle == BUTTON_GRID_BORDER_STYLE_RAISED)
        ButtonGrid_DrawRaisedBorder(grid, hdc, &rc, &gapRc);
    else if (grid->borderStyle == BUTTON_GRID_BORDER_STYLE_DOUBLE)
        ButtonGrid_DrawDoubleBorder(grid, hdc, &rc, &gapRc);
    else
        ButtonGrid_DrawEtchedBorder(grid, hdc, &rc, &gapRc);

    if (ButtonGrid_ShouldShowBorderTitle(grid))
    {
        ButtonGrid_FillTitleGap(grid, hdc, &gapRc);
        ButtonGrid_DrawBorderTitleText(grid, hdc, &textRc, titleFont);
    }

    if (titleFont)
        DeleteObject(titleFont);
}

static void ButtonGrid_DrawGridChrome(ButtonGrid *grid, HDC hdc)
{
    RECT rc;

    GetClientRect(grid->hwnd, &rc);
    ButtonGrid_FillSolid(hdc, &rc, ButtonGrid_GetGridBackColor(grid));

    ButtonGrid_DrawBorder(grid, hdc);
    ButtonGrid_DrawGearIcon(grid, hdc);
}

LRESULT ButtonGrid_HandlePaint(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc;
    HDC memoryDc;
    HBITMAP bitmap;
    HGDIOBJ oldBitmap;
    RECT rc;
    int width;
    int height;
    ButtonGrid *grid;

    grid = ButtonGrid_Get(hwnd);

    hdc = BeginPaint(hwnd, &ps);

    GetClientRect(hwnd, &rc);

    width = rc.right - rc.left;
    height = rc.bottom - rc.top;

    if (!grid || width <= 0 || height <= 0)
    {
        ButtonGrid_FillSolid(hdc, &rc, ButtonGrid_GetFallbackBackColor());
        EndPaint(hwnd, &ps);
        return 0;
    }

    ButtonGrid_UpdateDpi(grid);

    memoryDc = CreateCompatibleDC(hdc);

    if (!memoryDc)
    {
        ButtonGrid_DrawGridChrome(grid, hdc);
        EndPaint(hwnd, &ps);
        return 0;
    }

    bitmap = CreateCompatibleBitmap(hdc, width, height);

    if (!bitmap)
    {
        DeleteDC(memoryDc);
        ButtonGrid_DrawGridChrome(grid, hdc);
        EndPaint(hwnd, &ps);
        return 0;
    }

    oldBitmap = SelectObject(memoryDc, bitmap);

    ButtonGrid_DrawGridChrome(grid, memoryDc);

    BitBlt(
        hdc,
        0,
        0,
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

    EndPaint(hwnd, &ps);

    return 0;
}

static void ButtonGrid_FillButtonBackground(ButtonGrid *grid, HDC hdc, const RECT *rc)
{
    ButtonGrid_FillSolid(hdc, rc, ButtonGrid_GetButtonBackgroundColor(grid));
}

static void ButtonGrid_DrawGeneratedFallback(
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

    ButtonGrid_FillSolid(hdc, rc, color);

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

static void ButtonGrid_DrawKeyboardFocus(ButtonGrid *grid, HDC hdc, RECT *rc, ButtonItem *button)
{
    RECT focusRc;

    if (!grid || !button)
        return;

    if (GetFocus() != button->hwnd)
        return;

    focusRc = *rc;

    InflateRect(
        &focusRc,
        -ButtonGrid_DpiScaleMin(grid, 4, 2),
        -ButtonGrid_DpiScaleMin(grid, 4, 2)
    );

    DrawFocusRect(hdc, &focusRc);
}

static void ButtonGrid_DrawButtonContents(
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

    ButtonGrid_FillButtonBackground(grid, hdc, rc);

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
            ButtonGrid_DrawGeneratedFallback(
                grid,
                hdc,
                rc,
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
            rc,
            grid->foreColor
        );
    }

    ButtonGrid_DrawButtonFrame(grid, hdc, rc);
    ButtonGrid_DrawKeyboardFocus(grid, hdc, rc, button);
}

static void ButtonGrid_DrawButtonDirect(
    ButtonGrid *grid,
    DRAWITEMSTRUCT *draw,
    ButtonItem *button
)
{
    RECT rc;

    rc = draw->rcItem;

    ButtonGrid_DrawButtonContents(
        grid,
        draw->hDC,
        &rc,
        button
    );
}

static void ButtonGrid_DrawButtonBuffered(
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
        ButtonGrid_DrawButtonDirect(grid, draw, button);
        return;
    }

    bitmap = CreateCompatibleBitmap(screenDc, width, height);

    if (!bitmap)
    {
        DeleteDC(memoryDc);
        ButtonGrid_DrawButtonDirect(grid, draw, button);
        return;
    }

    oldBitmap = SelectObject(memoryDc, bitmap);

    SetRect(&sourceRc, 0, 0, width, height);

    ButtonGrid_DrawButtonContents(
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

static void ButtonGrid_DrawButton(ButtonGrid *grid, DRAWITEMSTRUCT *draw)
{
    int index;
    ButtonItem *button;

    if (!grid || !draw)
        return;

    index = (int)draw->CtlID - grid->idBase;

    if (index < 0 || index >= grid->buttonCount)
        return;

    button = &grid->buttons[index];

    ButtonGrid_DrawButtonBuffered(
        grid,
        draw,
        button
    );
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
    ButtonGrid *grid;

    hdc = (HDC)wParam;
    grid = ButtonGrid_Get(hwnd);

    GetClientRect(hwnd, &rc);

    ButtonGrid_FillSolid(
        hdc,
        &rc,
        ButtonGrid_GetGridBackColor(grid)
    );

    return 1;
}