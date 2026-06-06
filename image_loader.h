#ifndef IMAGE_LOADER_H
#define IMAGE_LOADER_H

#include <windows.h>

typedef struct GpImage GpImage;

typedef struct AppImage
{
    GpImage *image;
    UINT width;
    UINT height;
} AppImage;

int ImageLoader_Startup(void);
void ImageLoader_Shutdown(void);

AppImage *ImageLoader_Load(const char *fileName);

AppImage *ImageLoader_LoadFromMemory(
    const unsigned char *data,
    unsigned int size
);

AppImage *ImageLoader_LoadButtonIcon(
    HINSTANCE hInstance,
    const char *themeName,
    const char *iconBaseName,
    const char *stateName,
    int *loadFailed
);

void ImageLoader_Free(AppImage *image);

int ImageLoader_GetSize(
    AppImage *image,
    int *width,
    int *height
);

void ImageLoader_Draw(
    HDC hdc,
    AppImage *image,
    const RECT *targetRect,
    int stretch
);

#endif