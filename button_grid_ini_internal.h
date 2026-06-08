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
    Basic utility helpers.
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
    INI key readers.
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
    Enum readers used while applying the parsed INI to ButtonGridConfig.
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
    Apply parsed INI sections to the loaded config.
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
    File/path/resource helpers.
*/
int Ini_FileExists(
    const char *fileName
);

int Ini_PathIsAbsolute(
    const char *path
);

void Ini_GetBaseName(
    const char *path,
    char *buffer,
    int bufferSize
);

void Ini_NormalizeSlashName(
    const char *name,
    char *buffer,
    int bufferSize
);

int Ini_BuildExeRelativePath(
    const char *fileName,
    char *buffer,
    int bufferSize
);

char *Ini_TextFromBytes(
    const BYTE *data,
    DWORD size
);

int Ini_LoadFromBytes(
    const char *displayName,
    const BYTE *data,
    DWORD size,
    ButtonGridIniConfig *loaded
);

int Ini_LoadFileToBytes(
    const char *fileName,
    BYTE **data,
    DWORD *size
);

int Ini_FindWin32ResourceBytes(
    const char *fileName,
    const BYTE **data,
    DWORD *size
);

int Ini_FindBuiltinResourceBytes(
    const char *fileName,
    const BYTE **data,
    DWORD *size
);

#endif