#include "button_grid_internal.h"

static DWORD ButtonGrid_GetButtonStyle(void)
{
    return
        WS_CHILD |
        WS_VISIBLE |
        WS_CLIPSIBLINGS |
        SS_NOTIFY |
        SS_OWNERDRAW;
}

static void ButtonGrid_SetDefaultButtonName(ButtonGrid *grid, int buttonIndex)
{
    int indexNumber;

    indexNumber = buttonIndex + grid->firstIndex;

    wsprintf(
        grid->buttons[buttonIndex].name,
        "%s%d",
        grid->namePrefix,
        indexNumber
    );
}

static void ButtonGrid_SetDefaultButtonAction(ButtonGrid *grid, int buttonIndex)
{
    int indexNumber;

    indexNumber = buttonIndex + grid->firstIndex;

    wsprintf(
        grid->buttons[buttonIndex].action,
        "%s%d",
        grid->actionPrefix,
        indexNumber
    );
}

static void ButtonGrid_SetDefaultButtonText(ButtonGrid *grid, int buttonIndex)
{
    int indexNumber;

    indexNumber = buttonIndex + grid->firstIndex;

    wsprintf(
        grid->buttons[buttonIndex].text,
        grid->textFormat,
        indexNumber
    );
}

static void ButtonGrid_InitializeButtonData(ButtonGrid *grid, int buttonIndex)
{
    const ButtonGridItemConfig *item;

    item = NULL;

    if (grid->configuredItems && buttonIndex < grid->configuredItemCount)
        item = &grid->configuredItems[buttonIndex];

    ButtonGrid_SetDefaultButtonName(grid, buttonIndex);
    ButtonGrid_SetDefaultButtonAction(grid, buttonIndex);
    ButtonGrid_SetDefaultButtonText(grid, buttonIndex);

    ButtonGrid_CopyText(
        grid->buttons[buttonIndex].tooltip,
        BUTTON_GRID_TOOLTIP_SIZE,
        grid->buttons[buttonIndex].name
    );

    grid->buttons[buttonIndex].behavior = BUTTON_GRID_BUTTON_TOGGLE;
    grid->buttons[buttonIndex].radioGroup = 0;
    grid->buttons[buttonIndex].isOn = grid->defaultState;

    grid->buttons[buttonIndex].widthOverride = 0;
    grid->buttons[buttonIndex].heightOverride = 0;
    grid->buttons[buttonIndex].sizeModeOverride = BUTTON_GRID_SIZE_USE_DEFAULT;

    if (item)
    {
        if (item->name && item->name[0])
            ButtonGrid_CopyText(grid->buttons[buttonIndex].name, BUTTON_GRID_NAME_SIZE, item->name);

        if (item->action && item->action[0])
            ButtonGrid_CopyText(grid->buttons[buttonIndex].action, BUTTON_GRID_ACTION_SIZE, item->action);
        else
            ButtonGrid_CopyText(grid->buttons[buttonIndex].action, BUTTON_GRID_ACTION_SIZE, grid->buttons[buttonIndex].name);

        if (item->text)
            ButtonGrid_CopyText(grid->buttons[buttonIndex].text, BUTTON_GRID_TEXT_SIZE, item->text);

        if (item->tooltip)
            ButtonGrid_CopyText(grid->buttons[buttonIndex].tooltip, BUTTON_GRID_TOOLTIP_SIZE, item->tooltip);

        grid->buttons[buttonIndex].behavior = item->behavior;
        grid->buttons[buttonIndex].radioGroup = item->radioGroup;
        grid->buttons[buttonIndex].isOn = item->defaultState ? 1 : 0;

        grid->buttons[buttonIndex].widthOverride = item->widthOverride;
        grid->buttons[buttonIndex].heightOverride = item->heightOverride;
        grid->buttons[buttonIndex].sizeModeOverride = item->sizeModeOverride;

        grid->buttons[buttonIndex].pictureOff = item->pictureOff;
        grid->buttons[buttonIndex].pictureOn = item->pictureOn;

        grid->buttons[buttonIndex].ownsPictureOff = item->ownsPictureOff;
        grid->buttons[buttonIndex].ownsPictureOn = item->ownsPictureOn;

        grid->buttons[buttonIndex].pictureOffLoadFailed = item->pictureOffLoadFailed ? 1 : 0;
        grid->buttons[buttonIndex].pictureOnLoadFailed = item->pictureOnLoadFailed ? 1 : 0;

        if (grid->buttons[buttonIndex].behavior != BUTTON_GRID_BUTTON_TOGGLE &&
            grid->buttons[buttonIndex].behavior != BUTTON_GRID_BUTTON_RADIO &&
            grid->buttons[buttonIndex].behavior != BUTTON_GRID_BUTTON_DISABLED)
        {
            grid->buttons[buttonIndex].behavior = BUTTON_GRID_BUTTON_TOGGLE;
        }

        if (grid->buttons[buttonIndex].sizeModeOverride != BUTTON_GRID_SIZE_USE_DEFAULT)
        {
            grid->buttons[buttonIndex].sizeModeOverride =
                ButtonGrid_NormalizeSizeMode(grid->buttons[buttonIndex].sizeModeOverride);
        }
    }

    ButtonGrid_ResolveButtonSize(grid, &grid->buttons[buttonIndex]);
}

static HWND ButtonGrid_CreateOneButton(ButtonGrid *grid, int buttonIndex)
{
    return CreateWindowEx(
        0,
        "STATIC",
        grid->buttons[buttonIndex].text,
        ButtonGrid_GetButtonStyle(),
        0,
        0,
        grid->buttons[buttonIndex].width,
        grid->buttons[buttonIndex].height,
        grid->hwnd,
        (HMENU)(grid->idBase + buttonIndex),
        grid->hInstance,
        NULL
    );
}

int ButtonGrid_CreateButtons(ButtonGrid *grid)
{
    int i;

    grid->buttons = (ButtonItem *)malloc(sizeof(ButtonItem) * grid->buttonCount);

    if (!grid->buttons)
        return 0;

    ZeroMemory(grid->buttons, sizeof(ButtonItem) * grid->buttonCount);

    ButtonGrid_CreateTooltipWindow(grid);

    for (i = 0; i < grid->buttonCount; i++)
    {
        ButtonGrid_InitializeButtonData(grid, i);
        grid->buttons[i].hwnd = ButtonGrid_CreateOneButton(grid, i);
        ButtonGrid_AddTooltip(grid, i);
    }

    ButtonGrid_Layout(grid);

    return 1;
}

void ButtonGrid_FreeButtonImages(ButtonItem *button)
{
    if (!button)
        return;

    if (button->ownsPictureOff && button->pictureOff)
    {
        ImageLoader_Free(button->pictureOff);
        button->pictureOff = NULL;
    }

    if (button->ownsPictureOn && button->pictureOn)
    {
        ImageLoader_Free(button->pictureOn);
        button->pictureOn = NULL;
    }

    button->ownsPictureOff = 0;
    button->ownsPictureOn = 0;
}

void ButtonGrid_Free(ButtonGrid *grid)
{
    int i;

    if (!grid)
        return;

    if (grid->tooltipHwnd)
    {
        DestroyWindow(grid->tooltipHwnd);
        grid->tooltipHwnd = NULL;
    }

    if (grid->buttons)
    {
        for (i = 0; i < grid->buttonCount; i++)
            ButtonGrid_FreeButtonImages(&grid->buttons[i]);

        free(grid->buttons);
        grid->buttons = NULL;
    }

    if (grid->buttonBrush)
    {
        DeleteObject(grid->buttonBrush);
        grid->buttonBrush = NULL;
    }

    free(grid);
}

int ButtonGrid_FindButtonIndexByName(ButtonGrid *grid, const char *name)
{
    int i;

    if (!grid || !grid->buttons || !name)
        return -1;

    for (i = 0; i < grid->buttonCount; i++)
    {
        if (ButtonGrid_SameText(grid->buttons[i].name, name))
            return i;
    }

    return -1;
}

int ButtonGrid_FindButtonIndexByAction(ButtonGrid *grid, const char *action)
{
    int i;

    if (!grid || !grid->buttons || !action)
        return -1;

    for (i = 0; i < grid->buttonCount; i++)
    {
        if (ButtonGrid_SameText(grid->buttons[i].action, action))
            return i;
    }

    return -1;
}

void ButtonGrid_SetPictures(
    HWND gridHwnd,
    AppImage *pictureOff,
    AppImage *pictureOn,
    int stretchPictures
)
{
    ButtonGrid *grid;
    int i;

    grid = ButtonGrid_Get(gridHwnd);

    if (!grid)
        return;

    for (i = 0; i < grid->buttonCount; i++)
    {
        ButtonGrid_FreeButtonImages(&grid->buttons[i]);

        grid->buttons[i].pictureOff = pictureOff;
        grid->buttons[i].pictureOn = pictureOn;

        grid->buttons[i].ownsPictureOff = 0;
        grid->buttons[i].ownsPictureOn = 0;

        grid->buttons[i].pictureOffLoadFailed = 0;
        grid->buttons[i].pictureOnLoadFailed = 0;

        ButtonGrid_ResolveButtonSize(grid, &grid->buttons[i]);
    }

    grid->stretchPictures = stretchPictures ? 1 : 0;
    grid->usePictures = 1;

    ButtonGrid_Layout(grid);
    ButtonGrid_RedrawAllButtons(grid);
}