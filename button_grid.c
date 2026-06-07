#include "button_grid_internal.h"
#include "button_grid_settings.h"

#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0
#endif

static LRESULT CALLBACK ButtonGrid_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

ButtonGrid *ButtonGrid_Get(HWND hwnd)
{
    return (ButtonGrid *)GetProp(hwnd, BUTTON_GRID_PROP_NAME);
}

void ButtonGrid_CopyText(char *dest, int destSize, const char *src)
{
    if (!dest || destSize <= 0)
        return;

    if (!src)
        src = "";

    lstrcpyn(dest, src, destSize);
    dest[destSize - 1] = '\0';
}

int ButtonGrid_SameText(const char *a, const char *b)
{
    if (!a || !b)
        return 0;

    return lstrcmp(a, b) == 0;
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

    ButtonGrid_UpdateDpi(grid);

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

    ButtonGrid_DestroySettingsPage(grid);

    RemoveProp(hwnd, BUTTON_GRID_PROP_NAME);
    ButtonGrid_Free(grid);
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
    wc.hbrBackground = NULL;

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
        WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
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

void ButtonGrid_SetClickCallback(
    HWND gridHwnd,
    ButtonGridClickCallback onClick
)
{
    ButtonGrid *grid;

    grid = ButtonGrid_Get(gridHwnd);

    if (!grid)
        return;

    grid->onClick = onClick;
}

void ButtonGrid_SetRect(
    HWND gridHwnd,
    int x,
    int y,
    int width,
    int height
)
{
    ButtonGrid *grid;

    if (!gridHwnd)
        return;

    if (width < 1)
        width = 1;

    if (height < 1)
        height = 1;

    grid = ButtonGrid_Get(gridHwnd);

    if (grid)
    {
        ButtonGrid_UpdateDpi(grid);
        ButtonGrid_AdjustRectToLayoutSteps(grid, &width, &height);
    }

    SetWindowPos(
        gridHwnd,
        NULL,
        x,
        y,
        width,
        height,
        SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS
    );

    if (grid)
    {
        ButtonGrid_Layout(grid);
        ButtonGrid_LayoutSettingsPage(grid);
    }

    InvalidateRect(gridHwnd, NULL, FALSE);
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

    ButtonGrid_UpdateDpi(grid);
    ButtonGrid_UpdateAllButtonSizes(grid);
    ButtonGrid_Layout(grid);
    ButtonGrid_RedrawAllButtons(grid);

    InvalidateRect(grid->hwnd, NULL, FALSE);
}

void ButtonGrid_SetSizeMode(
    HWND gridHwnd,
    int sizeMode
)
{
    ButtonGrid *grid;

    grid = ButtonGrid_Get(gridHwnd);

    if (!grid)
        return;

    grid->sizeMode = ButtonGrid_NormalizeSizeMode(sizeMode);

    ButtonGrid_UpdateDpi(grid);
    ButtonGrid_UpdateAllButtonSizes(grid);
    ButtonGrid_Layout(grid);
    ButtonGrid_RedrawAllButtons(grid);

    InvalidateRect(grid->hwnd, NULL, FALSE);
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

    ButtonGrid_UpdateDpi(grid);
    ButtonGrid_Layout(grid);

    InvalidateRect(grid->hwnd, NULL, FALSE);
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

    ButtonGrid_UpdateDpi(grid);
    ButtonGrid_UpdateAllButtonSizes(grid);
    ButtonGrid_Layout(grid);
    ButtonGrid_RedrawAllButtons(grid);

    InvalidateRect(grid->hwnd, NULL, FALSE);
}

void ButtonGrid_Relayout(HWND gridHwnd)
{
    ButtonGrid *grid;

    grid = ButtonGrid_Get(gridHwnd);

    if (!grid)
        return;

    ButtonGrid_UpdateDpi(grid);
    ButtonGrid_UpdateAllButtonSizes(grid);
    ButtonGrid_Layout(grid);
    ButtonGrid_LayoutSettingsPage(grid);
    ButtonGrid_RedrawAllButtons(grid);

    InvalidateRect(gridHwnd, NULL, FALSE);
}

static void ButtonGrid_HandleDpiChanged(ButtonGrid *grid)
{
    if (!grid)
        return;

    ButtonGrid_UpdateDpi(grid);
    ButtonGrid_UpdateAllButtonSizes(grid);
    ButtonGrid_Layout(grid);
    ButtonGrid_LayoutSettingsPage(grid);
    ButtonGrid_RedrawAllButtons(grid);

    InvalidateRect(grid->hwnd, NULL, FALSE);
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
            if (grid)
            {
                ButtonGrid_UpdateDpi(grid);
                ButtonGrid_Layout(grid);
                ButtonGrid_LayoutSettingsPage(grid);
                InvalidateRect(hwnd, NULL, FALSE);
            }

            return 0;
        }

        case WM_DPICHANGED:
        {
            ButtonGrid_HandleDpiChanged(grid);
            return 0;
        }

        case WM_LBUTTONDOWN:
        {
            int x;
            int y;

            x = LOWORD(lParam);
            y = HIWORD(lParam);

            if (ButtonGrid_HandleGearClick(grid, x, y))
                return 0;

            break;
        }

        case WM_PAINT:
        {
            return ButtonGrid_HandlePaint(hwnd);
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