#include "debug_window_internal.h"

static int Debug_VariableMatchesFilter(
    const DebugVariable *variable,
    const char *value,
    const char *filter
)
{
    if (!filter || !filter[0])
        return 1;

    if (Debug_TextContainsNoCase(variable->aspect, filter))
        return 1;

    if (Debug_TextContainsNoCase(variable->name, filter))
        return 1;

    if (Debug_TextContainsNoCase(value, filter))
        return 1;

    return 0;
}

void Debug_UpdateDumpView(void)
{
    char filter[256];
    char dump[32768];
    char line[1024];
    char value[DEBUG_VALUE_SIZE];
    int i;

    if (!g_dumpEdit)
        return;

    if (!g_window || !IsWindowVisible(g_window))
        return;

    if (!g_updatesEnabled)
        return;

    Debug_GetWindowTextSafe(g_dumpFilterEdit, filter, sizeof(filter));

    dump[0] = '\0';

    for (i = 0; i < g_variableCount; i++)
    {
        if (!g_variables[i].active)
            continue;

        value[0] = '\0';

        if (g_variables[i].callback)
            g_variables[i].callback(value, sizeof(value), g_variables[i].userData);

        value[sizeof(value) - 1] = '\0';

        if (!Debug_VariableMatchesFilter(&g_variables[i], value, filter))
            continue;

        wsprintf(
            line,
            "%-14s  %-32s  %s\r\n",
            g_variables[i].aspect,
            g_variables[i].name,
            value
        );

        Debug_AppendText(dump, sizeof(dump), line);
    }

    Debug_SetEditTextPreserveSelection(g_dumpEdit, dump);
}