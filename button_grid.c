#include <windows.h>
#include <stdlib.h>

#include "button_grid.h"

#ifndef SS_OWNERDRAW
#define SS_OWNERDRAW 0x0000000D
#endif

#ifndef SWP_NOCOPYBITS
#define SWP_NOCOPYBITS 0x0100
#endif

#define BUTTON_GRID_CLASS_NAME "ButtonGridChildClass"
#define BUTTON_GRID_PROP_NAME "ButtonGridData"

#define BUTTON_GRID_NAME_SIZE 64
#define BUTTON_GRID_TEXT_SIZE 64
#define BUTTON_GRID_FORMAT_SIZE 64

#define BUTTON_GRID_PICTURE_TYPE_OFF 0
#define BUTTON_GRID_PICTURE_TYPE_ON 1
#define BUTTON_GRID_PICTURE_TYPE_ERROR 2

typedef struct ButtonItem
{
    HWND hwnd;
    char name[BUTTON_GRID_NAME_SIZE];
    char text[BUTTON_GRID_TEXT_SIZE];
    int isOn;
} ButtonItem;

typedef struct GridLayout
{
    int cols;
    int rows;
    int startX;
    int startY;
} GridLayout;

typedef struct ButtonGrid
{
    HWND hwnd;
    HINSTANCE hInstance;

    ButtonGridClickCallback onClick;

    ButtonItem *buttons;
    int buttonCount;

    int buttonWidth;
    int buttonHeight;

    int horizontalSpacing;
    int verticalSpacing;

    int layout;

    int idBase;
    int firstIndex;

    char namePrefix[BUTTON_GRID_FORMAT_SIZE];
    char textFormat[BUTTON_GRID_FORMAT_SIZE];
    char clickIdentifierFormat[BUTTON_GRID_FORMAT_SIZE];

    COLORREF backColor;
    COLORREF foreColor;

    int usePictures;
    int toggleOnClick;
    int defaultState;
    int stretchPictures;

    HBITMAP pictureOff;
    HBITMAP pictureOn;

    int pictureOffLoadFailed;
    int pictureOnLoadFailed;

    int ownsPictureOff;
    int ownsPictureOn;

    COLORREF generatedOffPictureColor;
    COLORREF generatedOnPictureColor;

    HBRUSH buttonBrush;
} ButtonGrid;

typedef struct ButtonGridCreateParams
{
    ButtonGridConfig config;
    ButtonGridClickCallback onClick;
} ButtonGridCreateParams;

static LRESULT CALLBACK ButtonGrid_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

static ButtonGrid *ButtonGrid_Get(HWND hwnd)
{
    return (ButtonGrid *)GetProp(hwnd, BUTTON_GRID_PROP_NAME);
}

static void CopyText(char *dest, int destSize, const char *src)
{
    if (!dest || destSize <= 0)
        return;

    if (!src)
        src = "";

    lstrcpyn(dest, src, destSize);
    dest[destSize - 1] = '\0';
}

static int ClampInt(int value, int minValue, int maxValue)
{
    if (value < minValue)
        return minValue;

    if (value > maxValue)
        return maxValue;

    return value;
}

static void MoveChildClean(HWND hwnd, int x, int y, int w, int h)
{
    if (!hwnd)
        return;

    SetWindowPos(
        hwnd,
        NULL,
        x,
        y,
        w,
        h,
        SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS
    );

    InvalidateRect(hwnd, NULL, TRUE);
}

static void RedrawContainer(HWND hwnd)
{
    RedrawWindow(
        hwnd,
        NULL,
        NULL,
        RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_UPDATENOW
    );
}

void ButtonGrid_GetDefaultConfig(ButtonGridConfig *config)
{
    if (!config)
        return;

    config->buttonCount = BUTTON_GRID_DEFAULT_BUTTON_COUNT;

    config->buttonWidth = BUTTON_GRID_DEFAULT_BUTTON_WIDTH;
    config->buttonHeight = BUTTON_GRID_DEFAULT_BUTTON_HEIGHT;

    config->horizontalSpacing = BUTTON_GRID_DEFAULT_HORIZONTAL_SPACING;
    config->verticalSpacing = BUTTON_GRID_DEFAULT_VERTICAL_SPACING;

    config->layout = BUTTON_GRID_DEFAULT_LAYOUT;

    config->idBase = BUTTON_GRID_DEFAULT_ID_BASE;
    config->firstIndex = BUTTON_GRID_DEFAULT_FIRST_INDEX;

    config->namePrefix = BUTTON_GRID_DEFAULT_NAME_PREFIX;
    config->textFormat = BUTTON_GRID_DEFAULT_TEXT_FORMAT;
    config->clickIdentifierFormat = BUTTON_GRID_DEFAULT_CLICK_IDENTIFIER_FORMAT;

    config->backColor = BUTTON_GRID_DEFAULT_BACK_COLOR;
    config->foreColor = BUTTON_GRID_DEFAULT_FORE_COLOR;

    config->usePictures = BUTTON_GRID_DEFAULT_USE_PICTURES;
    config->toggleOnClick = BUTTON_GRID_DEFAULT_TOGGLE_ON_CLICK;
    config->defaultState = BUTTON_GRID_DEFAULT_STATE;
    config->stretchPictures = BUTTON_GRID_DEFAULT_STRETCH_PICTURES;

    config->pictureOff = NULL;
    config->pictureOn = NULL;

    config->pictureOffLoadFailed = 0;
    config->pictureOnLoadFailed = 0;

    config->generatedOffPictureColor = BUTTON_GRID_DEFAULT_OFF_PICTURE_COLOR;
    config->generatedOnPictureColor = BUTTON_GRID_DEFAULT_ON_PICTURE_COLOR;
}

static void ButtonGrid_NormalizeConfig(ButtonGridConfig *config)
{
    if (config->buttonCount < 1)
        config->buttonCount = 1;

    if (config->buttonWidth < 1)
        config->buttonWidth = 1;

    if (config->buttonHeight < 1)
        config->buttonHeight = 1;

    if (config->horizontalSpacing < 0)
        config->horizontalSpacing = 0;

    if (config->verticalSpacing < 0)
        config->verticalSpacing = 0;

    if (config->layout != BUTTON_GRID_LAYOUT_HORIZONTAL &&
        config->layout != BUTTON_GRID_LAYOUT_VERTICAL)
    {
        config->layout = BUTTON_GRID_LAYOUT_HORIZONTAL;
    }

    if (!config->namePrefix)
        config->namePrefix = BUTTON_GRID_DEFAULT_NAME_PREFIX;

    if (!config->textFormat)
        config->textFormat = BUTTON_GRID_DEFAULT_TEXT_FORMAT;

    if (!config->clickIdentifierFormat)
        config->clickIdentifierFormat = BUTTON_GRID_DEFAULT_CLICK_IDENTIFIER_FORMAT;

    config->defaultState = config->defaultState ? 1 : 0;
    config->usePictures = config->usePictures ? 1 : 0;
    config->toggleOnClick = config->toggleOnClick ? 1 : 0;
    config->stretchPictures = config->stretchPictures ? 1 : 0;

    config->pictureOffLoadFailed = config->pictureOffLoadFailed ? 1 : 0;
    config->pictureOnLoadFailed = config->pictureOnLoadFailed ? 1 : 0;
}

static HBITMAP ButtonGrid_CreateGeneratedPicture(
    int width,
    int height,
    COLORREF color,
    int pictureType
)
{
    HDC screenDc;
    HDC memDc;
    HBITMAP bitmap;
    HGDIOBJ oldBitmap;
    HBRUSH brush;
    RECT rc;
    HPEN pen;
    HGDIOBJ oldPen;
    COLORREF markColor;

    if (width < 1)
        width = 1;

    if (height < 1)
        height = 1;

    screenDc = GetDC(NULL);

    if (!screenDc)
        return NULL;

    memDc = CreateCompatibleDC(screenDc);

    if (!memDc)
    {
        ReleaseDC(NULL, screenDc);
        return NULL;
    }

    bitmap = CreateCompatibleBitmap(screenDc, width, height);

    if (!bitmap)
    {
        DeleteDC(memDc);
        ReleaseDC(NULL, screenDc);
        return NULL;
    }

    oldBitmap = SelectObject(memDc, bitmap);

    rc.left = 0;
    rc.top = 0;
    rc.right = width;
    rc.bottom = height;

    brush = CreateSolidBrush(color);
    FillRect(memDc, &rc, brush);
    DeleteObject(brush);

    /*
        pictureType:
            OFF   = plain color, no mark
            ON    = check mark
            ERROR = X mark
    */

    if (pictureType == BUTTON_GRID_PICTURE_TYPE_ON)
    {
        markColor = RGB(0, 100, 0);

        pen = CreatePen(PS_SOLID, 5, markColor);
        oldPen = SelectObject(memDc, pen);

        MoveToEx(memDc, width / 5, height / 2, NULL);
        LineTo(memDc, (width * 2) / 5, (height * 3) / 4);
        LineTo(memDc, (width * 4) / 5, height / 4);

        SelectObject(memDc, oldPen);
        DeleteObject(pen);
    }
    else if (pictureType == BUTTON_GRID_PICTURE_TYPE_ERROR)
    {
        markColor = RGB(160, 0, 0);

        pen = CreatePen(PS_SOLID, 5, markColor);
        oldPen = SelectObject(memDc, pen);

        MoveToEx(memDc, width / 4, height / 4, NULL);
        LineTo(memDc, (width * 3) / 4, (height * 3) / 4);

        MoveToEx(memDc, (width * 3) / 4, height / 4, NULL);
        LineTo(memDc, width / 4, (height * 3) / 4);

        SelectObject(memDc, oldPen);
        DeleteObject(pen);
    }

    SelectObject(memDc, oldBitmap);

    DeleteDC(memDc);
    ReleaseDC(NULL, screenDc);

    return bitmap;
}

static void ButtonGrid_DeleteOwnedPictures(ButtonGrid *grid)
{
    if (!grid)
        return;

    if (grid->ownsPictureOff && grid->pictureOff)
    {
        DeleteObject(grid->pictureOff);
        grid->pictureOff = NULL;
    }

    if (grid->ownsPictureOn && grid->pictureOn)
    {
        DeleteObject(grid->pictureOn);
        grid->pictureOn = NULL;
    }

    grid->ownsPictureOff = 0;
    grid->ownsPictureOn = 0;
}

static void ButtonGrid_EnsurePictures(ButtonGrid *grid)
{
    int offType;
    int onType;

    if (!grid || !grid->usePictures)
        return;

    if (grid->pictureOffLoadFailed)
        offType = BUTTON_GRID_PICTURE_TYPE_ERROR;
    else
        offType = BUTTON_GRID_PICTURE_TYPE_OFF;

    if (grid->pictureOnLoadFailed)
        onType = BUTTON_GRID_PICTURE_TYPE_ERROR;
    else
        onType = BUTTON_GRID_PICTURE_TYPE_ON;

    if (!grid->pictureOff)
    {
        grid->pictureOff = ButtonGrid_CreateGeneratedPicture(
            grid->buttonWidth,
            grid->buttonHeight,
            grid->generatedOffPictureColor,
            offType
        );

        if (grid->pictureOff)
            grid->ownsPictureOff = 1;
    }

    if (!grid->pictureOn)
    {
        grid->pictureOn = ButtonGrid_CreateGeneratedPicture(
            grid->buttonWidth,
            grid->buttonHeight,
            grid->generatedOnPictureColor,
            onType
        );

        if (grid->pictureOn)
            grid->ownsPictureOn = 1;
    }
}

static void ButtonGrid_ApplyConfig(ButtonGrid *grid, const ButtonGridConfig *config)
{
    grid->buttonCount = config->buttonCount;

    grid->buttonWidth = config->buttonWidth;
    grid->buttonHeight = config->buttonHeight;

    grid->horizontalSpacing = config->horizontalSpacing;
    grid->verticalSpacing = config->verticalSpacing;

    grid->layout = config->layout;

    grid->idBase = config->idBase;
    grid->firstIndex = config->firstIndex;

    CopyText(grid->namePrefix, BUTTON_GRID_FORMAT_SIZE, config->namePrefix);
    CopyText(grid->textFormat, BUTTON_GRID_FORMAT_SIZE, config->textFormat);
    CopyText(grid->clickIdentifierFormat, BUTTON_GRID_FORMAT_SIZE, config->clickIdentifierFormat);

    grid->backColor = config->backColor;
    grid->foreColor = config->foreColor;

    grid->usePictures = config->usePictures;
    grid->toggleOnClick = config->toggleOnClick;
    grid->defaultState = config->defaultState;
    grid->stretchPictures = config->stretchPictures;

    grid->pictureOff = config->pictureOff;
    grid->pictureOn = config->pictureOn;

    grid->pictureOffLoadFailed = config->pictureOffLoadFailed;
    grid->pictureOnLoadFailed = config->pictureOnLoadFailed;

    grid->ownsPictureOff = 0;
    grid->ownsPictureOn = 0;

    grid->generatedOffPictureColor = config->generatedOffPictureColor;
    grid->generatedOnPictureColor = config->generatedOnPictureColor;

    grid->buttonBrush = CreateSolidBrush(grid->backColor);

    ButtonGrid_EnsurePictures(grid);
}

static DWORD ButtonGrid_GetButtonStyle(void)
{
    return
        WS_CHILD |
        WS_VISIBLE |
        WS_CLIPSIBLINGS |
        SS_NOTIFY |
        SS_OWNERDRAW;
}

static void ButtonGrid_InitializeButtonData(ButtonGrid *grid, int buttonIndex)
{
    int indexNumber;

    indexNumber = buttonIndex + grid->firstIndex;

    wsprintf(
        grid->buttons[buttonIndex].name,
        "%s%d",
        grid->namePrefix,
        indexNumber
    );

    wsprintf(
        grid->buttons[buttonIndex].text,
        grid->textFormat,
        indexNumber
    );

    grid->buttons[buttonIndex].isOn = grid->defaultState;
}

static HWND ButtonGrid_CreateOneButton(ButtonGrid *grid, int buttonIndex)
{
    return CreateWindowEx(
        0,
        "STATIC",
        grid->buttons[buttonIndex].text,
        ButtonGrid_GetButtonStyle(),
        0,
        0,
        grid->buttonWidth,
        grid->buttonHeight,
        grid->hwnd,
        (HMENU)(grid->idBase + buttonIndex),
        grid->hInstance,
        NULL
    );
}

static void ButtonGrid_CalculateLayout(ButtonGrid *grid, int clientW, int clientH, GridLayout *layout)
{
    int gridW;
    int gridH;

    layout->cols = 1;
    layout->rows = 1;

    if (grid->layout == BUTTON_GRID_LAYOUT_HORIZONTAL)
    {
        layout->cols =
            (clientW + grid->horizontalSpacing) /
            (grid->buttonWidth + grid->horizontalSpacing);

        layout->cols = ClampInt(layout->cols, 1, grid->buttonCount);

        layout->rows = (grid->buttonCount + layout->cols - 1) / layout->cols;
    }
    else
    {
        layout->rows =
            (clientH + grid->verticalSpacing) /
            (grid->buttonHeight + grid->verticalSpacing);

        layout->rows = ClampInt(layout->rows, 1, grid->buttonCount);

        layout->cols = (grid->buttonCount + layout->rows - 1) / layout->rows;
    }

    gridW =
        layout->cols * grid->buttonWidth +
        (layout->cols - 1) * grid->horizontalSpacing;

    gridH =
        layout->rows * grid->buttonHeight +
        (layout->rows - 1) * grid->verticalSpacing;

    layout->startX = (clientW - gridW) / 2;
    layout->startY = (clientH - gridH) / 2;

    if (layout->startX < 0)
        layout->startX = 0;

    if (layout->startY < 0)
        layout->startY = 0;
}

static void ButtonGrid_GetButtonGridPosition(
    ButtonGrid *grid,
    int buttonIndex,
    const GridLayout *layout,
    int *row,
    int *col
)
{
    if (grid->layout == BUTTON_GRID_LAYOUT_HORIZONTAL)
    {
        *row = buttonIndex / layout->cols;
        *col = buttonIndex % layout->cols;
    }
    else
    {
        *col = buttonIndex / layout->rows;
        *row = buttonIndex % layout->rows;
    }
}

static void ButtonGrid_Layout(ButtonGrid *grid)
{
    RECT rc;
    GridLayout layout;

    int clientW;
    int clientH;
    int i;

    if (!grid || !grid->buttons)
        return;

    GetClientRect(grid->hwnd, &rc);

    clientW = rc.right - rc.left;
    clientH = rc.bottom - rc.top;

    ButtonGrid_CalculateLayout(grid, clientW, clientH, &layout);

    for (i = 0; i < grid->buttonCount; i++)
    {
        int row;
        int col;
        int x;
        int y;

        ButtonGrid_GetButtonGridPosition(grid, i, &layout, &row, &col);

        x = layout.startX + col * (grid->buttonWidth + grid->horizontalSpacing);
        y = layout.startY + row * (grid->buttonHeight + grid->verticalSpacing);

        MoveChildClean(
            grid->buttons[i].hwnd,
            x,
            y,
            grid->buttonWidth,
            grid->buttonHeight
        );
    }

    RedrawContainer(grid->hwnd);
}

static int ButtonGrid_CreateButtons(ButtonGrid *grid)
{
    int i;

    grid->buttons = (ButtonItem *)malloc(sizeof(ButtonItem) * grid->buttonCount);

    if (!grid->buttons)
        return 0;

    ZeroMemory(grid->buttons, sizeof(ButtonItem) * grid->buttonCount);

    for (i = 0; i < grid->buttonCount; i++)
    {
        ButtonGrid_InitializeButtonData(grid, i);
        grid->buttons[i].hwnd = ButtonGrid_CreateOneButton(grid, i);
    }

    ButtonGrid_Layout(grid);

    return 1;
}

static void ButtonGrid_RedrawButton(ButtonGrid *grid, int index)
{
    if (!grid || !grid->buttons)
        return;

    if (index < 0 || index >= grid->buttonCount)
        return;

    if (!grid->buttons[index].hwnd)
        return;

    InvalidateRect(grid->buttons[index].hwnd, NULL, TRUE);
    UpdateWindow(grid->buttons[index].hwnd);
}

static void ButtonGrid_RedrawAllButtons(ButtonGrid *grid)
{
    int i;

    if (!grid || !grid->buttons)
        return;

    for (i = 0; i < grid->buttonCount; i++)
        ButtonGrid_RedrawButton(grid, i);

    RedrawContainer(grid->hwnd);
}

static void ButtonGrid_Free(ButtonGrid *grid)
{
    if (!grid)
        return;

    ButtonGrid_DeleteOwnedPictures(grid);

    if (grid->buttonBrush)
    {
        DeleteObject(grid->buttonBrush);
        grid->buttonBrush = NULL;
    }

    if (grid->buttons)
    {
        free(grid->buttons);
        grid->buttons = NULL;
    }

    free(grid);
}

static LRESULT ButtonGrid_HandleCreate(HWND hwnd, LPARAM lParam)
{
    CREATESTRUCT *cs;
    ButtonGridCreateParams *params;
    ButtonGrid *grid;
    ButtonGridConfig config;

    cs = (CREATESTRUCT *)lParam;
    params = (ButtonGridCreateParams *)cs->lpCreateParams;

    grid = (ButtonGrid *)malloc(sizeof(ButtonGrid));

    if (!grid)
        return -1;

    ZeroMemory(grid, sizeof(ButtonGrid));

    grid->hwnd = hwnd;
    grid->hInstance = cs->hInstance;

    ButtonGrid_GetDefaultConfig(&config);

    if (params)
    {
        config = params->config;
        grid->onClick = params->onClick;
    }

    ButtonGrid_NormalizeConfig(&config);
    ButtonGrid_ApplyConfig(grid, &config);

    if (!SetProp(hwnd, BUTTON_GRID_PROP_NAME, (HANDLE)grid))
    {
        ButtonGrid_Free(grid);
        return -1;
    }

    if (!ButtonGrid_CreateButtons(grid))
    {
        RemoveProp(hwnd, BUTTON_GRID_PROP_NAME);
        ButtonGrid_Free(grid);
        return -1;
    }

    return 0;
}

static void ButtonGrid_HandleDestroy(HWND hwnd)
{
    ButtonGrid *grid;

    grid = ButtonGrid_Get(hwnd);

    if (!grid)
        return;

    RemoveProp(hwnd, BUTTON_GRID_PROP_NAME);
    ButtonGrid_Free(grid);
}

static void ButtonGrid_DrawBitmapInRect(
    HDC hdc,
    HBITMAP bitmap,
    const RECT *rect,
    int stretchPictures
)
{
    HDC memDc;
    HGDIOBJ oldBitmap;
    BITMAP bm;

    int rectW;
    int rectH;

    int drawW;
    int drawH;
    int x;
    int y;

    if (!bitmap)
        return;

    if (!GetObject(bitmap, sizeof(BITMAP), &bm))
        return;

    rectW = rect->right - rect->left;
    rectH = rect->bottom - rect->top;

    if (rectW < 1 || rectH < 1)
        return;

    memDc = CreateCompatibleDC(hdc);

    if (!memDc)
        return;

    oldBitmap = SelectObject(memDc, bitmap);

    if (stretchPictures)
    {
        SetStretchBltMode(hdc, COLORONCOLOR);

        StretchBlt(
            hdc,
            rect->left,
            rect->top,
            rectW,
            rectH,
            memDc,
            0,
            0,
            bm.bmWidth,
            bm.bmHeight,
            SRCCOPY
        );
    }
    else
    {
        drawW = bm.bmWidth;
        drawH = bm.bmHeight;

        if (drawW > rectW)
            drawW = rectW;

        if (drawH > rectH)
            drawH = rectH;

        x = rect->left + (rectW - drawW) / 2;
        y = rect->top + (rectH - drawH) / 2;

        BitBlt(
            hdc,
            x,
            y,
            drawW,
            drawH,
            memDc,
            0,
            0,
            SRCCOPY
        );
    }

    SelectObject(memDc, oldBitmap);
    DeleteDC(memDc);
}

static void ButtonGrid_DrawButton(ButtonGrid *grid, DRAWITEMSTRUCT *draw)
{
    int index;
    RECT rc;
    HDC hdc;
    HBITMAP picture;

    if (!grid || !draw)
        return;

    index = (int)draw->CtlID - grid->idBase;

    if (index < 0 || index >= grid->buttonCount)
        return;

    hdc = draw->hDC;
    rc = draw->rcItem;

    if (grid->buttonBrush)
        FillRect(hdc, &rc, grid->buttonBrush);
    else
        FillRect(hdc, &rc, (HBRUSH)(COLOR_WINDOW + 1));

    if (grid->usePictures)
    {
        if (grid->buttons[index].isOn)
            picture = grid->pictureOn;
        else
            picture = grid->pictureOff;

        ButtonGrid_DrawBitmapInRect(
            hdc,
            picture,
            &rc,
            grid->stretchPictures
        );
    }

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, grid->foreColor);

    DrawText(
        hdc,
        grid->buttons[index].text,
        -1,
        &rc,
        DT_CENTER | DT_VCENTER | DT_SINGLELINE
    );

    FrameRect(hdc, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));

    if (draw->itemState & ODS_FOCUS)
        DrawFocusRect(hdc, &rc);
}

static LRESULT ButtonGrid_HandleDrawItem(ButtonGrid *grid, LPARAM lParam)
{
    DRAWITEMSTRUCT *draw;

    draw = (DRAWITEMSTRUCT *)lParam;

    if (!grid || !draw)
        return 0;

    ButtonGrid_DrawButton(grid, draw);

    return TRUE;
}

static void ButtonGrid_HandleStaticClick(ButtonGrid *grid, int controlId)
{
    int index;
    char identifier[128];

    if (!grid)
        return;

    index = controlId - grid->idBase;

    if (index < 0 || index >= grid->buttonCount)
        return;

    if (grid->toggleOnClick)
    {
        grid->buttons[index].isOn = !grid->buttons[index].isOn;
        ButtonGrid_RedrawButton(grid, index);
    }

    wsprintf(
        identifier,
        grid->clickIdentifierFormat,
        grid->buttons[index].name
    );

    if (grid->onClick)
        grid->onClick(identifier);
}

static LRESULT ButtonGrid_HandleEraseBackground(HWND hwnd, WPARAM wParam)
{
    RECT rc;
    HDC hdc;

    hdc = (HDC)wParam;

    GetClientRect(hwnd, &rc);
    FillRect(hdc, &rc, (HBRUSH)(COLOR_WINDOW + 1));

    return 1;
}

BOOL ButtonGrid_RegisterClass(HINSTANCE hInstance)
{
    static int registered = 0;
    WNDCLASS wc;

    if (registered)
        return TRUE;

    ZeroMemory(&wc, sizeof(wc));

    wc.lpfnWndProc = ButtonGrid_WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = BUTTON_GRID_CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClass(&wc))
        return FALSE;

    registered = 1;
    return TRUE;
}

HWND ButtonGrid_CreateEx(
    HWND parent,
    HINSTANCE hInstance,
    int x,
    int y,
    int width,
    int height,
    const ButtonGridConfig *config,
    ButtonGridClickCallback onClick
)
{
    ButtonGridCreateParams params;

    ButtonGrid_GetDefaultConfig(&params.config);

    if (config)
        params.config = *config;

    params.onClick = onClick;

    ButtonGrid_NormalizeConfig(&params.config);

    if (!ButtonGrid_RegisterClass(hInstance))
        return NULL;

    return CreateWindowEx(
        0,
        BUTTON_GRID_CLASS_NAME,
        "",
        WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN,
        x,
        y,
        width,
        height,
        parent,
        NULL,
        hInstance,
        &params
    );
}

HWND ButtonGrid_Create(
    HWND parent,
    HINSTANCE hInstance,
    int x,
    int y,
    int width,
    int height,
    ButtonGridClickCallback onClick
)
{
    return ButtonGrid_CreateEx(
        parent,
        hInstance,
        x,
        y,
        width,
        height,
        NULL,
        onClick
    );
}

void ButtonGrid_SetRect(
    HWND gridHwnd,
    int x,
    int y,
    int width,
    int height
)
{
    if (!gridHwnd)
        return;

    if (width < 1)
        width = 1;

    if (height < 1)
        height = 1;

    SetWindowPos(
        gridHwnd,
        NULL,
        x,
        y,
        width,
        height,
        SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS
    );

    InvalidateRect(gridHwnd, NULL, TRUE);
}

void ButtonGrid_SetButtonSize(
    HWND gridHwnd,
    int buttonWidth,
    int buttonHeight
)
{
    ButtonGrid *grid;

    grid = ButtonGrid_Get(gridHwnd);

    if (!grid)
        return;

    if (buttonWidth < 1)
        buttonWidth = 1;

    if (buttonHeight < 1)
        buttonHeight = 1;

    grid->buttonWidth = buttonWidth;
    grid->buttonHeight = buttonHeight;

    ButtonGrid_Layout(grid);
}

void ButtonGrid_SetSpacing(
    HWND gridHwnd,
    int horizontalSpacing,
    int verticalSpacing
)
{
    ButtonGrid *grid;

    grid = ButtonGrid_Get(gridHwnd);

    if (!grid)
        return;

    if (horizontalSpacing < 0)
        horizontalSpacing = 0;

    if (verticalSpacing < 0)
        verticalSpacing = 0;

    grid->horizontalSpacing = horizontalSpacing;
    grid->verticalSpacing = verticalSpacing;

    ButtonGrid_Layout(grid);
}

void ButtonGrid_SetLayout(HWND gridHwnd, int layout)
{
    ButtonGrid *grid;

    grid = ButtonGrid_Get(gridHwnd);

    if (!grid)
        return;

    if (layout != BUTTON_GRID_LAYOUT_HORIZONTAL &&
        layout != BUTTON_GRID_LAYOUT_VERTICAL)
    {
        layout = BUTTON_GRID_LAYOUT_HORIZONTAL;
    }

    grid->layout = layout;

    ButtonGrid_Layout(grid);
}

void ButtonGrid_Relayout(HWND gridHwnd)
{
    ButtonGrid_Layout(ButtonGrid_Get(gridHwnd));
}

void ButtonGrid_SetPictures(
    HWND gridHwnd,
    HBITMAP pictureOff,
    HBITMAP pictureOn,
    int stretchPictures
)
{
    ButtonGrid *grid;

    grid = ButtonGrid_Get(gridHwnd);

    if (!grid)
        return;

    ButtonGrid_DeleteOwnedPictures(grid);

    grid->pictureOff = pictureOff;
    grid->pictureOn = pictureOn;

    grid->pictureOffLoadFailed = 0;
    grid->pictureOnLoadFailed = 0;

    grid->ownsPictureOff = 0;
    grid->ownsPictureOn = 0;

    grid->stretchPictures = stretchPictures ? 1 : 0;
    grid->usePictures = 1;

    ButtonGrid_EnsurePictures(grid);
    ButtonGrid_RedrawAllButtons(grid);
}

void ButtonGrid_SetButtonStateByNumber(
    HWND gridHwnd,
    int buttonNumber,
    int isOn
)
{
    ButtonGrid *grid;
    int index;

    grid = ButtonGrid_Get(gridHwnd);

    if (!grid)
        return;

    index = buttonNumber - grid->firstIndex;

    if (index < 0 || index >= grid->buttonCount)
        return;

    grid->buttons[index].isOn = isOn ? 1 : 0;
    ButtonGrid_RedrawButton(grid, index);
}

int ButtonGrid_GetButtonStateByNumber(
    HWND gridHwnd,
    int buttonNumber
)
{
    ButtonGrid *grid;
    int index;

    grid = ButtonGrid_Get(gridHwnd);

    if (!grid)
        return -1;

    index = buttonNumber - grid->firstIndex;

    if (index < 0 || index >= grid->buttonCount)
        return -1;

    return grid->buttons[index].isOn;
}

void ButtonGrid_ToggleButtonStateByNumber(
    HWND gridHwnd,
    int buttonNumber
)
{
    ButtonGrid *grid;
    int index;

    grid = ButtonGrid_Get(gridHwnd);

    if (!grid)
        return;

    index = buttonNumber - grid->firstIndex;

    if (index < 0 || index >= grid->buttonCount)
        return;

    grid->buttons[index].isOn = !grid->buttons[index].isOn;
    ButtonGrid_RedrawButton(grid, index);
}

static LRESULT CALLBACK ButtonGrid_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    ButtonGrid *grid;

    grid = ButtonGrid_Get(hwnd);

    switch (msg)
    {
        case WM_CREATE:
        {
            return ButtonGrid_HandleCreate(hwnd, lParam);
        }

        case WM_SIZE:
        {
            ButtonGrid_Layout(grid);
            return 0;
        }

        case WM_COMMAND:
        {
            int controlId;
            int notifyCode;

            controlId = LOWORD(wParam);
            notifyCode = HIWORD(wParam);

            if (notifyCode == STN_CLICKED)
            {
                ButtonGrid_HandleStaticClick(grid, controlId);
                return 0;
            }

            break;
        }

        case WM_DRAWITEM:
        {
            return ButtonGrid_HandleDrawItem(grid, lParam);
        }

        case WM_ERASEBKGND:
        {
            return ButtonGrid_HandleEraseBackground(hwnd, wParam);
        }

        case WM_NCDESTROY:
        {
            ButtonGrid_HandleDestroy(hwnd);
            break;
        }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}