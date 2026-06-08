#include "npp_mockup_internal.h"

HINSTANCE g_npp_hInstance = NULL;
HWND g_npp_window = NULL;
HWND g_npp_status = NULL;

HWND g_npp_toolbarButtons[NPP_MAX_TOOL_BUTTONS];
int g_npp_toolbarButtonCount = 0;

NppPane g_npp_panes[2];
int g_npp_activePane = 0;

int g_npp_splitPercent = 500;
int g_npp_draggingSplitter = 0;
RECT g_npp_splitterRect;

HMODULE g_npp_richEditModule = NULL;
const char *g_npp_editClassName = "EDIT";
int g_npp_usingRichEdit = 0;

HBRUSH g_npp_statusBrush = NULL;
NppMockupWindowClosedCallback g_npp_onClosed = NULL;

HMODULE g_npp_shell32 = NULL;
NppDragQueryFileProc g_npp_dragQueryFile = NULL;
NppDragFinishProc g_npp_dragFinish = NULL;

void Npp_CopyText(char *dest, int destSize, const char *src)
{
    if (!dest || destSize <= 0)
        return;

    if (!src)
        src = "";

    lstrcpyn(dest, src, destSize);
    dest[destSize - 1] = '\0';
}

char *Npp_DuplicateText(const char *text)
{
    char *copy;
    int len;

    if (!text)
        text = "";

    len = lstrlen(text);

    copy = (char *)malloc(len + 1);

    if (!copy)
        return NULL;

    CopyMemory(copy, text, len + 1);
    return copy;
}

static void Npp_CreateMenuBar(HWND hwnd)
{
    HMENU menu;
    HMENU popup;

    const char *names[] =
    {
        "File", "Edit", "Search", "View", "Encoding", "Language",
        "Settings", "Tools", "Macro", "Run", "Plugins", "Window", "?"
    };

    int i;

    menu = CreateMenu();

    for (i = 0; i < (int)(sizeof(names) / sizeof(names[0])); i++)
    {
        popup = CreatePopupMenu();

        AppendMenu(
            popup,
            MF_STRING | MF_GRAYED,
            0,
            "(placeholder)"
        );

        AppendMenu(
            menu,
            MF_POPUP,
            (UINT_PTR)popup,
            names[i]
        );
    }

    SetMenu(hwnd, menu);
}

static HWND Npp_CreateSmallToolbarButton(HWND parent, const char *text)
{
    HWND button;

    button = CreateWindowEx(
        0,
        "BUTTON",
        text,
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        0,
        0,
        26,
        24,
        parent,
        NULL,
        g_hInstance,
        NULL
    );

    SendMessage(button, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);

    return button;
}

static void Npp_CreateToolbar(HWND hwnd)
{
    const char *buttons[] =
    {
        "N", "O", "S", "SA", "X", "C", "P", "U", "R",
        "F", "Z", "+", "-", "<", ">", "[]", "{}", "fx",
        "A", "B", "C", "1", "2", "3", "Py", "Run",
        "Blk", "K", "<", ">", "|<", ">|", "L", "R",
        "&", "&&", "<<"
    };

    int i;

    g_toolbarButtonCount = (int)(sizeof(buttons) / sizeof(buttons[0]));

    if (g_toolbarButtonCount > NPP_MAX_TOOL_BUTTONS)
        g_toolbarButtonCount = NPP_MAX_TOOL_BUTTONS;

    for (i = 0; i < g_toolbarButtonCount; i++)
        g_toolbarButtons[i] = Npp_CreateSmallToolbarButton(hwnd, buttons[i]);
}

static void Npp_CreateControls(HWND hwnd)
{
    INITCOMMONCONTROLSEX icc;

    ZeroMemory(&icc, sizeof(icc));
    icc.dwSize = sizeof(icc);
    icc.dwICC = ICC_TAB_CLASSES | ICC_BAR_CLASSES;

    InitCommonControlsEx(&icc);

    Npp_LoadRichEditClass();

    Npp_CreateToolbar(hwnd);

    g_panes[0].paneIndex = 0;
    g_panes[0].activeDoc = -1;
    g_panes[0].tab = Npp_CreateTabControl(hwnd, ID_NPP_LEFT_TAB);
    g_panes[0].edit = Npp_CreateEditor(hwnd, ID_NPP_LEFT_EDIT);
    Npp_SubclassEdit(&g_panes[0]);

    g_panes[1].paneIndex = 1;
    g_panes[1].activeDoc = -1;
    g_panes[1].tab = Npp_CreateTabControl(hwnd, ID_NPP_RIGHT_TAB);
    g_panes[1].edit = Npp_CreateEditor(hwnd, ID_NPP_RIGHT_EDIT);
    Npp_SubclassEdit(&g_panes[1]);

    g_status = CreateWindowEx(
        0,
        "STATIC",
        "",
        WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE,
        0,
        0,
        100,
        NPP_STATUS_HEIGHT,
        hwnd,
        NULL,
        g_hInstance,
        NULL
    );

    SendMessage(g_status, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);

    if (!g_statusBrush)
        g_statusBrush = CreateSolidBrush(RGB(28, 28, 28));
}

static void Npp_LayoutToolbar(HWND hwnd, int width)
{
    int i;
    int x;
    int y;
    int w;
    int h;

    (void)hwnd;

    x = 4;
    y = 3;
    h = 23;

    for (i = 0; i < g_toolbarButtonCount; i++)
    {
        w = 26;

        if (x + w > width - 4)
        {
            ShowWindow(g_toolbarButtons[i], SW_HIDE);
            continue;
        }

        ShowWindow(g_toolbarButtons[i], SW_SHOW);

        MoveWindow(
            g_toolbarButtons[i],
            x,
            y,
            w,
            h,
            TRUE
        );

        x += w + 2;
    }
}

static void Npp_LayoutPane(NppPane *pane, RECT rc)
{
    if (!pane)
        return;

    MoveWindow(
        pane->tab,
        rc.left,
        rc.top,
        rc.right - rc.left,
        NPP_TAB_HEIGHT,
        TRUE
    );

    MoveWindow(
        pane->edit,
        rc.left,
        rc.top + NPP_TAB_HEIGHT,
        rc.right - rc.left,
        rc.bottom - rc.top - NPP_TAB_HEIGHT,
        TRUE
    );
}

static void Npp_Layout(HWND hwnd)
{
    RECT rc;
    RECT leftRc;
    RECT rightRc;
    int width;
    int height;
    int usableWidth;
    int paneTop;
    int paneBottom;
    int leftWidth;

    GetClientRect(hwnd, &rc);

    width = rc.right - rc.left;
    height = rc.bottom - rc.top;

    Npp_LayoutToolbar(hwnd, width);

    paneTop = NPP_TOOLBAR_HEIGHT;
    paneBottom = height - NPP_STATUS_HEIGHT;

    usableWidth = width - NPP_SPLITTER_WIDTH;

    if (usableWidth < NPP_MIN_PANE_WIDTH * 2)
        usableWidth = NPP_MIN_PANE_WIDTH * 2;

    leftWidth = (usableWidth * g_splitPercent) / 1000;

    if (leftWidth < NPP_MIN_PANE_WIDTH)
        leftWidth = NPP_MIN_PANE_WIDTH;

    if (leftWidth > usableWidth - NPP_MIN_PANE_WIDTH)
        leftWidth = usableWidth - NPP_MIN_PANE_WIDTH;

    SetRect(&leftRc, 0, paneTop, leftWidth, paneBottom);

    SetRect(
        &g_splitterRect,
        leftWidth,
        paneTop,
        leftWidth + NPP_SPLITTER_WIDTH,
        paneBottom
    );

    SetRect(
        &rightRc,
        leftWidth + NPP_SPLITTER_WIDTH,
        paneTop,
        width,
        paneBottom
    );

    Npp_LayoutPane(&g_panes[0], leftRc);
    Npp_LayoutPane(&g_panes[1], rightRc);

    MoveWindow(
        g_status,
        0,
        height - NPP_STATUS_HEIGHT,
        width,
        NPP_STATUS_HEIGHT,
        TRUE
    );

    InvalidateRect(hwnd, &g_splitterRect, FALSE);
}

static int Npp_PointInSplitter(int x, int y)
{
    POINT pt;

    pt.x = x;
    pt.y = y;

    return PtInRect(&g_splitterRect, pt);
}

static void Npp_UpdateSplitterFromMouse(HWND hwnd, int x)
{
    RECT rc;
    int width;
    int usableWidth;

    GetClientRect(hwnd, &rc);

    width = rc.right - rc.left;
    usableWidth = width - NPP_SPLITTER_WIDTH;

    if (usableWidth <= 1)
        return;

    if (x < NPP_MIN_PANE_WIDTH)
        x = NPP_MIN_PANE_WIDTH;

    if (x > usableWidth - NPP_MIN_PANE_WIDTH)
        x = usableWidth - NPP_MIN_PANE_WIDTH;

    g_splitPercent = (x * 1000) / usableWidth;

    if (g_splitPercent < 100)
        g_splitPercent = 100;

    if (g_splitPercent > 900)
        g_splitPercent = 900;

    Npp_Layout(hwnd);
}

static LRESULT CALLBACK NppMockupWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_CREATE:
        {
            g_window = hwnd;

            Npp_CreateMenuBar(hwnd);
            Npp_CreateControls(hwnd);
            Npp_CreateInitialDocuments();
            Npp_Layout(hwnd);

            SetTimer(hwnd, NPP_TIMER_ID, NPP_TIMER_MS, NULL);

            Debug_Log("NppMockup", "WindowCreate", "NP++ mockup window created.");

            return 0;
        }

        case WM_SIZE:
        {
            Npp_Layout(hwnd);
            return 0;
        }

        case WM_SETCURSOR:
        {
            POINT pt;

            GetCursorPos(&pt);
            ScreenToClient(hwnd, &pt);

            if (Npp_PointInSplitter(pt.x, pt.y) || g_draggingSplitter)
            {
                SetCursor(LoadCursor(NULL, IDC_SIZEWE));
                return TRUE;
            }

            break;
        }

        case WM_LBUTTONDOWN:
        {
            int x;
            int y;

            x = LOWORD(lParam);
            y = HIWORD(lParam);

            if (Npp_PointInSplitter(x, y))
            {
                g_draggingSplitter = 1;
                SetCapture(hwnd);
                SetCursor(LoadCursor(NULL, IDC_SIZEWE));
                return 0;
            }

            break;
        }

        case WM_MOUSEMOVE:
        {
            if (g_draggingSplitter)
            {
                Npp_UpdateSplitterFromMouse(hwnd, LOWORD(lParam));
                return 0;
            }

            break;
        }

        case WM_LBUTTONUP:
        {
            if (g_draggingSplitter)
            {
                g_draggingSplitter = 0;
                ReleaseCapture();
                return 0;
            }

            break;
        }

        case WM_NOTIFY:
        {
            Npp_HandleTabNotify((NMHDR *)lParam);
            Npp_UpdateStatus();
            break;
        }

        case WM_TIMER:
        {
            if (wParam == NPP_TIMER_ID)
            {
                Npp_UpdateStatus();
                return 0;
            }

            break;
        }

        case WM_DROPFILES:
        {
            Npp_HandleDropFiles((HDROP)wParam, g_activePane);
            return 0;
        }

        case WM_CTLCOLORSTATIC:
        {
            if ((HWND)lParam == g_status)
            {
                SetBkColor((HDC)wParam, RGB(28, 28, 28));
                SetTextColor((HDC)wParam, RGB(220, 220, 220));
                return (LRESULT)g_statusBrush;
            }

            break;
        }

        case WM_ERASEBKGND:
        {
            return 1;
        }

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc;
            HBRUSH brush;

            hdc = BeginPaint(hwnd, &ps);

            brush = CreateSolidBrush(RGB(24, 24, 24));
            FillRect(hdc, &g_splitterRect, brush);
            DeleteObject(brush);

            EndPaint(hwnd, &ps);

            return 0;
        }

        case WM_CLOSE:
        {
            DestroyWindow(hwnd);
            return 0;
        }

        case WM_NCDESTROY:
        {
            NppMockupWindowClosedCallback callback;

            KillTimer(hwnd, NPP_TIMER_ID);

            Npp_PaneSaveActiveDocument(&g_panes[0]);
            Npp_PaneSaveActiveDocument(&g_panes[1]);

            Npp_FreePane(&g_panes[0]);
            Npp_FreePane(&g_panes[1]);

            if (g_statusBrush)
            {
                DeleteObject(g_statusBrush);
                g_statusBrush = NULL;
            }

            Debug_Log("NppMockup", "WindowDestroy", "NP++ mockup window destroyed.");

            g_window = NULL;
            g_status = NULL;
            g_toolbarButtonCount = 0;

            callback = g_onClosed;
            g_onClosed = NULL;

            if (callback)
                callback();

            return 0;
        }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

static BOOL NppMockupWindow_RegisterClass(HINSTANCE hInstance)
{
    static int registered = 0;
    WNDCLASS wc;

    if (registered)
        return TRUE;

    ZeroMemory(&wc, sizeof(wc));

    wc.lpfnWndProc = NppMockupWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = NPP_MOCKUP_WINDOW_CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;

    if (!RegisterClass(&wc))
    {
        if (GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
            return FALSE;
    }

    registered = 1;
    return TRUE;
}

HWND NppMockupWindow_Show(
    HINSTANCE hInstance,
    NppMockupWindowClosedCallback onClosed
)
{
    g_hInstance = hInstance;
    g_onClosed = onClosed;

    if (g_window && IsWindow(g_window))
    {
        ShowWindow(g_window, SW_SHOW);
        SetForegroundWindow(g_window);
        return g_window;
    }

    if (!NppMockupWindow_RegisterClass(hInstance))
        return NULL;

    g_window = CreateWindowEx(
        WS_EX_ACCEPTFILES,
        NPP_MOCKUP_WINDOW_CLASS_NAME,
        NPP_MOCKUP_WINDOW_TITLE,
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        1100,
        720,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!g_window)
        return NULL;

    ShowWindow(g_window, SW_SHOW);
    UpdateWindow(g_window);

    return g_window;
}

void NppMockupWindow_Close(void)
{
    if (g_window && IsWindow(g_window))
        DestroyWindow(g_window);
}

HWND NppMockupWindow_GetHwnd(void)
{
    return g_window;
}

int NppMockupWindow_IsOpen(void)
{
    return g_window && IsWindow(g_window);
}