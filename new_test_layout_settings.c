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

static const NewTestLayoutSettingsIntDef g_intDefs[NTL_SETTINGS_INT_COUNT] =
{
    { "Overlay width", offsetof(NewTestLayoutSettingsConfig, overlayWidth) },
    { "Overlay margin", offsetof(NewTestLayoutSettingsConfig, overlayMargin) },
    { "Row height", offsetof(NewTestLayoutSettingsConfig, rowHeight) },
    { "Gap", offsetof(NewTestLayoutSettingsConfig, gap) },
    { "Single row icon grid height", offsetof(NewTestLayoutSettingsConfig, singleRowModeGridHeight) }
};

static const NewTestLayoutSettingsBoolDef g_boolDefs[NTL_SETTINGS_BOOL_COUNT] =
{
    { "Auto layout enabled", offsetof(NewTestLayoutSettingsConfig, autoLayoutEnabled) },
    { "Show replace box", offsetof(NewTestLayoutSettingsConfig, showReplaceBox) },
    { "Show utility buttons", offsetof(NewTestLayoutSettingsConfig, showUtilityButtons) },
    { "Show mode icon grid", offsetof(NewTestLayoutSettingsConfig, showModeGrid) },
    { "Show Find In Document grid", offsetof(NewTestLayoutSettingsConfig, showFindDocumentGrid) },
    { "Show Replace In Document grid", offsetof(NewTestLayoutSettingsConfig, showReplaceDocumentGrid) },
    { "Show Replace In Selection grid", offsetof(NewTestLayoutSettingsConfig, showReplaceSelectionGrid) },
    { "Show counts", offsetof(NewTestLayoutSettingsConfig, showCounts) },
    { "Show zero counts", offsetof(NewTestLayoutSettingsConfig, showZeroCounts) },
    { "Counts in parentheses", offsetof(NewTestLayoutSettingsConfig, countInParentheses) },
    { "Enable left mode panel", offsetof(NewTestLayoutSettingsConfig, enableLeftModePanel) },
    { "Enable group borders", offsetof(NewTestLayoutSettingsConfig, enableGrowingBorder) },
    { "Recent find dropdown", offsetof(NewTestLayoutSettingsConfig, enableRecentFindDropdown) },
    { "Recent replace dropdown", offsetof(NewTestLayoutSettingsConfig, enableRecentReplaceDropdown) },
    { "Large placeholder text", offsetof(NewTestLayoutSettingsConfig, fauxComboPlaceholderLarge) },
    { "Blue count text", offsetof(NewTestLayoutSettingsConfig, actionButtonCountColorEnabled) }
};

static int *Settings_IntField(
    NewTestLayoutSettingsConfig *config,
    size_t offset
)
{
    return (int *)((BYTE *)config + offset);
}

static void Settings_CopyConfig(
    NewTestLayoutSettingsConfig *dest,
    const NewTestLayoutSettingsConfig *src
)
{
    if (!dest)
        return;

    if (src)
        CopyMemory(dest, src, sizeof(*dest));
    else
        NewTestLayoutSettings_GetDefaultConfig(dest);
}

static void Settings_RecreateBrushes(
    NewTestLayoutSettingsPanel *panel
)
{
    if (!panel)
        return;

    if (panel->backBrush)
        DeleteObject(panel->backBrush);

    if (panel->editBrush)
        DeleteObject(panel->editBrush);

    panel->backBrush = CreateSolidBrush(panel->config.overlayBackColor);
    panel->editBrush = CreateSolidBrush(RGB(18, 18, 18));
}

static HWND Settings_CreateStatic(
    NewTestLayoutSettingsPanel *panel,
    const char *text
)
{
    HWND hwnd;

    hwnd = CreateWindowEx(
        0,
        "STATIC",
        text ? text : "",
        WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE,
        0,
        0,
        10,
        10,
        panel->container,
        NULL,
        panel->hInstance,
        NULL
    );

    if (hwnd)
        SendMessage(hwnd, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);

    return hwnd;
}

static HWND Settings_CreateEdit(
    NewTestLayoutSettingsPanel *panel,
    int id
)
{
    HWND hwnd;

    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        "EDIT",
        "",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
        0,
        0,
        10,
        10,
        panel->container,
        (HMENU)id,
        panel->hInstance,
        NULL
    );

    if (hwnd)
        SendMessage(hwnd, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);

    return hwnd;
}

static HWND Settings_CreateCheckbox(
    NewTestLayoutSettingsPanel *panel,
    int id,
    const char *text
)
{
    HWND hwnd;

    hwnd = CreateWindowEx(
        0,
        "BUTTON",
        text ? text : "",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX,
        0,
        0,
        10,
        10,
        panel->container,
        (HMENU)id,
        panel->hInstance,
        NULL
    );

    if (hwnd)
        SendMessage(hwnd, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);

    return hwnd;
}

static void Settings_NotifyChanged(
    NewTestLayoutSettingsPanel *panel
)
{
    if (!panel)
        return;

    if (panel->changedCallback)
        panel->changedCallback(panel->changedUserData);
}

void NewTestLayoutSettings_GetDefaultConfig(
    NewTestLayoutSettingsConfig *config
)
{
    if (!config)
        return;

    ZeroMemory(config, sizeof(*config));

    config->overlayWidth = NEW_TEST_LAYOUT_SETTINGS_DEFAULT_OVERLAY_WIDTH;
    config->overlayMargin = NEW_TEST_LAYOUT_SETTINGS_DEFAULT_MARGIN;
    config->rowHeight = NEW_TEST_LAYOUT_SETTINGS_DEFAULT_ROW_HEIGHT;
    config->gap = NEW_TEST_LAYOUT_SETTINGS_DEFAULT_GAP;
    config->singleRowModeGridHeight = NEW_TEST_LAYOUT_SETTINGS_DEFAULT_SINGLE_ROW_GRID_HEIGHT;

    config->autoLayoutEnabled = 1;
    config->showReplaceBox = 1;
    config->showUtilityButtons = 1;
    config->showModeGrid = 1;
    config->showFindDocumentGrid = 1;
    config->showReplaceDocumentGrid = 1;
    config->showReplaceSelectionGrid = 1;

    config->showCounts = 1;
    config->showZeroCounts = 0;
    config->countInParentheses = 1;

    config->enableLeftModePanel = 1;
    config->enableGrowingBorder = 1;
    config->enableRecentFindDropdown = 1;
    config->enableRecentReplaceDropdown = 1;

    config->fauxComboPlaceholderLarge = 1;
    config->actionButtonCountColorEnabled = 1;

    config->overlayBackColor = RGB(22, 22, 22);
    config->overlayBorderColor = RGB(86, 86, 86);
    config->overlayTitleColor = RGB(230, 230, 230);
    config->labelColor = RGB(190, 190, 190);
    config->textColor = RGB(230, 230, 230);
    config->mutedTextColor = RGB(130, 130, 130);
    config->accentColor = RGB(0, 105, 210);
    config->buttonBackColor = RGB(28, 28, 28);
    config->buttonBorderColor = RGB(90, 90, 90);
    config->buttonTextColor = RGB(220, 220, 220);
    config->countTextColor = RGB(0, 105, 210);
}

NewTestLayoutSettingsPanel *NewTestLayoutSettings_Create(
    HWND parent,
    HINSTANCE hInstance,
    const NewTestLayoutSettingsConfig *initialConfig,
    NewTestLayoutSettingsChangedCallback changedCallback,
    void *changedUserData
)
{
    NewTestLayoutSettingsPanel *panel;
    int i;

    panel = (NewTestLayoutSettingsPanel *)malloc(sizeof(*panel));

    if (!panel)
        return NULL;

    ZeroMemory(panel, sizeof(*panel));

    panel->parent = parent;
    panel->hInstance = hInstance;
    panel->changedCallback = changedCallback;
    panel->changedUserData = changedUserData;
    panel->visible = 0;

    Settings_CopyConfig(&panel->config, initialConfig);
    Settings_RecreateBrushes(panel);

    panel->container = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        "STATIC",
        "",
        WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
        0,
        0,
        panel->config.overlayWidth,
        300,
        parent,
        NULL,
        hInstance,
        NULL
    );

    if (!panel->container)
    {
        NewTestLayoutSettings_Destroy(panel);
        return NULL;
    }

    panel->titleLabel = Settings_CreateStatic(panel, "Layout Settings");

    panel->closeButton = CreateWindowEx(
        0,
        "BUTTON",
        "X",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
        0,
        0,
        28,
        24,
        panel->container,
        (HMENU)NTL_SETTINGS_ID_CLOSE_BUTTON,
        hInstance,
        NULL
    );

    if (panel->closeButton)
        SendMessage(panel->closeButton, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);

    for (i = 0; i < NTL_SETTINGS_INT_COUNT; i++)
    {
        panel->intLabels[i] = Settings_CreateStatic(panel, g_intDefs[i].label);
        panel->intEdits[i] = Settings_CreateEdit(panel, NTL_SETTINGS_ID_INT_BASE + i);
    }

    for (i = 0; i < NTL_SETTINGS_BOOL_COUNT; i++)
    {
        panel->boolChecks[i] = Settings_CreateCheckbox(
            panel,
            NTL_SETTINGS_ID_BOOL_BASE + i,
            g_boolDefs[i].label
        );
    }

    NewTestLayoutSettings_RequestRefresh(panel);
    ShowWindow(panel->container, SW_HIDE);

    return panel;
}

void NewTestLayoutSettings_Destroy(
    NewTestLayoutSettingsPanel *panel
)
{
    if (!panel)
        return;

    if (panel->container)
        DestroyWindow(panel->container);

    if (panel->backBrush)
        DeleteObject(panel->backBrush);

    if (panel->editBrush)
        DeleteObject(panel->editBrush);

    free(panel);
}

void NewTestLayoutSettings_SetConfig(
    NewTestLayoutSettingsPanel *panel,
    const NewTestLayoutSettingsConfig *config
)
{
    if (!panel)
        return;

    Settings_CopyConfig(&panel->config, config);
    Settings_RecreateBrushes(panel);
    NewTestLayoutSettings_RequestRefresh(panel);
}

void NewTestLayoutSettings_GetConfig(
    NewTestLayoutSettingsPanel *panel,
    NewTestLayoutSettingsConfig *config
)
{
    if (!panel || !config)
        return;

    CopyMemory(config, &panel->config, sizeof(*config));
}

void NewTestLayoutSettings_Show(
    NewTestLayoutSettingsPanel *panel,
    int show
)
{
    if (!panel || !panel->container)
        return;

    panel->visible = show ? 1 : 0;

    ShowWindow(panel->container, panel->visible ? SW_SHOW : SW_HIDE);

    if (panel->visible)
    {
        SetWindowPos(
            panel->container,
            HWND_TOP,
            0,
            0,
            0,
            0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE
        );
    }
}

void NewTestLayoutSettings_Toggle(
    NewTestLayoutSettingsPanel *panel
)
{
    if (!panel)
        return;

    NewTestLayoutSettings_Show(panel, !panel->visible);
}

int NewTestLayoutSettings_IsVisible(
    NewTestLayoutSettingsPanel *panel
)
{
    if (!panel)
        return 0;

    return panel->visible;
}

void NewTestLayoutSettings_Layout(
    NewTestLayoutSettingsPanel *panel,
    const RECT *parentClientRect
)
{
    int margin;
    int gap;
    int rowH;
    int x;
    int y;
    int w;
    int h;
    int labelW;
    int editW;
    int i;

    if (!panel || !panel->container || !parentClientRect)
        return;

    margin = panel->config.overlayMargin;
    gap = panel->config.gap;
    rowH = panel->config.rowHeight;

    if (margin < 4)
        margin = 4;

    if (gap < 2)
        gap = 2;

    if (rowH < 20)
        rowH = 20;

    w = panel->config.overlayWidth;

    if (w < 220)
        w = 220;

    if (w > parentClientRect->right - parentClientRect->left - margin * 2)
        w = parentClientRect->right - parentClientRect->left - margin * 2;

    if (w < 120)
        w = 120;

    h = parentClientRect->bottom - parentClientRect->top - margin * 2;

    if (h < 120)
        h = 120;

    x = parentClientRect->right - margin - w;
    y = parentClientRect->top + margin;

    MoveWindow(panel->container, x, y, w, h, TRUE);

    x = margin;
    y = margin;

    MoveWindow(
        panel->titleLabel,
        x,
        y,
        w - margin * 2 - 36,
        rowH,
        TRUE
    );

    MoveWindow(
        panel->closeButton,
        w - margin - 30,
        y,
        30,
        rowH,
        TRUE
    );

    y += rowH + gap * 2;

    labelW = (w - margin * 2 - gap) / 2;
    editW = w - margin * 2 - labelW - gap;

    if (labelW < 90)
        labelW = 90;

    if (editW < 60)
        editW = 60;

    for (i = 0; i < NTL_SETTINGS_INT_COUNT; i++)
    {
        MoveWindow(
            panel->intLabels[i],
            margin,
            y,
            labelW,
            rowH,
            TRUE
        );

        MoveWindow(
            panel->intEdits[i],
            margin + labelW + gap,
            y,
            editW,
            rowH,
            TRUE
        );

        y += rowH + gap;
    }

    y += gap;

    for (i = 0; i < NTL_SETTINGS_BOOL_COUNT; i++)
    {
        MoveWindow(
            panel->boolChecks[i],
            margin,
            y,
            w - margin * 2,
            rowH,
            TRUE
        );

        y += rowH + gap;
    }

    InvalidateRect(panel->container, NULL, TRUE);
}

static void Settings_SetEditInt(
    HWND hwnd,
    int value
)
{
    char text[64];

    wsprintf(text, "%d", value);
    SetWindowText(hwnd, text);
}

static int Settings_GetEditInt(
    HWND hwnd,
    int defaultValue
)
{
    char text[64];

    if (!hwnd)
        return defaultValue;

    GetWindowText(hwnd, text, sizeof(text));
    text[sizeof(text) - 1] = '\0';

    if (!text[0])
        return defaultValue;

    return (int)strtol(text, NULL, 0);
}

void NewTestLayoutSettings_RequestRefresh(
    NewTestLayoutSettingsPanel *panel
)
{
    int i;
    int *field;

    if (!panel)
        return;

    panel->updatingControls = 1;

    for (i = 0; i < NTL_SETTINGS_INT_COUNT; i++)
    {
        field = Settings_IntField(&panel->config, g_intDefs[i].offset);
        Settings_SetEditInt(panel->intEdits[i], *field);
    }

    for (i = 0; i < NTL_SETTINGS_BOOL_COUNT; i++)
    {
        field = Settings_IntField(&panel->config, g_boolDefs[i].offset);

        SendMessage(
            panel->boolChecks[i],
            BM_SETCHECK,
            *field ? BST_CHECKED : BST_UNCHECKED,
            0
        );
    }

    panel->updatingControls = 0;
}

static int Settings_HandleIntEditCommand(
    NewTestLayoutSettingsPanel *panel,
    int id,
    int notifyCode
)
{
    int index;
    int *field;
    int oldValue;
    int newValue;

    if (!panel)
        return 0;

    index = id - NTL_SETTINGS_ID_INT_BASE;

    if (index < 0 || index >= NTL_SETTINGS_INT_COUNT)
        return 0;

    if (notifyCode != EN_CHANGE)
        return 1;

    if (panel->updatingControls)
        return 1;

    field = Settings_IntField(&panel->config, g_intDefs[index].offset);

    oldValue = *field;
    newValue = Settings_GetEditInt(panel->intEdits[index], oldValue);

    if (index == 0 && newValue < 180)
        newValue = 180;

    if (index == 4)
    {
        if (newValue < 44)
            newValue = 44;

        if (newValue > 140)
            newValue = 140;
    }
    else if (index != 0 && newValue < 0)
    {
        newValue = 0;
    }

    if (oldValue != newValue)
    {
        *field = newValue;
        Settings_NotifyChanged(panel);
    }

    return 1;
}

static int Settings_HandleBoolCommand(
    NewTestLayoutSettingsPanel *panel,
    int id,
    int notifyCode
)
{
    int index;
    int *field;
    int checked;

    if (!panel)
        return 0;

    index = id - NTL_SETTINGS_ID_BOOL_BASE;

    if (index < 0 || index >= NTL_SETTINGS_BOOL_COUNT)
        return 0;

    if (notifyCode != BN_CLICKED)
        return 1;

    if (panel->updatingControls)
        return 1;

    field = Settings_IntField(&panel->config, g_boolDefs[index].offset);

    checked = SendMessage(panel->boolChecks[index], BM_GETCHECK, 0, 0) == BST_CHECKED;

    if (*field != checked)
    {
        *field = checked;
        Settings_NotifyChanged(panel);
    }

    return 1;
}

int NewTestLayoutSettings_HandleCommand(
    NewTestLayoutSettingsPanel *panel,
    WPARAM wParam,
    LPARAM lParam
)
{
    int id;
    int notifyCode;

    (void)lParam;

    if (!panel)
        return 0;

    id = LOWORD(wParam);
    notifyCode = HIWORD(wParam);

    if (id == NTL_SETTINGS_ID_CLOSE_BUTTON && notifyCode == BN_CLICKED)
    {
        NewTestLayoutSettings_Show(panel, 0);
        return 1;
    }

    if (id >= NTL_SETTINGS_ID_INT_BASE &&
        id < NTL_SETTINGS_ID_INT_BASE + NTL_SETTINGS_INT_COUNT)
    {
        return Settings_HandleIntEditCommand(panel, id, notifyCode);
    }

    if (id >= NTL_SETTINGS_ID_BOOL_BASE &&
        id < NTL_SETTINGS_ID_BOOL_BASE + NTL_SETTINGS_BOOL_COUNT)
    {
        return Settings_HandleBoolCommand(panel, id, notifyCode);
    }

    return 0;
}

int NewTestLayoutSettings_HandleNotify(
    NewTestLayoutSettingsPanel *panel,
    WPARAM wParam,
    LPARAM lParam
)
{
    (void)panel;
    (void)wParam;
    (void)lParam;

    return 0;
}

int NewTestLayoutSettings_HandleMouseWheel(
    NewTestLayoutSettingsPanel *panel,
    WPARAM wParam,
    LPARAM lParam
)
{
    (void)panel;
    (void)wParam;
    (void)lParam;

    return 0;
}

int NewTestLayoutSettings_HandleCtlColorStatic(
    NewTestLayoutSettingsPanel *panel,
    HDC hdc,
    HWND hwnd,
    LRESULT *result
)
{
    int i;

    if (!panel || !hdc || !result)
        return 0;

    if (hwnd == panel->container)
    {
        SetBkColor(hdc, panel->config.overlayBackColor);
        *result = (LRESULT)panel->backBrush;
        return 1;
    }

    if (hwnd == panel->titleLabel)
    {
        SetBkColor(hdc, panel->config.overlayBackColor);
        SetTextColor(hdc, panel->config.overlayTitleColor);
        *result = (LRESULT)panel->backBrush;
        return 1;
    }

    for (i = 0; i < NTL_SETTINGS_INT_COUNT; i++)
    {
        if (hwnd == panel->intLabels[i])
        {
            SetBkColor(hdc, panel->config.overlayBackColor);
            SetTextColor(hdc, panel->config.labelColor);
            *result = (LRESULT)panel->backBrush;
            return 1;
        }
    }

    return 0;
}
