#include "new_test_layout_controls_internal.h"

static int ActionGroup_RectAlreadyMatches(
    HWND hwnd,
    const RECT *rect
)
{
    RECT current;

    if (!hwnd || !rect)
        return 0;

    if (!Ui_GetWindowRectInParent(hwnd, &current))
        return 0;

    if (current.left != rect->left)
        return 0;

    if (current.top != rect->top)
        return 0;

    if (current.right != rect->right)
        return 0;

    if (current.bottom != rect->bottom)
        return 0;

    return 1;
}

static void ActionGroup_Draw(NewTestLayoutActionGroup *group, HDC hdc)
{
    RECT rc;
    RECT titleRc;
    HFONT font;
    HFONT oldFont;
    int oldBkMode;

    if (!group || !hdc)
        return;

    GetClientRect(group->hwnd, &rc);

    Ntl_FillRect(hdc, &rc, group->theme.groupBackColor);

    if (group->borderVisible)
        Ntl_DrawBorder(hdc, &rc, group->theme.groupBorderColor);

    titleRc = rc;
    titleRc.left += group->padding;
    titleRc.right -= group->padding;
    titleRc.bottom = titleRc.top + 24;

    font = group->theme.titleFont;

    if (!font)
        font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

    oldFont = (HFONT)SelectObject(hdc, font);
    oldBkMode = SetBkMode(hdc, TRANSPARENT);

    SetTextColor(hdc, group->theme.groupTitleColor);

    DrawText(
        hdc,
        group->title,
        -1,
        &titleRc,
        DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX
    );

    SetBkMode(hdc, oldBkMode);
    SelectObject(hdc, oldFont);
}

static void ActionGroup_DrawBuffered(void *context, HDC hdc, RECT *rc)
{
    (void)rc;
    ActionGroup_Draw((NewTestLayoutActionGroup *)context, hdc);
}

LRESULT CALLBACK NewTestLayoutActionGroup_WndProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam
)
{
    NewTestLayoutActionGroup *group;

    group = (NewTestLayoutActionGroup *)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    switch (msg)
    {
        case WM_ERASEBKGND:
        {
            return 1;
        }

        case WM_SIZE:
        {
            NewTestLayoutActionGroup_LayoutButtons(group);
            return 0;
        }

        case WM_COMMAND:
        {
            if (group)
            {
                Ntl_SendCommand(
                    group->parent,
                    (HWND)lParam,
                    LOWORD(wParam),
                    HIWORD(wParam)
                );

                return 0;
            }

            break;
        }

        case WM_PAINT:
        {
            Ntl_DoubleBufferedPaint(hwnd, group, ActionGroup_DrawBuffered);
            return 0;
        }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

NewTestLayoutActionGroup *NewTestLayoutActionGroup_Create(
    HWND parent,
    HINSTANCE hInstance,
    int id,
    const char *title,
    const NewTestLayoutActionButtonDef *buttons,
    int buttonCount,
    const NewTestLayoutTheme *theme
)
{
    NewTestLayoutActionGroup *group;
    int i;

    if (buttonCount < 0)
        buttonCount = 0;

    if (buttonCount > NTL_CONTROLS_MAX_ACTION_BUTTONS)
        buttonCount = NTL_CONTROLS_MAX_ACTION_BUTTONS;

    group = (NewTestLayoutActionGroup *)malloc(sizeof(*group));

    if (!group)
        return NULL;

    ZeroMemory(group, sizeof(*group));

    group->parent = parent;
    group->hInstance = hInstance;
    group->id = id;
    group->visible = 1;
    group->borderVisible = 0;
    group->padding = 10;
    group->buttonCount = buttonCount;

    Ntl_CopyText(group->title, sizeof(group->title), title);
    Ntl_CopyTheme(&group->theme, theme);
    NewTestLayoutTheme_CreateDefaultFonts(&group->theme);

    group->hwnd = CreateWindowEx(
        0,
        NTL_ACTION_GROUP_CLASS_NAME,
        "",
        WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
        0,
        0,
        100,
        80,
        parent,
        (HMENU)id,
        hInstance,
        NULL
    );

    if (!group->hwnd)
    {
        NewTestLayoutTheme_DeleteFonts(&group->theme);
        free(group);
        return NULL;
    }

    SetWindowLongPtr(group->hwnd, GWLP_USERDATA, (LONG_PTR)group);

    for (i = 0; i < group->buttonCount; i++)
    {
        group->buttons[i] = NewTestLayoutActionButton_Create(
            group->hwnd,
            hInstance,
            buttons[i].id,
            buttons[i].label,
            &group->theme
        );

        if (group->buttons[i])
        {
            NewTestLayoutActionButton_SetCount(
                group->buttons[i],
                buttons[i].count,
                buttons[i].hasCount
            );
        }
    }

    return group;
}

void NewTestLayoutActionGroup_Destroy(NewTestLayoutActionGroup *group)
{
    int i;

    if (!group)
        return;

    for (i = 0; i < group->buttonCount; i++)
        NewTestLayoutActionButton_Destroy(group->buttons[i]);

    if (group->hwnd)
        DestroyWindow(group->hwnd);

    NewTestLayoutTheme_DeleteFonts(&group->theme);

    free(group);
}

HWND NewTestLayoutActionGroup_GetHwnd(NewTestLayoutActionGroup *group)
{
    return group ? group->hwnd : NULL;
}

void NewTestLayoutActionGroup_SetTheme(NewTestLayoutActionGroup *group, const NewTestLayoutTheme *theme)
{
    int i;

    if (!group)
        return;

    NewTestLayoutTheme_DeleteFonts(&group->theme);
    Ntl_CopyTheme(&group->theme, theme);
    NewTestLayoutTheme_CreateDefaultFonts(&group->theme);

    for (i = 0; i < group->buttonCount; i++)
        NewTestLayoutActionButton_SetTheme(group->buttons[i], &group->theme);

    InvalidateRect(group->hwnd, NULL, FALSE);
}

void NewTestLayoutActionGroup_SetRect(NewTestLayoutActionGroup *group, const RECT *rect)
{
    if (!group || !rect || !group->hwnd)
        return;

    if (ActionGroup_RectAlreadyMatches(group->hwnd, rect))
        return;

    SetWindowPos(
        group->hwnd,
        NULL,
        rect->left,
        rect->top,
        rect->right - rect->left,
        rect->bottom - rect->top,
        SWP_NOZORDER | SWP_NOACTIVATE
    );
}

void NewTestLayoutActionGroup_Show(NewTestLayoutActionGroup *group, int show)
{
    int visible;

    if (!group || !group->hwnd)
        return;

    visible = show ? 1 : 0;

    if (group->visible == visible)
        return;

    group->visible = visible;
    ShowWindow(group->hwnd, group->visible ? SW_SHOW : SW_HIDE);
}

int NewTestLayoutActionGroup_IsVisible(NewTestLayoutActionGroup *group)
{
    return group ? group->visible : 0;
}

void NewTestLayoutActionGroup_SetTitle(NewTestLayoutActionGroup *group, const char *title)
{
    if (!group)
        return;

    if (!title)
        title = "";

    if (lstrcmp(group->title, title) == 0)
        return;

    Ntl_CopyText(group->title, sizeof(group->title), title);
    InvalidateRect(group->hwnd, NULL, FALSE);
}

void NewTestLayoutActionGroup_SetBorderVisible(NewTestLayoutActionGroup *group, int visible)
{
    int normalizedVisible;

    if (!group)
        return;

    normalizedVisible = visible ? 1 : 0;

    if (group->borderVisible == normalizedVisible)
        return;

    group->borderVisible = normalizedVisible;
    InvalidateRect(group->hwnd, NULL, FALSE);
}

void NewTestLayoutActionGroup_SetPadding(NewTestLayoutActionGroup *group, int padding)
{
    if (!group)
        return;

    if (padding < 0)
        padding = 0;

    if (group->padding == padding)
        return;

    group->padding = padding;
    NewTestLayoutActionGroup_LayoutButtons(group);
    InvalidateRect(group->hwnd, NULL, FALSE);
}

void NewTestLayoutActionGroup_SetButtonCount(
    NewTestLayoutActionGroup *group,
    int buttonIndex,
    int count,
    int hasCount
)
{
    if (!group)
        return;

    if (buttonIndex < 0 || buttonIndex >= group->buttonCount)
        return;

    NewTestLayoutActionButton_SetCount(
        group->buttons[buttonIndex],
        count,
        hasCount
    );
}

void NewTestLayoutActionGroup_SetCountOptions(
    NewTestLayoutActionGroup *group,
    int showCounts,
    int showZeroCounts,
    int countInParentheses,
    int colorCountText
)
{
    int i;

    if (!group)
        return;

    for (i = 0; i < group->buttonCount; i++)
    {
        NewTestLayoutActionButton_SetCountOptions(
            group->buttons[i],
            showCounts,
            showZeroCounts,
            countInParentheses,
            colorCountText
        );
    }
}

void NewTestLayoutActionGroup_LayoutButtons(NewTestLayoutActionGroup *group)
{
    RECT rc;
    RECT buttonRc;
    int titleH;
    int gap;
    int x;
    int y;
    int w;
    int h;
    int buttonW;
    int i;

    if (!group || !group->hwnd)
        return;

    GetClientRect(group->hwnd, &rc);

    titleH = 28;
    gap = 12;

    x = group->padding;
    y = titleH + group->padding / 2;
    w = rc.right - rc.left - group->padding * 2;
    h = rc.bottom - y - group->padding;

    if (w < 1)
        w = 1;

    if (h < 1)
        h = 1;

    if (group->buttonCount <= 0)
        return;

    buttonW = (w - gap * (group->buttonCount - 1)) / group->buttonCount;

    if (buttonW < 40)
        buttonW = 40;

    for (i = 0; i < group->buttonCount; i++)
    {
        SetRect(
            &buttonRc,
            x + i * (buttonW + gap),
            y,
            x + i * (buttonW + gap) + buttonW,
            y + h
        );

        NewTestLayoutActionButton_SetRect(group->buttons[i], &buttonRc);
    }
}