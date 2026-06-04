#include <windows.h>
#include <stdio.h>

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

static HWND g_buttonGrid = NULL;
static int g_squareSize = BUTTON_GRID_DEFAULT_BUTTON_WIDTH;

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
    char msg[128];

    wsprintf(msg, "Clicked: %s", controlName);
    AppNotify("Static Click", msg);
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

            if (!ButtonGrid_RegisterClass(cs->hInstance))
            {
                AppNotify("Error", "Could not register button grid class.");
                return -1;
            }

            g_buttonGrid = ButtonGrid_Create(
                hwnd,
                cs->hInstance,
                GRID_MARGIN,
                GRID_MARGIN,
                400,
                300,
                OnSquareClicked
            );

            if (!g_buttonGrid)
            {
                AppNotify("Error", "Could not create button grid.");
                return -1;
            }

            ButtonGrid_SetButtonSize(g_buttonGrid, g_squareSize, g_squareSize);
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

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    printf("Application exiting.\n");

    return 0;
}