#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "console.h"
#include "button_grid.h"

#ifndef VK_OEM_PLUS
#define VK_OEM_PLUS 0xBB
#endif

#ifndef VK_OEM_MINUS
#define VK_OEM_MINUS 0xBD
#endif

#define MAIN_WINDOW_CLASS_NAME "SimpleWindowClass"
#define MAIN_WINDOW_TITLE "Simple TCC Window"

#define GRID_MARGIN 12

#define APP_BUTTON_NAME_PREFIX "mybutton_"

static HWND g_buttonGrid = NULL;
static int g_squareSize = BUTTON_GRID_DEFAULT_BUTTON_WIDTH;

static int GetButtonNumberFromControlName(const char *controlName)
{
    int i;
    int prefixLen;

    if (!controlName)
        return -1;

    prefixLen = lstrlen(APP_BUTTON_NAME_PREFIX);

    for (i = 0; i < prefixLen; i++)
    {
        if (controlName[i] != APP_BUTTON_NAME_PREFIX[i])
            return -1;
    }

    return atoi(controlName + prefixLen);
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
    int buttonNumber;
    int isOn;

    buttonNumber = GetButtonNumberFromControlName(controlName);
    isOn = ButtonGrid_GetButtonStateByNumber(g_buttonGrid, buttonNumber);

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

static void ConfigureButtonGrid(ButtonGridConfig *config)
{
    ButtonGrid_GetDefaultConfig(config);

    config->buttonCount = 16;

    config->buttonWidth = 90;
    config->buttonHeight = 90;

    config->horizontalSpacing = 10;
    config->verticalSpacing = 10;

    config->layout = BUTTON_GRID_LAYOUT_HORIZONTAL;

    config->namePrefix = APP_BUTTON_NAME_PREFIX;
    config->textFormat = "%d";
    config->clickIdentifierFormat = "%s";

    config->backColor = RGB(192, 192, 192);
    config->foreColor = RGB(0, 0, 0);

    config->usePictures = 1;
    config->toggleOnClick = 1;
    config->defaultState = 0;
    config->stretchPictures = 1;

    /*
        No real pictures configured yet.

        OFF:
            pictureOff = NULL
            pictureOffLoadFailed = 0
            result = plain gray fallback

        ON:
            pictureOn = NULL
            pictureOnLoadFailed = 0
            result = generated green/check fallback

        If later you try to load a file and it fails, set the matching
        pictureXLoadFailed value to 1. Then that state will show an X.
    */
    config->pictureOff = NULL;
    config->pictureOn = NULL;

    config->pictureOffLoadFailed = 0;
    config->pictureOnLoadFailed = 0;

    config->generatedOffPictureColor = RGB(150, 150, 150);
    config->generatedOnPictureColor = RGB(80, 190, 80);
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

            ConfigureButtonGrid(&gridConfig);

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

    ZeroMemory(&wc, sizeof(wc));

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = MAIN_WINDOW_CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClass(&wc))
    {
        AppNotify("Error", "Could not register main window class.");
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
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    printf("Application started.\n");
    printf("Press + or - to change square size.\n");
    printf("Click a square to toggle ON/OFF picture state.\n");

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    printf("Application exiting.\n");

    return 0;
}