#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "button_grid_internal.h"
#include "button_grid_settings.h"
#include "button_grid_settings_internal.h"

int ButtonGrid_SettingsClampInt(int value, int minValue, int maxValue)
{
    if (value < minValue)
        return minValue;

    if (value > maxValue)
        return maxValue;

    return value;
}

int ButtonGrid_SettingsGetIntField(
    ButtonGrid *grid,
    const ButtonGridSettingDefinition *def
)
{
    return *(int *)((char *)grid + def->offset);
}

void ButtonGrid_SettingsSetIntField(
    ButtonGrid *grid,
    const ButtonGridSettingDefinition *def,
    int value
)
{
    *(int *)((char *)grid + def->offset) = value;
}

COLORREF ButtonGrid_SettingsGetColorField(
    ButtonGrid *grid,
    const ButtonGridSettingDefinition *def
)
{
    return *(COLORREF *)((char *)grid + def->offset);
}

void ButtonGrid_SettingsSetColorField(
    ButtonGrid *grid,
    const ButtonGridSettingDefinition *def,
    COLORREF value
)
{
    *(COLORREF *)((char *)grid + def->offset) = value;
}

char *ButtonGrid_SettingsGetTextField(
    ButtonGrid *grid,
    const ButtonGridSettingDefinition *def
)
{
    return (char *)grid + def->offset;
}

static void ButtonGrid_SettingsColorToText(
    COLORREF color,
    char *buffer,
    int bufferSize
)
{
    wsprintf(
        buffer,
        "#%02X%02X%02X",
        GetRValue(color),
        GetGValue(color),
        GetBValue(color)
    );
}

static int ButtonGrid_SettingsHexDigitValue(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';

    if (c >= 'a' && c <= 'f')
        return 10 + c - 'a';

    if (c >= 'A' && c <= 'F')
        return 10 + c - 'A';

    return -1;
}

static int ButtonGrid_SettingsParseHexColor(
    const char *text,
    COLORREF *color
)
{
    int r1;
    int r2;
    int g1;
    int g2;
    int b1;
    int b2;

    if (!text || text[0] != '#')
        return 0;

    if (lstrlen(text) < 7)
        return 0;

    r1 = ButtonGrid_SettingsHexDigitValue(text[1]);
    r2 = ButtonGrid_SettingsHexDigitValue(text[2]);
    g1 = ButtonGrid_SettingsHexDigitValue(text[3]);
    g2 = ButtonGrid_SettingsHexDigitValue(text[4]);
    b1 = ButtonGrid_SettingsHexDigitValue(text[5]);
    b2 = ButtonGrid_SettingsHexDigitValue(text[6]);

    if (r1 < 0 || r2 < 0 || g1 < 0 || g2 < 0 || b1 < 0 || b2 < 0)
        return 0;

    *color = RGB(r1 * 16 + r2, g1 * 16 + g2, b1 * 16 + b2);
    return 1;
}

static int ButtonGrid_SettingsParseRgbColor(
    const char *text,
    COLORREF *color
)
{
    int r;
    int g;
    int b;

    if (!text)
        return 0;

    if (sscanf(text, "RGB(%d,%d,%d)", &r, &g, &b) == 3 ||
        sscanf(text, "rgb(%d,%d,%d)", &r, &g, &b) == 3 ||
        sscanf(text, "%d,%d,%d", &r, &g, &b) == 3)
    {
        r = ButtonGrid_SettingsClampInt(r, 0, 255);
        g = ButtonGrid_SettingsClampInt(g, 0, 255);
        b = ButtonGrid_SettingsClampInt(b, 0, 255);

        *color = RGB(r, g, b);
        return 1;
    }

    return 0;
}

static int ButtonGrid_SettingsParseColorText(
    const char *text,
    COLORREF *color
)
{
    if (ButtonGrid_SettingsParseHexColor(text, color))
        return 1;

    if (ButtonGrid_SettingsParseRgbColor(text, color))
        return 1;

    if (!text || !text[0])
        return 0;

    *color = (COLORREF)strtoul(text, NULL, 0);
    return 1;
}

void ButtonGrid_SettingsWriteRawText(HWND pageHwnd, int index, ButtonGrid *grid)
{
    const ButtonGridSettingDefinition *def;
    char buffer[256];

    def = ButtonGrid_SettingsGetDefinition(index);

    if (!def)
        return;

    buffer[0] = '\0';

    if (def->type == BG_SETTING_COLOR)
    {
        ButtonGrid_SettingsColorToText(
            ButtonGrid_SettingsGetColorField(grid, def),
            buffer,
            sizeof(buffer)
        );
    }
    else if (def->type == BG_SETTING_TEXT)
    {
        ButtonGrid_CopyText(
            buffer,
            sizeof(buffer),
            ButtonGrid_SettingsGetTextField(grid, def)
        );
    }
    else
    {
        wsprintf(
            buffer,
            "%d",
            ButtonGrid_SettingsGetIntField(grid, def)
        );
    }

    SetWindowText(
        ButtonGrid_SettingsGetRawControl(pageHwnd, index),
        buffer
    );
}

static void ButtonGrid_SettingsNormalizeLiveGrid(ButtonGrid *grid)
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
    {
        grid->layout = BUTTON_GRID_LAYOUT_HORIZONTAL;
    }

    grid->sizeMode = ButtonGrid_NormalizeSizeMode(grid->sizeMode);

    grid->showText = grid->showText ? 1 : 0;
    grid->hidePartialButtons = grid->hidePartialButtons ? 1 : 0;
    grid->resizeInLayoutSteps = grid->resizeInLayoutSteps ? 1 : 0;
    grid->settingsWheelScrub = grid->settingsWheelScrub ? 1 : 0;

    if (grid->borderStyle != BUTTON_GRID_BORDER_STYLE_NONE &&
        grid->borderStyle != BUTTON_GRID_BORDER_STYLE_SIMPLE &&
        grid->borderStyle != BUTTON_GRID_BORDER_STYLE_ETCHED &&
        grid->borderStyle != BUTTON_GRID_BORDER_STYLE_ROUNDED)
    {
        grid->borderStyle = BUTTON_GRID_BORDER_STYLE_ETCHED;
    }

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
    {
        grid->gearCorner = BUTTON_GRID_GEAR_CORNER_TOP_RIGHT;
    }

    if (grid->gearSize < 8)
        grid->gearSize = 8;

    if (grid->gearMargin < 0)
        grid->gearMargin = 0;
}

void ButtonGrid_SettingsApplyGridChange(ButtonGrid *grid)
{
    if (!grid)
        return;

    ButtonGrid_SettingsNormalizeLiveGrid(grid);

    ButtonGrid_UpdateAllButtonSizes(grid);
    ButtonGrid_Layout(grid);
    ButtonGrid_RedrawAllButtons(grid);
    ButtonGrid_LayoutSettingsPage(grid);

    InvalidateRect(grid->hwnd, NULL, TRUE);
}

void ButtonGrid_SettingsApplyValue(
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

    if (index < 0 || index >= ButtonGrid_SettingsGetCount())
        return;

    grid = ButtonGrid_SettingsGetGrid(pageHwnd);

    if (!grid)
        return;

    def = ButtonGrid_SettingsGetDefinition(index);

    if (!def)
        return;

    if (def->type == BG_SETTING_BOOL)
    {
        if (usePrimary)
            value = valueFromPrimary ? 1 : 0;
        else
            value = atoi(textValue) ? 1 : 0;

        ButtonGrid_SettingsSetIntField(grid, def, value);
    }
    else if (def->type == BG_SETTING_INT)
    {
        if (usePrimary)
            value = valueFromPrimary;
        else
            value = atoi(textValue);

        value = ButtonGrid_SettingsClampInt(
            value,
            def->minValue,
            def->maxValue
        );

        ButtonGrid_SettingsSetIntField(grid, def, value);
    }
    else if (def->type == BG_SETTING_ENUM)
    {
        if (usePrimary)
            value = valueFromPrimary;
        else
            value = atoi(textValue);

        ButtonGrid_SettingsSetIntField(grid, def, value);
    }
    else if (def->type == BG_SETTING_COLOR)
    {
        if (ButtonGrid_SettingsParseColorText(textValue, &color))
            ButtonGrid_SettingsSetColorField(grid, def, color);
    }
    else if (def->type == BG_SETTING_TEXT)
    {
        ButtonGrid_CopyText(
            ButtonGrid_SettingsGetTextField(grid, def),
            BUTTON_GRID_TITLE_SIZE,
            textValue
        );
    }

    ButtonGrid_SettingsApplyGridChange(grid);

    ButtonGrid_SettingsBeginControlUpdate();
    ButtonGrid_SettingsRefreshOne(pageHwnd, index, grid);
    ButtonGrid_SettingsEndControlUpdate();
}