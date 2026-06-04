#include <windows.h>
#include <stdlib.h>

#include "button_grid.h"

#ifndef SS_TYPEMASK
#define SS_TYPEMASK 0x0000001F
#endif

#ifndef SS_ENHMETAFILE
#define SS_ENHMETAFILE 0x0000000F
#endif

#ifndef SWP_NOCOPYBITS
#define SWP_NOCOPYBITS 0x0100
#endif

#define BUTTON_GRID_CLASS_NAME "ButtonGridChildClass"
#define BUTTON_GRID_PROP_NAME "ButtonGridData"

typedef struct ButtonItem
{
    HWND hwnd;
    char name[64];
    char text[64];
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

    ButtonItem buttons[BUTTON_GRID_COUNT];

    int buttonWidth;
    int buttonHeight;
    int horizontalSpacing;
    int verticalSpacing;
    int layout;
    int useMetafile;

    COLORREF backColor;
    COLORREF foreColor;

    HBRUSH buttonBrush;
} ButtonGrid;

typedef struct ButtonGridCreateParams
{
    ButtonGridClickCallback onClick;
} ButtonGridCreateParams;

static LRESULT CALLBACK ButtonGrid_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

static ButtonGrid *ButtonGrid_Get(HWND hwnd)
{
    return (ButtonGrid *)GetProp(hwnd, BUTTON_GRID_PROP_NAME);
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

static DWORD ButtonGrid_GetButtonStyle(ButtonGrid *grid)
{
    DWORD style;

    style =
        WS_CHILD |
        WS_VISIBLE |
        WS_CLIPSIBLINGS |
        SS_NOTIFY |
        SS_CENTER |
        SS_CENTERIMAGE;

    if (grid->useMetafile)
    {
        style &= ~SS_TYPEMASK;
        style |= SS_ENHMETAFILE;
    }

    return style;
}

static void ButtonGrid_InitializeButtonData(ButtonItem *button, int indexNumber)
{
    wsprintf(button->name, "%s%d", BUTTON_GRID_NAME_PREFIX, indexNumber);
    wsprintf(button->text, BUTTON_GRID_TEXT_FORMAT, indexNumber);
}

static HWND ButtonGrid_CreateOneButton(ButtonGrid *grid, int buttonIndex)
{
    return CreateWindowEx(
        0,
        "STATIC",
        grid->buttons[buttonIndex].text,
        ButtonGrid_GetButtonStyle(grid),
        0,
        0,
        grid->buttonWidth,
        grid->buttonHeight,
        grid->hwnd,
        (HMENU)(BUTTON_ID_BASE + buttonIndex),
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

        layout->cols = ClampInt(layout->cols, 1, BUTTON_GRID_COUNT);

        layout->rows = (BUTTON_GRID_COUNT + layout->cols - 1) / layout->cols;
    }
    else
    {
        layout->rows =
            (clientH + grid->verticalSpacing) /
            (grid->buttonHeight + grid->verticalSpacing);

        layout->rows = ClampInt(layout->rows, 1, BUTTON_GRID_COUNT);

        layout->cols = (BUTTON_GRID_COUNT + layout->rows - 1) / layout->rows;
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

    if (!grid)
        return;

    GetClientRect(grid->hwnd, &rc);

    clientW = rc.right - rc.left;
    clientH = rc.bottom - rc.top;

    ButtonGrid_CalculateLayout(grid, clientW, clientH, &layout);

    for (i = 0; i < BUTTON_GRID_COUNT; i++)
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

static void ButtonGrid_CreateButtons(ButtonGrid *grid)
{
    int i;

    for (i = 0; i < BUTTON_GRID_COUNT; i++)
    {
        int indexNumber;

        indexNumber = i + BUTTON_FIRST_INDEX;

        ButtonGrid_InitializeButtonData(&grid->buttons[i], indexNumber);
        grid->buttons[i].hwnd = ButtonGrid_CreateOneButton(grid, i);
    }

    ButtonGrid_Layout(grid);
}

static void ButtonGrid_SetDefaults(ButtonGrid *grid)
{
    grid->buttonWidth = BUTTON_GRID_DEFAULT_BUTTON_WIDTH;
    grid->buttonHeight = BUTTON_GRID_DEFAULT_BUTTON_HEIGHT;

    grid->horizontalSpacing = BUTTON_GRID_DEFAULT_HORIZONTAL_SPACING;
    grid->verticalSpacing = BUTTON_GRID_DEFAULT_VERTICAL_SPACING;

    grid->layout = BUTTON_GRID_DEFAULT_LAYOUT;
    grid->useMetafile = BUTTON_GRID_DEFAULT_IS_METAFILE;

    grid->backColor = BUTTON_GRID_BACK_COLOR;
    grid->foreColor = BUTTON_GRID_FORE_COLOR;

    grid->buttonBrush = CreateSolidBrush(grid->backColor);
}

static LRESULT ButtonGrid_HandleCreate(HWND hwnd, LPARAM lParam)
{
    CREATESTRUCT *cs;
    ButtonGridCreateParams *params;
    ButtonGrid *grid;

    cs = (CREATESTRUCT *)lParam;
    params = (ButtonGridCreateParams *)cs->lpCreateParams;

    grid = (ButtonGrid *)malloc(sizeof(ButtonGrid));

    if (!grid)
        return -1;

    ZeroMemory(grid, sizeof(ButtonGrid));

    grid->hwnd = hwnd;
    grid->hInstance = cs->hInstance;

    if (params)
        grid->onClick = params->onClick;

    ButtonGrid_SetDefaults(grid);

    SetProp(hwnd, BUTTON_GRID_PROP_NAME, (HANDLE)grid);

    ButtonGrid_CreateButtons(grid);

    return 0;
}

static void ButtonGrid_HandleDestroy(HWND hwnd)
{
    ButtonGrid *grid;

    grid = ButtonGrid_Get(hwnd);

    if (!grid)
        return;

    if (grid->buttonBrush)
    {
        DeleteObject(grid->buttonBrush);
        grid->buttonBrush = NULL;
    }

    RemoveProp(hwnd, BUTTON_GRID_PROP_NAME);
    free(grid);
}

static LRESULT ButtonGrid_HandleCtlColorStatic(ButtonGrid *grid, WPARAM wParam)
{
    HDC hdc;

    if (!grid)
        return 0;

    hdc = (HDC)wParam;

    SetTextColor(hdc, grid->foreColor);
    SetBkColor(hdc, grid->backColor);

    return (LRESULT)grid->buttonBrush;
}

static void ButtonGrid_HandleStaticClick(ButtonGrid *grid, int controlId)
{
    int index;
    char identifier[128];

    if (!grid)
        return;

    index = controlId - BUTTON_ID_BASE;

    if (index < 0 || index >= BUTTON_GRID_COUNT)
        return;

    wsprintf(
        identifier,
        BUTTON_GRID_CLICK_IDENTIFIER_FORMAT,
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
    ButtonGridCreateParams params;

    params.onClick = onClick;

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

    MoveWindow(gridHwnd, x, y, width, height, TRUE);
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

        case WM_CTLCOLORSTATIC:
        {
            return ButtonGrid_HandleCtlColorStatic(grid, wParam);
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