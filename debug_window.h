#ifndef DEBUG_WINDOW_H
#define DEBUG_WINDOW_H

#include <windows.h>

typedef void (*DebugWindowClosedCallback)(void);
typedef void (*DebugVariableValueCallback)(char *buffer, int bufferSize, void *userData);

HWND DebugWindow_Show(
    HINSTANCE hInstance,
    DebugWindowClosedCallback onClosed
);

void DebugWindow_Close(void);
HWND DebugWindow_GetHwnd(void);
int DebugWindow_IsOpen(void);

int Debug_RegisterVariable(
    const char *aspect,
    const char *name,
    DebugVariableValueCallback callback,
    void *userData
);

int Debug_RegisterAvailableVariable(
    const char *aspect,
    const char *name,
    DebugVariableValueCallback callback,
    void *userData
);

int Debug_RegisterIntPointer(
    const char *aspect,
    const char *name,
    int *value
);

int Debug_RegisterAvailableIntPointer(
    const char *aspect,
    const char *name,
    int *value
);

int Debug_RegisterHwndPointer(
    const char *aspect,
    const char *name,
    HWND *value
);

int Debug_RegisterAvailableHwndPointer(
    const char *aspect,
    const char *name,
    HWND *value
);

int Debug_SetVariableActive(
    const char *aspect,
    const char *name,
    int active
);

void Debug_SetAllVariablesActive(int active);

void Debug_Log(
    const char *aspect,
    const char *eventName,
    const char *format,
    ...
);

void Debug_LogColor(
    const char *aspect,
    const char *eventName,
    COLORREF color,
    const char *format,
    ...
);

void Debug_ClearLog(void);

#endif