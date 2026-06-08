#include "main_internal.h"

static HWND Main_CreateToggleButton(HWND parent, int id, int y)
{
    return CreateWindowEx(
        0,
        "BUTTON",
        "",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
        MAIN_MARGIN,
        y,
        MAIN_BUTTON_WIDTH,
        MAIN_BUTTON_HEIGHT,
        parent,
        (HMENU)id,
        g_main_hInstance,
        NULL
    );
}

void Main_UpdateButtons(void)
{
    if (!g_mainWindow)
        return;

    if (g_gridTesterButton)
    {
        SetWindowText(
            g_gridTesterButton,
            GridTesterWindow_IsOpen() ? "Hide grid tester" : "Show grid tester"
        );
    }

    if (g_gridTesterIniButton)
    {
        SetWindowText(
            g_gridTesterIniButton,
            GridTesterIniWindow_IsOpen() ? "Hide grid tester (ini)" : "Show grid tester (ini)"
        );
    }

    if (g_newTestLayoutButton)
    {
        SetWindowText(
            g_newTestLayoutButton,
            NewTestLayoutWindow_IsOpen() ? "Hide new test layout" : "Show new test layout"
        );
    }

    if (g_findReplaceDialogButton)
    {
        SetWindowText(
            g_findReplaceDialogButton,
            FindReplaceDialogWindow_IsOpen() ? "Hide find and replace dialog" : "Show find and replace dialog"
        );
    }

    if (g_nppMockupButton)
    {
        SetWindowText(
            g_nppMockupButton,
            NppMockupWindow_IsOpen() ? "Hide NP++ mockup" : "Show NP++ mockup"
        );
    }

    if (g_debugWindowButton)
    {
        SetWindowText(
            g_debugWindowButton,
            DebugWindow_IsOpen() ? "Hide debug window" : "Show debug window"
        );
    }

    if (g_consoleButton)
    {
        SetWindowText(
            g_consoleButton,
            Main_IsConsoleVisible() ? "Hide console" : "Show console"
        );
    }
}

void Main_CreateButtons(HWND hwnd)
{
    int y;

    y = MAIN_MARGIN;

    g_gridTesterButton = Main_CreateToggleButton(hwnd, ID_SHOW_GRID_TESTER_BUTTON, y);
    y += MAIN_BUTTON_HEIGHT + MAIN_BUTTON_SPACING;

    g_gridTesterIniButton = Main_CreateToggleButton(hwnd, ID_SHOW_GRID_TESTER_INI_BUTTON, y);
    y += MAIN_BUTTON_HEIGHT + MAIN_BUTTON_SPACING;

    g_newTestLayoutButton = Main_CreateToggleButton(hwnd, ID_SHOW_NEW_TEST_LAYOUT_BUTTON, y);
    y += MAIN_BUTTON_HEIGHT + MAIN_BUTTON_SPACING;

    g_findReplaceDialogButton = Main_CreateToggleButton(hwnd, ID_SHOW_FIND_REPLACE_DIALOG_BUTTON, y);
    y += MAIN_BUTTON_HEIGHT + MAIN_BUTTON_SPACING;

    g_nppMockupButton = Main_CreateToggleButton(hwnd, ID_SHOW_NPP_MOCKUP_BUTTON, y);
    y += MAIN_BUTTON_HEIGHT + MAIN_BUTTON_SPACING;

    g_debugWindowButton = Main_CreateToggleButton(hwnd, ID_SHOW_DEBUG_WINDOW_BUTTON, y);
    y += MAIN_BUTTON_HEIGHT + MAIN_BUTTON_SPACING;

    g_consoleButton = Main_CreateToggleButton(hwnd, ID_SHOW_CONSOLE_BUTTON, y);

    Main_UpdateButtons();
}

static void Main_SetButtonRect(HWND button, int x, int y, int width)
{
    if (!button)
        return;

    SetWindowPos(
        button,
        NULL,
        x,
        y,
        width,
        MAIN_BUTTON_HEIGHT,
        SWP_NOZORDER | SWP_NOACTIVATE
    );
}

void Main_LayoutButtons(HWND hwnd)
{
    RECT rc;
    int x;
    int y;
    int w;

    GetClientRect(hwnd, &rc);

    x = MAIN_MARGIN;
    y = MAIN_MARGIN;

    w = rc.right - rc.left - (MAIN_MARGIN * 2);

    if (w > MAIN_BUTTON_WIDTH)
        w = MAIN_BUTTON_WIDTH;

    if (w < 80)
        w = 80;

    Main_SetButtonRect(g_gridTesterButton, x, y, w);
    y += MAIN_BUTTON_HEIGHT + MAIN_BUTTON_SPACING;

    Main_SetButtonRect(g_gridTesterIniButton, x, y, w);
    y += MAIN_BUTTON_HEIGHT + MAIN_BUTTON_SPACING;

    Main_SetButtonRect(g_newTestLayoutButton, x, y, w);
    y += MAIN_BUTTON_HEIGHT + MAIN_BUTTON_SPACING;

    Main_SetButtonRect(g_findReplaceDialogButton, x, y, w);
    y += MAIN_BUTTON_HEIGHT + MAIN_BUTTON_SPACING;

    Main_SetButtonRect(g_nppMockupButton, x, y, w);
    y += MAIN_BUTTON_HEIGHT + MAIN_BUTTON_SPACING;

    Main_SetButtonRect(g_debugWindowButton, x, y, w);
    y += MAIN_BUTTON_HEIGHT + MAIN_BUTTON_SPACING;

    Main_SetButtonRect(g_consoleButton, x, y, w);
}