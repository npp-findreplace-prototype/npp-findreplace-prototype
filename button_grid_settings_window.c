#include <windows.h>

#include "button_grid_internal.h"
#include "button_grid_settings.h"
#include "button_grid_settings_internal.h"

static LRESULT CALLBACK ButtonGrid_SettingsWndProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam
);

ButtonGrid *ButtonGrid_SettingsGetGrid(HWND hwnd)
{
    return (ButtonGrid *)GetProp(hwnd, BUTTON_GRID_SETTINGS_PROP_NAME);
}

void ButtonGrid_SettingsSetGrid(HWND hwnd, ButtonGrid *grid)
{
    SetProp(hwnd, BUTTON_GRID_SETTINGS_PROP_NAME, (HANDLE)grid);
}

BOOL ButtonGrid_SettingsRegisterClass(HINSTANCE hInstance)
{
    static int registered = 0;
    WNDCLASS wc;

    if (registered)
        return TRUE;

    InitCommonControls();

    ZeroMemory(&wc, sizeof(wc));

    wc.lpfnWndProc = ButtonGrid_SettingsWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = BUTTON_GRID_SETTINGS_CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClass(&wc))
        return FALSE;

    registered = 1;
    return TRUE;
}

static void ButtonGrid_GetSettingsWindowRect(ButtonGrid *grid, RECT *outRc)
{
    RECT gridRc;
    RECT workRc;
    int w;
    int h;
    int x;
    int y;

    SetRect(
        outRc,
        100,
        100,
        100 + BG_SETTINGS_WINDOW_WIDTH,
        100 + BG_SETTINGS_WINDOW_HEIGHT
    );

    if (!grid || !grid->hwnd)
        return;

    GetWindowRect(grid->hwnd, &gridRc);

    if (!SystemParametersInfo(SPI_GETWORKAREA, 0, &workRc, 0))
    {
        workRc.left = 0;
        workRc.top = 0;
        workRc.right = GetSystemMetrics(SM_CXSCREEN);
        workRc.bottom = GetSystemMetrics(SM_CYSCREEN);
    }

    w = BG_SETTINGS_WINDOW_WIDTH;
    h = BG_SETTINGS_WINDOW_HEIGHT;

    if (w > workRc.right - workRc.left)
        w = workRc.right - workRc.left;

    if (h > workRc.bottom - workRc.top)
        h = workRc.bottom - workRc.top;

    x = gridRc.right + BG_SETTINGS_WINDOW_GAP;
    y = gridRc.top;

    if (x + w > workRc.right)
        x = gridRc.left - w - BG_SETTINGS_WINDOW_GAP;

    if (x < workRc.left)
        x = workRc.right - w - BG_SETTINGS_WINDOW_GAP;

    if (x < workRc.left)
        x = workRc.left;

    if (y + h > workRc.bottom)
        y = workRc.bottom - h - BG_SETTINGS_WINDOW_GAP;

    if (y < workRc.top)
        y = workRc.top;

    SetRect(outRc, x, y, x + w, y + h);
}

static void ButtonGrid_PositionSettingsWindow(ButtonGrid *grid, HWND pageHwnd)
{
    RECT rc;

    if (!grid || !pageHwnd)
        return;

    ButtonGrid_GetSettingsWindowRect(grid, &rc);

    SetWindowPos(
        pageHwnd,
        HWND_TOP,
        rc.left,
        rc.top,
        rc.right - rc.left,
        rc.bottom - rc.top,
        SWP_NOACTIVATE
    );
}

static HWND ButtonGrid_GetOwnerWindow(ButtonGrid *grid)
{
    HWND owner;

    owner = NULL;

    if (grid && grid->hwnd)
        owner = GetAncestor(grid->hwnd, GA_ROOT);

    if (!owner && grid)
        owner = grid->hwnd;

    return owner;
}

static HWND ButtonGrid_CreateSettingsPage(ButtonGrid *grid)
{
    RECT rc;
    HWND owner;

    if (!grid)
        return NULL;

    if (grid->settingsPageHwnd)
        return grid->settingsPageHwnd;

    if (!ButtonGrid_SettingsRegisterClass(grid->hInstance))
        return NULL;

    ButtonGrid_GetSettingsWindowRect(grid, &rc);
    owner = ButtonGrid_GetOwnerWindow(grid);

    grid->settingsPageHwnd = CreateWindowEx(
        WS_EX_TOOLWINDOW,
        BUTTON_GRID_SETTINGS_CLASS_NAME,
        "Grid Settings",
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_VSCROLL,
        rc.left,
        rc.top,
        rc.right - rc.left,
        rc.bottom - rc.top,
        owner,
        NULL,
        grid->hInstance,
        grid
    );

    return grid->settingsPageHwnd;
}

void ButtonGrid_ShowSettingsPage(ButtonGrid *grid, int show)
{
    HWND pageHwnd;

    if (!grid)
        return;

    if (show)
    {
        pageHwnd = ButtonGrid_CreateSettingsPage(grid);

        if (!pageHwnd)
            return;

        grid->settingsPageVisible = 1;

        if (!IsWindowVisible(pageHwnd))
            ButtonGrid_PositionSettingsWindow(grid, pageHwnd);

        ButtonGrid_SettingsRefreshAll(pageHwnd);
        ButtonGrid_SettingsLayoutControls(pageHwnd);

        ShowWindow(pageHwnd, SW_SHOWNORMAL);
        BringWindowToTop(pageHwnd);
        SetForegroundWindow(pageHwnd);
        SetFocus(GetDlgItem(pageHwnd, BG_SETTINGS_ID_FILTER));
    }
    else
    {
        grid->settingsPageVisible = 0;

        if (grid->settingsPageHwnd)
            ShowWindow(grid->settingsPageHwnd, SW_HIDE);
    }

    InvalidateRect(grid->hwnd, NULL, TRUE);
}

void ButtonGrid_ToggleSettingsPage(ButtonGrid *grid)
{
    if (!grid)
        return;

    ButtonGrid_ShowSettingsPage(grid, !grid->settingsPageVisible);
}

void ButtonGrid_DestroySettingsPage(ButtonGrid *grid)
{
    HWND pageHwnd;

    if (!grid)
        return;

    pageHwnd = grid->settingsPageHwnd;

    grid->settingsPageHwnd = NULL;
    grid->settingsPageVisible = 0;

    if (pageHwnd)
        DestroyWindow(pageHwnd);
}

static LRESULT ButtonGrid_SettingsHandleCreate(HWND hwnd, LPARAM lParam)
{
    CREATESTRUCT *cs;
    ButtonGrid *grid;

    cs = (CREATESTRUCT *)lParam;
    grid = (ButtonGrid *)cs->lpCreateParams;

    ButtonGrid_SettingsSetGrid(hwnd, grid);

    ButtonGrid_SettingsCreateControls(hwnd, grid);
    ButtonGrid_SettingsLayoutControls(hwnd);

    return 0;
}

static void ButtonGrid_SettingsHandleDestroy(HWND hwnd)
{
    ButtonGrid *grid;

    grid = ButtonGrid_SettingsGetGrid(hwnd);

    if (grid && grid->settingsPageHwnd == hwnd)
    {
        grid->settingsPageHwnd = NULL;
        grid->settingsPageVisible = 0;
    }

    RemoveProp(hwnd, BUTTON_GRID_SETTINGS_PROP_NAME);
}

static LRESULT CALLBACK ButtonGrid_SettingsWndProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam
)
{
    ButtonGrid *grid;

    grid = ButtonGrid_SettingsGetGrid(hwnd);

    switch (msg)
    {
        case WM_CREATE:
        {
            return ButtonGrid_SettingsHandleCreate(hwnd, lParam);
        }

        case WM_SIZE:
        {
            ButtonGrid_SettingsLayoutControls(hwnd);
            return 0;
        }

        case WM_CLOSE:
        {
            if (grid)
                ButtonGrid_ShowSettingsPage(grid, 0);
            else
                DestroyWindow(hwnd);

            return 0;
        }

        case WM_VSCROLL:
        {
            ButtonGrid_SettingsScroll(hwnd, LOWORD(wParam), 0);
            return 0;
        }

        case WM_MOUSEWHEEL:
        {
            ButtonGrid_SettingsScroll(hwnd, -1, GET_WHEEL_DELTA_WPARAM(wParam));
            return 0;
        }

        case WM_COMMAND:
        {
            if (ButtonGrid_SettingsHandleCommand(hwnd, wParam, lParam))
                return 0;

            break;
        }

        case WM_HSCROLL:
        {
            if (ButtonGrid_SettingsHandleHScroll(hwnd, wParam, lParam))
                return 0;

            break;
        }

        case WM_KEYDOWN:
        {
            if (wParam == VK_ESCAPE)
            {
                ButtonGrid_ShowSettingsPage(grid, 0);
                return 0;
            }

            break;
        }

        case WM_NCDESTROY:
        {
            ButtonGrid_SettingsHandleDestroy(hwnd);
            break;
        }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}