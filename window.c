#include <windows.h>
#include <stdio.h>

#ifndef ATTACH_PARENT_PROCESS
#define ATTACH_PARENT_PROCESS ((DWORD)-1)
#endif

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
static int g_consoleAttached = 0;

void SetupConsoleOutput(void)
{
    typedef BOOL (WINAPI *AttachConsoleFunc)(DWORD);

    HMODULE kernel32;
    AttachConsoleFunc pAttachConsole;

    kernel32 = GetModuleHandle("kernel32.dll");

    if (!kernel32)
        return;

    pAttachConsole = (AttachConsoleFunc)GetProcAddress(kernel32, "AttachConsole");

    if (!pAttachConsole)
        return;

    if (pAttachConsole(ATTACH_PARENT_PROCESS))
    {
        g_consoleAttached = 1;

        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);

        setvbuf(stdout, NULL, _IONBF, 0);
        setvbuf(stderr, NULL, _IONBF, 0);

        printf("Console attached.\n");
    }
}

void OnSquareClicked(const char *controlName)
{
    if (g_consoleAttached)
    {
        printf("Clicked: %s\n", controlName);
        return;
    }

    {
        char msg[128];
        wsprintf(msg, "Clicked: %s", controlName);
        MessageBox(NULL, msg, "Static Click", MB_OK);
    }
}

void LayoutButtons(HWND hwnd)
{
    RECT rc;
    int clientW;
    int clientH;

    int btnW;
    int btnH;
    int gapX;
    int gapY;

    int cols;
    int rows;

    int gridW;
    int gridH;

    int startX;
    int startY;

    int i;

    GetClientRect(hwnd, &rc);

    clientW = rc.right - rc.left;
    clientH = rc.bottom - rc.top;

    btnW = BUTTON_DEFAULT_WIDTH;
    btnH = BUTTON_DEFAULT_HEIGHT;
    gapX = BUTTON_HORIZONTAL_SPACING;
    gapY = BUTTON_VERTICAL_SPACING;

    cols = 1;
    rows = 1;

    if (button_layout == horizontal)
    {
        cols = (clientW + gapX) / (btnW + gapX);

        if (cols < 1)
            cols = 1;

        if (cols > BUTTON_COUNT)
            cols = BUTTON_COUNT;

        rows = (BUTTON_COUNT + cols - 1) / cols;
    }
    else
    {
        rows = (clientH + gapY) / (btnH + gapY);

        if (rows < 1)
            rows = 1;

        if (rows > BUTTON_COUNT)
            rows = BUTTON_COUNT;

        cols = (BUTTON_COUNT + rows - 1) / rows;
    }

    gridW = cols * btnW + (cols - 1) * gapX;
    gridH = rows * btnH + (rows - 1) * gapY;

    startX = (clientW - gridW) / 2;
    startY = (clientH - gridH) / 2;

    if (startX < 0)
        startX = 0;

    if (startY < 0)
        startY = 0;

    for (i = 0; i < BUTTON_COUNT; i++)
    {
        int row;
        int col;
        int x;
        int y;

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

        x = startX + col * (btnW + gapX);
        y = startY + row * (btnH + gapY);

        MoveWindow(g_buttons[i], x, y, btnW, btnH, TRUE);
    }
}

void CreateSquareButtons(HWND hwnd, HINSTANCE hInstance)
{
    DWORD style;
    int i;

    style = WS_CHILD | WS_VISIBLE | SS_NOTIFY | SS_CENTER | SS_CENTERIMAGE;

    if (ismetafile)
    {
        style &= ~SS_TYPEMASK;
        style |= SS_ENHMETAFILE;
    }

    for (i = 0; i < BUTTON_COUNT; i++)
    {
        int indexNumber;

        indexNumber = i + BUTTON_FIRST_INDEX;

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
            CREATESTRUCT *cs;

            printf("Window created.\n");

            g_buttonBrush = CreateSolidBrush(BUTTON_BACK_COLOR);

            cs = (CREATESTRUCT *)lParam;
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
            HDC hdc;

            hdc = (HDC)wParam;

            SetTextColor(hdc, BUTTON_FORE_COLOR);
            SetBkColor(hdc, BUTTON_BACK_COLOR);

            return (LRESULT)g_buttonBrush;
        }

        case WM_COMMAND:
        {
            int controlId;
            int notifyCode;

            controlId = LOWORD(wParam);
            notifyCode = HIWORD(wParam);

            if (notifyCode == STN_CLICKED)
            {
                int index;

                index = controlId - BUTTON_ID_BASE;

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
            printf("Window destroyed.\n");

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
    HWND hwnd;
    MSG msg;

    SetupConsoleOutput();

    ZeroMemory(&wc, sizeof(wc));

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClass(&wc);

    hwnd = CreateWindowEx(
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
    {
        printf("CreateWindowEx failed.\n");
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    printf("Application started.\n");

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    printf("Application exiting.\n");

    return 0;
}