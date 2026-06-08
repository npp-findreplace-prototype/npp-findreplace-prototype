#ifndef BUTTON_GRID_INI_INTERNAL_H
#define BUTTON_GRID_INI_INTERNAL_H

#include "win_compat.h"
#include "ui_common.h"

#include <stdio.h>
#include <stdlib.h>

#include "button_grid_internal.h"
#include "button_grid_ini.h"
#include "embedded_resources.h"

#ifndef RT_RCDATA
#define RT_RCDATA MAKEINTRESOURCE(10)
#endif

typedef struct IniSource
{
    const char *displayName;
    const char *memoryText;
} IniSource;

/*
    Shared basic utility helpers.
*/
void Ini_CopyText(
    char *dest,
    int destSize,
    const char *src
);

void Ini_AppendText(
    char *dest,
    int destSize,
    const char *src
);

void Ini_TrimInPlace(
    char *text
);

int Ini_LineIsCommentOrEmpty(
    const char *line
);

void Ini_ReadMemoryLine(
    const char **cursor,
    char *line,
    int lineSize
);

/*
    Generic INI value readers.
*/
void Ini_ReadString(
    const IniSource *source,
    const char *section,
    const char *key,
    const char *defaultValue,
    char *buffer,
    int bufferSize
);

int Ini_ReadInt(
    const IniSource *source,
    const char *section,
    const char *key,
    int defaultValue
);

int Ini_ReadBool(
    const IniSource *source,
    const char *section,
    const char *key,
    int defaultValue
);

COLORREF Ini_ReadColor(
    const IniSource *source,
    const char *section,
    const char *key,
    COLORREF defaultValue
);

/*
    Enum readers used by button_grid_ini_apply_*.
*/
int Ini_ReadLayout(
    const IniSource *source,
    const char *section,
    const char *key,
    int defaultValue
);

int Ini_ReadSizeMode(
    const IniSource *source,
    const char *section,
    const char *key,
    int defaultValue
);

int Ini_ReadButtonBackMode(
    const IniSource *source,
    const char *section,
    const char *key,
    int defaultValue
);

int Ini_ReadBorderStyle(
    const IniSource *source,
    const char *section,
    const char *key,
    int defaultValue
);

int Ini_ReadGearCorner(
    const IniSource *source,
    const char *section,
    const char *key,
    int defaultValue
);

int Ini_ReadContentAlignment(
    const IniSource *source,
    const char *section,
    const char *key,
    int defaultValue
);

int Ini_ReadBehavior(
    const IniSource *source,
    const char *section,
    const char *key,
    int defaultValue
);

int Ini_ReadShowTextOverride(
    const IniSource *source,
    const char *section,
    const char *key,
    int defaultValue
);

/*
    Apply INI sections to ButtonGridIniConfig.
*/
void Ini_ReadGridSection(
    const IniSource *source,
    ButtonGridIniConfig *loaded
);

void Ini_ReadButtonSection(
    const IniSource *source,
    ButtonGridIniConfig *loaded,
    int index
);

int ButtonGridIni_LoadFromSource(
    const IniSource *source,
    ButtonGridIniConfig *loaded
);

/*
    Resource/file loading entry points used by button_grid_ini.c.
    Lower-level byte/path helpers inside button_grid_ini_resources.c
    should stay static and are intentionally not declared here.
*/
int Ini_FileExists(
    const char *fileName
);

int Ini_PathIsAbsolute(
    const char *path
);

int Ini_BuildExeRelativePath(
    const char *fileName,
    char *buffer,
    int bufferSize
);

int Ini_LoadFileToConfig(
    const char *fileName,
    ButtonGridIniConfig *loaded
);

int Ini_LoadWin32ResourceToConfig(
    const char *fileName,
    ButtonGridIniConfig *loaded
);

int Ini_LoadEmbeddedResourceToConfig(
    const char *fileName,
    ButtonGridIniConfig *loaded
);

int Ini_LoadBuiltinResourceToConfig(
    const char *fileName,
    ButtonGridIniConfig *loaded
);

#endif