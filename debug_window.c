#include "debug_window_internal.h"

HWND g_window = NULL;
HWND g_updateButton = NULL;
HWND g_loggingButton = NULL;
HWND g_clearLogButton = NULL;
HWND g_dumpFilterEdit = NULL;
HWND g_dumpEdit = NULL;

HWND g_availableList = NULL;
HWND g_addVariableButton = NULL;
HWND g_removeVariableButton = NULL;
HWND g_showAllVariablesButton = NULL;
HWND g_hideAllVariablesButton = NULL;

HWND g_logFilterEdit = NULL;
HWND g_logEdit = NULL;

HINSTANCE g_hInstance = NULL;
DebugWindowClosedCallback g_onClosed = NULL;

HMODULE g_richEditModule = NULL;
int g_logIsRichEdit = 0;

int g_updatesEnabled = 1;
int g_loggingEnabled = 1;
int g_controlsUpdating = 0;

DebugVariable g_variables[DEBUG_MAX_VARIABLES];
int g_variableCount = 0;

DebugLogEntry g_logEntries[DEBUG_MAX_LOG_ENTRIES];
int g_logCount = 0;

static LRESULT CALLBACK DebugWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_CREATE:
        {
            g_window = hwnd;

            Debug_CreateControls(hwnd);
            Debug_LayoutControls(hwnd);
            Debug_UpdateDumpView();
            Debug_RebuildLogView();

            SetTimer(hwnd, DEBUG_TIMER_ID, DEBUG_TIMER_MS, NULL);

            Debug_Log("Debug", "WindowCreate", "Debug window created.");

            return 0;
        }

        case WM_SIZE:
        {
            Debug_LayoutControls(hwnd);
            return 0;
        }

        case WM_TIMER:
        {
            if (wParam == DEBUG_TIMER_ID)
            {
                Debug_UpdateDumpView();
                return 0;
            }

            break;
        }

        case WM_COMMAND:
        {
            int controlId;
            int notifyCode;

            controlId = LOWORD(wParam);
            notifyCode = HIWORD(wParam);

            if (controlId == ID_DEBUG_UPDATE_BUTTON && notifyCode == BN_CLICKED)
            {
                g_updatesEnabled = !g_updatesEnabled;
                Debug_UpdateButtons();
                Debug_Log("Debug", "ToggleUpdates", "%s", g_updatesEnabled ? "enabled" : "disabled");
                Debug_UpdateDumpView();
                return 0;
            }

            if (controlId == ID_DEBUG_LOGGING_BUTTON && notifyCode == BN_CLICKED)
            {
                if (g_loggingEnabled)
                {
                    Debug_Log("Debug", "ToggleLogging", "disabled");
                    g_loggingEnabled = 0;
                }
                else
                {
                    g_loggingEnabled = 1;
                    Debug_Log("Debug", "ToggleLogging", "enabled");
                }

                Debug_UpdateButtons();
                return 0;
            }

            if (controlId == ID_DEBUG_CLEAR_LOG_BUTTON && notifyCode == BN_CLICKED)
            {
                Debug_ClearLog();
                return 0;
            }

            if (controlId == ID_DEBUG_ADD_VARIABLE_BUTTON && notifyCode == BN_CLICKED)
            {
                Debug_AddSelectedAvailableVariable();
                return 0;
            }

            if (controlId == ID_DEBUG_REMOVE_VARIABLE_BUTTON && notifyCode == BN_CLICKED)
            {
                Debug_RemoveSelectedAvailableVariable();
                return 0;
            }

            if (controlId == ID_DEBUG_SHOW_ALL_VARIABLES_BUTTON && notifyCode == BN_CLICKED)
            {
                Debug_SetAllVariablesActive(1);
                Debug_Log("Debug", "ShowAllVariables", "All variables activated.");
                Debug_UpdateDumpView();
                return 0;
            }

            if (controlId == ID_DEBUG_HIDE_ALL_VARIABLES_BUTTON && notifyCode == BN_CLICKED)
            {
                Debug_SetAllVariablesActive(0);
                Debug_Log("Debug", "HideAllVariables", "All variables hidden.");
                Debug_UpdateDumpView();
                return 0;
            }

            if (controlId == ID_DEBUG_AVAILABLE_LIST && notifyCode == LBN_DBLCLK)
            {
                Debug_AddSelectedAvailableVariable();
                return 0;
            }

            if (controlId == ID_DEBUG_DUMP_FILTER_EDIT && notifyCode == EN_CHANGE)
            {
                if (!g_controlsUpdating)
                    Debug_UpdateDumpView();

                return 0;
            }

            if (controlId == ID_DEBUG_LOG_FILTER_EDIT && notifyCode == EN_CHANGE)
            {
                if (!g_controlsUpdating)
                    Debug_RebuildLogView();

                return 0;
            }

            break;
        }

        case WM_SHOWWINDOW:
        {
            if (wParam)
            {
                Debug_UpdateAvailableList();
                Debug_UpdateDumpView();
                Debug_RebuildLogView();
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
            DebugWindowClosedCallback callback;

            KillTimer(hwnd, DEBUG_TIMER_ID);

            Debug_Log("Debug", "WindowDestroy", "Debug window destroyed.");

            g_window = NULL;
            g_updateButton = NULL;
            g_loggingButton = NULL;
            g_clearLogButton = NULL;
            g_dumpFilterEdit = NULL;
            g_dumpEdit = NULL;

            g_availableList = NULL;
            g_addVariableButton = NULL;
            g_removeVariableButton = NULL;
            g_showAllVariablesButton = NULL;
            g_hideAllVariablesButton = NULL;

            g_logFilterEdit = NULL;
            g_logEdit = NULL;

            callback = g_onClosed;
            g_onClosed = NULL;

            if (callback)
                callback();

            return 0;
        }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

static BOOL DebugWindow_RegisterClass(HINSTANCE hInstance)
{
    static int registered = 0;
    WNDCLASS wc;

    if (registered)
        return TRUE;

    ZeroMemory(&wc, sizeof(wc));

    wc.lpfnWndProc = DebugWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = DEBUG_WINDOW_CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClass(&wc))
        return FALSE;

    registered = 1;
    return TRUE;
}

HWND DebugWindow_Show(
    HINSTANCE hInstance,
    DebugWindowClosedCallback onClosed
)
{
    g_hInstance = hInstance;
    g_onClosed = onClosed;

    if (g_window && IsWindow(g_window))
    {
        ShowWindow(g_window, SW_SHOW);
        SetForegroundWindow(g_window);
        Debug_UpdateAvailableList();
        Debug_UpdateDumpView();
        Debug_RebuildLogView();
        return g_window;
    }

    if (!DebugWindow_RegisterClass(hInstance))
        return NULL;

    g_window = CreateWindowEx(
        0,
        DEBUG_WINDOW_CLASS_NAME,
        DEBUG_WINDOW_TITLE,
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        1000,
        720,
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

void DebugWindow_Close(void)
{
    if (g_window && IsWindow(g_window))
        DestroyWindow(g_window);
}

HWND DebugWindow_GetHwnd(void)
{
    return g_window;
}

int DebugWindow_IsOpen(void)
{
    return g_window && IsWindow(g_window);
}