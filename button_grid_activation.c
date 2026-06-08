#include "button_grid_core_internal.h"

static const char *ButtonGrid_GetButtonActionName(ButtonItem *button)
{
    if (!button)
        return "";

    if (button->action[0])
        return button->action;

    return button->name;
}

static void ButtonGrid_RedrawChangedButton(ButtonGrid *grid, int index)
{
    if (!grid)
        return;

    if (index < 0 || index >= grid->buttonCount)
        return;

    ButtonGrid_RedrawButton(grid, index);
}

static int ButtonGrid_SelectRadioButton(ButtonGrid *grid, int index)
{
    int i;
    int changed;

    if (!grid || !grid->buttons)
        return 0;

    if (index < 0 || index >= grid->buttonCount)
        return 0;

    if (grid->buttons[index].isOn)
        return 0;

    changed = 0;

    for (i = 0; i < grid->buttonCount; i++)
    {
        if (i == index)
            continue;

        if (grid->buttons[i].behavior != BUTTON_GRID_BUTTON_RADIO)
            continue;

        if (grid->buttons[i].radioGroup != grid->buttons[index].radioGroup)
            continue;

        if (!grid->buttons[i].isOn)
            continue;

        grid->buttons[i].isOn = 0;
        ButtonGrid_RedrawChangedButton(grid, i);
        changed = 1;
    }

    grid->buttons[index].isOn = 1;
    ButtonGrid_RedrawChangedButton(grid, index);

    return 1;
}

static int ButtonGrid_ToggleButton(ButtonGrid *grid, int index)
{
    if (!grid || !grid->buttons)
        return 0;

    if (index < 0 || index >= grid->buttonCount)
        return 0;

    if (!grid->toggleOnClick)
        return 0;

    grid->buttons[index].isOn = grid->buttons[index].isOn ? 0 : 1;
    ButtonGrid_RedrawChangedButton(grid, index);

    return 1;
}

void ButtonGrid_ActivateButtonByIndex(ButtonGrid *grid, int index)
{
    ButtonItem *button;
    int changed;

    if (!grid || !grid->buttons)
        return;

    if (index < 0 || index >= grid->buttonCount)
        return;

    button = &grid->buttons[index];

    if (button->behavior == BUTTON_GRID_BUTTON_DISABLED)
        return;

    ButtonGrid_FocusButtonByIndex(grid, index);

    changed = 0;

    if (button->behavior == BUTTON_GRID_BUTTON_RADIO)
    {
        changed = ButtonGrid_SelectRadioButton(grid, index);

        if (!changed)
            return;
    }
    else
    {
        changed = ButtonGrid_ToggleButton(grid, index);
        (void)changed;
    }

    if (grid->onClick)
        grid->onClick(ButtonGrid_GetButtonActionName(button));
}