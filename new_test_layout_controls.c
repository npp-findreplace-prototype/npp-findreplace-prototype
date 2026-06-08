#include "new_test_layout_controls_internal.h"

static int Ntl_RegisterOneClass(
    HINSTANCE hInstance,
    const char *className,
    WNDPROC proc,
    HCURSOR cursor
)
{
    WNDCLASS wc;

    ZeroMemory(&wc, sizeof(wc));

    wc.lpfnWndProc = proc;
    wc.hInstance = hInstance;
    wc.lpszClassName = className;
    wc.hCursor = cursor;
    wc.hbrBackground = NULL;

    if (RegisterClass(&wc))
        return 1;

    if (GetLastError() == ERROR_CLASS_ALREADY_EXISTS)
        return 1;

    return 0;
}

int NewTestLayoutControls_RegisterClasses(HINSTANCE hInstance)
{
    if (!Ntl_RegisterOneClass(
            hInstance,
            NTL_FAUX_COMBO_CLASS_NAME,
            NewTestLayoutFauxCombo_WndProc,
            LoadCursor(NULL, IDC_IBEAM)
        ))
    {
        return 0;
    }

    if (!Ntl_RegisterOneClass(
            hInstance,
            NTL_ACTION_BUTTON_CLASS_NAME,
            NewTestLayoutActionButton_WndProc,
            LoadCursor(NULL, IDC_HAND)
        ))
    {
        return 0;
    }

    if (!Ntl_RegisterOneClass(
            hInstance,
            NTL_ACTION_GROUP_CLASS_NAME,
            NewTestLayoutActionGroup_WndProc,
            LoadCursor(NULL, IDC_ARROW)
        ))
    {
        return 0;
    }

    if (!Ntl_RegisterOneClass(
            hInstance,
            NTL_GEAR_BUTTON_CLASS_NAME,
            NewTestLayoutGearButton_WndProc,
            LoadCursor(NULL, IDC_HAND)
        ))
    {
        return 0;
    }

    return 1;
}

void NewTestLayoutControls_UnregisterClasses(HINSTANCE hInstance)
{
    UnregisterClass(NTL_FAUX_COMBO_CLASS_NAME, hInstance);
    UnregisterClass(NTL_ACTION_BUTTON_CLASS_NAME, hInstance);
    UnregisterClass(NTL_ACTION_GROUP_CLASS_NAME, hInstance);
    UnregisterClass(NTL_GEAR_BUTTON_CLASS_NAME, hInstance);
}