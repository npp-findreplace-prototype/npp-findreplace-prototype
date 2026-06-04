#include <windows.h>
#include <stdio.h>

#define BUTTON_COUNT 16

#define BUTTON_DEFAULT_WIDTH 90
#define BUTTON_DEFAULT_HEIGHT 90

#define BUTTON_HORIZONTAL_SPACING 10
#define BUTTON_VERTICAL_SPACING 10

#define horizontal 0
#define vertical 1
#define button_layout horizontal

#define BUTTON_NAME_PREFIX "mybutton_"
#define BUTTON_TEXT_FORMAT "%d"
#define BUTTON_CLICK_IDENTIFIER_FORMAT "%s"

#define BUTTON_BACK_COLOR RGB(192, 192, 192)
#define BUTTON_FORE_COLOR RGB(0, 0, 0)

#define ismetafile 0

#define BUTTON_ID_BASE 1000
#define BUTTON_FIRST_INDEX 1

#ifndef SS_TYPEMASK
#define SS_TYPEMASK 0x0000001F
#endif

static HWND g_buttons[BUTTON_COUNT];
static char g_buttonNames[BUTTON_COUNT][64];
static char g_buttonTexts[BUTTON_COUNT][64];
static HBRUSH g_buttonBrush = NULL;

void OnSquareClicked(const char *controlName)
{
    char msg[128];
    wsprintf(msg, "Clicked: %s", controlName);
    MessageBox(NULL, msg, "Static Click", MB_OK);
}

void LayoutButtons(HWND hwnd)
{
    RECT rc;
    GetClientRect(hwnd, &rc);

    int clientW = rc.right - rc.left;
    int clientH = rc.bottom - rc.top;

    int btnW = BUTTON_DEFAULT_WIDTH;
    int btnH = BUTTON_DEFAULT_HEIGHT;
    int gapX = BUTTON_HORIZONTAL_SPACING;
    int gapY = BUTTON_VERTICAL_SPACING;

    int cols = 1;
    int rows = 1;

    if (button_layout == horizontal)
    {
        cols = (clientW + gapX) / (btnW + gapX);
        if (cols < 1) cols = 1;
        if (cols > BUTTON_COUNT) cols = BUTTON_COUNT;

        rows = (BUTTON_COUNT + cols - 1) / cols;
    }
    else
    {
        rows = (clientH + gapY) / (btnH + gapY);
        if (rows < 1) rows = 1;
        if (rows > BUTTON_COUNT) rows = BUTTON_COUNT;

        cols = (BUTTON_COUNT + rows - 1) / rows;
    }

    int gridW = cols * btnW + (cols - 1) * gapX;
    int gridH = rows * btnH + (rows - 1) * gapY;

    int startX = (clientW - gridW) / 2;
    int startY = (clientH - gridH) / 2;

    if (startX < 0) startX = 0;
    if (startY < 0) startY = 0;

    for (int i = 0; i < BUTTON_COUNT; i++)
    {
        int row, col;

        if (button_layout == horizontal)
        {
            row = i / cols;
            col = i % cols;
        }
        else
        {
            col = i / rows;
            row = i % rows;
        }

        int x = startX + col * (btnW + gapX);
        int y = startY + row * (btnH + gapY);

        MoveWindow(g_buttons[i], x, y, btnW, btnH, TRUE);
    }
}

void CreateSquareButtons(HWND hwnd, HINSTANCE hInstance)
{
    DWORD style;

    style = WS_CHILD | WS_VISIBLE | SS_NOTIFY | SS_CENTER | SS_CENTERIMAGE;

    if (ismetafile)
    {
        style &= ~SS_TYPEMASK;
        style |= SS_ENHMETAFILE;
    }

    for (int i = 0; i < BUTTON_COUNT; i++)
    {
        int indexNumber = i + BUTTON_FIRST_INDEX;

        wsprintf(g_buttonNames[i], "%s%d", BUTTON_NAME_PREFIX, indexNumber);
        wsprintf(g_buttonTexts[i], BUTTON_TEXT_FORMAT, indexNumber);

        g_buttons[i] = CreateWindowEx(
            0,
            "STATIC",
            g_buttonTexts[i],
            style,
            0,
            0,
            BUTTON_DEFAULT_WIDTH,
            BUTTON_DEFAULT_HEIGHT,
            hwnd,
            (HMENU)(BUTTON_ID_BASE + i),
            hInstance,
            NULL
        );
    }

    LayoutButtons(hwnd);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_CREATE:
        {
            g_buttonBrush = CreateSolidBrush(BUTTON_BACK_COLOR);

            CREATESTRUCT *cs = (CREATESTRUCT *)lParam;
            CreateSquareButtons(hwnd, cs->hInstance);
            return 0;
        }

        case WM_SIZE:
        {
            LayoutButtons(hwnd);
            return 0;
        }

        case WM_CTLCOLORSTATIC:
        {
            HDC hdc = (HDC)wParam;

            SetTextColor(hdc, BUTTON_FORE_COLOR);
            SetBkColor(hdc, BUTTON_BACK_COLOR);

            return (LRESULT)g_buttonBrush;
        }

        case WM_COMMAND:
        {
            int controlId = LOWORD(wParam);
            int notifyCode = HIWORD(wParam);

            if (notifyCode == STN_CLICKED)
            {
                int index = controlId - BUTTON_ID_BASE;

                if (index >= 0 && index < BUTTON_COUNT)
                {
                    char identifier[128];

                    wsprintf(
                        identifier,
                        BUTTON_CLICK_IDENTIFIER_FORMAT,
                        g_buttonNames[index]
                    );

                    OnSquareClicked(identifier);
                    return 0;
                }
            }

            break;
        }

        case WM_DESTROY:
        {
            if (g_buttonBrush)
            {
                DeleteObject(g_buttonBrush);
                g_buttonBrush = NULL;
            }

            PostQuitMessage(0);
            return 0;
        }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow
)
{
    const char CLASS_NAME[] = "SimpleWindowClass";

    WNDCLASS wc;
    ZeroMemory(&wc, sizeof(wc));

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "Simple TCC Window",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        800,
        600,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL)
        return 0;

    ShowWindow(hwnd, nCmdShow);

    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}