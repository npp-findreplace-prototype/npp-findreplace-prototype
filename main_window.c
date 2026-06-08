#include "main_internal.h"

static LRESULT CALLBACK MainWindowProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam
)
{
    switch (msg)
    {
        case WM_CREATE:
        {
            CREATESTRUCT *cs;

            cs = (CREATESTRUCT *)lParam;

            g_main_hInstance = cs->hInstance;
            g_mainWindow = hwnd;

            Main_CreateButtons(hwnd);
            Main_LayoutButtons(hwnd);
            Main_RegisterDebugVariables();

            printf("Main launcher window created.\n");
            Debug_Log("Main", "WindowCreate", "Main launcher window created.");

            return 0;
        }

        case WM_SIZE:
        {
            Main_LayoutButtons(hwnd);
            return 0;
        }

        case WM_ACTIVATE:
        case WM_SETFOCUS:
        {
            Main_UpdateButtons();
            break;
        }

        case WM_COMMAND:
        {
            int controlId;
            int notifyCode;

            controlId = LOWORD(wParam);
            notifyCode = HIWORD(wParam);

            if (notifyCode == BN_CLICKED)
            {
                if (controlId == ID_SHOW_GRID_TESTER_BUTTON)
                {
                    Main_ToggleGridTesterWindow();
                    return 0;
                }

                if (controlId == ID_SHOW_GRID_TESTER_INI_BUTTON)
                {
                    Main_ToggleGridTesterIniWindow();
                    return 0;
                }

                if (controlId == ID_SHOW_NEW_TEST_LAYOUT_BUTTON)
                {
                    Main_ToggleNewTestLayoutWindow();
                    return 0;
                }

                if (controlId == ID_SHOW_FIND_REPLACE_DIALOG_BUTTON)
                {
                    Main_ToggleFindReplaceDialogWindow();
                    return 0;
                }

                if (controlId == ID_SHOW_NPP_MOCKUP_BUTTON)
                {
                    Main_ToggleNppMockupWindow();
                    return 0;
                }

                if (controlId == ID_SHOW_DEBUG_WINDOW_BUTTON)
                {
                    Main_ToggleDebugWindow();
                    return 0;
                }

                if (controlId == ID_SHOW_CONSOLE_BUTTON)
                {
                    Main_ToggleConsoleWindow();
                    return 0;
                }
            }

            break;
        }

        case WM_DESTROY:
        {
            Debug_Log("Main", "WindowDestroy", "Main launcher window destroyed.");

            Main_CloseChildWindows();

            g_mainWindow = NULL;

            printf("Main launcher window destroyed.\n");

            PostQuitMessage(0);
            return 0;
        }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

BOOL MainWindow_RegisterClass(HINSTANCE hInstance)
{
    WNDCLASS wc;

    ZeroMemory(&wc, sizeof(wc));

    wc.lpfnWndProc = MainWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = MAIN_WINDOW_CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (RegisterClass(&wc))
        return TRUE;

    if (GetLastError() == ERROR_CLASS_ALREADY_EXISTS)
        return TRUE;

    return FALSE;
}

int Main_IsDialogMessageForOpenWindow(MSG *msg)
{
    HWND hwnd;

    hwnd = GridTesterWindow_GetHwnd();

    if (hwnd && IsWindow(hwnd) && IsDialogMessage(hwnd, msg))
        return 1;

    hwnd = GridTesterIniWindow_GetHwnd();

    if (hwnd && IsWindow(hwnd) && IsDialogMessage(hwnd, msg))
        return 1;

    hwnd = NewTestLayoutWindow_GetHwnd();

    if (hwnd && IsWindow(hwnd) && IsDialogMessage(hwnd, msg))
        return 1;

    hwnd = FindReplaceDialogWindow_GetHwnd();

    if (hwnd && IsWindow(hwnd) && IsDialogMessage(hwnd, msg))
        return 1;

    hwnd = NppMockupWindow_GetHwnd();

    if (hwnd && IsWindow(hwnd) && IsDialogMessage(hwnd, msg))
        return 1;

    hwnd = DebugWindow_GetHwnd();

    if (hwnd && IsWindow(hwnd) && IsDialogMessage(hwnd, msg))
        return 1;

    hwnd = g_mainWindow;

    if (hwnd && IsWindow(hwnd) && IsDialogMessage(hwnd, msg))
        return 1;

    return 0;
}