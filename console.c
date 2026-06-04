#include <windows.h>
#include <stdio.h>

#include "console.h"

#ifndef ATTACH_PARENT_PROCESS
#define ATTACH_PARENT_PROCESS ((DWORD)-1)
#endif

static int g_consoleAttached = 0;

void Console_Setup(void)
{
    typedef BOOL (WINAPI *AttachConsoleFunc)(DWORD);

    HMODULE kernel32;
    AttachConsoleFunc pAttachConsole;

    kernel32 = GetModuleHandle("kernel32.dll");

    if (!kernel32)
        return;

    pAttachConsole = (AttachConsoleFunc)GetProcAddress(kernel32, "AttachConsole");

    if (!pAttachConsole)
        return;

    if (pAttachConsole(ATTACH_PARENT_PROCESS))
    {
        g_consoleAttached = 1;

        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);

        setvbuf(stdout, NULL, _IONBF, 0);
        setvbuf(stderr, NULL, _IONBF, 0);

        printf("Console attached.\n");
    }
}

int Console_IsAttached(void)
{
    return g_consoleAttached;
}

void AppNotify(const char *title, const char *message)
{
    if (g_consoleAttached)
    {
        printf("%s: %s\n", title, message);
        return;
    }

    MessageBox(NULL, message, title, MB_OK);
}