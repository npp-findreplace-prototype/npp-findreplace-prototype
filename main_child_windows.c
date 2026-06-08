#include "main_internal.h"

void Main_OnChildWindowClosed(void)
{
    Debug_Log("Main", "ChildWindowClosed", "A child/test window closed.");
    Main_UpdateButtons();
}

void Main_ToggleGridTesterWindow(void)
{
    if (GridTesterWindow_IsOpen())
    {
        Debug_Log("Main", "HideGridTester", "Closing grid tester.");
        GridTesterWindow_Close();
    }
    else
    {
        Debug_Log("Main", "ShowGridTester", "Opening grid tester.");
        GridTesterWindow_Show(g_main_hInstance, Main_OnChildWindowClosed);
    }

    Main_UpdateButtons();
}

void Main_ToggleGridTesterIniWindow(void)
{
    if (GridTesterIniWindow_IsOpen())
    {
        Debug_Log("Main", "HideGridTesterIni", "Closing grid tester INI.");
        GridTesterIniWindow_Close();
    }
    else
    {
        Debug_Log("Main", "ShowGridTesterIni", "Opening grid tester INI.");
        GridTesterIniWindow_Show(g_main_hInstance, Main_OnChildWindowClosed);
    }

    Main_UpdateButtons();
}

void Main_ToggleNewTestLayoutWindow(void)
{
    if (NewTestLayoutWindow_IsOpen())
    {
        Debug_Log("Main", "HideNewTestLayout", "Closing new test layout.");
        NewTestLayoutWindow_Close();
    }
    else
    {
        Debug_Log("Main", "ShowNewTestLayout", "Opening new test layout.");
        NewTestLayoutWindow_Show(g_main_hInstance, Main_OnChildWindowClosed);
    }

    Main_UpdateButtons();
}

void Main_ToggleFindReplaceDialogWindow(void)
{
    if (FindReplaceDialogWindow_IsOpen())
    {
        Debug_Log("Main", "HideFindReplace", "Closing find and replace dialog.");
        FindReplaceDialogWindow_Close();
    }
    else
    {
        Debug_Log("Main", "ShowFindReplace", "Opening find and replace dialog.");
        FindReplaceDialogWindow_Show(g_main_hInstance, Main_OnChildWindowClosed);
    }

    Main_UpdateButtons();
}

void Main_ToggleNppMockupWindow(void)
{
    if (NppMockupWindow_IsOpen())
    {
        Debug_Log("Main", "HideNppMockup", "Closing NP++ mockup.");
        NppMockupWindow_Close();
    }
    else
    {
        Debug_Log("Main", "ShowNppMockup", "Opening NP++ mockup.");
        NppMockupWindow_Show(g_main_hInstance, Main_OnChildWindowClosed);
    }

    Main_UpdateButtons();
}

void Main_ToggleDebugWindow(void)
{
    if (DebugWindow_IsOpen())
    {
        Debug_Log("Main", "HideDebugWindow", "Closing debug window.");
        DebugWindow_Close();
    }
    else
    {
        Debug_Log("Main", "ShowDebugWindow", "Opening debug window.");
        DebugWindow_Show(g_main_hInstance, Main_OnChildWindowClosed);
    }

    Main_UpdateButtons();
}

void Main_ToggleConsoleWindow(void)
{
    if (Main_IsConsoleVisible())
    {
        Debug_Log("Main", "HideConsole", "Hiding console window.");
        Main_ShowConsole(0);
    }
    else
    {
        Main_ShowConsole(1);
        Debug_Log("Main", "ShowConsole", "Showing console window.");
    }

    Main_UpdateButtons();
}

void Main_CloseChildWindows(void)
{
    GridTesterWindow_Close();
    GridTesterIniWindow_Close();
    NewTestLayoutWindow_Close();
    FindReplaceDialogWindow_Close();
    NppMockupWindow_Close();
    DebugWindow_Close();
}