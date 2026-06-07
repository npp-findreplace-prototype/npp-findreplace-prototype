#include <windows.h>
#include <stdio.h>

#include "console.h"
#include "button_grid.h"
#include "grid_tester_window.h"

#ifndef VK_OEM_PLUS
#define VK_OEM_PLUS 0xBB
#endif

#ifndef VK_OEM_MINUS
#define VK_OEM_MINUS 0xBD
#endif

#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0
#endif

#ifndef LOGPIXELSX
#define LOGPIXELSX 88
#endif

#define GRID_TESTER_WINDOW_CLASS_NAME "GridTesterWindowClass"
#define GRID_TESTER_WINDOW_TITLE "Grid Tester Window"

#define GRID_MARGIN 12
#define SEARCH_MODE_RADIO_GROUP 1

typedef struct SearchGridButtonDefinition
{
    const char *name;
    const char *action;
    const char *text;
    const char *tooltip;
    const char *iconBaseName;

    int behavior;
    int radioGroup;
    int defaultState;

    int widthOverride;
    int heightOverride;
    int sizeModeOverride;

    int showTextOverride;
} SearchGridButtonDefinition;

#define SEARCH_GRID_BUTTON(name, action, text, tooltip, iconBaseName, behavior, radioGroup, defaultState, widthOverride, heightOverride, sizeModeOverride, showTextOverride) \
    { name, action, text, tooltip, iconBaseName, behavior, radioGroup, defaultState, widthOverride, heightOverride, sizeModeOverride, showTextOverride }

static const SearchGridButtonDefinition SEARCH_GRID_BUTTONS[] =
{
    SEARCH_GRID_BUTTON("LiteralSearch",        NULL, "Literal",   "Literal Search",         NULL, BUTTON_GRID_BUTTON_RADIO,  SEARCH_MODE_RADIO_GROUP, 1, 0, 0, BUTTON_GRID_SIZE_USE_DEFAULT, BUTTON_GRID_TEXT_USE_DEFAULT),
    SEARCH_GRID_BUTTON("EscapedLiteralSearch", NULL, "Escaped",   "Escaped Literal Search", NULL, BUTTON_GRID_BUTTON_RADIO,  SEARCH_MODE_RADIO_GROUP, 0, 0, 0, BUTTON_GRID_SIZE_USE_DEFAULT, BUTTON_GRID_TEXT_USE_DEFAULT),
    SEARCH_GRID_BUTTON("RegExSearch",          NULL, "Regex",     "Regex Search",           NULL, BUTTON_GRID_BUTTON_RADIO,  SEARCH_MODE_RADIO_GROUP, 0, 0, 0, BUTTON_GRID_SIZE_USE_DEFAULT, BUTTON_GRID_TEXT_USE_DEFAULT),
    SEARCH_GRID_BUTTON("SemanticSearch",       NULL, "Semantic",  "Semantic Search",        NULL, BUTTON_GRID_BUTTON_RADIO,  SEARCH_MODE_RADIO_GROUP, 0, 0, 0, BUTTON_GRID_SIZE_USE_DEFAULT, BUTTON_GRID_TEXT_USE_DEFAULT),

    SEARCH_GRID_BUTTON("CaseSensitive",        NULL, "Case",      "Case Sensitive",         NULL, BUTTON_GRID_BUTTON_TOGGLE, 0, 0, 0, 0, BUTTON_GRID_SIZE_USE_DEFAULT, BUTTON_GRID_TEXT_USE_DEFAULT),
    SEARCH_GRID_BUTTON("DiacriticSensitive",   NULL, "Diacritic", "Diacritic Sensitive",    NULL, BUTTON_GRID_BUTTON_TOGGLE, 0, 0, 0, 0, BUTTON_GRID_SIZE_USE_DEFAULT, BUTTON_GRID_TEXT_USE_DEFAULT),
    SEARCH_GRID_BUTTON("DotIncludesNewline",   NULL, "Dot NL",    "Dot Includes Newline",   NULL, BUTTON_GRID_BUTTON_TOGGLE, 0, 0, 0, 0, BUTTON_GRID_SIZE_USE_DEFAULT, BUTTON_GRID_TEXT_USE_DEFAULT),
    SEARCH_GRID_BUTTON("FuzzyLogicSearch",     NULL, "Fuzzy",     "Fuzzy Logic Search",     NULL, BUTTON_GRID_BUTTON_TOGGLE, 0, 0, 0, 0, BUTTON_GRID_SIZE_USE_DEFAULT, BUTTON_GRID_TEXT_USE_DEFAULT),

    SEARCH_GRID_BUTTON("WrapAround",           NULL, "Wrap",      "Wrap Around",            NULL, BUTTON_GRID_BUTTON_TOGGLE, 0, 0, 0, 0, BUTTON_GRID_SIZE_USE_DEFAULT, BUTTON_GRID_TEXT_USE_DEFAULT),
    SEARCH_GRID_BUTTON("WholeWord",            NULL, "Word",      "Whole Word",             NULL, BUTTON_GRID_BUTTON_TOGGLE, 0, 0, 0, 0, BUTTON_GRID_SIZE_USE_DEFAULT, BUTTON_GRID_TEXT_USE_DEFAULT),
    SEARCH_GRID_BUTTON("BooleanSearch",        NULL, "Boolean",   "Boolean Search",         NULL, BUTTON_GRID_BUTTON_TOGGLE, 0, 0, 0, 0, BUTTON_GRID_SIZE_USE_DEFAULT, BUTTON_GRID_TEXT_USE_DEFAULT),
    SEARCH_GRID_BUTTON("Settings",             NULL, "Settings",  "Settings",               NULL, BUTTON_GRID_BUTTON_TOGGLE, 0, 0, 0, 0, BUTTON_GRID_SIZE_USE_DEFAULT, BUTTON_GRID_TEXT_USE_DEFAULT)
};

#define SEARCH_GRID_BUTTON_COUNT (sizeof(SEARCH_GRID_BUTTONS) / sizeof(SEARCH_GRID_BUTTONS[0]))

static HINSTANCE g_hInstance = NULL;
static HWND g_window = NULL;
static HWND g_buttonGrid = NULL;
static int g_squareSize = BUTTON_GRID_DEFAULT_BUTTON_WIDTH;
static ButtonGridItemConfig g_searchGridItems[SEARCH_GRID_BUTTON_COUNT];
static GridTesterWindowClosedCallback g_onClosed = NULL;

static void PrepareSearchGridItems(void)
{
    int i;

    ZeroMemory(g_searchGridItems, sizeof(g_searchGridItems));

    for (i = 0; i < (int)SEARCH_GRID_BUTTON_COUNT; i++)
    {
        const char *iconBaseName;

        iconBaseName = SEARCH_GRID_BUTTONS[i].iconBaseName;

        if (!iconBaseName)
            iconBaseName = SEARCH_GRID_BUTTONS[i].name;

        g_searchGridItems[i].name = SEARCH_GRID_BUTTONS[i].name;
        g_searchGridItems[i].action = SEARCH_GRID_BUTTONS[i].action;
        g_searchGridItems[i].text = SEARCH_GRID_BUTTONS[i].text;
        g_searchGridItems[i].tooltip = SEARCH_GRID_BUTTONS[i].tooltip;
        g_searchGridItems[i].iconBaseName = iconBaseName;

        g_searchGridItems[i].behavior = SEARCH_GRID_BUTTONS[i].behavior;
        g_searchGridItems[i].radioGroup = SEARCH_GRID_BUTTONS[i].radioGroup;
        g_searchGridItems[i].defaultState = SEARCH_GRID_BUTTONS[i].defaultState;

        g_searchGridItems[i].widthOverride = SEARCH_GRID_BUTTONS[i].widthOverride;
        g_searchGridItems[i].heightOverride = SEARCH_GRID_BUTTONS[i].heightOverride;
        g_searchGridItems[i].sizeModeOverride = SEARCH_GRID_BUTTONS[i].sizeModeOverride;
        g_searchGridItems[i].showTextOverride = SEARCH_GRID_BUTTONS[i].showTextOverride;

        g_searchGridItems[i].pictureOff = NULL;
        g_searchGridItems[i].pictureOn = NULL;

        g_searchGridItems[i].ownsPictureOff = 0;
        g_searchGridItems[i].ownsPictureOn = 0;

        g_searchGridItems[i].pictureOffLoadFailed = 0;
        g_searchGridItems[i].pictureOnLoadFailed = 0;
    }
}

static void ConfigureButtonGrid(ButtonGridConfig *config)
{
    ButtonGrid_GetDefaultConfig(config);

    PrepareSearchGridItems();

    config->buttonCount = (int)SEARCH_GRID_BUTTON_COUNT;
    config->items = g_searchGridItems;

    config->buttonWidth = 90;
    config->buttonHeight = 90;

    config->horizontalSpacing = 10;
    config->verticalSpacing = 10;

    config->layout = BUTTON_GRID_LAYOUT_HORIZONTAL;
    config->sizeMode = BUTTON_GRID_SIZE_MATCH_IMAGE_ASPECT_BY_LAYOUT;

    config->showText = 0;
    config->hidePartialButtons = 1;
    config->resizeInLayoutSteps = 0;
    config->settingsWheelScrub = 0;

    config->dpiScaleEnabled = 1;

    config->contentAlignment = BUTTON_GRID_ALIGN_TOP_LEFT;
    config->contentAlignX = 0;
    config->contentAlignY = 0;
    config->contentAlignPercentX = 50;
    config->contentAlignPercentY = 50;

    config->themeName = BUTTON_GRID_DEFAULT_THEME_NAME;
    config->allowThemeSelection = 1;

    config->showBorder = 1;
    config->showBorderTitle = 1;
    config->borderTitle = "Search Options";
    config->borderPadding = 12;
    config->borderTitleHeight = 22;
    config->borderTitlePadding = 6;
    config->borderTitleFontSize = 0;
    config->borderTitleTransparent = 1;
    config->borderTitleAutoBackColor = 1;
    config->borderStyle = BUTTON_GRID_BORDER_STYLE_ROUNDED;
    config->borderThickness = 1;
    config->borderCornerRadius = 10;
    config->borderColor = RGB(96, 96, 96);
    config->borderLightColor = RGB(255, 255, 255);
    config->borderShadowColor = RGB(128, 128, 128);
    config->borderTitleColor = RGB(0, 0, 0);
    config->borderTitleBackColor = RGB(240, 240, 240);

    config->showGearIcon = 1;
    config->gearCorner = BUTTON_GRID_GEAR_CORNER_TOP_RIGHT;
    config->gearSize = 24;
    config->gearMargin = 8;
    config->gearColor = RGB(60, 60, 60);
    config->gearBackColor = RGB(245, 245, 245);
    config->gearBorderColor = RGB(120, 120, 120);

    config->buttonBackMode = BUTTON_GRID_BUTTON_BACK_TRANSPARENT;
    config->showButtonBorder = 0;
    config->buttonBorderThickness = 1;
    config->buttonBorderColor = RGB(0, 0, 0);

    config->backColor = RGB(192, 192, 192);
    config->foreColor = RGB(0, 0, 0);

    config->usePictures = 1;
    config->toggleOnClick = 1;
    config->defaultState = 0;
    config->stretchPictures = 1;

    config->generatedOffPictureColor = RGB(150, 150, 150);
    config->generatedOnPictureColor = RGB(80, 190, 80);
    config->generatedErrorPictureColor = RGB(190, 100, 100);
}

static void SetGridTesterWindowTitle(HWND hwnd)
{
    char title[256];

    wsprintf(
        title,
        "%s - square size %d x %d",
        GRID_TESTER_WINDOW_TITLE,
        g_squareSize,
        g_squareSize
    );

    SetWindowText(hwnd, title);
}

static void LayoutGridTesterWindow(HWND hwnd)
{
    RECT rc;
    int x;
    int y;
    int w;
    int h;

    if (!g_buttonGrid)
        return;

    GetClientRect(hwnd, &rc);

    x = GRID_MARGIN;
    y = GRID_MARGIN;
    w = (rc.right - rc.left) - (GRID_MARGIN * 2);
    h = (rc.bottom - rc.top) - (GRID_MARGIN * 2);

    if (w < 1)
        w = 1;

    if (h < 1)
        h = 1;

    ButtonGrid_SetRect(g_buttonGrid, x, y, w, h);
}

static void OnGridButtonClicked(const char *actionName)
{
    int isOn;

    if (!g_buttonGrid || !actionName)
        return;

    isOn = ButtonGrid_GetButtonStateByAction(g_buttonGrid, actionName);

    printf(
        "Grid tester clicked: %s is now %s\n",
        actionName,
        isOn ? "ON" : "OFF"
    );
}

static int CreateSearchGrid(HWND hwnd)
{
    ButtonGridConfig gridConfig;

    ConfigureButtonGrid(&gridConfig);

    g_squareSize = gridConfig.buttonWidth;

    g_buttonGrid = ButtonGrid_CreateEx(
        hwnd,
        g_hInstance,
        GRID_MARGIN,
        GRID_MARGIN,
        400,
        300,
        &gridConfig,
        OnGridButtonClicked
    );

    if (!g_buttonGrid)
        return 0;

    LayoutGridTesterWindow(hwnd);
    SetGridTesterWindowTitle(hwnd);

    return 1;
}

static void SetSquareSize(HWND hwnd, int newSize)
{
    if (newSize < 20)
        newSize = 20;

    if (newSize > 300)
        newSize = 300;

    if (g_squareSize == newSize)
        return;

    g_squareSize = newSize;

    ButtonGrid_SetButtonSize(g_buttonGrid, g_squareSize, g_squareSize);
    SetGridTesterWindowTitle(hwnd);

    printf("Grid tester square size changed to %d x %d\n", g_squareSize, g_squareSize);
}

static void HandleDpiChanged(HWND hwnd, LPARAM lParam)
{
    RECT *suggested;

    suggested = (RECT *)lParam;

    if (suggested)
    {
        SetWindowPos(
            hwnd,
            NULL,
            suggested->left,
            suggested->top,
            suggested->right - suggested->left,
            suggested->bottom - suggested->top,
            SWP_NOZORDER | SWP_NOACTIVATE
        );
    }

    LayoutGridTesterWindow(hwnd);

    if (g_buttonGrid)
        ButtonGrid_Relayout(g_buttonGrid);
}

static LRESULT CALLBACK GridTesterWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_CREATE:
        {
            CREATESTRUCT *cs;

            cs = (CREATESTRUCT *)lParam;
            g_hInstance = cs->hInstance;
            g_window = hwnd;

            printf("Grid tester window created.\n");

            if (!CreateSearchGrid(hwnd))
            {
                AppNotify("Error", "Could not create grid tester button grid.");
                return -1;
            }

            return 0;
        }

        case WM_SIZE:
        {
            LayoutGridTesterWindow(hwnd);
            return 0;
        }

        case WM_DPICHANGED:
        {
            HandleDpiChanged(hwnd, lParam);
            return 0;
        }

        case WM_KEYDOWN:
        {
            if (wParam == VK_ADD || wParam == VK_OEM_PLUS)
            {
                SetSquareSize(hwnd, g_squareSize + 10);
                return 0;
            }

            if (wParam == VK_SUBTRACT || wParam == VK_OEM_MINUS)
            {
                SetSquareSize(hwnd, g_squareSize - 10);
                return 0;
            }

            break;
        }

        case WM_CLOSE:
        {
            DestroyWindow(hwnd);
            return 0;
        }

        case WM_NCDESTROY:
        {
            GridTesterWindowClosedCallback callback;

            printf("Grid tester window destroyed.\n");

            g_window = NULL;
            g_buttonGrid = NULL;

            callback = g_onClosed;
            g_onClosed = NULL;

            if (callback)
                callback();

            return 0;
        }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

static BOOL GridTesterWindow_RegisterClass(HINSTANCE hInstance)
{
    static int registered = 0;
    WNDCLASS wc;

    if (registered)
        return TRUE;

    ZeroMemory(&wc, sizeof(wc));

    wc.lpfnWndProc = GridTesterWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = GRID_TESTER_WINDOW_CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClass(&wc))
        return FALSE;

    registered = 1;
    return TRUE;
}

HWND GridTesterWindow_Show(
    HINSTANCE hInstance,
    GridTesterWindowClosedCallback onClosed
)
{
    g_onClosed = onClosed;

    if (g_window && IsWindow(g_window))
    {
        ShowWindow(g_window, SW_SHOW);
        SetForegroundWindow(g_window);
        return g_window;
    }

    if (!GridTesterWindow_RegisterClass(hInstance))
        return NULL;

    g_window = CreateWindowEx(
        0,
        GRID_TESTER_WINDOW_CLASS_NAME,
        GRID_TESTER_WINDOW_TITLE,
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        1000,
        760,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!g_window)
        return NULL;

    ShowWindow(g_window, SW_SHOW);
    UpdateWindow(g_window);

    return g_window;
}

void GridTesterWindow_Close(void)
{
    if (g_window && IsWindow(g_window))
        DestroyWindow(g_window);
}

HWND GridTesterWindow_GetHwnd(void)
{
    return g_window;
}

int GridTesterWindow_IsOpen(void)
{
    return g_window && IsWindow(g_window);
}