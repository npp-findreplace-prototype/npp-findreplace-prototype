#include "main_internal.h"

void Main_EnableDpiAwareness(void)
{
    HMODULE user32;
    HMODULE shcore;

    BOOL (WINAPI *setProcessDpiAwarenessContext)(HANDLE);
    HRESULT (WINAPI *setProcessDpiAwareness)(int);
    BOOL (WINAPI *setProcessDpiAware)(void);

    Main_SetDpiAwarenessStatus("DPI awareness: unavailable or already fixed by Windows.");

    user32 = LoadLibrary("user32.dll");

    if (user32)
    {
        setProcessDpiAwarenessContext =
            (BOOL (WINAPI *)(HANDLE))GetProcAddress(
                user32,
                "SetProcessDpiAwarenessContext"
            );

        if (setProcessDpiAwarenessContext)
        {
            if (setProcessDpiAwarenessContext((HANDLE)-4))
            {
                Main_SetDpiAwarenessStatus("DPI awareness: Per-monitor V2.");
                FreeLibrary(user32);
                return;
            }

            if (setProcessDpiAwarenessContext((HANDLE)-3))
            {
                Main_SetDpiAwarenessStatus("DPI awareness: Per-monitor V1.");
                FreeLibrary(user32);
                return;
            }

            if (setProcessDpiAwarenessContext((HANDLE)-2))
            {
                Main_SetDpiAwarenessStatus("DPI awareness: System aware.");
                FreeLibrary(user32);
                return;
            }
        }

        FreeLibrary(user32);
    }

    shcore = LoadLibrary("shcore.dll");

    if (shcore)
    {
        setProcessDpiAwareness =
            (HRESULT (WINAPI *)(int))GetProcAddress(
                shcore,
                "SetProcessDpiAwareness"
            );

        if (setProcessDpiAwareness)
        {
            if (setProcessDpiAwareness(2) == 0)
            {
                Main_SetDpiAwarenessStatus("DPI awareness: shcore per-monitor.");
                FreeLibrary(shcore);
                return;
            }

            if (setProcessDpiAwareness(1) == 0)
            {
                Main_SetDpiAwarenessStatus("DPI awareness: shcore system aware.");
                FreeLibrary(shcore);
                return;
            }
        }

        FreeLibrary(shcore);
    }

    user32 = LoadLibrary("user32.dll");

    if (user32)
    {
        setProcessDpiAware =
            (BOOL (WINAPI *)(void))GetProcAddress(
                user32,
                "SetProcessDPIAware"
            );

        if (setProcessDpiAware && setProcessDpiAware())
            Main_SetDpiAwarenessStatus("DPI awareness: legacy system aware.");

        FreeLibrary(user32);
    }
}