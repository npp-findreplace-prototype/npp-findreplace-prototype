#include "debug_window_internal.h"

static void Debug_FormatIntValue(char *buffer, int bufferSize, void *userData)
{
    int *value;

    value = (int *)userData;

    if (!value)
    {
        Debug_CopyText(buffer, bufferSize, "(null)");
        return;
    }

    wsprintf(buffer, "%d", *value);
}

static void Debug_FormatHwndValue(char *buffer, int bufferSize, void *userData)
{
    HWND *value;

    value = (HWND *)userData;

    if (!value || !*value)
    {
        Debug_CopyText(buffer, bufferSize, "NULL");
        return;
    }

    wsprintf(
        buffer,
        "0x%08lX%s",
        (DWORD)(ULONG_PTR)(*value),
        IsWindow(*value) ? " valid" : " invalid"
    );
}

static int Debug_FindVariableIndex(const char *aspect, const char *name)
{
    int i;

    for (i = 0; i < g_variableCount; i++)
    {
        if (lstrcmpi(g_variables[i].aspect, aspect ? aspect : "General") == 0 &&
            lstrcmpi(g_variables[i].name, name ? name : "(unnamed)") == 0)
        {
            return i;
        }
    }

    return -1;
}

void Debug_UpdateAvailableList(void)
{
    int i;
    int selected;
    char line[256];

    if (!g_availableList)
        return;

    selected = (int)SendMessage(g_availableList, LB_GETCURSEL, 0, 0);

    g_controlsUpdating = 1;

    SendMessage(g_availableList, WM_SETREDRAW, FALSE, 0);
    SendMessage(g_availableList, LB_RESETCONTENT, 0, 0);

    for (i = 0; i < g_variableCount; i++)
    {
        wsprintf(
            line,
            "%s  %-14s  %s",
            g_variables[i].active ? "[x]" : "[ ]",
            g_variables[i].aspect,
            g_variables[i].name
        );

        SendMessage(g_availableList, LB_ADDSTRING, 0, (LPARAM)line);
    }

    if (selected >= g_variableCount)
        selected = g_variableCount - 1;

    if (selected >= 0)
        SendMessage(g_availableList, LB_SETCURSEL, selected, 0);

    SendMessage(g_availableList, WM_SETREDRAW, TRUE, 0);
    InvalidateRect(g_availableList, NULL, FALSE);

    g_controlsUpdating = 0;
}

static int Debug_RegisterVariableInternal(
    const char *aspect,
    const char *name,
    DebugVariableValueCallback callback,
    void *userData,
    int active
)
{
    DebugVariable *variable;
    int existingIndex;

    if (!callback)
        return 0;

    existingIndex = Debug_FindVariableIndex(aspect, name);

    if (existingIndex >= 0)
    {
        g_variables[existingIndex].callback = callback;
        g_variables[existingIndex].userData = userData;

        if (active)
            g_variables[existingIndex].active = 1;

        Debug_UpdateAvailableList();
        return 1;
    }

    if (g_variableCount >= DEBUG_MAX_VARIABLES)
        return 0;

    variable = &g_variables[g_variableCount];

    Debug_CopyText(variable->aspect, DEBUG_ASPECT_SIZE, aspect ? aspect : "General");
    Debug_CopyText(variable->name, DEBUG_NAME_SIZE, name ? name : "(unnamed)");
    variable->callback = callback;
    variable->userData = userData;
    variable->active = active ? 1 : 0;

    g_variableCount++;

    Debug_UpdateAvailableList();

    Debug_Log(
        "Debug",
        active ? "RegisterVariable" : "RegisterAvailableVariable",
        "%s.%s",
        variable->aspect,
        variable->name
    );

    return 1;
}

int Debug_RegisterVariable(
    const char *aspect,
    const char *name,
    DebugVariableValueCallback callback,
    void *userData
)
{
    return Debug_RegisterVariableInternal(
        aspect,
        name,
        callback,
        userData,
        1
    );
}

int Debug_RegisterAvailableVariable(
    const char *aspect,
    const char *name,
    DebugVariableValueCallback callback,
    void *userData
)
{
    return Debug_RegisterVariableInternal(
        aspect,
        name,
        callback,
        userData,
        0
    );
}

int Debug_RegisterIntPointer(
    const char *aspect,
    const char *name,
    int *value
)
{
    return Debug_RegisterVariable(
        aspect,
        name,
        Debug_FormatIntValue,
        value
    );
}

int Debug_RegisterAvailableIntPointer(
    const char *aspect,
    const char *name,
    int *value
)
{
    return Debug_RegisterAvailableVariable(
        aspect,
        name,
        Debug_FormatIntValue,
        value
    );
}

int Debug_RegisterHwndPointer(
    const char *aspect,
    const char *name,
    HWND *value
)
{
    return Debug_RegisterVariable(
        aspect,
        name,
        Debug_FormatHwndValue,
        value
    );
}

int Debug_RegisterAvailableHwndPointer(
    const char *aspect,
    const char *name,
    HWND *value
)
{
    return Debug_RegisterAvailableVariable(
        aspect,
        name,
        Debug_FormatHwndValue,
        value
    );
}

int Debug_SetVariableActive(
    const char *aspect,
    const char *name,
    int active
)
{
    int index;

    index = Debug_FindVariableIndex(aspect, name);

    if (index < 0)
        return 0;

    g_variables[index].active = active ? 1 : 0;

    Debug_UpdateAvailableList();

    if (g_dumpEdit)
        Debug_SetEditTextPreserveSelection(g_dumpEdit, "");

    return 1;
}

void Debug_SetAllVariablesActive(int active)
{
    int i;

    for (i = 0; i < g_variableCount; i++)
        g_variables[i].active = active ? 1 : 0;

    Debug_UpdateAvailableList();
}

void Debug_AddSelectedAvailableVariable(void)
{
    int index;

    if (!g_availableList)
        return;

    index = (int)SendMessage(g_availableList, LB_GETCURSEL, 0, 0);

    if (index < 0 || index >= g_variableCount)
        return;

    g_variables[index].active = 1;

    Debug_Log(
        "Debug",
        "VariableAdded",
        "%s.%s",
        g_variables[index].aspect,
        g_variables[index].name
    );

    Debug_UpdateAvailableList();
    Debug_UpdateDumpView();
}

void Debug_RemoveSelectedAvailableVariable(void)
{
    int index;

    if (!g_availableList)
        return;

    index = (int)SendMessage(g_availableList, LB_GETCURSEL, 0, 0);

    if (index < 0 || index >= g_variableCount)
        return;

    g_variables[index].active = 0;

    Debug_Log(
        "Debug",
        "VariableRemoved",
        "%s.%s",
        g_variables[index].aspect,
        g_variables[index].name
    );

    Debug_UpdateAvailableList();
    Debug_UpdateDumpView();
}