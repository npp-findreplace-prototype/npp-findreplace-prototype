#include "button_grid_core_internal.h"

ButtonGrid *ButtonGrid_Get(HWND hwnd)
{
    return (ButtonGrid *)GetProp(hwnd, BUTTON_GRID_PROP_NAME);
}

void ButtonGrid_CopyText(char *dest, int destSize, const char *src)
{
    Ui_CopyText(dest, destSize, src);
}

int ButtonGrid_SameText(const char *a, const char *b)
{
    if (!a || !b)
        return 0;

    return lstrcmp(a, b) == 0;
}

LRESULT ButtonGrid_HandleCreate(HWND hwnd, LPARAM lParam)
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

    ButtonGrid_PrepareButtonsForKeyboard(grid);
    ButtonGrid_RelayoutAndRedraw(grid, 0);

    return 0;
}

void ButtonGrid_HandleDestroy(HWND hwnd)
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
        WS_EX_CONTROLPARENT,
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