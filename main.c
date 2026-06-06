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

#ifndef VK_F6
#define VK_F6 0x75
#endif

#ifndef VK_F7
#define VK_F7 0x76
#endif

#define MAIN_WINDOW_CLASS_NAME "SimpleWindowClass"
#define MAIN_WINDOW_TITLE "Simple TCC Window"

#define GRID_MARGIN 12
#define SEARCH_MODE_RADIO_GROUP 1

#define MAX_THEMES 64
#define THEME_NAME_SIZE 64

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
} SearchGridButtonDefinition;

typedef struct ThemeList
{
    char names[MAX_THEMES][THEME_NAME_SIZE];
    int count;
    int currentIndex;
} ThemeList;

#define SEARCH_GRID_BUTTON(name, action, text, tooltip, iconBaseName, behavior, radioGroup, defaultState, widthOverride, heightOverride, sizeModeOverride) \
    { name, action, text, tooltip, iconBaseName, behavior, radioGroup, defaultState, widthOverride, heightOverride, sizeModeOverride }

/*
    Search grid definition.

    name:
        Internal button name.
        Also used as the default icon base name.

    action:
        Identifier passed to the click callback.
        NULL means use name.

    text:
        Text drawn on the button.
        "" means icon-only / no text.

    tooltip:
        Hover tooltip.

    iconBaseName:
        NULL means use name.
        Loader searches:
            themes\<ThemeName>\<iconBaseName>_OFF.bmp/png/jpg
            themes\<ThemeName>\<iconBaseName>_ON.bmp/png/jpg
            embedded theme resources
            default files beside exe
            default embedded resources

    behavior:
        BUTTON_GRID_BUTTON_RADIO
        BUTTON_GRID_BUTTON_TOGGLE
        BUTTON_GRID_BUTTON_DISABLED

    radioGroup:
        Radio buttons with the same non-zero group exclude each other.

    defaultState:
        0 = OFF
        1 = ON

    widthOverride / heightOverride:
        0 means use grid default.

    sizeModeOverride:
        BUTTON_GRID_SIZE_USE_DEFAULT
        BUTTON_GRID_SIZE_FIXED
        BUTTON_GRID_SIZE_MATCH_IMAGE_SIZE
        BUTTON_GRID_SIZE_MATCH_IMAGE_ASPECT_HORIZONTAL
        BUTTON_GRID_SIZE_MATCH_IMAGE_ASPECT_VERTICAL
*/

static const SearchGridButtonDefinition SEARCH_GRID_BUTTONS[] =
{
    SEARCH_GRID_BUTTON("LiteralSearch",        NULL, "Literal",   "Literal Search",         NULL, BUTTON_GRID_BUTTON_RADIO,  SEARCH_MODE_RADIO_GROUP, 1, 0, 0, BUTTON_GRID_SIZE_USE_DEFAULT),
    SEARCH_GRID_BUTTON("EscapedLiteralSearch", NULL, "Escaped",   "Escaped Literal Search", NULL, BUTTON_GRID_BUTTON_RADIO,  SEARCH_MODE_RADIO_GROUP, 0, 0, 0, BUTTON_GRID_SIZE_USE_DEFAULT),
    SEARCH_GRID_BUTTON("RegExSearch",          NULL, "Regex",     "Regex Search",           NULL, BUTTON_GRID_BUTTON_RADIO,  SEARCH_MODE_RADIO_GROUP, 0, 0, 0, BUTTON_GRID_SIZE_USE_DEFAULT),
    SEARCH_GRID_BUTTON("SemanticSearch",       NULL, "Semantic",  "Semantic Search",        NULL, BUTTON_GRID_BUTTON_RADIO,  SEARCH_MODE_RADIO_GROUP, 0, 0, 0, BUTTON_GRID_SIZE_USE_DEFAULT),

    SEARCH_GRID_BUTTON("CaseSensitive",        NULL, "Case",      "Case Sensitive",         NULL, BUTTON_GRID_BUTTON_TOGGLE, 0, 0, 0, 0, BUTTON_GRID_SIZE_USE_DEFAULT),
    SEARCH_GRID_BUTTON("DiacriticSensitive",   NULL, "Diacritic", "Diacritic Sensitive",    NULL, BUTTON_GRID_BUTTON_TOGGLE, 0, 0, 0, 0, BUTTON_GRID_SIZE_USE_DEFAULT),
    SEARCH_GRID_BUTTON("DotIncludesNewline",   NULL, "Dot NL",    "Dot Includes Newline",   NULL, BUTTON_GRID_BUTTON_TOGGLE, 0, 0, 0, 0, BUTTON_GRID_SIZE_USE_DEFAULT),
    SEARCH_GRID_BUTTON("FuzzyLogicSearch",     NULL, "Fuzzy",     "Fuzzy Logic Search",     NULL, BUTTON_GRID_BUTTON_TOGGLE, 0, 0, 0, 0, BUTTON_GRID_SIZE_USE_DEFAULT),

    SEARCH_GRID_BUTTON("WrapAround",           NULL, "Wrap",      "Wrap Around",            NULL, BUTTON_GRID_BUTTON_TOGGLE, 0, 0, 0, 0, BUTTON_GRID_SIZE_USE_DEFAULT),
    SEARCH_GRID_BUTTON("WholeWord",            NULL, "Word",      "Whole Word",             NULL, BUTTON_GRID_BUTTON_TOGGLE, 0, 0, 0, 0, BUTTON_GRID_SIZE_USE_DEFAULT),
    SEARCH_GRID_BUTTON("BooleanSearch",        NULL, "Boolean",   "Boolean Search",         NULL, BUTTON_GRID_BUTTON_TOGGLE, 0, 0, 0, 0, BUTTON_GRID_SIZE_USE_DEFAULT),
    SEARCH_GRID_BUTTON("Settings",             NULL, "Settings",  "Settings",               NULL, BUTTON_GRID_BUTTON_TOGGLE, 0, 0, 0, 0, BUTTON_GRID_SIZE_USE_DEFAULT)
};

#define SEARCH_GRID_BUTTON_COUNT (sizeof(SEARCH_GRID_BUTTONS) / sizeof(SEARCH_GRID_BUTTONS[0]))

static const char *APP_BUILT_IN_THEMES[] =
{
    "Default"
};

#define APP_BUILT_IN_THEME_COUNT (sizeof(APP_BUILT_IN_THEMES) / sizeof(APP_BUILT_IN_THEMES[0]))

static HINSTANCE g_hInstance = NULL;
static HWND g_buttonGrid = NULL;

static int g_squareSize = BUTTON_GRID_DEFAULT_BUTTON_WIDTH;

static ThemeList g_themes;
static ButtonGridItemConfig g_searchGridItems[SEARCH_GRID_BUTTON_COUNT];

static void GetExeDirectory(char *buffer, int bufferSize)
{
    int len;
    int i;

    if (!buffer || bufferSize <= 0)
        return;

    buffer[0] = '\0';

    GetModuleFileName(NULL, buffer, bufferSize);

    len = lstrlen(buffer);

    for (i = len - 1; i >= 0; i--)
    {
        if (buffer[i] == '\\' || buffer[i] == '/')
        {
            buffer[i] = '\0';
            return;
        }
    }

    lstrcpy(buffer, ".");
}

static int SameText(const char *a, const char *b)
{
    if (!a || !b)
        return 0;

    return lstrcmpi(a, b) == 0;
}

static void ThemeList_Add(ThemeList *list, const char *name)
{
    int i;

    if (!list || !name || !name[0])
        return;

    for (i = 0; i < list->count; i++)
    {
        if (SameText(list->names[i], name))
            return;
    }

    if (list->count >= MAX_THEMES)
        return;

    lstrcpyn(list->names[list->count], name, THEME_NAME_SIZE);
    list->names[list->count][THEME_NAME_SIZE - 1] = '\0';
    list->count++;
}

static void ThemeList_Rebuild(ThemeList *list)
{
    char exeDir[MAX_PATH];
    char searchPath[MAX_PATH];
    WIN32_FIND_DATA findData;
    HANDLE findHandle;
    int i;

    if (!list)
        return;

    ZeroMemory(list, sizeof(ThemeList));

    for (i = 0; i < (int)APP_BUILT_IN_THEME_COUNT; i++)
        ThemeList_Add(list, APP_BUILT_IN_THEMES[i]);

    GetExeDirectory(exeDir, MAX_PATH);

    wsprintf(searchPath, "%s\\themes\\*", exeDir);

    findHandle = FindFirstFile(searchPath, &findData);

    if (findHandle != INVALID_HANDLE_VALUE)
    {
        do
        {
            if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
                lstrcmp(findData.cFileName, ".") != 0 &&
                lstrcmp(findData.cFileName, "..") != 0)
            {
                ThemeList_Add(list, findData.cFileName);
            }
        }
        while (FindNextFile(findHandle, &findData));

        FindClose(findHandle);
    }

    if (list->count < 1)
        ThemeList_Add(list, "Default");

    list->currentIndex = 0;
}

static const char *ThemeList_GetCurrentDisplayName(ThemeList *list)
{
    if (!list || list->count < 1)
        return "Default";

    return list->names[list->currentIndex];
}

static const char *ThemeList_GetCurrentThemeName(ThemeList *list)
{
    const char *name;

    name = ThemeList_GetCurrentDisplayName(list);

    if (SameText(name, "Default"))
        return NULL;

    return name;
}

static void ThemeList_Cycle(ThemeList *list, int direction)
{
    if (!list || list->count < 1)
        return;

    list->currentIndex += direction;

    while (list->currentIndex < 0)
        list->currentIndex += list->count;

    while (list->currentIndex >= list->count)
        list->currentIndex -= list->count;
}

static void PrepareSearchGridItems(HINSTANCE hInstance, const char *themeName)
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
        g_searchGridItems[i].action = SEARCH_GRID_BUTTONS[i].action;
        g_searchGridItems[i].text = SEARCH_GRID_BUTTONS[i].text;
        g_searchGridItems[i].tooltip = SEARCH_GRID_BUTTONS[i].tooltip;

        g_searchGridItems[i].behavior = SEARCH_GRID_BUTTONS[i].behavior;
        g_searchGridItems[i].radioGroup = SEARCH_GRID_BUTTONS[i].radioGroup;
        g_searchGridItems[i].defaultState = SEARCH_GRID_BUTTONS[i].defaultState;

        g_searchGridItems[i].widthOverride = SEARCH_GRID_BUTTONS[i].widthOverride;
        g_searchGridItems[i].heightOverride = SEARCH_GRID_BUTTONS[i].heightOverride;
        g_searchGridItems[i].sizeModeOverride = SEARCH_GRID_BUTTONS[i].sizeModeOverride;

        g_searchGridItems[i].pictureOff = ImageLoader_LoadButtonIcon(
            hInstance,
            themeName,
            iconBaseName,
            "OFF",
            &offFailed
        );

        g_searchGridItems[i].pictureOn = ImageLoader_LoadButtonIcon(
            hInstance,
            themeName,
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
    const char *themeName;

    ButtonGrid_GetDefaultConfig(config);

    themeName = ThemeList_GetCurrentThemeName(&g_themes);

    PrepareSearchGridItems(hInstance, themeName);

    config->buttonCount = (int)SEARCH_GRID_BUTTON_COUNT;
    config->items = g_searchGridItems;

    config->buttonWidth = 90;
    config->buttonHeight = 90;

    config->horizontalSpacing = 10;
    config->verticalSpacing = 10;

    config->layout = BUTTON_GRID_LAYOUT_HORIZONTAL;
    config->sizeMode = BUTTON_GRID_SIZE_FIXED;

    config->showBorder = 1;
    config->borderTitle = "Search Options";
    config->borderPadding = 10;
    config->borderTitleHeight = 22;
    config->borderColor = RGB(0, 0, 0);
    config->borderTitleColor = RGB(0, 0, 0);

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
    char title[256];

    wsprintf(
        title,
        "%s - square size %d x %d - theme: %s",
        MAIN_WINDOW_TITLE,
        g_squareSize,
        g_squareSize,
        ThemeList_GetCurrentDisplayName(&g_themes)
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

static void OnSquareClicked(const char *actionName)
{
    int isOn;

    isOn = ButtonGrid_GetButtonStateByAction(g_buttonGrid, actionName);

    printf(
        "Clicked: %s is now %s\n",
        actionName,
        isOn ? "ON" : "OFF"
    );
}

static void DestroySearchGrid(void)
{
    if (g_buttonGrid)
    {
        DestroyWindow(g_buttonGrid);
        g_buttonGrid = NULL;
    }
}

static int RecreateSearchGrid(HWND hwnd)
{
    ButtonGridConfig gridConfig;

    DestroySearchGrid();

    ConfigureButtonGrid(&gridConfig, g_hInstance);

    g_squareSize = gridConfig.buttonWidth;

    g_buttonGrid = ButtonGrid_CreateEx(
        hwnd,
        g_hInstance,
        GRID_MARGIN,
        GRID_MARGIN,
        400,
        300,
        &gridConfig,
        OnSquareClicked
    );

    if (!g_buttonGrid)
        return 0;

    LayoutMainWindow(hwnd);
    SetMainWindowTitle(hwnd);

    return 1;
}

static void CycleTheme(HWND hwnd, int direction)
{
    ThemeList_Cycle(&g_themes, direction);

    printf("Theme changed to: %s\n", ThemeList_GetCurrentDisplayName(&g_themes));

    if (!RecreateSearchGrid(hwnd))
        AppNotify("Error", "Could not recreate button grid for theme.");
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

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_CREATE:
        {
            CREATESTRUCT *cs;

            printf("Window created.\n");

            cs = (CREATESTRUCT *)lParam;
            g_hInstance = cs->hInstance;

            ThemeList_Rebuild(&g_themes);

            if (!RecreateSearchGrid(hwnd))
            {
                AppNotify("Error", "Could not create button grid.");
                return -1;
            }

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

            if (wParam == VK_F6)
            {
                CycleTheme(hwnd, -1);
                return 0;
            }

            if (wParam == VK_F7)
            {
                CycleTheme(hwnd, 1);
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
    printf("Press + or - to change default square size.\n");
    printf("Press F6 / F7 to cycle themes.\n");
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