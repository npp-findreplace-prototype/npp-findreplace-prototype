#include "main_internal.h"

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow
)
{
    HWND hwnd;
    MSG msg;

    (void)hPrevInstance;
    (void)lpCmdLine;

    Main_EnableDpiAwareness();

    Console_Setup();

    printf("%s\n", g_mainDpiAwarenessStatus);

    if (!ImageLoader_Startup())
        printf("GDI+ image loader unavailable. Falling back to generated button art.\n");

    if (!MainWindow_RegisterClass(hInstance))
    {
        AppNotify("Error", "Could not register main launcher window class.");
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
        390,
        420,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hwnd)
    {
        AppNotify("Error", "Could not create main launcher window.");
        ImageLoader_Shutdown();
        return 0;
    }

    g_mainWindow = hwnd;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    Debug_Log("App", "Startup", "Application started.");

    printf("Application started.\n");
    printf("Use the launcher buttons to show or hide test windows.\n");

    Main_UpdateButtons();

    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (Main_IsDialogMessageForOpenWindow(&msg))
            continue;

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    Debug_Log("App", "Shutdown", "Application exiting.");

    printf("Application exiting.\n");

    ImageLoader_Shutdown();

    return 0;
}