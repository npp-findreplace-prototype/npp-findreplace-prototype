#include <windows.h>
#include <stdlib.h>
#include <ole2.h>

#include "image_loader.h"
#include "theme_resources.h"

#ifndef Ok
#define Ok 0
#endif

typedef int GpStatus;
typedef struct GpGraphics GpGraphics;

typedef struct GdiplusStartupInput
{
    UINT32 GdiplusVersion;
    void *DebugEventCallback;
    BOOL SuppressBackgroundThread;
    BOOL SuppressExternalCodecs;
} GdiplusStartupInput;

typedef struct GdiplusStartupOutput
{
    void *NotificationHook;
    void *NotificationUnhook;
} GdiplusStartupOutput;

extern GpStatus __stdcall GdiplusStartup(
    ULONG_PTR *token,
    const GdiplusStartupInput *input,
    GdiplusStartupOutput *output
);

extern void __stdcall GdiplusShutdown(ULONG_PTR token);

extern GpStatus __stdcall GdipLoadImageFromFile(
    const WCHAR *filename,
    GpImage **image
);

extern GpStatus __stdcall GdipLoadImageFromStream(
    IStream *stream,
    GpImage **image
);

extern GpStatus __stdcall GdipDisposeImage(GpImage *image);

extern GpStatus __stdcall GdipGetImageWidth(
    GpImage *image,
    UINT *width
);

extern GpStatus __stdcall GdipGetImageHeight(
    GpImage *image,
    UINT *height
);

extern GpStatus __stdcall GdipCreateFromHDC(
    HDC hdc,
    GpGraphics **graphics
);

extern GpStatus __stdcall GdipDeleteGraphics(
    GpGraphics *graphics
);

extern GpStatus __stdcall GdipDrawImageRectI(
    GpGraphics *graphics,
    GpImage *image,
    INT x,
    INT y,
    INT width,
    INT height
);

static ULONG_PTR g_gdiplusToken = 0;
static int g_gdiplusStarted = 0;

static void ImageLoader_CopyText(char *dest, int destSize, const char *src)
{
    if (!dest || destSize <= 0)
        return;

    if (!src)
        src = "";

    lstrcpyn(dest, src, destSize);
    dest[destSize - 1] = '\0';
}

static int ImageLoader_IsExeTheme(const char *themeName)
{
    if (!themeName)
        return 0;

    return
        themeName[0] == 'e' &&
        themeName[1] == 'x' &&
        themeName[2] == 'e' &&
        themeName[3] == ':';
}

static const char *ImageLoader_StripExeThemePrefix(const char *themeName)
{
    if (ImageLoader_IsExeTheme(themeName))
        return themeName + 4;

    return themeName;
}

static void ImageLoader_GetExeDirectory(char *buffer, int bufferSize)
{
    int len;
    int i;

    if (!buffer || bufferSize <= 0)
        return;

    buffer[0] = '\0';

    GetModuleFileName(NULL, buffer, bufferSize);

    len = lstrlen(buffer);

    for (i = len - 1; i >= 0; i--)
    {
        if (buffer[i] == '\\' || buffer[i] == '/')
        {
            buffer[i] = '\0';
            return;
        }
    }

    ImageLoader_CopyText(buffer, bufferSize, ".");
}

static int ImageLoader_FileExists(const char *path)
{
    DWORD attr;

    if (!path || !path[0])
        return 0;

    attr = GetFileAttributes(path);

    if (attr == INVALID_FILE_ATTRIBUTES)
        return 0;

    if (attr & FILE_ATTRIBUTE_DIRECTORY)
        return 0;

    return 1;
}

static void ImageLoader_ToWide(
    const char *src,
    WCHAR *dest,
    int destCount
)
{
    if (!dest || destCount <= 0)
        return;

    dest[0] = 0;

    if (!src)
        return;

    MultiByteToWideChar(
        CP_ACP,
        0,
        src,
        -1,
        dest,
        destCount
    );

    dest[destCount - 1] = 0;
}

int ImageLoader_Startup(void)
{
    GdiplusStartupInput input;

    if (g_gdiplusStarted)
        return 1;

    ZeroMemory(&input, sizeof(input));

    input.GdiplusVersion = 1;
    input.DebugEventCallback = NULL;
    input.SuppressBackgroundThread = FALSE;
    input.SuppressExternalCodecs = FALSE;

    if (GdiplusStartup(&g_gdiplusToken, &input, NULL) != Ok)
        return 0;

    g_gdiplusStarted = 1;
    return 1;
}

void ImageLoader_Shutdown(void)
{
    if (!g_gdiplusStarted)
        return;

    GdiplusShutdown(g_gdiplusToken);

    g_gdiplusToken = 0;
    g_gdiplusStarted = 0;
}

AppImage *ImageLoader_Load(const char *fileName)
{
    WCHAR widePath[MAX_PATH];
    GpImage *image;
    AppImage *appImage;

    if (!fileName || !fileName[0])
        return NULL;

    if (!g_gdiplusStarted)
        return NULL;

    ImageLoader_ToWide(fileName, widePath, MAX_PATH);

    image = NULL;

    if (GdipLoadImageFromFile(widePath, &image) != Ok)
        return NULL;

    appImage = (AppImage *)malloc(sizeof(AppImage));

    if (!appImage)
    {
        GdipDisposeImage(image);
        return NULL;
    }

    ZeroMemory(appImage, sizeof(AppImage));

    appImage->image = image;

    GdipGetImageWidth(image, &appImage->width);
    GdipGetImageHeight(image, &appImage->height);

    return appImage;
}

AppImage *ImageLoader_LoadFromMemory(
    const unsigned char *data,
    unsigned int size
)
{
    HGLOBAL memory;
    void *locked;
    IStream *stream;
    GpImage *image;
    AppImage *appImage;

    if (!data || size < 1)
        return NULL;

    if (!g_gdiplusStarted)
        return NULL;

    memory = GlobalAlloc(GMEM_MOVEABLE, size);

    if (!memory)
        return NULL;

    locked = GlobalLock(memory);

    if (!locked)
    {
        GlobalFree(memory);
        return NULL;
    }

    CopyMemory(locked, data, size);
    GlobalUnlock(memory);

    stream = NULL;

    if (CreateStreamOnHGlobal(memory, TRUE, &stream) != S_OK)
    {
        GlobalFree(memory);
        return NULL;
    }

    image = NULL;

    if (GdipLoadImageFromStream(stream, &image) != Ok)
    {
        stream->lpVtbl->Release(stream);
        return NULL;
    }

    stream->lpVtbl->Release(stream);

    appImage = (AppImage *)malloc(sizeof(AppImage));

    if (!appImage)
    {
        GdipDisposeImage(image);
        return NULL;
    }

    ZeroMemory(appImage, sizeof(AppImage));

    appImage->image = image;

    GdipGetImageWidth(image, &appImage->width);
    GdipGetImageHeight(image, &appImage->height);

    return appImage;
}

static AppImage *ImageLoader_LoadThemeFileFromFolder(
    const char *themeName,
    const char *fileName
)
{
    char exeDir[MAX_PATH];
    char path[MAX_PATH];

    if (!themeName || !themeName[0] || !fileName || !fileName[0])
        return NULL;

    ImageLoader_GetExeDirectory(exeDir, MAX_PATH);

    wsprintf(
        path,
        "%s\\themes\\%s\\%s",
        exeDir,
        themeName,
        fileName
    );

    if (!ImageLoader_FileExists(path))
        return NULL;

    return ImageLoader_Load(path);
}

static AppImage *ImageLoader_LoadThemeFileFromExe(
    const char *themeName,
    const char *fileName
)
{
    const unsigned char *data;
    unsigned int size;

    if (!themeName || !themeName[0] || !fileName || !fileName[0])
        return NULL;

    size = 0;

    data = ThemeResources_FindFile(
        themeName,
        fileName,
        &size
    );

    if (!data || size < 1)
        return NULL;

    return ImageLoader_LoadFromMemory(data, size);
}

static AppImage *ImageLoader_LoadThemeFile(
    const char *themeName,
    const char *fileName
)
{
    const char *plainThemeName;
    AppImage *image;

    if (!themeName || !themeName[0] || !fileName || !fileName[0])
        return NULL;

    plainThemeName = ImageLoader_StripExeThemePrefix(themeName);

    if (ImageLoader_IsExeTheme(themeName))
    {
        return ImageLoader_LoadThemeFileFromExe(
            plainThemeName,
            fileName
        );
    }

    image = ImageLoader_LoadThemeFileFromFolder(
        plainThemeName,
        fileName
    );

    if (image)
        return image;

    return ImageLoader_LoadThemeFileFromExe(
        plainThemeName,
        fileName
    );
}

AppImage *ImageLoader_LoadButtonIcon(
    HINSTANCE hInstance,
    const char *themeName,
    const char *iconBaseName,
    const char *stateName,
    int *loadFailed
)
{
    static const char *extensions[] =
    {
        "bmp",
        "png",
        "jpg",
        NULL
    };

    char fileName[MAX_PATH];
    int i;
    AppImage *image;

    (void)hInstance;

    if (loadFailed)
        *loadFailed = 0;

    if (!themeName || !themeName[0])
        return NULL;

    if (!iconBaseName || !iconBaseName[0])
        return NULL;

    if (!stateName || !stateName[0])
        return NULL;

    for (i = 0; extensions[i]; i++)
    {
        wsprintf(
            fileName,
            "%s_%s.%s",
            iconBaseName,
            stateName,
            extensions[i]
        );

        image = ImageLoader_LoadThemeFile(
            themeName,
            fileName
        );

        if (image)
            return image;
    }

    if (loadFailed)
        *loadFailed = 0;

    return NULL;
}

void ImageLoader_Free(AppImage *image)
{
    if (!image)
        return;

    if (image->image)
    {
        GdipDisposeImage(image->image);
        image->image = NULL;
    }

    free(image);
}

int ImageLoader_GetSize(
    AppImage *image,
    int *width,
    int *height
)
{
    if (width)
        *width = 0;

    if (height)
        *height = 0;

    if (!image)
        return 0;

    if (width)
        *width = (int)image->width;

    if (height)
        *height = (int)image->height;

    return image->width > 0 && image->height > 0;
}

void ImageLoader_Draw(
    HDC hdc,
    AppImage *image,
    const RECT *targetRect,
    int stretch
)
{
    GpGraphics *graphics;
    int targetW;
    int targetH;
    int drawW;
    int drawH;
    int drawX;
    int drawY;

    if (!hdc || !image || !image->image || !targetRect)
        return;

    targetW = targetRect->right - targetRect->left;
    targetH = targetRect->bottom - targetRect->top;

    if (targetW <= 0 || targetH <= 0)
        return;

    if (stretch)
    {
        drawX = targetRect->left;
        drawY = targetRect->top;
        drawW = targetW;
        drawH = targetH;
    }
    else
    {
        drawW = (int)image->width;
        drawH = (int)image->height;

        if (drawW > targetW)
            drawW = targetW;

        if (drawH > targetH)
            drawH = targetH;

        drawX = targetRect->left + (targetW - drawW) / 2;
        drawY = targetRect->top + (targetH - drawH) / 2;
    }

    graphics = NULL;

    if (GdipCreateFromHDC(hdc, &graphics) != Ok)
        return;

    GdipDrawImageRectI(
        graphics,
        image->image,
        drawX,
        drawY,
        drawW,
        drawH
    );

    GdipDeleteGraphics(graphics);
}