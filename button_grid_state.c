#include "button_grid_internal.h"

void ButtonGrid_RedrawButton(ButtonGrid *grid, int index)
{
    if (!grid || !grid->buttons)
        return;

    if (index < 0 || index >= grid->buttonCount)
        return;

    if (!grid->buttons[index].hwnd)
        return;

    InvalidateRect(grid->buttons[index].hwnd, NULL, TRUE);
    UpdateWindow(grid->buttons[index].hwnd);
}

void ButtonGrid_RedrawAllButtons(ButtonGrid *grid)
{
    int i;

    if (!grid || !grid->buttons)
        return;

    for (i = 0; i < grid->buttonCount; i++)
        ButtonGrid_RedrawButton(grid, i);

    ButtonGrid_RedrawContainer(grid->hwnd);
}

static void ButtonGrid_SelectRadioButton(ButtonGrid *grid, int selectedIndex)
{
    int i;
    int group;

    if (!grid || selectedIndex < 0 || selectedIndex >= grid->buttonCount)
        return;

    group = grid->buttons[selectedIndex].radioGroup;

    if (group == 0)
    {
        grid->buttons[selectedIndex].isOn = 1;
        ButtonGrid_RedrawButton(grid, selectedIndex);
        return;
    }

    for (i = 0; i < grid->buttonCount; i++)
    {
        if (grid->buttons[i].behavior == BUTTON_GRID_BUTTON_RADIO &&
            grid->buttons[i].radioGroup == group)
        {
            grid->buttons[i].isOn = (i == selectedIndex);
        }
    }

    ButtonGrid_RedrawAllButtons(grid);
}

static void ButtonGrid_SetButtonStateByIndex(ButtonGrid *grid, int index, int isOn)
{
    if (!grid || !grid->buttons)
        return;

    if (index < 0 || index >= grid->buttonCount)
        return;

    if (grid->buttons[index].behavior == BUTTON_GRID_BUTTON_DISABLED)
        return;

    if (grid->buttons[index].behavior == BUTTON_GRID_BUTTON_RADIO && isOn)
    {
        ButtonGrid_SelectRadioButton(grid, index);
        return;
    }

    grid->buttons[index].isOn = isOn ? 1 : 0;
    ButtonGrid_RedrawButton(grid, index);
}

static int ButtonGrid_GetButtonStateByIndex(ButtonGrid *grid, int index)
{
    if (!grid || !grid->buttons)
        return -1;

    if (index < 0 || index >= grid->buttonCount)
        return -1;

    return grid->buttons[index].isOn;
}

static void ButtonGrid_ToggleButtonStateByIndex(ButtonGrid *grid, int index)
{
    if (!grid || !grid->buttons)
        return;

    if (index < 0 || index >= grid->buttonCount)
        return;

    if (grid->buttons[index].behavior == BUTTON_GRID_BUTTON_DISABLED)
        return;

    if (grid->buttons[index].behavior == BUTTON_GRID_BUTTON_RADIO)
    {
        ButtonGrid_SelectRadioButton(grid, index);
        return;
    }

    grid->buttons[index].isOn = !grid->buttons[index].isOn;
    ButtonGrid_RedrawButton(grid, index);
}

void ButtonGrid_HandleStaticClick(ButtonGrid *grid, int controlId)
{
    int index;
    char identifier[128];
    ButtonItem *button;

    if (!grid)
        return;

    index = controlId - grid->idBase;

    if (index < 0 || index >= grid->buttonCount)
        return;

    button = &grid->buttons[index];

    if (button->behavior == BUTTON_GRID_BUTTON_DISABLED)
        return;

    if (grid->toggleOnClick)
        ButtonGrid_ToggleButtonStateByIndex(grid, index);

    wsprintf(
        identifier,
        grid->clickIdentifierFormat,
        button->action
    );

    if (grid->onClick)
        grid->onClick(identifier);
}

void ButtonGrid_SetButtonStateByNumber(
    HWND gridHwnd,
    int buttonNumber,
    int isOn
)
{
    ButtonGrid *grid;
    int index;

    grid = ButtonGrid_Get(gridHwnd);

    if (!grid)
        return;

    index = buttonNumber - grid->firstIndex;
    ButtonGrid_SetButtonStateByIndex(grid, index, isOn);
}

int ButtonGrid_GetButtonStateByNumber(
    HWND gridHwnd,
    int buttonNumber
)
{
    ButtonGrid *grid;
    int index;

    grid = ButtonGrid_Get(gridHwnd);

    if (!grid)
        return -1;

    index = buttonNumber - grid->firstIndex;
    return ButtonGrid_GetButtonStateByIndex(grid, index);
}

void ButtonGrid_ToggleButtonStateByNumber(
    HWND gridHwnd,
    int buttonNumber
)
{
    ButtonGrid *grid;
    int index;

    grid = ButtonGrid_Get(gridHwnd);

    if (!grid)
        return;

    index = buttonNumber - grid->firstIndex;
    ButtonGrid_ToggleButtonStateByIndex(grid, index);
}

void ButtonGrid_SetButtonStateByName(
    HWND gridHwnd,
    const char *name,
    int isOn
)
{
    ButtonGrid *grid;
    int index;

    grid = ButtonGrid_Get(gridHwnd);

    if (!grid)
        return;

    index = ButtonGrid_FindButtonIndexByName(grid, name);
    ButtonGrid_SetButtonStateByIndex(grid, index, isOn);
}

int ButtonGrid_GetButtonStateByName(
    HWND gridHwnd,
    const char *name
)
{
    ButtonGrid *grid;
    int index;

    grid = ButtonGrid_Get(gridHwnd);

    if (!grid)
        return -1;

    index = ButtonGrid_FindButtonIndexByName(grid, name);
    return ButtonGrid_GetButtonStateByIndex(grid, index);
}

void ButtonGrid_ToggleButtonStateByName(
    HWND gridHwnd,
    const char *name
)
{
    ButtonGrid *grid;
    int index;

    grid = ButtonGrid_Get(gridHwnd);

    if (!grid)
        return;

    index = ButtonGrid_FindButtonIndexByName(grid, name);
    ButtonGrid_ToggleButtonStateByIndex(grid, index);
}

void ButtonGrid_SetButtonStateByAction(
    HWND gridHwnd,
    const char *action,
    int isOn
)
{
    ButtonGrid *grid;
    int index;

    grid = ButtonGrid_Get(gridHwnd);

    if (!grid)
        return;

    index = ButtonGrid_FindButtonIndexByAction(grid, action);
    ButtonGrid_SetButtonStateByIndex(grid, index, isOn);
}

int ButtonGrid_GetButtonStateByAction(
    HWND gridHwnd,
    const char *action
)
{
    ButtonGrid *grid;
    int index;

    grid = ButtonGrid_Get(gridHwnd);

    if (!grid)
        return -1;

    index = ButtonGrid_FindButtonIndexByAction(grid, action);
    return ButtonGrid_GetButtonStateByIndex(grid, index);
}

void ButtonGrid_ToggleButtonStateByAction(
    HWND gridHwnd,
    const char *action
)
{
    ButtonGrid *grid;
    int index;

    grid = ButtonGrid_Get(gridHwnd);

    if (!grid)
        return;

    index = ButtonGrid_FindButtonIndexByAction(grid, action);
    ButtonGrid_ToggleButtonStateByIndex(grid, index);
}