#include "npp_mockup_internal.h"

static void Npp_LoadShellDropFunctions(void)
{
    if (g_dragQueryFile && g_dragFinish)
        return;

    if (!g_shell32)
        g_shell32 = LoadLibrary("shell32.dll");

    if (!g_shell32)
        return;

    g_dragQueryFile =
        (NppDragQueryFileProc)GetProcAddress(
            g_shell32,
            "DragQueryFileA"
        );

    g_dragFinish =
        (NppDragFinishProc)GetProcAddress(
            g_shell32,
            "DragFinish"
        );
}

static UINT Npp_DragQueryFileSafe(HDROP drop, UINT index, LPSTR buffer, UINT bufferSize)
{
    Npp_LoadShellDropFunctions();

    if (!g_dragQueryFile)
        return 0;

    return g_dragQueryFile(drop, index, buffer, bufferSize);
}

static void Npp_DragFinishSafe(HDROP drop)
{
    Npp_LoadShellDropFunctions();

    if (g_dragFinish)
        g_dragFinish(drop);
}

static void Npp_GetBaseName(const char *path, char *buffer, int bufferSize)
{
    const char *base;
    int i;

    base = path ? path : "";

    for (i = 0; path && path[i]; i++)
    {
        if (path[i] == '\\' || path[i] == '/')
            base = path + i + 1;
    }

    Npp_CopyText(buffer, bufferSize, base);
}

static char *Npp_TextFromUtf16Le(const BYTE *data, DWORD size, DWORD start)
{
    DWORD i;
    DWORD out;
    DWORD count;
    char *text;
    unsigned int ch;

    count = (size - start) / 2;
    text = (char *)malloc(count + 1);

    if (!text)
        return NULL;

    out = 0;

    for (i = start; i + 1 < size; i += 2)
    {
        ch = (unsigned int)data[i] | ((unsigned int)data[i + 1] << 8);

        if (ch == 0)
            continue;

        text[out++] = (ch <= 255) ? (char)ch : '?';
    }

    text[out] = '\0';
    return text;
}

static char *Npp_TextFromUtf16Be(const BYTE *data, DWORD size, DWORD start)
{
    DWORD i;
    DWORD out;
    DWORD count;
    char *text;
    unsigned int ch;

    count = (size - start) / 2;
    text = (char *)malloc(count + 1);

    if (!text)
        return NULL;

    out = 0;

    for (i = start; i + 1 < size; i += 2)
    {
        ch = ((unsigned int)data[i] << 8) | (unsigned int)data[i + 1];

        if (ch == 0)
            continue;

        text[out++] = (ch <= 255) ? (char)ch : '?';
    }

    text[out] = '\0';
    return text;
}

static int Npp_LooksLikeUtf16Le(const BYTE *data, DWORD size)
{
    DWORD i;
    DWORD zeros;

    if (!data || size < 4)
        return 0;

    zeros = 0;

    for (i = 1; i < size && i < 64; i += 2)
    {
        if (data[i] == 0)
            zeros++;
    }

    return zeros >= 4;
}

static int Npp_LooksLikeUtf16Be(const BYTE *data, DWORD size)
{
    DWORD i;
    DWORD zeros;

    if (!data || size < 4)
        return 0;

    zeros = 0;

    for (i = 0; i < size && i < 64; i += 2)
    {
        if (data[i] == 0)
            zeros++;
    }

    return zeros >= 4;
}

static char *Npp_TextFromBytes(const BYTE *data, DWORD size)
{
    char *text;
    DWORD start;

    if (!data || size == 0)
        return Npp_DuplicateText("");

    if (size >= 2 && data[0] == 0xFF && data[1] == 0xFE)
        return Npp_TextFromUtf16Le(data, size, 2);

    if (size >= 2 && data[0] == 0xFE && data[1] == 0xFF)
        return Npp_TextFromUtf16Be(data, size, 2);

    if (Npp_LooksLikeUtf16Le(data, size))
        return Npp_TextFromUtf16Le(data, size, 0);

    if (Npp_LooksLikeUtf16Be(data, size))
        return Npp_TextFromUtf16Be(data, size, 0);

    start = 0;

    if (size >= 3 && data[0] == 0xEF && data[1] == 0xBB && data[2] == 0xBF)
        start = 3;

    text = (char *)malloc((size - start) + 1);

    if (!text)
        return NULL;

    CopyMemory(text, data + start, size - start);
    text[size - start] = '\0';

    return text;
}

static char *Npp_ReadFileText(const char *path)
{
    HANDLE file;
    DWORD size;
    DWORD readSize;
    BYTE *bytes;
    char *text;

    if (!path || !path[0])
        return NULL;

    file = CreateFile(
        path,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (file == INVALID_HANDLE_VALUE)
        return NULL;

    size = GetFileSize(file, NULL);

    if (size == INVALID_FILE_SIZE)
    {
        CloseHandle(file);
        return NULL;
    }

    bytes = (BYTE *)malloc(size ? size : 1);

    if (!bytes)
    {
        CloseHandle(file);
        return NULL;
    }

    readSize = 0;

    if (size > 0)
    {
        if (!ReadFile(file, bytes, size, &readSize, NULL) || readSize != size)
        {
            free(bytes);
            CloseHandle(file);
            return NULL;
        }
    }

    CloseHandle(file);

    text = Npp_TextFromBytes(bytes, size);

    free(bytes);

    return text;
}

static void Npp_OpenFileInPane(int paneIndex, const char *path, int selectNew)
{
    char title[MAX_PATH];
    char *text;
    NppPane *pane;

    if (paneIndex < 0 || paneIndex > 1)
        paneIndex = g_activePane;

    pane = &g_panes[paneIndex];

    text = Npp_ReadFileText(path);

    if (!text)
    {
        Debug_Log("NppMockup", "OpenFileFailed", "%s", path ? path : "");
        return;
    }

    Npp_GetBaseName(path, title, sizeof(title));

    if (!title[0])
        Npp_CopyText(title, sizeof(title), "untitled");

    Npp_PaneAddDocumentFromText(
        pane,
        title,
        path,
        text,
        selectNew
    );

    Debug_Log("NppMockup", "OpenFile", "%s", path);

    free(text);
}

void Npp_HandleDropFiles(HDROP drop, int paneIndex)
{
    UINT count;
    UINT i;
    char path[MAX_PATH];

    count = Npp_DragQueryFileSafe(drop, 0xFFFFFFFF, NULL, 0);

    for (i = 0; i < count; i++)
    {
        path[0] = '\0';

        if (Npp_DragQueryFileSafe(drop, i, path, sizeof(path)) > 0)
            Npp_OpenFileInPane(paneIndex, path, 1);
    }

    Npp_DragFinishSafe(drop);
}