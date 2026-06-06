#ifndef IMAGE_LOADER_H
#define IMAGE_LOADER_H

#include <windows.h>

typedef struct AppImage AppImage;

int ImageLoader_Startup(void);
void ImageLoader_Shutdown(void);

AppImage *ImageLoader_LoadButtonIcon(
    HINSTANCE hInstance,
    const char *themeName,
    const char *buttonName,
    const char *stateName,
    int *loadFailed
);

void ImageLoader_Free(AppImage *image);

int ImageLoader_Draw(
    HDC hdc,
    AppImage *image,
    const RECT *rect,
    int stretch
);

int ImageLoader_GetSize(
    AppImage *image,
    int *width,
    int *height
);

#endif