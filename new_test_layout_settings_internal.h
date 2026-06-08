#ifndef NEW_TEST_LAYOUT_SETTINGS_INTERNAL_H
#define NEW_TEST_LAYOUT_SETTINGS_INTERNAL_H

#include <windows.h>
#include <stdlib.h>
#include <stddef.h>

#include "new_test_layout_settings.h"

#define NTL_SETTINGS_ID_CLOSE_BUTTON 6100
#define NTL_SETTINGS_ID_INT_BASE 6200
#define NTL_SETTINGS_ID_BOOL_BASE 6300

#define NTL_SETTINGS_INT_COUNT 5
#define NTL_SETTINGS_BOOL_COUNT 16

typedef struct NewTestLayoutSettingsIntDef
{
    const char *label;
    size_t offset;
} NewTestLayoutSettingsIntDef;

typedef struct NewTestLayoutSettingsBoolDef
{
    const char *label;
    size_t offset;
} NewTestLayoutSettingsBoolDef;

struct NewTestLayoutSettingsPanel
{
    HWND parent;
    HWND container;
    HWND titleLabel;
    HWND closeButton;

    HWND intLabels[NTL_SETTINGS_INT_COUNT];
    HWND intEdits[NTL_SETTINGS_INT_COUNT];

    HWND boolChecks[NTL_SETTINGS_BOOL_COUNT];

    HINSTANCE hInstance;

    NewTestLayoutSettingsConfig config;
    NewTestLayoutSettingsChangedCallback changedCallback;
    void *changedUserData;

    HBRUSH backBrush;
    HBRUSH editBrush;

    int visible;
    int updatingControls;
};

extern const NewTestLayoutSettingsIntDef g_ntlSettingsIntDefs[NTL_SETTINGS_INT_COUNT];
extern const NewTestLayoutSettingsBoolDef g_ntlSettingsBoolDefs[NTL_SETTINGS_BOOL_COUNT];

int *Settings_IntField(
    NewTestLayoutSettingsConfig *config,
    size_t offset
);

void Settings_CopyConfig(
    NewTestLayoutSettingsConfig *dest,
    const NewTestLayoutSettingsConfig *src
);

void Settings_RecreateBrushes(
    NewTestLayoutSettingsPanel *panel
);

void Settings_NotifyChanged(
    NewTestLayoutSettingsPanel *panel
);

int Settings_CreateChildControls(
    NewTestLayoutSettingsPanel *panel
);

void Settings_SetEditInt(
    HWND hwnd,
    int value
);

int Settings_GetEditInt(
    HWND hwnd,
    int defaultValue
);

#endif