#include <windows.h>
#include <objidl.h>
#include <stdlib.h>
#include <string.h>

#include "image_loader.h"

typedef int GpStatus;
typedef void GpImage;
typedef void GpGraphics;
typedef DWORD ARGB;

typedef struct GdiplusStartupInputLocal
{
    UINT GdiplusVersion;
    void *DebugEventCallback;
    BOOL SuppressBackgroundThread;
    BOOL SuppressExternalCodecs;
} GdiplusStartupInputLocal;

typedef GpStatus (WINAPI *GdiplusStartupProc)(
    ULONG_PTR *token,
    const GdiplusStartupInputLocal *input,
    void *output
);

typedef void (WINAPI *GdiplusShutdownProc)(ULONG_PTR token);

typedef GpStatus (WINAPI *GdipLoadImageFromFileProc)(
    const WCHAR *filename,
    GpImage **image
);

typedef GpStatus (WINAPI *GdipLoadImageFromStreamProc)(
    IStream *stream,
    GpImage **image
);

typedef GpStatus (WINAPI *GdipDisposeImageProc)(GpImage *image);

typedef GpStatus (WINAPI *GdipGetImageWidthProc)(
    GpImage *image,
    UINT *width
);

typedef GpStatus (WINAPI *GdipGetImageHeightProc)(
    GpImage *image,
    UINT *height
);

typedef GpStatus (WINAPI *GdipCreateFromHDCProc)(
    HDC hdc,
    GpGraphics **graphics
);

typedef GpStatus (WINAPI *GdipDeleteGraphicsProc)(GpGraphics *graphics);

typedef GpStatus (WINAPI *GdipDrawImageRectIProc)(
    GpGraphics *graphics,
    GpImage *image,
    INT x,
    INT y,
    INT width,
    INT height
);

typedef GpStatus (WINAPI *GdipSetInterpolationModeProc)(
    GpGraphics *graphics,
    int interpolationMode
);

struct AppImage
{
    GpImage *image;
    UINT width;
    UINT height;
};

static HMODULE g_gdiplus = NULL;
static ULONG_PTR g_gdiplusToken = 0;
static int g_started = 0;

static GdiplusStartupProc pGdiplusStartup = NULL;
static GdiplusShutdownProc pGdiplusShutdown = NULL;
static GdipLoadImageFromFileProc pGdipLoadImageFromFile = NULL;
static GdipLoadImageFromStreamProc pGdipLoadImageFromStream = NULL;
static GdipDisposeImageProc pGdipDisposeImage = NULL;
static GdipGetImageWidthProc pGdipGetImageWidth = NULL;
static GdipGetImageHeightProc pGdipGetImageHeight = NULL;
static GdipCreateFromHDCProc pGdipCreateFromHDC = NULL;
static GdipDeleteGraphicsProc pGdipDeleteGraphics = NULL;
static GdipDrawImageRectIProc pGdipDrawImageRectI = NULL;
static GdipSetInterpolationModeProc pGdipSetInterpolationMode = NULL;

static FARPROC LoadGdiPlusFunction(const char *name)
{
    return GetProcAddress(g_gdiplus, name);
}

static int LoadGdiPlusFunctions(void)
{
    pGdiplusStartup = (GdiplusStartupProc)LoadGdiPlusFunction("GdiplusStartup");
    pGdiplusShutdown = (GdiplusShutdownProc)LoadGdiPlusFunction("GdiplusShutdown");
    pGdipLoadImageFromFile = (GdipLoadImageFromFileProc)LoadGdiPlusFunction("GdipLoadImageFromFile");
    pGdipLoadImageFromStream = (GdipLoadImageFromStreamProc)LoadGdiPlusFunction("GdipLoadImageFromStream");
    pGdipDisposeImage = (GdipDisposeImageProc)LoadGdiPlusFunction("GdipDisposeImage");
    pGdipGetImageWidth = (GdipGetImageWidthProc)LoadGdiPlusFunction("GdipGetImageWidth");
    pGdipGetImageHeight = (GdipGetImageHeightProc)LoadGdiPlusFunction("GdipGetImageHeight");
    pGdipCreateFromHDC = (GdipCreateFromHDCProc)LoadGdiPlusFunction("GdipCreateFromHDC");
    pGdipDeleteGraphics = (GdipDeleteGraphicsProc)LoadGdiPlusFunction("GdipDeleteGraphics");
    pGdipDrawImageRectI = (GdipDrawImageRectIProc)LoadGdiPlusFunction("GdipDrawImageRectI");
    pGdipSetInterpolationMode = (GdipSetInterpolationModeProc)LoadGdiPlusFunction("GdipSetInterpolationMode");

    if (!pGdiplusStartup ||
        !pGdiplusShutdown ||
        !pGdipLoadImageFromFile ||
        !pGdipLoadImageFromStream ||
        !pGdipDisposeImage ||
        !pGdipGetImageWidth ||
        !pGdipGetImageHeight ||
        !pGdipCreateFromHDC ||
        !pGdipDeleteGraphics ||
        !pGdipDrawImageRectI)
    {
        return 0;
    }

    return 1;
}

int ImageLoader_Startup(void)
{
    GdiplusStartupInputLocal input;

    if (g_started)
        return 1;

    g_gdiplus = LoadLibrary("gdiplus.dll");

    if (!g_gdiplus)
        return 0;

    if (!LoadGdiPlusFunctions())
    {
        FreeLibrary(g_gdiplus);
        g_gdiplus = NULL;
        return 0;
    }

    ZeroMemory(&input, sizeof(input));

    input.GdiplusVersion = 1;
    input.DebugEventCallback = NULL;
    input.SuppressBackgroundThread = FALSE;
    input.SuppressExternalCodecs = FALSE;

    if (pGdiplusStartup(&g_gdiplusToken, &input, NULL) != 0)
    {
        FreeLibrary(g_gdiplus);
        g_gdiplus = NULL;
        return 0;
    }

    g_started = 1;
    return 1;
}

void ImageLoader_Shutdown(void)
{
    if (g_started && pGdiplusShutdown)
        pGdiplusShutdown(g_gdiplusToken);

    g_started = 0;
    g_gdiplusToken = 0;

    if (g_gdiplus)
    {
        FreeLibrary(g_gdiplus);
        g_gdiplus = NULL;
    }
}

static AppImage *ImageLoader_CreateFromGpImage(GpImage *gpImage)
{
    AppImage *image;
    UINT width;
    UINT height;

    if (!gpImage)
        return NULL;

    width = 0;
    height = 0;

    if (pGdipGetImageWidth(gpImage, &width) != 0 ||
        pGdipGetImageHeight(gpImage, &height) != 0)
    {
        pGdipDisposeImage(gpImage);
        return NULL;
    }

    image = (AppImage *)malloc(sizeof(AppImage));

    if (!image)
    {
        pGdipDisposeImage(gpImage);
        return NULL;
    }

    image->image = gpImage;
    image->width = width;
    image->height = height;

    return image;
}

static AppImage *ImageLoader_LoadFromFile(const char *path)
{
    WCHAR widePath[MAX_PATH];
    GpImage *gpImage;

    if (!g_started || !path)
        return NULL;

    if (!MultiByteToWideChar(CP_ACP, 0, path, -1, widePath, MAX_PATH))
        return NULL;

    gpImage = NULL;

    if (pGdipLoadImageFromFile(widePath, &gpImage) != 0)
        return NULL;

    return ImageLoader_CreateFromGpImage(gpImage);
}

static AppImage *ImageLoader_LoadFromResource(
    HINSTANCE hInstance,
    const char *resourceName
)
{
    HRSRC resource;
    HGLOBAL loadedResource;
    DWORD resourceSize;
    void *resourceData;

    HGLOBAL memory;
    void *memoryData;
    IStream *stream;
    GpImage *gpImage;
    AppImage *image;

    if (!g_started || !hInstance || !resourceName)
        return NULL;

    resource = FindResource(hInstance, resourceName, RT_RCDATA);

    if (!resource)
        return NULL;

    loadedResource = LoadResource(hInstance, resource);

    if (!loadedResource)
        return NULL;

    resourceSize = SizeofResource(hInstance, resource);
    resourceData = LockResource(loadedResource);

    if (!resourceData || resourceSize == 0)
        return NULL;

    memory = GlobalAlloc(GMEM_MOVEABLE, resourceSize);

    if (!memory)
        return NULL;

    memoryData = GlobalLock(memory);

    if (!memoryData)
    {
        GlobalFree(memory);
        return NULL;
    }

    CopyMemory(memoryData, resourceData, resourceSize);
    GlobalUnlock(memory);

    stream = NULL;

    if (CreateStreamOnHGlobal(memory, TRUE, &stream) != S_OK)
    {
        GlobalFree(memory);
        return NULL;
    }

    gpImage = NULL;

    if (pGdipLoadImageFromStream(stream, &gpImage) != 0)
    {
        stream->lpVtbl->Release(stream);
        return NULL;
    }

    stream->lpVtbl->Release(stream);

    image = ImageLoader_CreateFromGpImage(gpImage);

    return image;
}

static void GetExeDirectory(char *buffer, int bufferSize)
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

    lstrcpy(buffer, ".");
}

static int FileExists(const char *path)
{
    DWORD attrs;

    if (!path)
        return 0;

    attrs = GetFileAttributes(path);

    if (attrs == INVALID_FILE_ATTRIBUTES)
        return 0;

    if (attrs & FILE_ATTRIBUTE_DIRECTORY)
        return 0;

    return 1;
}

static void MakeResourceName(
    char *buffer,
    const char *buttonName,
    const char *stateName,
    const char *extensionUpper
)
{
    wsprintf(buffer, "%s_%s_%s", buttonName, stateName, extensionUpper);
}

static void MakeFilePath(
    char *buffer,
    const char *directory,
    const char *buttonName,
    const char *stateName,
    const char *extensionLower
)
{
    wsprintf(
        buffer,
        "%s\\%s_%s.%s",
        directory,
        buttonName,
        stateName,
        extensionLower
    );
}

AppImage *ImageLoader_LoadButtonIcon(
    HINSTANCE hInstance,
    const char *buttonName,
    const char *stateName,
    int *loadFailed
)
{
    static const char *extensionsLower[] = { "bmp", "png", "jpg" };
    static const char *extensionsUpper[] = { "BMP", "PNG", "JPG" };

    int i;
    char resourceName[256];
    char exeDir[MAX_PATH];
    char filePath[MAX_PATH];
    AppImage *image;
    HRSRC resource;

    if (loadFailed)
        *loadFailed = 0;

    if (!buttonName || !stateName)
        return NULL;

    /*
        First try embedded resources:
            LiteralSearch_OFF_BMP
            LiteralSearch_OFF_PNG
            LiteralSearch_OFF_JPG

        Then try files beside the exe:
            LiteralSearch_OFF.bmp
            LiteralSearch_OFF.png
            LiteralSearch_OFF.jpg
    */

    for (i = 0; i < 3; i++)
    {
        MakeResourceName(
            resourceName,
            buttonName,
            stateName,
            extensionsUpper[i]
        );

        resource = FindResource(hInstance, resourceName, RT_RCDATA);

        if (resource)
        {
            image = ImageLoader_LoadFromResource(hInstance, resourceName);

            if (image)
                return image;

            if (loadFailed)
                *loadFailed = 1;

            return NULL;
        }
    }

    GetExeDirectory(exeDir, MAX_PATH);

    for (i = 0; i < 3; i++)
    {
        MakeFilePath(
            filePath,
            exeDir,
            buttonName,
            stateName,
            extensionsLower[i]
        );

        if (FileExists(filePath))
        {
            image = ImageLoader_LoadFromFile(filePath);

            if (image)
                return image;

            if (loadFailed)
                *loadFailed = 1;

            return NULL;
        }
    }

    return NULL;
}

void ImageLoader_Free(AppImage *image)
{
    if (!image)
        return;

    if (image->image && pGdipDisposeImage)
        pGdipDisposeImage(image->image);

    image->image = NULL;

    free(image);
}

int ImageLoader_Draw(
    HDC hdc,
    AppImage *image,
    const RECT *rect,
    int stretch
)
{
    GpGraphics *graphics;
    int rectW;
    int rectH;
    int drawW;
    int drawH;
    int x;
    int y;
    int result;

    if (!g_started || !hdc || !image || !image->image || !rect)
        return 0;

    rectW = rect->right - rect->left;
    rectH = rect->bottom - rect->top;

    if (rectW < 1 || rectH < 1)
        return 0;

    graphics = NULL;

    if (pGdipCreateFromHDC(hdc, &graphics) != 0)
        return 0;

    if (pGdipSetInterpolationMode)
        pGdipSetInterpolationMode(graphics, 7);

    if (stretch)
    {
        x = rect->left;
        y = rect->top;
        drawW = rectW;
        drawH = rectH;
    }
    else
    {
        drawW = (int)image->width;
        drawH = (int)image->height;

        x = rect->left + (rectW - drawW) / 2;
        y = rect->top + (rectH - drawH) / 2;
    }

    result = pGdipDrawImageRectI(
        graphics,
        image->image,
        x,
        y,
        drawW,
        drawH
    ) == 0;

    pGdipDeleteGraphics(graphics);

    return result;
}