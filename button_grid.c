#include "button_grid_internal.h"
#include "button_grid_settings.h"

#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0
#endif

#ifndef DLGC_WANTARROWS
#define DLGC_WANTARROWS 0x0001
#endif

#ifndef DLGC_WANTTAB
#define DLGC_WANTTAB 0x0002
#endif

#ifndef DLGC_WANTCHARS
#define DLGC_WANTCHARS 0x0080
#endif

#ifndef VK_RETURN
#define VK_RETURN 0x0D
#endif

#ifndef VK_TAB
#define VK_TAB 0x09
#endif

#ifndef VK_SPACE
#define VK_SPACE 0x20
#endif

#ifndef WS_EX_CONTROLPARENT
#define WS_EX_CONTROLPARENT 0x00010000L
#endif

#ifndef GWLP_WNDPROC
#define GWLP_WNDPROC GWL_WNDPROC
#endif

#ifndef GetWindowLongPtr
#define GetWindowLongPtr GetWindowLong
#endif

#ifndef SetWindowLongPtr
#define SetWindowLongPtr SetWindowLong
#endif

#ifndef LONG_PTR
#define LONG_PTR LONG
#endif

#define BUTTON_GRID_BUTTON_PROP_GRID "ButtonGridButtonGrid"
#define BUTTON_GRID_BUTTON_PROP_OLDPROC "ButtonGridButtonOldProc"

static LRESULT CALLBACK ButtonGrid_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK ButtonGrid_ButtonWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

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

static const char *ButtonGrid_GetButtonActionName(ButtonItem *button)
{
    if (!button)
        return "";

    if (button->action[0])
        return button->action;

    return button->name;
}

int ButtonGrid_FindButtonIndexByHwnd(ButtonGrid *grid, HWND hwnd)
{
    int i;

    if (!grid || !grid->buttons || !hwnd)
        return -1;

    for (i = 0; i < grid->buttonCount; i++)
    {
        if (grid->buttons[i].hwnd == hwnd)
            return i;
    }

    return -1;
}

static int ButtonGrid_ButtonCanTakeFocus(ButtonGrid *grid, int index)
{
    HWND hwnd;

    if (!grid || !grid->buttons)
        return 0;

    if (index < 0 || index >= grid->buttonCount)
        return 0;

    if (grid->buttons[index].behavior == BUTTON_GRID_BUTTON_DISABLED)
        return 0;

    hwnd = grid->buttons[index].hwnd;

    if (!hwnd)
        return 0;

    if (!IsWindowVisible(hwnd))
        return 0;

    return 1;
}

void ButtonGrid_FocusButtonByIndex(ButtonGrid *grid, int index)
{
    if (!ButtonGrid_ButtonCanTakeFocus(grid, index))
        return;

    SetFocus(grid->buttons[index].hwnd);
}

void ButtonGrid_FocusNextButton(ButtonGrid *grid, int currentIndex, int direction)
{
    int i;
    int index;

    if (!grid || !grid->buttons || grid->buttonCount < 1)
        return;

    if (direction == 0)
        direction = 1;

    if (currentIndex < 0 || currentIndex >= grid->buttonCount)
    {
        if (direction > 0)
            currentIndex = -1;
        else
            currentIndex = grid->buttonCount;
    }

    for (i = 0; i < grid->buttonCount; i++)
    {
        index = currentIndex + direction;

        if (index >= grid->buttonCount)
            index = 0;

        if (index < 0)
            index = grid->buttonCount - 1;

        if (ButtonGrid_ButtonCanTakeFocus(grid, index))
        {
            SetFocus(grid->buttons[index].hwnd);
            return;
        }

        currentIndex = index;
    }
}

static void ButtonGrid_RedrawChangedButton(ButtonGrid *grid, int index)
{
    if (!grid)
        return;

    if (index < 0 || index >= grid->buttonCount)
        return;

    ButtonGrid_RedrawButton(grid, index);
}

static int ButtonGrid_SelectRadioButton(ButtonGrid *grid, int index)
{
    int i;
    int changed;

    if (!grid || !grid->buttons)
        return 0;

    if (index < 0 || index >= grid->buttonCount)
        return 0;

    if (grid->buttons[index].isOn)
        return 0;

    changed = 0;

    for (i = 0; i < grid->buttonCount; i++)
    {
        if (i == index)
            continue;

        if (grid->buttons[i].behavior != BUTTON_GRID_BUTTON_RADIO)
            continue;

        if (grid->buttons[i].radioGroup != grid->buttons[index].radioGroup)
            continue;

        if (!grid->buttons[i].isOn)
            continue;

        grid->buttons[i].isOn = 0;
        ButtonGrid_RedrawChangedButton(grid, i);
        changed = 1;
    }

    grid->buttons[index].isOn = 1;
    ButtonGrid_RedrawChangedButton(grid, index);

    return 1;
}

static int ButtonGrid_ToggleButton(ButtonGrid *grid, int index)
{
    if (!grid || !grid->buttons)
        return 0;

    if (index < 0 || index >= grid->buttonCount)
        return 0;

    if (!grid->toggleOnClick)
        return 0;

    grid->buttons[index].isOn = grid->buttons[index].isOn ? 0 : 1;
    ButtonGrid_RedrawChangedButton(grid, index);

    return 1;
}

void ButtonGrid_ActivateButtonByIndex(ButtonGrid *grid, int index)
{
    ButtonItem *button;
    int changed;

    if (!grid || !grid->buttons)
        return;

    if (index < 0 || index >= grid->buttonCount)
        return;

    button = &grid->buttons[index];

    if (button->behavior == BUTTON_GRID_BUTTON_DISABLED)
        return;

    ButtonGrid_FocusButtonByIndex(grid, index);

    changed = 0;

    if (button->behavior == BUTTON_GRID_BUTTON_RADIO)
    {
        changed = ButtonGrid_SelectRadioButton(grid, index);

        if (!changed)
            return;
    }
    else
    {
        changed = ButtonGrid_ToggleButton(grid, index);
        (void)changed;
    }

    if (grid->onClick)
        grid->onClick(ButtonGrid_GetButtonActionName(button));
}

static void ButtonGrid_PrepareButtonForKeyboard(ButtonGrid *grid, int index)
{
    HWND hwnd;
    LONG style;
    WNDPROC oldProc;

    if (!grid || !grid->buttons)
        return;

    if (index < 0 || index >= grid->buttonCount)
        return;

    hwnd = grid->buttons[index].hwnd;

    if (!hwnd)
        return;

    style = GetWindowLong(hwnd, GWL_STYLE);

    if (!(style & WS_TABSTOP))
        SetWindowLong(hwnd, GWL_STYLE, style | WS_TABSTOP);

    if (GetProp(hwnd, BUTTON_GRID_BUTTON_PROP_OLDPROC))
    {
        SetProp(hwnd, BUTTON_GRID_BUTTON_PROP_GRID, (HANDLE)grid);
        return;
    }

    SetProp(hwnd, BUTTON_GRID_BUTTON_PROP_GRID, (HANDLE)grid);

    oldProc = (WNDPROC)SetWindowLongPtr(
        hwnd,
        GWLP_WNDPROC,
        (LONG_PTR)ButtonGrid_ButtonWndProc
    );

    SetProp(hwnd, BUTTON_GRID_BUTTON_PROP_OLDPROC, (HANDLE)oldProc);
}

static void ButtonGrid_PrepareButtonsForKeyboard(ButtonGrid *grid)
{
    int i;

    if (!grid || !grid->buttons)
        return;

    for (i = 0; i < grid->buttonCount; i++)
        ButtonGrid_PrepareButtonForKeyboard(grid, i);
}

static LRESULT ButtonGrid_CallOldButtonProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    WNDPROC oldProc;

    oldProc = (WNDPROC)GetProp(hwnd, BUTTON_GRID_BUTTON_PROP_OLDPROC);

    if (oldProc)
        return CallWindowProc(oldProc, hwnd, msg, wParam, lParam);

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

static LRESULT CALLBACK ButtonGrid_ButtonWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    ButtonGrid *grid;
    int index;

    grid = (ButtonGrid *)GetProp(hwnd, BUTTON_GRID_BUTTON_PROP_GRID);

    switch (msg)
    {
        case WM_GETDLGCODE:
        {
            return DLGC_WANTTAB | DLGC_WANTARROWS | DLGC_WANTCHARS;
        }

        case WM_SETFOCUS:
        case WM_KILLFOCUS:
        {
            InvalidateRect(hwnd, NULL, FALSE);
            break;
        }

        case WM_LBUTTONDOWN:
        {
            SetFocus(hwnd);
            break;
        }

        case WM_KEYDOWN:
        {
            index = ButtonGrid_FindButtonIndexByHwnd(grid, hwnd);

            if (wParam == VK_SPACE || wParam == VK_RETURN)
            {
                ButtonGrid_ActivateButtonByIndex(grid, index);
                return 0;
            }

            if (wParam == VK_TAB)
            {
                if (GetKeyState(VK_SHIFT) & 0x8000)
                    ButtonGrid_FocusNextButton(grid, index, -1);
                else
                    ButtonGrid_FocusNextButton(grid, index, 1);

                return 0;
            }

            break;
        }

        case WM_ERASEBKGND:
        {
            return 1;
        }

        case WM_NCDESTROY:
        {
            WNDPROC oldProc;

            oldProc = (WNDPROC)GetProp(hwnd, BUTTON_GRID_BUTTON_PROP_OLDPROC);

            RemoveProp(hwnd, BUTTON_GRID_BUTTON_PROP_GRID);
            RemoveProp(hwnd, BUTTON_GRID_BUTTON_PROP_OLDPROC);

            if (oldProc)
            {
                SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)oldProc);
                return CallWindowProc(oldProc, hwnd, msg, wParam, lParam);
            }

            break;
        }
    }

    return ButtonGrid_CallOldButtonProc(hwnd, msg, wParam, lParam);
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

    ButtonGrid_PrepareButtonsForKeyboard(grid);

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

static int ButtonGrid_GetWindowRectInParent(HWND hwnd, RECT *rc)
{
    HWND parent;

    if (!hwnd || !rc)
        return 0;

    if (!GetWindowRect(hwnd, rc))
        return 0;

    parent = GetParent(hwnd);

    if (parent)
        MapWindowPoints(NULL, parent, (LPPOINT)rc, 2);

    return 1;
}

static int ButtonGrid_RectAlreadyMatches(
    HWND hwnd,
    int x,
    int y,
    int width,
    int height,
    int *sizeChanged
)
{
    RECT rc;
    int currentWidth;
    int currentHeight;

    if (sizeChanged)
        *sizeChanged = 1;

    if (!ButtonGrid_GetWindowRectInParent(hwnd, &rc))
        return 0;

    currentWidth = rc.right - rc.left;
    currentHeight = rc.bottom - rc.top;

    if (sizeChanged)
    {
        if (currentWidth == width && currentHeight == height)
            *sizeChanged = 0;
        else
            *sizeChanged = 1;
    }

    if (rc.left != x)
        return 0;

    if (rc.top != y)
        return 0;

    if (currentWidth != width)
        return 0;

    if (currentHeight != height)
        return 0;

    return 1;
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
    int sizeChanged;

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

    sizeChanged = 1;

    if (ButtonGrid_RectAlreadyMatches(
            gridHwnd,
            x,
            y,
            width,
            height,
            &sizeChanged
        ))
    {
        return;
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

    if (grid->buttonWidth == buttonWidth &&
        grid->buttonHeight == buttonHeight)
    {
        return;
    }

    grid->buttonWidth = buttonWidth;
    grid->buttonHeight = buttonHeight;

    ButtonGrid_UpdateDpi(grid);
    ButtonGrid_UpdateAllButtonSizes(grid);
    ButtonGrid_Layout(grid);
    ButtonGrid_RedrawAllButtons(grid);

    if (grid->showBorder || grid->showGearIcon)
        InvalidateRect(grid->hwnd, NULL, FALSE);
}

void ButtonGrid_SetSizeMode(
    HWND gridHwnd,
    int sizeMode
)
{
    ButtonGrid *grid;
    int normalized;

    grid = ButtonGrid_Get(gridHwnd);

    if (!grid)
        return;

    normalized = ButtonGrid_NormalizeSizeMode(sizeMode);

    if (grid->sizeMode == normalized)
        return;

    grid->sizeMode = normalized;

    ButtonGrid_UpdateDpi(grid);
    ButtonGrid_UpdateAllButtonSizes(grid);
    ButtonGrid_Layout(grid);
    ButtonGrid_RedrawAllButtons(grid);

    if (grid->showBorder || grid->showGearIcon)
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

    if (grid->horizontalSpacing == horizontalSpacing &&
        grid->verticalSpacing == verticalSpacing)
    {
        return;
    }

    grid->horizontalSpacing = horizontalSpacing;
    grid->verticalSpacing = verticalSpacing;

    ButtonGrid_UpdateDpi(grid);
    ButtonGrid_Layout(grid);

    if (grid->showBorder || grid->showGearIcon)
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

    if (grid->layout == layout)
        return;

    grid->layout = layout;

    ButtonGrid_UpdateDpi(grid);
    ButtonGrid_UpdateAllButtonSizes(grid);
    ButtonGrid_Layout(grid);
    ButtonGrid_RedrawAllButtons(grid);

    if (grid->showBorder || grid->showGearIcon)
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

    if (grid->showBorder || grid->showGearIcon)
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

                if (grid->showBorder || grid->showGearIcon)
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
            int index;

            if (!grid)
                break;

            controlId = LOWORD(wParam);
            notifyCode = HIWORD(wParam);

            if (notifyCode == STN_CLICKED)
            {
                index = controlId - grid->idBase;
                ButtonGrid_ActivateButtonByIndex(grid, index);
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