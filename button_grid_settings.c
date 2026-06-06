#include <windows.h>
#include <commctrl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "button_grid_internal.h"
#include "button_grid_settings.h"

#ifndef GA_ROOT
#define GA_ROOT 2
#endif

#ifndef TRACKBAR_CLASSA
#define TRACKBAR_CLASSA "msctls_trackbar32"
#endif

#ifndef TBM_SETRANGE
#define TBM_SETRANGE (WM_USER + 6)
#endif

#ifndef TBM_SETPOS
#define TBM_SETPOS (WM_USER + 5)
#endif

#ifndef TBM_GETPOS
#define TBM_GETPOS (WM_USER)
#endif

#ifndef TBS_AUTOTICKS
#define TBS_AUTOTICKS 0x0001
#endif

#ifndef TBS_TOOLTIPS
#define TBS_TOOLTIPS 0x0100
#endif

#define BUTTON_GRID_SETTINGS_CLASS_NAME "ButtonGridSettingsWindowClass"
#define BUTTON_GRID_SETTINGS_PROP_NAME "ButtonGridSettingsGrid"

#define BG_SETTINGS_ID_CLOSE 5001

#define BG_SETTINGS_ID_BASE 6000
#define BG_SETTINGS_ID_STEP 10

#define BG_SETTINGS_PART_LABEL 0
#define BG_SETTINGS_PART_PRIMARY 1
#define BG_SETTINGS_PART_RAW 2

#define BG_SETTINGS_WINDOW_WIDTH 760
#define BG_SETTINGS_WINDOW_HEIGHT 520
#define BG_SETTINGS_WINDOW_GAP 12

#define BG_SETTINGS_TOP 42
#define BG_SETTINGS_LEFT 12
#define BG_SETTINGS_ROW_HEIGHT 34
#define BG_SETTINGS_LABEL_WIDTH 170
#define BG_SETTINGS_PRIMARY_WIDTH 180
#define BG_SETTINGS_RAW_WIDTH 130
#define BG_SETTINGS_GAP 8
#define BG_SETTINGS_BOTTOM_PADDING 16

#define BG_SETTING_BOOL 1
#define BG_SETTING_INT 2
#define BG_SETTING_ENUM 3
#define BG_SETTING_COLOR 4
#define BG_SETTING_TEXT 5

typedef struct ButtonGridSettingOption
{
    const char *label;
    int value;
} ButtonGridSettingOption;

typedef struct ButtonGridSettingDefinition
{
    const char *key;
    const char *label;
    int type;
    size_t offset;
    int minValue;
    int maxValue;
    const ButtonGridSettingOption *options;
} ButtonGridSettingDefinition;

static LRESULT CALLBACK ButtonGrid_SettingsWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

static void ButtonGrid_SettingsLayoutControls(HWND pageHwnd);

static int g_settingsUpdatingControls = 0;

static const ButtonGridSettingOption g_boolOptions[] =
{
    { "Off", 0 },
    { "On", 1 },
    { NULL, 0 }
};

static const ButtonGridSettingOption g_layoutOptions[] =
{
    { "Horizontal", BUTTON_GRID_LAYOUT_HORIZONTAL },
    { "Vertical", BUTTON_GRID_LAYOUT_VERTICAL },
    { NULL, 0 }
};

static const ButtonGridSettingOption g_sizeModeOptions[] =
{
    { "Fixed", BUTTON_GRID_SIZE_FIXED },
    { "Match image size", BUTTON_GRID_SIZE_MATCH_IMAGE_SIZE },
    { "Aspect: horizontal", BUTTON_GRID_SIZE_MATCH_IMAGE_ASPECT_HORIZONTAL },
    { "Aspect: vertical", BUTTON_GRID_SIZE_MATCH_IMAGE_ASPECT_VERTICAL },
    { "Aspect: by layout", BUTTON_GRID_SIZE_MATCH_IMAGE_ASPECT_BY_LAYOUT },
    { NULL, 0 }
};

static const ButtonGridSettingOption g_borderStyleOptions[] =
{
    { "None", BUTTON_GRID_BORDER_STYLE_NONE },
    { "Simple", BUTTON_GRID_BORDER_STYLE_SIMPLE },
    { "Etched", BUTTON_GRID_BORDER_STYLE_ETCHED },
    { "Rounded", BUTTON_GRID_BORDER_STYLE_ROUNDED },
    { NULL, 0 }
};

static const ButtonGridSettingOption g_gearCornerOptions[] =
{
    { "Top left", BUTTON_GRID_GEAR_CORNER_TOP_LEFT },
    { "Top right", BUTTON_GRID_GEAR_CORNER_TOP_RIGHT },
    { "Bottom left", BUTTON_GRID_GEAR_CORNER_BOTTOM_LEFT },
    { "Bottom right", BUTTON_GRID_GEAR_CORNER_BOTTOM_RIGHT },
    { NULL, 0 }
};

static const ButtonGridSettingDefinition g_settings[] =
{
    { "buttonWidth",              "Button width",              BG_SETTING_INT,   offsetof(ButtonGrid, buttonWidth),              10, 400, NULL },
    { "buttonHeight",             "Button height",             BG_SETTING_INT,   offsetof(ButtonGrid, buttonHeight),             10, 400, NULL },
    { "horizontalSpacing",        "Horizontal spacing",        BG_SETTING_INT,   offsetof(ButtonGrid, horizontalSpacing),         0, 100, NULL },
    { "verticalSpacing",          "Vertical spacing",          BG_SETTING_INT,   offsetof(ButtonGrid, verticalSpacing),           0, 100, NULL },

    { "layout",                   "Layout",                    BG_SETTING_ENUM,  offsetof(ButtonGrid, layout),                    0,   0, g_layoutOptions },
    { "sizeMode",                 "Size mode",                 BG_SETTING_ENUM,  offsetof(ButtonGrid, sizeMode),                  0,   0, g_sizeModeOptions },

    { "showText",                 "Show button text",          BG_SETTING_BOOL,  offsetof(ButtonGrid, showText),                  0,   1, g_boolOptions },
    { "hidePartialButtons",       "Hide partial buttons",      BG_SETTING_BOOL,  offsetof(ButtonGrid, hidePartialButtons),        0,   1, g_boolOptions },
    { "resizeInLayoutSteps",      "Resize in layout steps",    BG_SETTING_BOOL,  offsetof(ButtonGrid, resizeInLayoutSteps),       0,   1, g_boolOptions },

    { "showBorder",               "Show border",               BG_SETTING_BOOL,  offsetof(ButtonGrid, showBorder),                0,   1, g_boolOptions },
    { "borderTitle",              "Border title",              BG_SETTING_TEXT,  offsetof(ButtonGrid, borderTitle),               0,   0, NULL },
    { "borderPadding",            "Border padding",            BG_SETTING_INT,   offsetof(ButtonGrid, borderPadding),             0, 100, NULL },
    { "borderTitleHeight",        "Border title height",       BG_SETTING_INT,   offsetof(ButtonGrid, borderTitleHeight),         0,  80, NULL },
    { "borderStyle",              "Border style",              BG_SETTING_ENUM,  offsetof(ButtonGrid, borderStyle),               0,   0, g_borderStyleOptions },
    { "borderThickness",          "Border thickness",          BG_SETTING_INT,   offsetof(ButtonGrid, borderThickness),           1,   8, NULL },
    { "borderCornerRadius",       "Border corner radius",      BG_SETTING_INT,   offsetof(ButtonGrid, borderCornerRadius),        0,  50, NULL },
    { "borderColor",              "Border color",              BG_SETTING_COLOR, offsetof(ButtonGrid, borderColor),               0,   0, NULL },
    { "borderLightColor",         "Border light color",        BG_SETTING_COLOR, offsetof(ButtonGrid, borderLightColor),          0,   0, NULL },
    { "borderShadowColor",        "Border shadow color",       BG_SETTING_COLOR, offsetof(ButtonGrid, borderShadowColor),         0,   0, NULL },
    { "borderTitleColor",         "Border title color",        BG_SETTING_COLOR, offsetof(ButtonGrid, borderTitleColor),          0,   0, NULL },
    { "borderTitleBackColor",     "Border title back color",   BG_SETTING_COLOR, offsetof(ButtonGrid, borderTitleBackColor),      0,   0, NULL },

    { "showGearIcon",             "Show gear icon",            BG_SETTING_BOOL,  offsetof(ButtonGrid, showGearIcon),              0,   1, g_boolOptions },
    { "gearCorner",               "Gear corner",               BG_SETTING_ENUM,  offsetof(ButtonGrid, gearCorner),                0,   0, g_gearCornerOptions },
    { "gearSize",                 "Gear size",                 BG_SETTING_INT,   offsetof(ButtonGrid, gearSize),                 10,  80, NULL },
    { "gearMargin",               "Gear margin",               BG_SETTING_INT,   offsetof(ButtonGrid, gearMargin),                0,  80, NULL },
    { "gearColor",                "Gear color",                BG_SETTING_COLOR, offsetof(ButtonGrid, gearColor),                 0,   0, NULL },
    { "gearBackColor",            "Gear background color",     BG_SETTING_COLOR, offsetof(ButtonGrid, gearBackColor),             0,   0, NULL },
    { "gearBorderColor",          "Gear border color",         BG_SETTING_COLOR, offsetof(ButtonGrid, gearBorderColor),           0,   0, NULL },

    { "backColor",                "Button back color",         BG_SETTING_COLOR, offsetof(ButtonGrid, backColor),                 0,   0, NULL },
    { "foreColor",                "Button text color",         BG_SETTING_COLOR, offsetof(ButtonGrid, foreColor),                 0,   0, NULL },
    { "generatedOffPictureColor", "Fallback OFF color",        BG_SETTING_COLOR, offsetof(ButtonGrid, generatedOffPictureColor),  0,   0, NULL },
    { "generatedOnPictureColor",  "Fallback ON color",         BG_SETTING_COLOR, offsetof(ButtonGrid, generatedOnPictureColor),   0,   0, NULL },
    { "generatedErrorPictureColor","Fallback error color",     BG_SETTING_COLOR, offsetof(ButtonGrid, generatedErrorPictureColor),0,   0, NULL }
};

#define BG_SETTINGS_COUNT ((int)(sizeof(g_settings) / sizeof(g_settings[0])))

static int BgIdLabel(int index)
{
    return BG_SETTINGS_ID_BASE + index * BG_SETTINGS_ID_STEP + BG_SETTINGS_PART_LABEL;
}

static int BgIdPrimary(int index)
{
    return BG_SETTINGS_ID_BASE + index * BG_SETTINGS_ID_STEP + BG_SETTINGS_PART_PRIMARY;
}

static int BgIdRaw(int index)
{
    return BG_SETTINGS_ID_BASE + index * BG_SETTINGS_ID_STEP + BG_SETTINGS_PART_RAW;
}

static int BgIdToIndex(int id)
{
    int rel;

    rel = id - BG_SETTINGS_ID_BASE;

    if (rel < 0)
        return -1;

    return rel / BG_SETTINGS_ID_STEP;
}

static int BgIdToPart(int id)
{
    int rel;

    rel = id - BG_SETTINGS_ID_BASE;

    if (rel < 0)
        return -1;

    return rel % BG_SETTINGS_ID_STEP;
}

static ButtonGrid *ButtonGrid_SettingsGetGrid(HWND hwnd)
{
    return (ButtonGrid *)GetProp(hwnd, BUTTON_GRID_SETTINGS_PROP_NAME);
}

static void ButtonGrid_SettingsSetGrid(HWND hwnd, ButtonGrid *grid)
{
    SetProp(hwnd, BUTTON_GRID_SETTINGS_PROP_NAME, (HANDLE)grid);
}

static int ClampInt(int value, int minValue, int maxValue)
{
    if (value < minValue)
        return minValue;

    if (value > maxValue)
        return maxValue;

    return value;
}

static int GetIntField(ButtonGrid *grid, const ButtonGridSettingDefinition *def)
{
    return *(int *)((char *)grid + def->offset);
}

static void SetIntField(ButtonGrid *grid, const ButtonGridSettingDefinition *def, int value)
{
    *(int *)((char *)grid + def->offset) = value;
}

static COLORREF GetColorField(ButtonGrid *grid, const ButtonGridSettingDefinition *def)
{
    return *(COLORREF *)((char *)grid + def->offset);
}

static void SetColorField(ButtonGrid *grid, const ButtonGridSettingDefinition *def, COLORREF value)
{
    *(COLORREF *)((char *)grid + def->offset) = value;
}

static char *GetTextField(ButtonGrid *grid, const ButtonGridSettingDefinition *def)
{
    return (char *)grid + def->offset;
}

static void ColorToText(COLORREF color, char *buffer, int bufferSize)
{
    wsprintf(
        buffer,
        "#%02X%02X%02X",
        GetRValue(color),
        GetGValue(color),
        GetBValue(color)
    );
}

static int HexDigitValue(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';

    if (c >= 'a' && c <= 'f')
        return 10 + c - 'a';

    if (c >= 'A' && c <= 'F')
        return 10 + c - 'A';

    return -1;
}

static int ParseHexColor(const char *text, COLORREF *color)
{
    int r1, r2, g1, g2, b1, b2;

    if (!text || text[0] != '#')
        return 0;

    r1 = HexDigitValue(text[1]);
    r2 = HexDigitValue(text[2]);
    g1 = HexDigitValue(text[3]);
    g2 = HexDigitValue(text[4]);
    b1 = HexDigitValue(text[5]);
    b2 = HexDigitValue(text[6]);

    if (r1 < 0 || r2 < 0 || g1 < 0 || g2 < 0 || b1 < 0 || b2 < 0)
        return 0;

    *color = RGB(r1 * 16 + r2, g1 * 16 + g2, b1 * 16 + b2);
    return 1;
}

static int ParseRgbColor(const char *text, COLORREF *color)
{
    int r, g, b;

    if (!text)
        return 0;

    if (sscanf(text, "RGB(%d,%d,%d)", &r, &g, &b) == 3 ||
        sscanf(text, "rgb(%d,%d,%d)", &r, &g, &b) == 3 ||
        sscanf(text, "%d,%d,%d", &r, &g, &b) == 3)
    {
        r = ClampInt(r, 0, 255);
        g = ClampInt(g, 0, 255);
        b = ClampInt(b, 0, 255);

        *color = RGB(r, g, b);
        return 1;
    }

    return 0;
}

static int ParseColorText(const char *text, COLORREF *color)
{
    if (ParseHexColor(text, color))
        return 1;

    if (ParseRgbColor(text, color))
        return 1;

    if (!text || !text[0])
        return 0;

    *color = (COLORREF)strtoul(text, NULL, 0);
    return 1;
}

static void ButtonGrid_SettingsApplyGridChange(ButtonGrid *grid)
{
    if (!grid)
        return;

    if (grid->buttonWidth < 1)
        grid->buttonWidth = 1;

    if (grid->buttonHeight < 1)
        grid->buttonHeight = 1;

    if (grid->horizontalSpacing < 0)
        grid->horizontalSpacing = 0;

    if (grid->verticalSpacing < 0)
        grid->verticalSpacing = 0;

    if (grid->layout != BUTTON_GRID_LAYOUT_HORIZONTAL &&
        grid->layout != BUTTON_GRID_LAYOUT_VERTICAL)
        grid->layout = BUTTON_GRID_LAYOUT_HORIZONTAL;

    grid->sizeMode = ButtonGrid_NormalizeSizeMode(grid->sizeMode);

    if (grid->borderStyle != BUTTON_GRID_BORDER_STYLE_NONE &&
        grid->borderStyle != BUTTON_GRID_BORDER_STYLE_SIMPLE &&
        grid->borderStyle != BUTTON_GRID_BORDER_STYLE_ETCHED &&
        grid->borderStyle != BUTTON_GRID_BORDER_STYLE_ROUNDED)
        grid->borderStyle = BUTTON_GRID_BORDER_STYLE_ETCHED;

    if (grid->borderPadding < 0)
        grid->borderPadding = 0;

    if (grid->borderTitleHeight < 0)
        grid->borderTitleHeight = 0;

    if (grid->borderThickness < 1)
        grid->borderThickness = 1;

    if (grid->borderThickness > 8)
        grid->borderThickness = 8;

    if (grid->borderCornerRadius < 0)
        grid->borderCornerRadius = 0;

    if (grid->gearCorner < BUTTON_GRID_GEAR_CORNER_TOP_LEFT ||
        grid->gearCorner > BUTTON_GRID_GEAR_CORNER_BOTTOM_RIGHT)
        grid->gearCorner = BUTTON_GRID_GEAR_CORNER_TOP_RIGHT;

    if (grid->gearSize < 8)
        grid->gearSize = 8;

    if (grid->gearMargin < 0)
        grid->gearMargin = 0;

    ButtonGrid_UpdateAllButtonSizes(grid);
    ButtonGrid_Layout(grid);
    ButtonGrid_RedrawAllButtons(grid);
    ButtonGrid_LayoutSettingsPage(grid);

    InvalidateRect(grid->hwnd, NULL, TRUE);
}

static void ButtonGrid_SettingsSetRawText(HWND pageHwnd, int index, ButtonGrid *grid)
{
    const ButtonGridSettingDefinition *def;
    char buffer[256];

    def = &g_settings[index];
    buffer[0] = '\0';

    if (def->type == BG_SETTING_COLOR)
        ColorToText(GetColorField(grid, def), buffer, sizeof(buffer));
    else if (def->type == BG_SETTING_TEXT)
        ButtonGrid_CopyText(buffer, sizeof(buffer), GetTextField(grid, def));
    else
        wsprintf(buffer, "%d", GetIntField(grid, def));

    SetWindowText(GetDlgItem(pageHwnd, BgIdRaw(index)), buffer);
}

static void ButtonGrid_SettingsRefreshOne(HWND pageHwnd, int index, ButtonGrid *grid)
{
    const ButtonGridSettingDefinition *def;
    HWND primary;
    int value;
    int i;

    def = &g_settings[index];
    primary = GetDlgItem(pageHwnd, BgIdPrimary(index));

    if (primary)
    {
        if (def->type == BG_SETTING_BOOL)
        {
            value = GetIntField(grid, def);
            SendMessage(primary, BM_SETCHECK, value ? BST_CHECKED : BST_UNCHECKED, 0);
        }
        else if (def->type == BG_SETTING_INT)
        {
            value = GetIntField(grid, def);
            SendMessage(primary, TBM_SETRANGE, TRUE, MAKELONG(def->minValue, def->maxValue));
            SendMessage(primary, TBM_SETPOS, TRUE, value);
        }
        else if (def->type == BG_SETTING_ENUM)
        {
            value = GetIntField(grid, def);
            SendMessage(primary, CB_RESETCONTENT, 0, 0);

            if (def->options)
            {
                for (i = 0; def->options[i].label; i++)
                {
                    int item;

                    item = (int)SendMessage(primary, CB_ADDSTRING, 0, (LPARAM)def->options[i].label);
                    SendMessage(primary, CB_SETITEMDATA, item, def->options[i].value);

                    if (def->options[i].value == value)
                        SendMessage(primary, CB_SETCURSEL, item, 0);
                }
            }
        }
    }

    ButtonGrid_SettingsSetRawText(pageHwnd, index, grid);
}

static void ButtonGrid_SettingsRefreshAll(HWND pageHwnd)
{
    ButtonGrid *grid;
    int i;

    grid = ButtonGrid_SettingsGetGrid(pageHwnd);

    if (!grid)
        return;

    g_settingsUpdatingControls = 1;

    for (i = 0; i < BG_SETTINGS_COUNT; i++)
        ButtonGrid_SettingsRefreshOne(pageHwnd, i, grid);

    g_settingsUpdatingControls = 0;
}

static void ButtonGrid_SettingsApplyValue(
    HWND pageHwnd,
    int index,
    const char *textValue,
    int valueFromPrimary,
    int usePrimary
)
{
    ButtonGrid *grid;
    const ButtonGridSettingDefinition *def;
    int value;
    COLORREF color;

    if (index < 0 || index >= BG_SETTINGS_COUNT)
        return;

    grid = ButtonGrid_SettingsGetGrid(pageHwnd);

    if (!grid)
        return;

    def = &g_settings[index];

    if (def->type == BG_SETTING_BOOL)
    {
        value = usePrimary ? (valueFromPrimary ? 1 : 0) : (atoi(textValue) ? 1 : 0);
        SetIntField(grid, def, value);
    }
    else if (def->type == BG_SETTING_INT)
    {
        value = usePrimary ? valueFromPrimary : atoi(textValue);
        value = ClampInt(value, def->minValue, def->maxValue);
        SetIntField(grid, def, value);
    }
    else if (def->type == BG_SETTING_ENUM)
    {
        value = usePrimary ? valueFromPrimary : atoi(textValue);
        SetIntField(grid, def, value);
    }
    else if (def->type == BG_SETTING_COLOR)
    {
        if (ParseColorText(textValue, &color))
            SetColorField(grid, def, color);
    }
    else if (def->type == BG_SETTING_TEXT)
    {
        ButtonGrid_CopyText(GetTextField(grid, def), BUTTON_GRID_TITLE_SIZE, textValue);
    }

    ButtonGrid_SettingsApplyGridChange(grid);

    g_settingsUpdatingControls = 1;
    ButtonGrid_SettingsRefreshOne(pageHwnd, index, grid);
    g_settingsUpdatingControls = 0;
}

static void ButtonGrid_SettingsApplyRawEdit(HWND pageHwnd, int index)
{
    char buffer[256];

    buffer[0] = '\0';
    GetWindowText(GetDlgItem(pageHwnd, BgIdRaw(index)), buffer, sizeof(buffer));

    ButtonGrid_SettingsApplyValue(pageHwnd, index, buffer, 0, 0);
}

static void ButtonGrid_SettingsApplyPrimary(HWND pageHwnd, int index)
{
    const ButtonGridSettingDefinition *def;
    HWND primary;
    int value;
    int selection;

    if (index < 0 || index >= BG_SETTINGS_COUNT)
        return;

    def = &g_settings[index];
    primary = GetDlgItem(pageHwnd, BgIdPrimary(index));

    if (!primary)
        return;

    value = 0;

    if (def->type == BG_SETTING_BOOL)
    {
        value = SendMessage(primary, BM_GETCHECK, 0, 0) == BST_CHECKED;
    }
    else if (def->type == BG_SETTING_INT)
    {
        value = (int)SendMessage(primary, TBM_GETPOS, 0, 0);
    }
    else if (def->type == BG_SETTING_ENUM)
    {
        selection = (int)SendMessage(primary, CB_GETCURSEL, 0, 0);

        if (selection < 0)
            return;

        value = (int)SendMessage(primary, CB_GETITEMDATA, selection, 0);
    }
    else
    {
        return;
    }

    ButtonGrid_SettingsApplyValue(pageHwnd, index, NULL, value, 1);
}

static void ButtonGrid_SettingsCreateControls(HWND pageHwnd, ButtonGrid *grid)
{
    int i;

    InitCommonControls();

    CreateWindowEx(
        0,
        "STATIC",
        "Grid settings",
        WS_CHILD | WS_VISIBLE,
        12,
        10,
        180,
        22,
        pageHwnd,
        NULL,
        grid->hInstance,
        NULL
    );

    CreateWindowEx(
        0,
        "BUTTON",
        "Close",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        0,
        0,
        70,
        24,
        pageHwnd,
        (HMENU)BG_SETTINGS_ID_CLOSE,
        grid->hInstance,
        NULL
    );

    for (i = 0; i < BG_SETTINGS_COUNT; i++)
    {
        const ButtonGridSettingDefinition *def;

        def = &g_settings[i];

        CreateWindowEx(
            0,
            "STATIC",
            def->label,
            WS_CHILD | WS_VISIBLE,
            0,
            0,
            10,
            10,
            pageHwnd,
            (HMENU)BgIdLabel(i),
            grid->hInstance,
            NULL
        );

        if (def->type == BG_SETTING_BOOL)
        {
            CreateWindowEx(
                0,
                "BUTTON",
                "On",
                WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
                0,
                0,
                10,
                10,
                pageHwnd,
                (HMENU)BgIdPrimary(i),
                grid->hInstance,
                NULL
            );
        }
        else if (def->type == BG_SETTING_INT)
        {
            CreateWindowEx(
                0,
                TRACKBAR_CLASSA,
                "",
                WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_TOOLTIPS,
                0,
                0,
                10,
                10,
                pageHwnd,
                (HMENU)BgIdPrimary(i),
                grid->hInstance,
                NULL
            );
        }
        else if (def->type == BG_SETTING_ENUM)
        {
            CreateWindowEx(
                0,
                "COMBOBOX",
                "",
                WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
                0,
                0,
                10,
                200,
                pageHwnd,
                (HMENU)BgIdPrimary(i),
                grid->hInstance,
                NULL
            );
        }

        CreateWindowEx(
            WS_EX_CLIENTEDGE,
            "EDIT",
            "",
            WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
            0,
            0,
            10,
            10,
            pageHwnd,
            (HMENU)BgIdRaw(i),
            grid->hInstance,
            NULL
        );
    }

    ButtonGrid_SettingsRefreshAll(pageHwnd);
}

void ButtonGrid_LayoutSettingsPage(ButtonGrid *grid)
{
    if (!grid || !grid->settingsPageHwnd)
        return;

    ButtonGrid_SettingsLayoutControls(grid->settingsPageHwnd);
}

static void ButtonGrid_SettingsLayoutControls(HWND pageHwnd)
{
    RECT rc;
    SCROLLINFO si;
    int clientW;
    int clientH;
    int contentH;
    int maxScroll;
    int scrollY;
    int i;
    int labelX;
    int primaryX;
    int rawX;
    int rowY;
    ButtonGrid *grid;

    grid = ButtonGrid_SettingsGetGrid(pageHwnd);

    if (!grid)
        return;

    GetClientRect(pageHwnd, &rc);

    clientW = rc.right - rc.left;
    clientH = rc.bottom - rc.top;

    contentH = BG_SETTINGS_TOP + BG_SETTINGS_COUNT * BG_SETTINGS_ROW_HEIGHT + BG_SETTINGS_BOTTOM_PADDING;
    maxScroll = contentH - clientH;

    if (maxScroll < 0)
        maxScroll = 0;

    ZeroMemory(&si, sizeof(si));

    si.cbSize = sizeof(si);
    si.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;
    GetScrollInfo(pageHwnd, SB_VERT, &si);

    scrollY = si.nPos;

    if (scrollY > maxScroll)
        scrollY = maxScroll;

    si.cbSize = sizeof(si);
    si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
    si.nMin = 0;
    si.nMax = contentH;
    si.nPage = clientH;
    si.nPos = scrollY;
    SetScrollInfo(pageHwnd, SB_VERT, &si, TRUE);

    MoveWindow(
        GetDlgItem(pageHwnd, BG_SETTINGS_ID_CLOSE),
        clientW - 84,
        8,
        72,
        24,
        TRUE
    );

    labelX = BG_SETTINGS_LEFT;
    primaryX = labelX + BG_SETTINGS_LABEL_WIDTH + BG_SETTINGS_GAP;
    rawX = primaryX + BG_SETTINGS_PRIMARY_WIDTH + BG_SETTINGS_GAP;

    for (i = 0; i < BG_SETTINGS_COUNT; i++)
    {
        const ButtonGridSettingDefinition *def;
        HWND primary;

        def = &g_settings[i];
        primary = GetDlgItem(pageHwnd, BgIdPrimary(i));

        rowY = BG_SETTINGS_TOP + i * BG_SETTINGS_ROW_HEIGHT - scrollY;

        MoveWindow(
            GetDlgItem(pageHwnd, BgIdLabel(i)),
            labelX,
            rowY + 5,
            BG_SETTINGS_LABEL_WIDTH,
            22,
            TRUE
        );

        if (primary)
        {
            MoveWindow(
                primary,
                primaryX,
                rowY,
                BG_SETTINGS_PRIMARY_WIDTH,
                def->type == BG_SETTING_ENUM ? 220 : 26,
                TRUE
            );
        }

        if (def->type == BG_SETTING_COLOR || def->type == BG_SETTING_TEXT)
        {
            MoveWindow(
                GetDlgItem(pageHwnd, BgIdRaw(i)),
                primaryX,
                rowY,
                BG_SETTINGS_PRIMARY_WIDTH + BG_SETTINGS_GAP + BG_SETTINGS_RAW_WIDTH,
                24,
                TRUE
            );
        }
        else
        {
            MoveWindow(
                GetDlgItem(pageHwnd, BgIdRaw(i)),
                rawX,
                rowY,
                BG_SETTINGS_RAW_WIDTH,
                24,
                TRUE
            );
        }
    }
}

static void ButtonGrid_SettingsScroll(HWND pageHwnd, int request, int wheelDelta)
{
    SCROLLINFO si;
    int oldPos;
    int newPos;

    ZeroMemory(&si, sizeof(si));

    si.cbSize = sizeof(si);
    si.fMask = SIF_ALL;

    GetScrollInfo(pageHwnd, SB_VERT, &si);

    oldPos = si.nPos;
    newPos = oldPos;

    if (request == SB_LINEUP)
        newPos -= BG_SETTINGS_ROW_HEIGHT;
    else if (request == SB_LINEDOWN)
        newPos += BG_SETTINGS_ROW_HEIGHT;
    else if (request == SB_PAGEUP)
        newPos -= (int)si.nPage;
    else if (request == SB_PAGEDOWN)
        newPos += (int)si.nPage;
    else if (request == SB_THUMBTRACK)
        newPos = si.nTrackPos;
    else if (request == -1)
        newPos -= wheelDelta / 3;

    if (newPos < si.nMin)
        newPos = si.nMin;

    if (newPos > si.nMax - (int)si.nPage + 1)
        newPos = si.nMax - (int)si.nPage + 1;

    if (newPos < 0)
        newPos = 0;

    si.fMask = SIF_POS;
    si.nPos = newPos;

    SetScrollInfo(pageHwnd, SB_VERT, &si, TRUE);

    if (newPos != oldPos)
        ButtonGrid_SettingsLayoutControls(pageHwnd);
}

BOOL ButtonGrid_SettingsRegisterClass(HINSTANCE hInstance)
{
    static int registered = 0;
    WNDCLASS wc;

    if (registered)
        return TRUE;

    InitCommonControls();

    ZeroMemory(&wc, sizeof(wc));

    wc.lpfnWndProc = ButtonGrid_SettingsWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = BUTTON_GRID_SETTINGS_CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClass(&wc))
        return FALSE;

    registered = 1;
    return TRUE;
}

static void ButtonGrid_GetSettingsWindowRect(ButtonGrid *grid, RECT *outRc)
{
    RECT gridRc;
    RECT workRc;
    int w;
    int h;
    int x;
    int y;

    SetRect(outRc, 100, 100, 100 + BG_SETTINGS_WINDOW_WIDTH, 100 + BG_SETTINGS_WINDOW_HEIGHT);

    if (!grid || !grid->hwnd)
        return;

    GetWindowRect(grid->hwnd, &gridRc);

    if (!SystemParametersInfo(SPI_GETWORKAREA, 0, &workRc, 0))
    {
        workRc.left = 0;
        workRc.top = 0;
        workRc.right = GetSystemMetrics(SM_CXSCREEN);
        workRc.bottom = GetSystemMetrics(SM_CYSCREEN);
    }

    w = BG_SETTINGS_WINDOW_WIDTH;
    h = BG_SETTINGS_WINDOW_HEIGHT;

    if (w > workRc.right - workRc.left)
        w = workRc.right - workRc.left;

    if (h > workRc.bottom - workRc.top)
        h = workRc.bottom - workRc.top;

    x = gridRc.right + BG_SETTINGS_WINDOW_GAP;
    y = gridRc.top;

    if (x + w > workRc.right)
        x = gridRc.left - w - BG_SETTINGS_WINDOW_GAP;

    if (x < workRc.left)
        x = workRc.right - w - BG_SETTINGS_WINDOW_GAP;

    if (x < workRc.left)
        x = workRc.left;

    if (y + h > workRc.bottom)
        y = workRc.bottom - h - BG_SETTINGS_WINDOW_GAP;

    if (y < workRc.top)
        y = workRc.top;

    SetRect(outRc, x, y, x + w, y + h);
}

static void ButtonGrid_PositionSettingsWindow(ButtonGrid *grid, HWND pageHwnd)
{
    RECT rc;

    if (!grid || !pageHwnd)
        return;

    ButtonGrid_GetSettingsWindowRect(grid, &rc);

    SetWindowPos(
        pageHwnd,
        HWND_TOP,
        rc.left,
        rc.top,
        rc.right - rc.left,
        rc.bottom - rc.top,
        SWP_NOACTIVATE
    );
}

static HWND ButtonGrid_GetOwnerWindow(ButtonGrid *grid)
{
    HWND owner;

    owner = NULL;

    if (grid && grid->hwnd)
        owner = GetAncestor(grid->hwnd, GA_ROOT);

    if (!owner && grid)
        owner = grid->hwnd;

    return owner;
}

static HWND ButtonGrid_CreateSettingsPage(ButtonGrid *grid)
{
    RECT rc;
    HWND owner;

    if (!grid)
        return NULL;

    if (grid->settingsPageHwnd)
        return grid->settingsPageHwnd;

    if (!ButtonGrid_SettingsRegisterClass(grid->hInstance))
        return NULL;

    ButtonGrid_GetSettingsWindowRect(grid, &rc);
    owner = ButtonGrid_GetOwnerWindow(grid);

    grid->settingsPageHwnd = CreateWindowEx(
        WS_EX_TOOLWINDOW,
        BUTTON_GRID_SETTINGS_CLASS_NAME,
        "Grid Settings",
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_VSCROLL,
        rc.left,
        rc.top,
        rc.right - rc.left,
        rc.bottom - rc.top,
        owner,
        NULL,
        grid->hInstance,
        grid
    );

    return grid->settingsPageHwnd;
}

void ButtonGrid_ShowSettingsPage(ButtonGrid *grid, int show)
{
    HWND pageHwnd;

    if (!grid)
        return;

    if (show)
    {
        pageHwnd = ButtonGrid_CreateSettingsPage(grid);

        if (!pageHwnd)
            return;

        grid->settingsPageVisible = 1;

        if (!IsWindowVisible(pageHwnd))
            ButtonGrid_PositionSettingsWindow(grid, pageHwnd);

        ButtonGrid_SettingsRefreshAll(pageHwnd);
        ButtonGrid_SettingsLayoutControls(pageHwnd);

        ShowWindow(pageHwnd, SW_SHOWNORMAL);
        BringWindowToTop(pageHwnd);
        SetForegroundWindow(pageHwnd);
        SetFocus(pageHwnd);
    }
    else
    {
        grid->settingsPageVisible = 0;

        if (grid->settingsPageHwnd)
            ShowWindow(grid->settingsPageHwnd, SW_HIDE);
    }

    InvalidateRect(grid->hwnd, NULL, TRUE);
}

void ButtonGrid_ToggleSettingsPage(ButtonGrid *grid)
{
    if (!grid)
        return;

    ButtonGrid_ShowSettingsPage(grid, !grid->settingsPageVisible);
}

void ButtonGrid_DestroySettingsPage(ButtonGrid *grid)
{
    HWND pageHwnd;

    if (!grid)
        return;

    pageHwnd = grid->settingsPageHwnd;

    grid->settingsPageHwnd = NULL;
    grid->settingsPageVisible = 0;

    if (pageHwnd)
        DestroyWindow(pageHwnd);
}

static void ButtonGrid_GetGearRect(ButtonGrid *grid, RECT *rc)
{
    RECT client;
    int size;
    int margin;

    SetRectEmpty(rc);

    if (!grid)
        return;

    GetClientRect(grid->hwnd, &client);

    size = grid->gearSize;
    margin = grid->gearMargin;

    if (size < 8)
        size = 8;

    if (margin < 0)
        margin = 0;

    if (grid->gearCorner == BUTTON_GRID_GEAR_CORNER_TOP_LEFT)
    {
        rc->left = client.left + margin;
        rc->top = client.top + margin;
    }
    else if (grid->gearCorner == BUTTON_GRID_GEAR_CORNER_BOTTOM_LEFT)
    {
        rc->left = client.left + margin;
        rc->top = client.bottom - margin - size;
    }
    else if (grid->gearCorner == BUTTON_GRID_GEAR_CORNER_BOTTOM_RIGHT)
    {
        rc->left = client.right - margin - size;
        rc->top = client.bottom - margin - size;
    }
    else
    {
        rc->left = client.right - margin - size;
        rc->top = client.top + margin;
    }

    rc->right = rc->left + size;
    rc->bottom = rc->top + size;
}

void ButtonGrid_DrawGearIcon(ButtonGrid *grid, HDC hdc)
{
    RECT rc;
    int cx;
    int cy;
    int size;
    int outerR;
    int innerR;
    HBRUSH backBrush;
    HBRUSH gearBrush;
    HBRUSH holeBrush;
    HPEN borderPen;
    HPEN gearPen;
    HGDIOBJ oldBrush;
    HGDIOBJ oldPen;

    if (!grid || !hdc)
        return;

    if (!grid->showGearIcon)
        return;

    ButtonGrid_GetGearRect(grid, &rc);

    size = rc.right - rc.left;

    if (size < 8)
        return;

    cx = (rc.left + rc.right) / 2;
    cy = (rc.top + rc.bottom) / 2;

    outerR = size / 4;
    innerR = size / 10;

    if (innerR < 2)
        innerR = 2;

    backBrush = CreateSolidBrush(grid->gearBackColor);
    borderPen = CreatePen(PS_SOLID, 1, grid->gearBorderColor);

    oldBrush = SelectObject(hdc, backBrush);
    oldPen = SelectObject(hdc, borderPen);

    RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, 6, 6);

    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);

    DeleteObject(borderPen);
    DeleteObject(backBrush);

    gearBrush = CreateSolidBrush(grid->gearColor);
    gearPen = CreatePen(PS_SOLID, 2, grid->gearColor);

    oldBrush = SelectObject(hdc, gearBrush);
    oldPen = SelectObject(hdc, gearPen);

    Rectangle(hdc, cx - 2, rc.top + 4, cx + 2, rc.top + 10);
    Rectangle(hdc, cx - 2, rc.bottom - 10, cx + 2, rc.bottom - 4);
    Rectangle(hdc, rc.left + 4, cy - 2, rc.left + 10, cy + 2);
    Rectangle(hdc, rc.right - 10, cy - 2, rc.right - 4, cy + 2);

    MoveToEx(hdc, rc.left + 7, rc.top + 7, NULL);
    LineTo(hdc, cx, cy);

    MoveToEx(hdc, rc.right - 7, rc.top + 7, NULL);
    LineTo(hdc, cx, cy);

    MoveToEx(hdc, rc.left + 7, rc.bottom - 7, NULL);
    LineTo(hdc, cx, cy);

    MoveToEx(hdc, rc.right - 7, rc.bottom - 7, NULL);
    LineTo(hdc, cx, cy);

    Ellipse(hdc, cx - outerR, cy - outerR, cx + outerR, cy + outerR);

    holeBrush = CreateSolidBrush(grid->gearBackColor);
    SelectObject(hdc, holeBrush);

    Ellipse(hdc, cx - innerR, cy - innerR, cx + innerR, cy + innerR);

    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);

    DeleteObject(holeBrush);
    DeleteObject(gearPen);
    DeleteObject(gearBrush);
}

int ButtonGrid_HandleGearClick(ButtonGrid *grid, int x, int y)
{
    RECT rc;

    if (!grid)
        return 0;

    if (!grid->showGearIcon)
        return 0;

    ButtonGrid_GetGearRect(grid, &rc);

    if (x < rc.left || x >= rc.right || y < rc.top || y >= rc.bottom)
        return 0;

    ButtonGrid_ToggleSettingsPage(grid);
    return 1;
}

static LRESULT CALLBACK ButtonGrid_SettingsWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    ButtonGrid *grid;

    grid = ButtonGrid_SettingsGetGrid(hwnd);

    switch (msg)
    {
        case WM_CREATE:
        {
            CREATESTRUCT *cs;

            cs = (CREATESTRUCT *)lParam;
            grid = (ButtonGrid *)cs->lpCreateParams;

            ButtonGrid_SettingsSetGrid(hwnd, grid);
            ButtonGrid_SettingsCreateControls(hwnd, grid);
            ButtonGrid_SettingsLayoutControls(hwnd);

            return 0;
        }

        case WM_SIZE:
        {
            ButtonGrid_SettingsLayoutControls(hwnd);
            return 0;
        }

        case WM_CLOSE:
        {
            if (grid)
                ButtonGrid_ShowSettingsPage(grid, 0);
            else
                DestroyWindow(hwnd);

            return 0;
        }

        case WM_VSCROLL:
        {
            ButtonGrid_SettingsScroll(hwnd, LOWORD(wParam), 0);
            return 0;
        }

        case WM_MOUSEWHEEL:
        {
            ButtonGrid_SettingsScroll(hwnd, -1, GET_WHEEL_DELTA_WPARAM(wParam));
            return 0;
        }

        case WM_COMMAND:
        {
            int id;
            int code;
            int index;
            int part;

            id = LOWORD(wParam);
            code = HIWORD(wParam);

            if (id == BG_SETTINGS_ID_CLOSE)
            {
                ButtonGrid_ShowSettingsPage(grid, 0);
                return 0;
            }

            if (g_settingsUpdatingControls)
                return 0;

            index = BgIdToIndex(id);
            part = BgIdToPart(id);

            if (index >= 0 && index < BG_SETTINGS_COUNT)
            {
                if (part == BG_SETTINGS_PART_PRIMARY)
                {
                    if (code == BN_CLICKED || code == CBN_SELCHANGE)
                    {
                        ButtonGrid_SettingsApplyPrimary(hwnd, index);
                        return 0;
                    }
                }
                else if (part == BG_SETTINGS_PART_RAW)
                {
                    if (code == EN_CHANGE)
                    {
                        ButtonGrid_SettingsApplyRawEdit(hwnd, index);
                        return 0;
                    }
                }
            }

            break;
        }

        case WM_HSCROLL:
        {
            HWND source;
            int controlId;
            int index;

            if (g_settingsUpdatingControls)
                return 0;

            source = (HWND)lParam;

            if (!source)
                return 0;

            controlId = GetDlgCtrlID(source);
            index = BgIdToIndex(controlId);

            if (index >= 0 && index < BG_SETTINGS_COUNT)
            {
                ButtonGrid_SettingsApplyPrimary(hwnd, index);
                return 0;
            }

            break;
        }

        case WM_KEYDOWN:
        {
            if (wParam == VK_ESCAPE)
            {
                ButtonGrid_ShowSettingsPage(grid, 0);
                return 0;
            }

            break;
        }

        case WM_NCDESTROY:
        {
            if (grid && grid->settingsPageHwnd == hwnd)
            {
                grid->settingsPageHwnd = NULL;
                grid->settingsPageVisible = 0;
            }

            RemoveProp(hwnd, BUTTON_GRID_SETTINGS_PROP_NAME);
            break;
        }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}