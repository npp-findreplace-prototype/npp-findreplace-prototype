#include <windows.h>
#include <stdio.h>

#include "console.h"
#include "button_grid.h"
#include "image_loader.h"

#ifndef VK_OEM_PLUS
#define VK_OEM_PLUS 0xBB
#endif

#ifndef VK_OEM_MINUS
#define VK_OEM_MINUS 0xBD
#endif

#define MAIN_WINDOW_CLASS_NAME "SimpleWindowClass"
#define MAIN_WINDOW_TITLE "Simple TCC Window"

#define GRID_MARGIN 12

#define SEARCH_MODE_RADIO_GROUP 1

typedef struct SearchGridButtonDefinition
{
    const char *name;
    const char *text;
    const char *tooltip;
    const char *iconBaseName;

    int behavior;
    int radioGroup;
    int defaultState;
} SearchGridButtonDefinition;

#define SEARCH_GRID_BUTTON(name, text, tooltip, iconBaseName, behavior, radioGroup, defaultState) \
    { name, text, tooltip, iconBaseName, behavior, radioGroup, defaultState }

/*
    Clean, editable button grid definition.

    name:
        Programmatic identifier.
        Icons are searched by this name unless iconBaseName is set.

    text:
        Text drawn on top of the button. Use "" for icon-only buttons.

    tooltip:
        Tooltip text shown on hover.

    iconBaseName:
        NULL means use name.
        Files/resources are searched as:
            iconBaseName_OFF.bmp
            iconBaseName_OFF.png
            iconBaseName_OFF.jpg
            iconBaseName_ON.bmp
            iconBaseName_ON.png
            iconBaseName_ON.jpg

    behavior:
        BUTTON_GRID_BUTTON_RADIO
        BUTTON_GRID_BUTTON_TOGGLE

    radioGroup:
        Radio buttons with the same non-zero radioGroup exclude each other.

    defaultState:
        0 = OFF
        1 = ON
*/

static const SearchGridButtonDefinition SEARCH_GRID_BUTTONS[] =
{
    SEARCH_GRID_BUTTON("LiteralSearch",        "Literal",   "Literal Search",         NULL, BUTTON_GRID_BUTTON_RADIO,  SEARCH_MODE_RADIO_GROUP, 1),
    SEARCH_GRID_BUTTON("EscapedLiteralSearch", "Escaped",   "Escaped Literal Search", NULL, BUTTON_GRID_BUTTON_RADIO,  SEARCH_MODE_RADIO_GROUP, 0),
    SEARCH_GRID_BUTTON("RegExSearch",          "Regex",     "Regex Search",           NULL, BUTTON_GRID_BUTTON_RADIO,  SEARCH_MODE_RADIO_GROUP, 0),
    SEARCH_GRID_BUTTON("SemanticSearch",       "Semantic",  "Semantic Search",        NULL, BUTTON_GRID_BUTTON_RADIO,  SEARCH_MODE_RADIO_GROUP, 0),

    SEARCH_GRID_BUTTON("CaseSensitive",        "Case",      "Case Sensitive",         NULL, BUTTON_GRID_BUTTON_TOGGLE, 0, 0),
    SEARCH_GRID_BUTTON("DiacriticSensitive",   "Diacritic", "Diacritic Sensitive",    NULL, BUTTON_GRID_BUTTON_TOGGLE, 0, 0),
    SEARCH_GRID_BUTTON("DotIncludesNewline",   "Dot NL",    "Dot Includes Newline",   NULL, BUTTON_GRID_BUTTON_TOGGLE, 0, 0),
    SEARCH_GRID_BUTTON("FuzzyLogicSearch",     "Fuzzy",     "Fuzzy Logic Search",     NULL, BUTTON_GRID_BUTTON_TOGGLE, 0, 0),

    SEARCH_GRID_BUTTON("WrapAround",           "Wrap",      "Wrap Around",            NULL, BUTTON_GRID_BUTTON_TOGGLE, 0, 0),
    SEARCH_GRID_BUTTON("WholeWord",            "Word",      "Whole Word",             NULL, BUTTON_GRID_BUTTON_TOGGLE, 0, 0),
    SEARCH_GRID_BUTTON("BooleanSearch",        "Boolean",   "Boolean Search",         NULL, BUTTON_GRID_BUTTON_TOGGLE, 0, 0),
    SEARCH_GRID_BUTTON("Settings",             "Settings",  "Settings",               NULL, BUTTON_GRID_BUTTON_TOGGLE, 0, 0),

    SEARCH_GRID_BUTTON("Reserved13",           "13",        "Reserved 13",            NULL, BUTTON_GRID_BUTTON_TOGGLE, 0, 0),
    SEARCH_GRID_BUTTON("Reserved14",           "14",        "Reserved 14",            NULL, BUTTON_GRID_BUTTON_TOGGLE, 0, 0),
    SEARCH_GRID_BUTTON("Reserved15",           "15",        "Reserved 15",            NULL, BUTTON_GRID_BUTTON_TOGGLE, 0, 0),
    SEARCH_GRID_BUTTON("Reserved16",           "16",        "Reserved 16",            NULL, BUTTON_GRID_BUTTON_TOGGLE, 0, 0)
};

#define SEARCH_GRID_BUTTON_COUNT (sizeof(SEARCH_GRID_BUTTONS) / sizeof(SEARCH_GRID_BUTTONS[0]))

static HWND g_buttonGrid = NULL;
static int g_squareSize = BUTTON_GRID_DEFAULT_BUTTON_WIDTH;

static ButtonGridItemConfig g_searchGridItems[SEARCH_GRID_BUTTON_COUNT];

static void PrepareSearchGridItems(HINSTANCE hInstance)
{
    int i;

    ZeroMemory(g_searchGridItems, sizeof(g_searchGridItems));

    for (i = 0; i < (int)SEARCH_GRID_BUTTON_COUNT; i++)
    {
        const char *iconBaseName;
        int offFailed;
        int onFailed;

        iconBaseName = SEARCH_GRID_BUTTONS[i].iconBaseName;

        if (!iconBaseName)
            iconBaseName = SEARCH_GRID_BUTTONS[i].name;

        offFailed = 0;
        onFailed = 0;

        g_searchGridItems[i].name = SEARCH_GRID_BUTTONS[i].name;
        g_searchGridItems[i].text = SEARCH_GRID_BUTTONS[i].text;
        g_searchGridItems[i].tooltip = SEARCH_GRID_BUTTONS[i].tooltip;

        g_searchGridItems[i].behavior = SEARCH_GRID_BUTTONS[i].behavior;
        g_searchGridItems[i].radioGroup = SEARCH_GRID_BUTTONS[i].radioGroup;
        g_searchGridItems[i].defaultState = SEARCH_GRID_BUTTONS[i].defaultState;

        g_searchGridItems[i].pictureOff = ImageLoader_LoadButtonIcon(
            hInstance,
            iconBaseName,
            "OFF",
            &offFailed
        );

        g_searchGridItems[i].pictureOn = ImageLoader_LoadButtonIcon(
            hInstance,
            iconBaseName,
            "ON",
            &onFailed
        );

        g_searchGridItems[i].ownsPictureOff = g_searchGridItems[i].pictureOff != NULL;
        g_searchGridItems[i].ownsPictureOn = g_searchGridItems[i].pictureOn != NULL;

        g_searchGridItems[i].pictureOffLoadFailed = offFailed;
        g_searchGridItems[i].pictureOnLoadFailed = onFailed;
    }
}

static void ConfigureButtonGrid(ButtonGridConfig *config, HINSTANCE hInstance)
{
    ButtonGrid_GetDefaultConfig(config);

    PrepareSearchGridItems(hInstance);

    config->buttonCount = (int)SEARCH_GRID_BUTTON_COUNT;
    config->items = g_searchGridItems;

    config->buttonWidth = 90;
    config->buttonHeight = 90;

    config->horizontalSpacing = 10;
    config->verticalSpacing = 10;

    config->layout = BUTTON_GRID_LAYOUT_HORIZONTAL;

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

static void SetMainWindowTitle(HWND hwnd)
{
    char title[128];

    wsprintf(
        title,
        "%s - square size %d x %d",
        MAIN_WINDOW_TITLE,
        g_squareSize,
        g_squareSize
    );

    SetWindowText(hwnd, title);
}

static void LayoutMainWindow(HWND hwnd)
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

static void SetSquareSize(HWND hwnd, int newSize)
{
    if (newSize < 20)
        newSize = 20;

    if (newSize > 300)
        newSize = 300;

    g_squareSize = newSize;

    ButtonGrid_SetButtonSize(g_buttonGrid, g_squareSize, g_squareSize);
    SetMainWindowTitle(hwnd);

    printf("Square size changed to %d x %d\n", g_squareSize, g_squareSize);
}

static void OnSquareClicked(const char *controlName)
{
    char msg[160];
    int isOn;

    isOn = ButtonGrid_GetButtonStateByName(g_buttonGrid, controlName);

    if (isOn >= 0)
    {
        wsprintf(
            msg,
            "Clicked: %s is now %s",
            controlName,
            isOn ? "ON" : "OFF"
        );
    }
    else
    {
        wsprintf(msg, "Clicked: %s", controlName);
    }

    AppNotify("Static Click", msg);
}

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_CREATE:
        {
            CREATESTRUCT *cs;
            ButtonGridConfig gridConfig;

            printf("Window created.\n");

            cs = (CREATESTRUCT *)lParam;

            ConfigureButtonGrid(&gridConfig, cs->hInstance);

            g_squareSize = gridConfig.buttonWidth;

            g_buttonGrid = ButtonGrid_CreateEx(
                hwnd,
                cs->hInstance,
                GRID_MARGIN,
                GRID_MARGIN,
                400,
                300,
                &gridConfig,
                OnSquareClicked
            );

            if (!g_buttonGrid)
            {
                AppNotify("Error", "Could not create button grid.");
                return -1;
            }

            LayoutMainWindow(hwnd);
            SetMainWindowTitle(hwnd);

            return 0;
        }

        case WM_SIZE:
        {
            LayoutMainWindow(hwnd);
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

        case WM_DESTROY:
        {
            printf("Window destroyed.\n");
            PostQuitMessage(0);
            return 0;
        }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow
)
{
    WNDCLASS wc;
    HWND hwnd;
    MSG msg;

    Console_Setup();

    if (!ImageLoader_Startup())
        printf("GDI+ image loader unavailable. Falling back to generated button art.\n");

    ZeroMemory(&wc, sizeof(wc));

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = MAIN_WINDOW_CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClass(&wc))
    {
        AppNotify("Error", "Could not register main window class.");
        ImageLoader_Shutdown();
        return 0;
    }

    hwnd = CreateWindowEx(
        0,
        MAIN_WINDOW_CLASS_NAME,
        MAIN_WINDOW_TITLE,
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        800,
        600,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hwnd)
    {
        AppNotify("Error", "CreateWindowEx failed.");
        ImageLoader_Shutdown();
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    printf("Application started.\n");
    printf("Press + or - to change square size.\n");
    printf("Click a square to toggle or select it.\n");

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    printf("Application exiting.\n");

    ImageLoader_Shutdown();

    return 0;
}