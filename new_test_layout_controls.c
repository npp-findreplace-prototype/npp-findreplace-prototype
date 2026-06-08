#include "new_test_layout_controls_internal.h"

int NewTestLayoutControls_RegisterClasses(HINSTANCE hInstance)
{
    WNDCLASS wc;

    ZeroMemory(&wc, sizeof(wc));
    wc.lpfnWndProc = NewTestLayoutFauxCombo_WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = NTL_FAUX_COMBO_CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_IBEAM);
    wc.hbrBackground = NULL;

    RegisterClass(&wc);

    ZeroMemory(&wc, sizeof(wc));
    wc.lpfnWndProc = NewTestLayoutActionButton_WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = NTL_ACTION_BUTTON_CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_HAND);
    wc.hbrBackground = NULL;

    RegisterClass(&wc);

    ZeroMemory(&wc, sizeof(wc));
    wc.lpfnWndProc = NewTestLayoutActionGroup_WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = NTL_ACTION_GROUP_CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;

    RegisterClass(&wc);

    ZeroMemory(&wc, sizeof(wc));
    wc.lpfnWndProc = NewTestLayoutGearButton_WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = NTL_GEAR_BUTTON_CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_HAND);
    wc.hbrBackground = NULL;

    RegisterClass(&wc);

    return 1;
}

void NewTestLayoutControls_UnregisterClasses(HINSTANCE hInstance)
{
    UnregisterClass(NTL_FAUX_COMBO_CLASS_NAME, hInstance);
    UnregisterClass(NTL_ACTION_BUTTON_CLASS_NAME, hInstance);
    UnregisterClass(NTL_ACTION_GROUP_CLASS_NAME, hInstance);
    UnregisterClass(NTL_GEAR_BUTTON_CLASS_NAME, hInstance);
}