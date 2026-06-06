#include <windows.h>
#include <commctrl.h>
#include <stdlib.h>

#include "button_grid.h"
#include "image_loader.h"

#ifndef SS_OWNERDRAW
#define SS_OWNERDRAW 0x0000000D
#endif

#ifndef SWP_NOCOPYBITS
#define SWP_NOCOPYBITS 0x0100
#endif

#ifndef TTS_ALWAYSTIP
#define TTS_ALWAYSTIP 0x01
#endif

#ifndef TTS_NOPREFIX
#define TTS_NOPREFIX 0x02
#endif

#ifndef TTF_IDISHWND
#define TTF_IDISHWND 0x0001
#endif

#ifndef TTF_SUBCLASS
#define TTF_SUBCLASS 0x0010
#endif

#ifndef TTM_ADDTOOLA
#define TTM_ADDTOOLA (WM_USER + 4)
#endif

#ifndef TOOLTIPS_CLASSA
#define TOOLTIPS_CLASSA "tooltips_class32"
#endif

#define BUTTON_GRID_CLASS_NAME "ButtonGridChildClass"
#define BUTTON_GRID_PROP_NAME "ButtonGridData"

#define BUTTON_GRID_NAME_SIZE 64
#define BUTTON_GRID_ACTION_SIZE 64
#define BUTTON_GRID_TEXT_SIZE 64
#define BUTTON_GRID_TOOLTIP_SIZE 128
#define BUTTON_GRID_FORMAT_SIZE 64
#define BUTTON_GRID_TITLE_SIZE 128

#define BUTTON_GRID_PICTURE_TYPE_OFF 0
#define BUTTON_GRID_PICTURE_TYPE_ON 1
#define BUTTON_GRID_PICTURE_TYPE_ERROR 2

typedef struct ButtonItem
{
    HWND hwnd;

    char name[BUTTON_GRID_NAME_SIZE];
    char action[BUTTON_GRID_ACTION_SIZE];
    char text[BUTTON_GRID_TEXT_SIZE];
    char tooltip[BUTTON_GRID_TOOLTIP_SIZE];

    int behavior;
    int radioGroup;
    int isOn;

    int widthOverride;
    int heightOverride;
    int sizeModeOverride;

    int width;
    int height;

    AppImage *pictureOff;
    AppImage *pictureOn;

    int ownsPictureOff;
    int ownsPictureOn;

    int pictureOffLoadFailed;
    int pictureOnLoadFailed;
} ButtonItem;

typedef struct GridPosition
{
    int x;
    int y;
} GridPosition;

typedef struct GridSize
{
    int width;
    int height;
} GridSize;

typedef struct ButtonGrid
{
    HWND hwnd;
    HWND tooltipHwnd;

    HINSTANCE hInstance;

    ButtonGridClickCallback onClick;

    ButtonItem *buttons;
    int buttonCount;

    const ButtonGridItemConfig *configuredItems;
    int configuredItemCount;

    int buttonWidth;
    int buttonHeight;

    int horizontalSpacing;
    int verticalSpacing;

    int layout;
    int sizeMode;

    int showBorder;
    char borderTitle[BUTTON_GRID_TITLE_SIZE];
    int borderPadding;
    int borderTitleHeight;
    COLORREF borderColor;
    COLORREF borderTitleColor;

    int idBase;
    int firstIndex;

    char namePrefix[BUTTON_GRID_FORMAT_SIZE];
    char actionPrefix[BUTTON_GRID_FORMAT_SIZE];
    char textFormat[BUTTON_GRID_FORMAT_SIZE];
    char clickIdentifierFormat[BUTTON_GRID_FORMAT_SIZE];

    COLORREF backColor;
    COLORREF foreColor;

    int usePictures;
    int toggleOnClick;
    int defaultState;
    int stretchPictures;

    COLORREF generatedOffPictureColor;
    COLORREF generatedOnPictureColor;
    COLORREF generatedErrorPictureColor;

    HBRUSH buttonBrush;
} ButtonGrid;

typedef struct ButtonGridCreateParams
{
    ButtonGridConfig config;
    ButtonGridClickCallback onClick;
} ButtonGridCreateParams;

static LRESULT CALLBACK ButtonGrid_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

static ButtonGrid *ButtonGrid_Get(HWND hwnd)
{
    return (ButtonGrid *)GetProp(hwnd, BUTTON_GRID_PROP_NAME);
}

static void CopyText(char *dest, int destSize, const char *src)
{
    if (!dest || destSize <= 0)
        return;

    if (!src)
        src = "";

    lstrcpyn(dest, src, destSize);
    dest[destSize - 1] = '\0';
}

static int SameText(const char *a, const char *b)
{
    if (!a || !b)
        return 0;

    return lstrcmp(a, b) == 0;
}

static int ClampInt(int value, int minValue, int maxValue)
{
    if (value < minValue)
        return minValue;

    if (value > maxValue)
        return maxValue;

    return value;
}

static int NormalizeSizeMode(int sizeMode)
{
    if (sizeMode != BUTTON_GRID_SIZE_FIXED &&
        sizeMode != BUTTON_GRID_SIZE_MATCH_IMAGE_SIZE &&
        sizeMode != BUTTON_GRID_SIZE_MATCH_IMAGE_ASPECT_HORIZONTAL &&
        sizeMode != BUTTON_GRID_SIZE_MATCH_IMAGE_ASPECT_VERTICAL)
    {
        return BUTTON_GRID_SIZE_FIXED;
    }

    return sizeMode;
}

static void MoveChildClean(HWND hwnd, int x, int y, int w, int h)
{
    if (!hwnd)
        return;

    SetWindowPos(
        hwnd,
        NULL,
        x,
        y,
        w,
        h,
        SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS
    );

    InvalidateRect(hwnd, NULL, TRUE);
}

static void RedrawContainer(HWND hwnd)
{
    RedrawWindow(
        hwnd,
        NULL,
        NULL,
        RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_UPDATENOW
    );
}

void ButtonGrid_GetDefaultConfig(ButtonGridConfig *config)
{
    if (!config)
        return;

    config->buttonCount = BUTTON_GRID_DEFAULT_BUTTON_COUNT;
    config->items = NULL;

    config->buttonWidth = BUTTON_GRID_DEFAULT_BUTTON_WIDTH;
    config->buttonHeight = BUTTON_GRID_DEFAULT_BUTTON_HEIGHT;

    config->horizontalSpacing = BUTTON_GRID_DEFAULT_HORIZONTAL_SPACING;
    config->verticalSpacing = BUTTON_GRID_DEFAULT_VERTICAL_SPACING;

    config->layout = BUTTON_GRID_DEFAULT_LAYOUT;
    config->sizeMode = BUTTON_GRID_DEFAULT_SIZE_MODE;

    config->showBorder = BUTTON_GRID_DEFAULT_SHOW_BORDER;
    config->borderTitle = BUTTON_GRID_DEFAULT_BORDER_TITLE;
    config->borderPadding = BUTTON_GRID_DEFAULT_BORDER_PADDING;
    config->borderTitleHeight = BUTTON_GRID_DEFAULT_BORDER_TITLE_HEIGHT;
    config->borderColor = BUTTON_GRID_DEFAULT_BORDER_COLOR;
    config->borderTitleColor = BUTTON_GRID_DEFAULT_BORDER_TITLE_COLOR;

    config->idBase = BUTTON_GRID_DEFAULT_ID_BASE;
    config->firstIndex = BUTTON_GRID_DEFAULT_FIRST_INDEX;

    config->namePrefix = BUTTON_GRID_DEFAULT_NAME_PREFIX;
    config->actionPrefix = BUTTON_GRID_DEFAULT_ACTION_PREFIX;
    config->textFormat = BUTTON_GRID_DEFAULT_TEXT_FORMAT;
    config->clickIdentifierFormat = BUTTON_GRID_DEFAULT_CLICK_IDENTIFIER_FORMAT;

    config->backColor = BUTTON_GRID_DEFAULT_BACK_COLOR;
    config->foreColor = BUTTON_GRID_DEFAULT_FORE_COLOR;

    config->usePictures = BUTTON_GRID_DEFAULT_USE_PICTURES;
    config->toggleOnClick = BUTTON_GRID_DEFAULT_TOGGLE_ON_CLICK;
    config->defaultState = BUTTON_GRID_DEFAULT_STATE;
    config->stretchPictures = BUTTON_GRID_DEFAULT_STRETCH_PICTURES;

    config->generatedOffPictureColor = BUTTON_GRID_DEFAULT_OFF_PICTURE_COLOR;
    config->generatedOnPictureColor = BUTTON_GRID_DEFAULT_ON_PICTURE_COLOR;
    config->generatedErrorPictureColor = BUTTON_GRID_DEFAULT_ERROR_PICTURE_COLOR;
}

static void ButtonGrid_NormalizeConfig(ButtonGridConfig *config)
{
    if (config->buttonCount < 1)
        config->buttonCount = 1;

    if (config->buttonWidth < 1)
        config->buttonWidth = 1;

    if (config->buttonHeight < 1)
        config->buttonHeight = 1;

    if (config->horizontalSpacing < 0)
        config->horizontalSpacing = 0;

    if (config->verticalSpacing < 0)
        config->verticalSpacing = 0;

    if (config->layout != BUTTON_GRID_LAYOUT_HORIZONTAL &&
        config->layout != BUTTON_GRID_LAYOUT_VERTICAL)
    {
        config->layout = BUTTON_GRID_LAYOUT_HORIZONTAL;
    }

    config->sizeMode = NormalizeSizeMode(config->sizeMode);

    config->showBorder = config->showBorder ? 1 : 0;

    if (config->borderPadding < 0)
        config->borderPadding = 0;

    if (config->borderTitleHeight < 0)
        config->borderTitleHeight = 0;

    if (!config->borderTitle)
        config->borderTitle = "";

    if (!config->namePrefix)
        config->namePrefix = BUTTON_GRID_DEFAULT_NAME_PREFIX;

    if (!config->actionPrefix)
        config->actionPrefix = BUTTON_GRID_DEFAULT_ACTION_PREFIX;

    if (!config->textFormat)
        config->textFormat = BUTTON_GRID_DEFAULT_TEXT_FORMAT;

    if (!config->clickIdentifierFormat)
        config->clickIdentifierFormat = BUTTON_GRID_DEFAULT_CLICK_IDENTIFIER_FORMAT;

    config->defaultState = config->defaultState ? 1 : 0;
    config->usePictures = config->usePictures ? 1 : 0;
    config->toggleOnClick = config->toggleOnClick ? 1 : 0;
    config->stretchPictures = config->stretchPictures ? 1 : 0;
}

static void ButtonGrid_ApplyConfig(ButtonGrid *grid, const ButtonGridConfig *config)
{
    grid->buttonCount = config->buttonCount;

    grid->configuredItems = config->items;
    grid->configuredItemCount = config->buttonCount;

    grid->buttonWidth = config->buttonWidth;
    grid->buttonHeight = config->buttonHeight;

    grid->horizontalSpacing = config->horizontalSpacing;
    grid->verticalSpacing = config->verticalSpacing;

    grid->layout = config->layout;
    grid->sizeMode = config->sizeMode;

    grid->showBorder = config->showBorder;
    CopyText(grid->borderTitle, BUTTON_GRID_TITLE_SIZE, config->borderTitle);
    grid->borderPadding = config->borderPadding;
    grid->borderTitleHeight = config->borderTitleHeight;
    grid->borderColor = config->borderColor;
    grid->borderTitleColor = config->borderTitleColor;

    grid->idBase = config->idBase;
    grid->firstIndex = config->firstIndex;

    CopyText(grid->namePrefix, BUTTON_GRID_FORMAT_SIZE, config->namePrefix);
    CopyText(grid->actionPrefix, BUTTON_GRID_FORMAT_SIZE, config->actionPrefix);
    CopyText(grid->textFormat, BUTTON_GRID_FORMAT_SIZE, config->textFormat);
    CopyText(grid->clickIdentifierFormat, BUTTON_GRID_FORMAT_SIZE, config->clickIdentifierFormat);

    grid->backColor = config->backColor;
    grid->foreColor = config->foreColor;

    grid->usePictures = config->usePictures;
    grid->toggleOnClick = config->toggleOnClick;
    grid->defaultState = config->defaultState;
    grid->stretchPictures = config->stretchPictures;

    grid->generatedOffPictureColor = config->generatedOffPictureColor;
    grid->generatedOnPictureColor = config->generatedOnPictureColor;
    grid->generatedErrorPictureColor = config->generatedErrorPictureColor;

    grid->buttonBrush = CreateSolidBrush(grid->backColor);
}

static DWORD ButtonGrid_GetButtonStyle(void)
{
    return
        WS_CHILD |
        WS_VISIBLE |
        WS_CLIPSIBLINGS |
        SS_NOTIFY |
        SS_OWNERDRAW;
}

static void ButtonGrid_GetBestImageSize(ButtonItem *button, int *imageW, int *imageH)
{
    int w1;
    int h1;
    int w2;
    int h2;
    int hasOff;
    int hasOn;

    w1 = 0;
    h1 = 0;
    w2 = 0;
    h2 = 0;

    hasOff = ImageLoader_GetSize(button->pictureOff, &w1, &h1);
    hasOn = ImageLoader_GetSize(button->pictureOn, &w2, &h2);

    if (hasOff && hasOn)
    {
        if (w2 * h2 >= w1 * h1)
        {
            *imageW = w2;
            *imageH = h2;
        }
        else
        {
            *imageW = w1;
            *imageH = h1;
        }

        return;
    }

    if (hasOn)
    {
        *imageW = w2;
        *imageH = h2;
        return;
    }

    if (hasOff)
    {
        *imageW = w1;
        *imageH = h1;
        return;
    }

    *imageW = 0;
    *imageH = 0;
}

static void ButtonGrid_ResolveButtonSize(ButtonGrid *grid, ButtonItem *button)
{
    int sizeMode;
    int imageW;
    int imageH;
    int w;
    int h;

    if (!grid || !button)
        return;

    sizeMode = button->sizeModeOverride;

    if (sizeMode == BUTTON_GRID_SIZE_USE_DEFAULT)
        sizeMode = grid->sizeMode;

    sizeMode = NormalizeSizeMode(sizeMode);

    imageW = 0;
    imageH = 0;

    ButtonGrid_GetBestImageSize(button, &imageW, &imageH);

    w = grid->buttonWidth;
    h = grid->buttonHeight;

    if (sizeMode == BUTTON_GRID_SIZE_MATCH_IMAGE_SIZE)
    {
        if (imageW > 0)
            w = imageW;

        if (imageH > 0)
            h = imageH;
    }
    else if (sizeMode == BUTTON_GRID_SIZE_MATCH_IMAGE_ASPECT_HORIZONTAL)
    {
        w = grid->buttonWidth;

        if (imageW > 0 && imageH > 0)
            h = (w * imageH) / imageW;
    }
    else if (sizeMode == BUTTON_GRID_SIZE_MATCH_IMAGE_ASPECT_VERTICAL)
    {
        h = grid->buttonHeight;

        if (imageW > 0 && imageH > 0)
            w = (h * imageW) / imageH;
    }

    if (button->widthOverride > 0)
        w = button->widthOverride;

    if (button->heightOverride > 0)
        h = button->heightOverride;

    if (w < 1)
        w = 1;

    if (h < 1)
        h = 1;

    button->width = w;
    button->height = h;
}

static void ButtonGrid_UpdateAllButtonSizes(ButtonGrid *grid)
{
    int i;

    if (!grid || !grid->buttons)
        return;

    for (i = 0; i < grid->buttonCount; i++)
        ButtonGrid_ResolveButtonSize(grid, &grid->buttons[i]);
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

    CopyText(
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
            CopyText(grid->buttons[buttonIndex].name, BUTTON_GRID_NAME_SIZE, item->name);

        if (item->action && item->action[0])
            CopyText(grid->buttons[buttonIndex].action, BUTTON_GRID_ACTION_SIZE, item->action);
        else
            CopyText(grid->buttons[buttonIndex].action, BUTTON_GRID_ACTION_SIZE, grid->buttons[buttonIndex].name);

        if (item->text)
            CopyText(grid->buttons[buttonIndex].text, BUTTON_GRID_TEXT_SIZE, item->text);

        if (item->tooltip)
            CopyText(grid->buttons[buttonIndex].tooltip, BUTTON_GRID_TOOLTIP_SIZE, item->tooltip);

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
            grid->buttons[buttonIndex].sizeModeOverride = NormalizeSizeMode(grid->buttons[buttonIndex].sizeModeOverride);
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

static void ButtonGrid_CreateTooltipWindow(ButtonGrid *grid)
{
    InitCommonControls();

    grid->tooltipHwnd = CreateWindowEx(
        WS_EX_TOPMOST,
        TOOLTIPS_CLASSA,
        NULL,
        WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        grid->hwnd,
        NULL,
        grid->hInstance,
        NULL
    );

    if (grid->tooltipHwnd)
    {
        SetWindowPos(
            grid->tooltipHwnd,
            HWND_TOPMOST,
            0,
            0,
            0,
            0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE
        );
    }
}

static void ButtonGrid_AddTooltip(ButtonGrid *grid, int buttonIndex)
{
    TOOLINFOA ti;

    if (!grid || !grid->tooltipHwnd)
        return;

    if (!grid->buttons[buttonIndex].hwnd)
        return;

    if (!grid->buttons[buttonIndex].tooltip[0])
        return;

    ZeroMemory(&ti, sizeof(ti));

    ti.cbSize = sizeof(ti);
    ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
    ti.hwnd = grid->hwnd;
    ti.uId = (UINT_PTR)grid->buttons[buttonIndex].hwnd;
    ti.lpszText = grid->buttons[buttonIndex].tooltip;

    SendMessage(grid->tooltipHwnd, TTM_ADDTOOLA, 0, (LPARAM)&ti);
}

static void ButtonGrid_GetContentRect(ButtonGrid *grid, RECT *rc)
{
    GetClientRect(grid->hwnd, rc);

    if (!grid->showBorder)
        return;

    rc->left += grid->borderPadding;
    rc->right -= grid->borderPadding;
    rc->top += grid->borderPadding;
    rc->bottom -= grid->borderPadding;

    if (grid->borderTitle[0])
        rc->top += grid->borderTitleHeight;

    if (rc->right <= rc->left)
        rc->right = rc->left + 1;

    if (rc->bottom <= rc->top)
        rc->bottom = rc->top + 1;
}

static void ButtonGrid_LayoutHorizontal(
    ButtonGrid *grid,
    int clientW,
    GridPosition *positions,
    GridSize *size
)
{
    int i;
    int x;
    int y;
    int rowH;
    int rowItems;
    int maxW;

    x = 0;
    y = 0;
    rowH = 0;
    rowItems = 0;
    maxW = 0;

    for (i = 0; i < grid->buttonCount; i++)
    {
        ButtonItem *button;
        int nextX;

        button = &grid->buttons[i];

        if (rowItems > 0)
            nextX = x + grid->horizontalSpacing + button->width;
        else
            nextX = x + button->width;

        if (rowItems > 0 && nextX > clientW)
        {
            if (x > maxW)
                maxW = x;

            y += rowH + grid->verticalSpacing;

            x = 0;
            rowH = 0;
            rowItems = 0;
        }

        if (rowItems > 0)
            x += grid->horizontalSpacing;

        positions[i].x = x;
        positions[i].y = y;

        x += button->width;

        if (button->height > rowH)
            rowH = button->height;

        rowItems++;
    }

    if (x > maxW)
        maxW = x;

    size->width = maxW;
    size->height = y + rowH;

    if (size->width < 1)
        size->width = 1;

    if (size->height < 1)
        size->height = 1;
}

static void ButtonGrid_LayoutVertical(
    ButtonGrid *grid,
    int clientH,
    GridPosition *positions,
    GridSize *size
)
{
    int i;
    int x;
    int y;
    int colW;
    int colItems;
    int maxH;

    x = 0;
    y = 0;
    colW = 0;
    colItems = 0;
    maxH = 0;

    for (i = 0; i < grid->buttonCount; i++)
    {
        ButtonItem *button;
        int nextY;

        button = &grid->buttons[i];

        if (colItems > 0)
            nextY = y + grid->verticalSpacing + button->height;
        else
            nextY = y + button->height;

        if (colItems > 0 && nextY > clientH)
        {
            if (y > maxH)
                maxH = y;

            x += colW + grid->horizontalSpacing;

            y = 0;
            colW = 0;
            colItems = 0;
        }

        if (colItems > 0)
            y += grid->verticalSpacing;

        positions[i].x = x;
        positions[i].y = y;

        y += button->height;

        if (button->width > colW)
            colW = button->width;

        if (y > maxH)
            maxH = y;

        colItems++;
    }

    size->width = x + colW;
    size->height = maxH;

    if (size->width < 1)
        size->width = 1;

    if (size->height < 1)
        size->height = 1;
}

static void ButtonGrid_Layout(ButtonGrid *grid)
{
    RECT rc;
    int clientW;
    int clientH;
    int i;
    int offsetX;
    int offsetY;

    GridPosition *positions;
    GridSize gridSize;

    if (!grid || !grid->buttons)
        return;

    ButtonGrid_GetContentRect(grid, &rc);

    clientW = rc.right - rc.left;
    clientH = rc.bottom - rc.top;

    if (clientW < 1)
        clientW = 1;

    if (clientH < 1)
        clientH = 1;

    ButtonGrid_UpdateAllButtonSizes(grid);

    positions = (GridPosition *)malloc(sizeof(GridPosition) * grid->buttonCount);

    if (!positions)
        return;

    ZeroMemory(positions, sizeof(GridPosition) * grid->buttonCount);

    if (grid->layout == BUTTON_GRID_LAYOUT_VERTICAL)
        ButtonGrid_LayoutVertical(grid, clientH, positions, &gridSize);
    else
        ButtonGrid_LayoutHorizontal(grid, clientW, positions, &gridSize);

    offsetX = rc.left + (clientW - gridSize.width) / 2;
    offsetY = rc.top + (clientH - gridSize.height) / 2;

    if (offsetX < rc.left)
        offsetX = rc.left;

    if (offsetY < rc.top)
        offsetY = rc.top;

    for (i = 0; i < grid->buttonCount; i++)
    {
        MoveChildClean(
            grid->buttons[i].hwnd,
            positions[i].x + offsetX,
            positions[i].y + offsetY,
            grid->buttons[i].width,
            grid->buttons[i].height
        );
    }

    free(positions);

    RedrawContainer(grid->hwnd);
}

static int ButtonGrid_CreateButtons(ButtonGrid *grid)
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

static void ButtonGrid_RedrawButton(ButtonGrid *grid, int index)
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

static void ButtonGrid_RedrawAllButtons(ButtonGrid *grid)
{
    int i;

    if (!grid || !grid->buttons)
        return;

    for (i = 0; i < grid->buttonCount; i++)
        ButtonGrid_RedrawButton(grid, i);

    RedrawContainer(grid->hwnd);
}

static int ButtonGrid_FindButtonIndexByName(ButtonGrid *grid, const char *name)
{
    int i;

    if (!grid || !grid->buttons || !name)
        return -1;

    for (i = 0; i < grid->buttonCount; i++)
    {
        if (SameText(grid->buttons[i].name, name))
            return i;
    }

    return -1;
}

static int ButtonGrid_FindButtonIndexByAction(ButtonGrid *grid, const char *action)
{
    int i;

    if (!grid || !grid->buttons || !action)
        return -1;

    for (i = 0; i < grid->buttonCount; i++)
    {
        if (SameText(grid->buttons[i].action, action))
            return i;
    }

    return -1;
}

static void ButtonGrid_FreeButtonImages(ButtonItem *button)
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

static void ButtonGrid_Free(ButtonGrid *grid)
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

static LRESULT ButtonGrid_HandleCreate(HWND hwnd, LPARAM lParam)
{
    CREATESTRUCT *cs;
    ButtonGridCreateParams *params;
    ButtonGrid *grid;
    ButtonGridConfig config;

    cs = (CREATESTRUCT *)lParam;
    params = (ButtonGridCreateParams *)cs->lpCreateParams;

    grid = (ButtonGrid *)malloc(sizeof(ButtonGrid));

    if (!grid)
        return -1;

    ZeroMemory(grid, sizeof(ButtonGrid));

    grid->hwnd = hwnd;
    grid->hInstance = cs->hInstance;

    ButtonGrid_GetDefaultConfig(&config);

    if (params)
    {
        config = params->config;
        grid->onClick = params->onClick;
    }

    ButtonGrid_NormalizeConfig(&config);
    ButtonGrid_ApplyConfig(grid, &config);

    if (!SetProp(hwnd, BUTTON_GRID_PROP_NAME, (HANDLE)grid))
    {
        ButtonGrid_Free(grid);
        return -1;
    }

    if (!ButtonGrid_CreateButtons(grid))
    {
        RemoveProp(hwnd, BUTTON_GRID_PROP_NAME);
        ButtonGrid_Free(grid);
        return -1;
    }

    return 0;
}

static void ButtonGrid_HandleDestroy(HWND hwnd)
{
    ButtonGrid *grid;

    grid = ButtonGrid_Get(hwnd);

    if (!grid)
        return;

    RemoveProp(hwnd, BUTTON_GRID_PROP_NAME);
    ButtonGrid_Free(grid);
}

static void ButtonGrid_DrawBorder(ButtonGrid *grid, HDC hdc)
{
    RECT rc;
    RECT titleRc;
    HPEN pen;
    HGDIOBJ oldPen;
    HBRUSH backgroundBrush;

    if (!grid || !grid->showBorder)
        return;

    GetClientRect(grid->hwnd, &rc);

    if (rc.right <= rc.left || rc.bottom <= rc.top)
        return;

    rc.left += 2;
    rc.top += 8;
    rc.right -= 2;
    rc.bottom -= 2;

    pen = CreatePen(PS_SOLID, 1, grid->borderColor);
    oldPen = SelectObject(hdc, pen);

    MoveToEx(hdc, rc.left, rc.top, NULL);
    LineTo(hdc, rc.right, rc.top);
    LineTo(hdc, rc.right, rc.bottom);
    LineTo(hdc, rc.left, rc.bottom);
    LineTo(hdc, rc.left, rc.top);

    SelectObject(hdc, oldPen);
    DeleteObject(pen);

    if (grid->borderTitle[0])
    {
        titleRc.left = rc.left + 10;
        titleRc.top = 0;
        titleRc.right = rc.right - 10;
        titleRc.bottom = grid->borderTitleHeight;

        backgroundBrush = (HBRUSH)(COLOR_WINDOW + 1);
        FillRect(hdc, &titleRc, backgroundBrush);

        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, grid->borderTitleColor);

        DrawText(
            hdc,
            grid->borderTitle,
            -1,
            &titleRc,
            DT_LEFT | DT_VCENTER | DT_SINGLELINE
        );
    }
}

static LRESULT ButtonGrid_HandlePaint(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc;
    ButtonGrid *grid;

    grid = ButtonGrid_Get(hwnd);

    hdc = BeginPaint(hwnd, &ps);

    ButtonGrid_DrawBorder(grid, hdc);

    EndPaint(hwnd, &ps);

    return 0;
}

static void ButtonGrid_DrawGeneratedFallback(
    HDC hdc,
    const RECT *rc,
    COLORREF color,
    int pictureType
)
{
    HBRUSH brush;
    HPEN pen;
    HGDIOBJ oldPen;
    COLORREF markColor;
    int width;
    int height;

    brush = CreateSolidBrush(color);
    FillRect(hdc, rc, brush);
    DeleteObject(brush);

    width = rc->right - rc->left;
    height = rc->bottom - rc->top;

    if (pictureType == BUTTON_GRID_PICTURE_TYPE_OFF)
        return;

    if (pictureType == BUTTON_GRID_PICTURE_TYPE_ON)
        markColor = RGB(0, 100, 0);
    else
        markColor = RGB(160, 0, 0);

    pen = CreatePen(PS_SOLID, 5, markColor);
    oldPen = SelectObject(hdc, pen);

    if (pictureType == BUTTON_GRID_PICTURE_TYPE_ON)
    {
        MoveToEx(hdc, rc->left + width / 5, rc->top + height / 2, NULL);
        LineTo(hdc, rc->left + (width * 2) / 5, rc->top + (height * 3) / 4);
        LineTo(hdc, rc->left + (width * 4) / 5, rc->top + height / 4);
    }
    else
    {
        MoveToEx(hdc, rc->left + width / 4, rc->top + height / 4, NULL);
        LineTo(hdc, rc->left + (width * 3) / 4, rc->top + (height * 3) / 4);

        MoveToEx(hdc, rc->left + (width * 3) / 4, rc->top + height / 4, NULL);
        LineTo(hdc, rc->left + width / 4, rc->top + (height * 3) / 4);
    }

    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}

static void ButtonGrid_DrawCenteredText(
    HDC hdc,
    const char *text,
    RECT *rc,
    COLORREF color
)
{
    RECT calcRect;
    RECT drawRect;
    int textHeight;
    int rectHeight;

    if (!text || !text[0])
        return;

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, color);

    calcRect = *rc;
    InflateRect(&calcRect, -4, -4);

    DrawText(
        hdc,
        text,
        -1,
        &calcRect,
        DT_CENTER | DT_WORDBREAK | DT_CALCRECT
    );

    textHeight = calcRect.bottom - calcRect.top;
    rectHeight = rc->bottom - rc->top;

    drawRect = *rc;
    InflateRect(&drawRect, -4, -4);

    if (textHeight < rectHeight)
    {
        drawRect.top = rc->top + (rectHeight - textHeight) / 2;
        drawRect.bottom = drawRect.top + textHeight;
    }

    DrawText(
        hdc,
        text,
        -1,
        &drawRect,
        DT_CENTER | DT_WORDBREAK
    );
}

static void ButtonGrid_DrawButton(ButtonGrid *grid, DRAWITEMSTRUCT *draw)
{
    int index;
    RECT rc;
    HDC hdc;
    ButtonItem *button;
    AppImage *picture;
    int failed;
    int pictureType;
    COLORREF fallbackColor;

    if (!grid || !draw)
        return;

    index = (int)draw->CtlID - grid->idBase;

    if (index < 0 || index >= grid->buttonCount)
        return;

    button = &grid->buttons[index];

    hdc = draw->hDC;
    rc = draw->rcItem;

    if (grid->buttonBrush)
        FillRect(hdc, &rc, grid->buttonBrush);
    else
        FillRect(hdc, &rc, (HBRUSH)(COLOR_WINDOW + 1));

    if (grid->usePictures)
    {
        if (button->isOn)
        {
            picture = button->pictureOn;
            failed = button->pictureOnLoadFailed;
            pictureType = failed ? BUTTON_GRID_PICTURE_TYPE_ERROR : BUTTON_GRID_PICTURE_TYPE_ON;
            fallbackColor = failed ? grid->generatedErrorPictureColor : grid->generatedOnPictureColor;
        }
        else
        {
            picture = button->pictureOff;
            failed = button->pictureOffLoadFailed;
            pictureType = failed ? BUTTON_GRID_PICTURE_TYPE_ERROR : BUTTON_GRID_PICTURE_TYPE_OFF;
            fallbackColor = failed ? grid->generatedErrorPictureColor : grid->generatedOffPictureColor;
        }

        if (picture)
        {
            ImageLoader_Draw(
                hdc,
                picture,
                &rc,
                grid->stretchPictures
            );
        }
        else
        {
            ButtonGrid_DrawGeneratedFallback(
                hdc,
                &rc,
                fallbackColor,
                pictureType
            );
        }
    }

    ButtonGrid_DrawCenteredText(hdc, button->text, &rc, grid->foreColor);

    FrameRect(hdc, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));

    if (draw->itemState & ODS_FOCUS)
        DrawFocusRect(hdc, &rc);
}

static LRESULT ButtonGrid_HandleDrawItem(ButtonGrid *grid, LPARAM lParam)
{
    DRAWITEMSTRUCT *draw;

    draw = (DRAWITEMSTRUCT *)lParam;

    if (!grid || !draw)
        return 0;

    ButtonGrid_DrawButton(grid, draw);

    return TRUE;
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

static void ButtonGrid_HandleStaticClick(ButtonGrid *grid, int controlId)
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
    {
        if (button->behavior == BUTTON_GRID_BUTTON_RADIO)
        {
            ButtonGrid_SelectRadioButton(grid, index);
        }
        else
        {
            button->isOn = !button->isOn;
            ButtonGrid_RedrawButton(grid, index);
        }
    }

    wsprintf(
        identifier,
        grid->clickIdentifierFormat,
        button->action
    );

    if (grid->onClick)
        grid->onClick(identifier);
}

static LRESULT ButtonGrid_HandleEraseBackground(HWND hwnd, WPARAM wParam)
{
    RECT rc;
    HDC hdc;

    hdc = (HDC)wParam;

    GetClientRect(hwnd, &rc);
    FillRect(hdc, &rc, (HBRUSH)(COLOR_WINDOW + 1));

    return 1;
}

BOOL ButtonGrid_RegisterClass(HINSTANCE hInstance)
{
    static int registered = 0;
    WNDCLASS wc;

    if (registered)
        return TRUE;

    ZeroMemory(&wc, sizeof(wc));

    wc.lpfnWndProc = ButtonGrid_WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = BUTTON_GRID_CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClass(&wc))
        return FALSE;

    registered = 1;
    return TRUE;
}

HWND ButtonGrid_CreateEx(
    HWND parent,
    HINSTANCE hInstance,
    int x,
    int y,
    int width,
    int height,
    const ButtonGridConfig *config,
    ButtonGridClickCallback onClick
)
{
    ButtonGridCreateParams params;

    ButtonGrid_GetDefaultConfig(&params.config);

    if (config)
        params.config = *config;

    params.onClick = onClick;

    ButtonGrid_NormalizeConfig(&params.config);

    if (!ButtonGrid_RegisterClass(hInstance))
        return NULL;

    return CreateWindowEx(
        0,
        BUTTON_GRID_CLASS_NAME,
        "",
        WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN,
        x,
        y,
        width,
        height,
        parent,
        NULL,
        hInstance,
        &params
    );
}

HWND ButtonGrid_Create(
    HWND parent,
    HINSTANCE hInstance,
    int x,
    int y,
    int width,
    int height,
    ButtonGridClickCallback onClick
)
{
    return ButtonGrid_CreateEx(
        parent,
        hInstance,
        x,
        y,
        width,
        height,
        NULL,
        onClick
    );
}

void ButtonGrid_SetClickCallback(
    HWND gridHwnd,
    ButtonGridClickCallback onClick
)
{
    ButtonGrid *grid;

    grid = ButtonGrid_Get(gridHwnd);

    if (!grid)
        return;

    grid->onClick = onClick;
}

void ButtonGrid_SetRect(
    HWND gridHwnd,
    int x,
    int y,
    int width,
    int height
)
{
    if (!gridHwnd)
        return;

    if (width < 1)
        width = 1;

    if (height < 1)
        height = 1;

    SetWindowPos(
        gridHwnd,
        NULL,
        x,
        y,
        width,
        height,
        SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS
    );

    InvalidateRect(gridHwnd, NULL, TRUE);
}

void ButtonGrid_SetButtonSize(
    HWND gridHwnd,
    int buttonWidth,
    int buttonHeight
)
{
    ButtonGrid *grid;

    grid = ButtonGrid_Get(gridHwnd);

    if (!grid)
        return;

    if (buttonWidth < 1)
        buttonWidth = 1;

    if (buttonHeight < 1)
        buttonHeight = 1;

    grid->buttonWidth = buttonWidth;
    grid->buttonHeight = buttonHeight;

    ButtonGrid_UpdateAllButtonSizes(grid);
    ButtonGrid_Layout(grid);
}

void ButtonGrid_SetSizeMode(
    HWND gridHwnd,
    int sizeMode
)
{
    ButtonGrid *grid;

    grid = ButtonGrid_Get(gridHwnd);

    if (!grid)
        return;

    grid->sizeMode = NormalizeSizeMode(sizeMode);

    ButtonGrid_UpdateAllButtonSizes(grid);
    ButtonGrid_Layout(grid);
}

void ButtonGrid_SetSpacing(
    HWND gridHwnd,
    int horizontalSpacing,
    int verticalSpacing
)
{
    ButtonGrid *grid;

    grid = ButtonGrid_Get(gridHwnd);

    if (!grid)
        return;

    if (horizontalSpacing < 0)
        horizontalSpacing = 0;

    if (verticalSpacing < 0)
        verticalSpacing = 0;

    grid->horizontalSpacing = horizontalSpacing;
    grid->verticalSpacing = verticalSpacing;

    ButtonGrid_Layout(grid);
}

void ButtonGrid_SetLayout(HWND gridHwnd, int layout)
{
    ButtonGrid *grid;

    grid = ButtonGrid_Get(gridHwnd);

    if (!grid)
        return;

    if (layout != BUTTON_GRID_LAYOUT_HORIZONTAL &&
        layout != BUTTON_GRID_LAYOUT_VERTICAL)
    {
        layout = BUTTON_GRID_LAYOUT_HORIZONTAL;
    }

    grid->layout = layout;

    ButtonGrid_Layout(grid);
}

void ButtonGrid_Relayout(HWND gridHwnd)
{
    ButtonGrid_Layout(ButtonGrid_Get(gridHwnd));
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

    if (index < 0 || index >= grid->buttonCount)
        return;

    if (grid->buttons[index].behavior == BUTTON_GRID_BUTTON_RADIO && isOn)
        ButtonGrid_SelectRadioButton(grid, index);
    else
    {
        grid->buttons[index].isOn = isOn ? 1 : 0;
        ButtonGrid_RedrawButton(grid, index);
    }
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

    if (index < 0 || index >= grid->buttonCount)
        return -1;

    return grid->buttons[index].isOn;
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

    if (index < 0 || index >= grid->buttonCount)
        return;

    if (grid->buttons[index].behavior == BUTTON_GRID_BUTTON_RADIO)
        ButtonGrid_SelectRadioButton(grid, index);
    else
    {
        grid->buttons[index].isOn = !grid->buttons[index].isOn;
        ButtonGrid_RedrawButton(grid, index);
    }
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

    if (index < 0)
        return;

    if (grid->buttons[index].behavior == BUTTON_GRID_BUTTON_RADIO && isOn)
        ButtonGrid_SelectRadioButton(grid, index);
    else
    {
        grid->buttons[index].isOn = isOn ? 1 : 0;
        ButtonGrid_RedrawButton(grid, index);
    }
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

    if (index < 0)
        return -1;

    return grid->buttons[index].isOn;
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

    if (index < 0)
        return;

    if (grid->buttons[index].behavior == BUTTON_GRID_BUTTON_RADIO)
        ButtonGrid_SelectRadioButton(grid, index);
    else
    {
        grid->buttons[index].isOn = !grid->buttons[index].isOn;
        ButtonGrid_RedrawButton(grid, index);
    }
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

    if (index < 0)
        return;

    if (grid->buttons[index].behavior == BUTTON_GRID_BUTTON_RADIO && isOn)
        ButtonGrid_SelectRadioButton(grid, index);
    else
    {
        grid->buttons[index].isOn = isOn ? 1 : 0;
        ButtonGrid_RedrawButton(grid, index);
    }
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

    if (index < 0)
        return -1;

    return grid->buttons[index].isOn;
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

    if (index < 0)
        return;

    if (grid->buttons[index].behavior == BUTTON_GRID_BUTTON_RADIO)
        ButtonGrid_SelectRadioButton(grid, index);
    else
    {
        grid->buttons[index].isOn = !grid->buttons[index].isOn;
        ButtonGrid_RedrawButton(grid, index);
    }
}

static LRESULT CALLBACK ButtonGrid_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    ButtonGrid *grid;

    grid = ButtonGrid_Get(hwnd);

    switch (msg)
    {
        case WM_CREATE:
        {
            return ButtonGrid_HandleCreate(hwnd, lParam);
        }

        case WM_SIZE:
        {
            ButtonGrid_Layout(grid);
            return 0;
        }

        case WM_PAINT:
        {
            return ButtonGrid_HandlePaint(hwnd);
        }

        case WM_COMMAND:
        {
            int controlId;
            int notifyCode;

            controlId = LOWORD(wParam);
            notifyCode = HIWORD(wParam);

            if (notifyCode == STN_CLICKED)
            {
                ButtonGrid_HandleStaticClick(grid, controlId);
                return 0;
            }

            break;
        }

        case WM_DRAWITEM:
        {
            return ButtonGrid_HandleDrawItem(grid, lParam);
        }

        case WM_ERASEBKGND:
        {
            return ButtonGrid_HandleEraseBackground(hwnd, wParam);
        }

        case WM_NCDESTROY:
        {
            ButtonGrid_HandleDestroy(hwnd);
            break;
        }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}