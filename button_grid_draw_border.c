#include "button_grid_draw_internal.h"

static int ButtonGridDraw_HasVisibleBorder(ButtonGrid *grid)
{
    if (!grid)
        return 0;

    if (!grid->showBorder)
        return 0;

    if (grid->borderStyle == BUTTON_GRID_BORDER_STYLE_NONE)
        return 0;

    return 1;
}

static int ButtonGridDraw_ShouldShowBorderTitle(ButtonGrid *grid)
{
    if (!ButtonGridDraw_HasVisibleBorder(grid))
        return 0;

    if (!grid->showBorderTitle)
        return 0;

    if (!grid->borderTitle[0])
        return 0;

    return 1;
}

static int ButtonGridDraw_ScaledBorderThickness(ButtonGrid *grid)
{
    return ButtonGrid_DpiScaleMin(grid, grid->borderThickness, 1);
}

static int ButtonGridDraw_ScaledBorderRadius(ButtonGrid *grid)
{
    return ButtonGrid_DpiScale(grid, grid->borderCornerRadius);
}

static int ButtonGridDraw_ScaledSmallMargin(ButtonGrid *grid, int value)
{
    return ButtonGrid_DpiScaleMin(grid, value, 1);
}

static HPEN ButtonGridDraw_CreateScaledPen(ButtonGrid *grid, COLORREF color)
{
    return CreatePen(
        PS_SOLID,
        ButtonGridDraw_ScaledBorderThickness(grid),
        color
    );
}

static void ButtonGridDraw_DrawTopLineWithGap(
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
        Ui_DrawLine(hdc, left, y, right, y, color);
        return;
    }

    if (gapLeft > left)
        Ui_DrawLine(hdc, left, y, gapLeft, y, color);

    if (gapRight < right)
        Ui_DrawLine(hdc, gapRight, y, right, y, color);
}

static void ButtonGridDraw_GetBorderDrawRect(ButtonGrid *grid, RECT *rc)
{
    int sideMargin;
    int topMargin;
    int bottomMargin;

    GetClientRect(grid->hwnd, rc);

    sideMargin = ButtonGridDraw_ScaledSmallMargin(grid, 2);
    topMargin = ButtonGridDraw_ScaledSmallMargin(grid, 8);
    bottomMargin = ButtonGridDraw_ScaledSmallMargin(grid, 2);

    rc->left += sideMargin;
    rc->top += topMargin;
    rc->right -= sideMargin;
    rc->bottom -= bottomMargin;

    if (rc->right < rc->left)
        rc->right = rc->left;

    if (rc->bottom < rc->top)
        rc->bottom = rc->top;
}

static HFONT ButtonGridDraw_CreateBorderTitleFont(ButtonGrid *grid)
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

static void ButtonGridDraw_GetBorderTitleMetrics(
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

    if (!ButtonGridDraw_ShouldShowBorderTitle(grid))
        return;

    titleFont = ButtonGridDraw_CreateBorderTitleFont(grid);

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

static void ButtonGridDraw_FillTitleGap(
    ButtonGrid *grid,
    HDC hdc,
    RECT *gapRc
)
{
    COLORREF color;

    if (gapRc->right <= gapRc->left || gapRc->bottom <= gapRc->top)
        return;

    if (grid->borderTitleAutoBackColor)
        color = ButtonGridDraw_GetAutoTitleBackColor(grid);
    else
        color = grid->borderTitleBackColor;

    ButtonGridDraw_FillSolid(hdc, gapRc, color);
}

static void ButtonGridDraw_DrawBorderTitleText(
    ButtonGrid *grid,
    HDC hdc,
    RECT *textRc,
    HFONT titleFont
)
{
    HGDIOBJ oldFont;

    if (!ButtonGridDraw_ShouldShowBorderTitle(grid))
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
            SetBkColor(hdc, ButtonGridDraw_GetAutoTitleBackColor(grid));
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

static void ButtonGridDraw_DrawSimpleBorder(
    ButtonGrid *grid,
    HDC hdc,
    RECT *rc,
    RECT *gapRc
)
{
    COLORREF color;

    color = grid->borderColor;

    ButtonGridDraw_DrawTopLineWithGap(hdc, rc->left, rc->top, rc->right, gapRc->left, gapRc->right, color);
    Ui_DrawLine(hdc, rc->left, rc->top, rc->left, rc->bottom, color);
    Ui_DrawLine(hdc, rc->right, rc->top, rc->right, rc->bottom, color);
    Ui_DrawLine(hdc, rc->left, rc->bottom, rc->right, rc->bottom, color);
}

static void ButtonGridDraw_DrawDoubleBorder(
    ButtonGrid *grid,
    HDC hdc,
    RECT *rc,
    RECT *gapRc
)
{
    RECT inner;
    int inset;

    inset = ButtonGrid_DpiScaleMin(grid, 3, 2);

    ButtonGridDraw_DrawSimpleBorder(grid, hdc, rc, gapRc);

    inner = *rc;
    InflateRect(&inner, -inset, -inset);

    if (inner.right > inner.left && inner.bottom > inner.top)
        ButtonGridDraw_DrawSimpleBorder(grid, hdc, &inner, gapRc);
}

static void ButtonGridDraw_DrawEtchedBorder(
    ButtonGrid *grid,
    HDC hdc,
    RECT *rc,
    RECT *gapRc
)
{
    int inset;
    RECT inner;

    inset = ButtonGrid_DpiScaleMin(grid, 1, 1);

    ButtonGridDraw_DrawTopLineWithGap(hdc, rc->left, rc->top, rc->right, gapRc->left, gapRc->right, grid->borderLightColor);
    Ui_DrawLine(hdc, rc->left, rc->top, rc->left, rc->bottom, grid->borderLightColor);
    Ui_DrawLine(hdc, rc->left, rc->bottom, rc->right, rc->bottom, grid->borderShadowColor);
    Ui_DrawLine(hdc, rc->right, rc->top, rc->right, rc->bottom, grid->borderShadowColor);

    inner = *rc;
    inner.left += inset;
    inner.top += inset;
    inner.right -= inset;
    inner.bottom -= inset;

    ButtonGridDraw_DrawTopLineWithGap(hdc, inner.left, inner.top, inner.right, gapRc->left, gapRc->right, grid->borderColor);
    Ui_DrawLine(hdc, inner.left, inner.top, inner.left, inner.bottom, grid->borderColor);
    Ui_DrawLine(hdc, inner.left, inner.bottom, inner.right, inner.bottom, grid->borderColor);
    Ui_DrawLine(hdc, inner.right, inner.top, inner.right, inner.bottom, grid->borderColor);
}

static void ButtonGridDraw_DrawRaisedBorder(
    ButtonGrid *grid,
    HDC hdc,
    RECT *rc,
    RECT *gapRc
)
{
    ButtonGridDraw_DrawTopLineWithGap(hdc, rc->left, rc->top, rc->right, gapRc->left, gapRc->right, grid->borderLightColor);
    Ui_DrawLine(hdc, rc->left, rc->top, rc->left, rc->bottom, grid->borderLightColor);
    Ui_DrawLine(hdc, rc->left, rc->bottom, rc->right, rc->bottom, grid->borderShadowColor);
    Ui_DrawLine(hdc, rc->right, rc->top, rc->right, rc->bottom, grid->borderShadowColor);
}

static void ButtonGridDraw_DrawSunkenBorder(
    ButtonGrid *grid,
    HDC hdc,
    RECT *rc,
    RECT *gapRc
)
{
    ButtonGridDraw_DrawTopLineWithGap(hdc, rc->left, rc->top, rc->right, gapRc->left, gapRc->right, grid->borderShadowColor);
    Ui_DrawLine(hdc, rc->left, rc->top, rc->left, rc->bottom, grid->borderShadowColor);
    Ui_DrawLine(hdc, rc->left, rc->bottom, rc->right, rc->bottom, grid->borderLightColor);
    Ui_DrawLine(hdc, rc->right, rc->top, rc->right, rc->bottom, grid->borderLightColor);
}

static void ButtonGridDraw_DrawContainerBorder(
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

    ButtonGridDraw_DrawTopLineWithGap(hdc, rc->left, rc->top, rc->right, gapRc->left, gapRc->right, shadow);
    Ui_DrawLine(hdc, rc->left, rc->top, rc->left, rc->bottom, shadow);
    Ui_DrawLine(hdc, rc->left, rc->bottom, rc->right, rc->bottom, light);
    Ui_DrawLine(hdc, rc->right, rc->top, rc->right, rc->bottom, light);
}

static void ButtonGridDraw_DrawRoundedBorder(
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

    radius = ButtonGridDraw_ScaledBorderRadius(grid);

    if (radius < 1)
        radius = 1;

    pen = ButtonGridDraw_CreateScaledPen(grid, etched ? grid->borderShadowColor : grid->borderColor);

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
        ButtonGridDraw_FillTitleGap(grid, hdc, &gap);
}

void ButtonGridDraw_DrawBorder(ButtonGrid *grid, HDC hdc)
{
    RECT rc;
    RECT gapRc;
    RECT textRc;
    HFONT titleFont;

    if (!ButtonGridDraw_HasVisibleBorder(grid))
        return;

    ButtonGridDraw_GetBorderDrawRect(grid, &rc);

    if (rc.right <= rc.left || rc.bottom <= rc.top)
        return;

    SetRectEmpty(&gapRc);
    SetRectEmpty(&textRc);
    titleFont = NULL;

    ButtonGridDraw_GetBorderTitleMetrics(grid, hdc, &rc, &gapRc, &textRc, &titleFont);

    if (ButtonGridDraw_ShouldShowBorderTitle(grid))
        ButtonGridDraw_FillTitleGap(grid, hdc, &gapRc);

    if (grid->borderStyle == BUTTON_GRID_BORDER_STYLE_SIMPLE)
        ButtonGridDraw_DrawSimpleBorder(grid, hdc, &rc, &gapRc);
    else if (grid->borderStyle == BUTTON_GRID_BORDER_STYLE_ROUNDED)
        ButtonGridDraw_DrawRoundedBorder(grid, hdc, &rc, &gapRc, 0);
    else if (grid->borderStyle == BUTTON_GRID_BORDER_STYLE_ETCHED_ROUNDED)
        ButtonGridDraw_DrawRoundedBorder(grid, hdc, &rc, &gapRc, 1);
    else if (grid->borderStyle == BUTTON_GRID_BORDER_STYLE_CONTAINER)
        ButtonGridDraw_DrawContainerBorder(grid, hdc, &rc, &gapRc);
    else if (grid->borderStyle == BUTTON_GRID_BORDER_STYLE_SUNKEN)
        ButtonGridDraw_DrawSunkenBorder(grid, hdc, &rc, &gapRc);
    else if (grid->borderStyle == BUTTON_GRID_BORDER_STYLE_RAISED)
        ButtonGridDraw_DrawRaisedBorder(grid, hdc, &rc, &gapRc);
    else if (grid->borderStyle == BUTTON_GRID_BORDER_STYLE_DOUBLE)
        ButtonGridDraw_DrawDoubleBorder(grid, hdc, &rc, &gapRc);
    else
        ButtonGridDraw_DrawEtchedBorder(grid, hdc, &rc, &gapRc);

    if (ButtonGridDraw_ShouldShowBorderTitle(grid))
    {
        ButtonGridDraw_FillTitleGap(grid, hdc, &gapRc);
        ButtonGridDraw_DrawBorderTitleText(grid, hdc, &textRc, titleFont);
    }

    if (titleFont)
        DeleteObject(titleFont);
}