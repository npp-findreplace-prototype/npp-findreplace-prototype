#ifndef BUTTON_GRID_INI_H
#define BUTTON_GRID_INI_H

#include <windows.h>

#include "button_grid.h"

#define BUTTON_GRID_INI_TEXT_SIZE 256

typedef struct ButtonGridIniItemText
{
    char name[BUTTON_GRID_INI_TEXT_SIZE];
    char action[BUTTON_GRID_INI_TEXT_SIZE];
    char text[BUTTON_GRID_INI_TEXT_SIZE];
    char tooltip[BUTTON_GRID_INI_TEXT_SIZE];
    char iconBaseName[BUTTON_GRID_INI_TEXT_SIZE];
} ButtonGridIniItemText;

typedef struct ButtonGridIniConfig
{
    ButtonGridConfig config;

    ButtonGridItemConfig *items;
    ButtonGridIniItemText *itemTexts;
    int itemCount;

    char fileName[MAX_PATH];

    char themeName[BUTTON_GRID_INI_TEXT_SIZE];
    char borderTitle[BUTTON_GRID_INI_TEXT_SIZE];

    char namePrefix[BUTTON_GRID_INI_TEXT_SIZE];
    char actionPrefix[BUTTON_GRID_INI_TEXT_SIZE];
    char textFormat[BUTTON_GRID_INI_TEXT_SIZE];
    char clickIdentifierFormat[BUTTON_GRID_INI_TEXT_SIZE];
} ButtonGridIniConfig;

int ButtonGridIni_Load(
    const char *fileName,
    ButtonGridIniConfig *loaded
);

int ButtonGridIni_LoadFromFileOrResource(
    const char *fileName,
    ButtonGridIniConfig *loaded
);

void ButtonGridIni_Free(ButtonGridIniConfig *loaded);

#endif