#include "button_grid_ini_internal.h"

static const char g_builtinGridTesterIni[] =
"[Grid]\r\n"
"buttonCount=12\r\n"
"\r\n"
"buttonWidth=90\r\n"
"buttonHeight=90\r\n"
"horizontalSpacing=10\r\n"
"verticalSpacing=10\r\n"
"\r\n"
"layout=horizontal\r\n"
"sizeMode=aspectByLayout\r\n"
"\r\n"
"showText=0\r\n"
"hidePartialButtons=1\r\n"
"resizeInLayoutSteps=0\r\n"
"settingsWheelScrub=0\r\n"
"dpiScaleEnabled=1\r\n"
"\r\n"
"themeName=darkmetalred\r\n"
"allowThemeSelection=1\r\n"
"\r\n"
"showBorder=1\r\n"
"showBorderTitle=1\r\n"
"borderTitle=Search Options\r\n"
"borderPadding=12\r\n"
"borderTitleHeight=22\r\n"
"borderTitlePadding=6\r\n"
"borderTitleFontSize=0\r\n"
"borderTitleTransparent=1\r\n"
"borderTitleAutoBackColor=1\r\n"
"borderStyle=rounded\r\n"
"borderThickness=1\r\n"
"borderCornerRadius=10\r\n"
"borderColor=#606060\r\n"
"borderLightColor=#FFFFFF\r\n"
"borderShadowColor=#808080\r\n"
"borderTitleColor=#000000\r\n"
"borderTitleBackColor=#F0F0F0\r\n"
"\r\n"
"showGearIcon=1\r\n"
"gearCorner=topRight\r\n"
"gearSize=24\r\n"
"gearMargin=8\r\n"
"gearColor=#3C3C3C\r\n"
"gearBackColor=#F5F5F5\r\n"
"gearBorderColor=#787878\r\n"
"\r\n"
"buttonBackMode=transparent\r\n"
"showButtonBorder=0\r\n"
"buttonBorderThickness=1\r\n"
"buttonBorderColor=#000000\r\n"
"backColor=#C0C0C0\r\n"
"foreColor=#000000\r\n"
"\r\n"
"usePictures=1\r\n"
"toggleOnClick=1\r\n"
"defaultState=0\r\n"
"stretchPictures=1\r\n"
"\r\n"
"generatedOffPictureColor=#969696\r\n"
"generatedOnPictureColor=#50BE50\r\n"
"generatedErrorPictureColor=#BE6464\r\n"
"\r\n"
"[Button1]\r\n"
"name=LiteralSearch\r\n"
"text=Literal\r\n"
"tooltip=Literal Search\r\n"
"iconBaseName=LiteralSearch\r\n"
"behavior=radio\r\n"
"radioGroup=1\r\n"
"defaultState=1\r\n"
"\r\n"
"[Button2]\r\n"
"name=EscapedLiteralSearch\r\n"
"text=Escaped\r\n"
"tooltip=Escaped Literal Search\r\n"
"iconBaseName=EscapedLiteralSearch\r\n"
"behavior=radio\r\n"
"radioGroup=1\r\n"
"\r\n"
"[Button3]\r\n"
"name=RegExSearch\r\n"
"text=Regex\r\n"
"tooltip=Regex Search\r\n"
"iconBaseName=RegExSearch\r\n"
"behavior=radio\r\n"
"radioGroup=1\r\n"
"\r\n"
"[Button4]\r\n"
"name=SemanticSearch\r\n"
"text=Semantic\r\n"
"tooltip=Semantic Search\r\n"
"iconBaseName=SemanticSearch\r\n"
"behavior=radio\r\n"
"radioGroup=1\r\n"
"\r\n"
"[Button5]\r\n"
"name=CaseSensitive\r\n"
"text=Case\r\n"
"tooltip=Case Sensitive\r\n"
"iconBaseName=CaseSensitive\r\n"
"behavior=toggle\r\n"
"\r\n"
"[Button6]\r\n"
"name=DiacriticSensitive\r\n"
"text=Diacritic\r\n"
"tooltip=Diacritic Sensitive\r\n"
"iconBaseName=DiacriticSensitive\r\n"
"behavior=toggle\r\n"
"\r\n"
"[Button7]\r\n"
"name=DotIncludesNewline\r\n"
"text=Dot NL\r\n"
"tooltip=Dot Includes Newline\r\n"
"iconBaseName=DotIncludesNewline\r\n"
"behavior=toggle\r\n"
"\r\n"
"[Button8]\r\n"
"name=FuzzyLogicSearch\r\n"
"text=Fuzzy\r\n"
"tooltip=Fuzzy Logic Search\r\n"
"iconBaseName=FuzzyLogicSearch\r\n"
"behavior=toggle\r\n"
"\r\n"
"[Button9]\r\n"
"name=WrapAround\r\n"
"text=Wrap\r\n"
"tooltip=Wrap Around\r\n"
"iconBaseName=WrapAround\r\n"
"behavior=toggle\r\n"
"\r\n"
"[Button10]\r\n"
"name=WholeWord\r\n"
"text=Word\r\n"
"tooltip=Whole Word\r\n"
"iconBaseName=WholeWord\r\n"
"behavior=toggle\r\n"
"\r\n"
"[Button11]\r\n"
"name=BooleanSearch\r\n"
"text=Boolean\r\n"
"tooltip=Boolean Search\r\n"
"iconBaseName=BooleanSearch\r\n"
"behavior=toggle\r\n"
"\r\n"
"[Button12]\r\n"
"name=Settings\r\n"
"text=Settings\r\n"
"tooltip=Settings\r\n"
"iconBaseName=Settings\r\n"
"behavior=toggle\r\n";

int Ini_FileExists(const char *fileName)
{
    DWORD attrs;

    if (!fileName || !fileName[0])
        return 0;

    attrs = GetFileAttributes(fileName);

    if (attrs == INVALID_FILE_ATTRIBUTES)
        return 0;

    if (attrs & FILE_ATTRIBUTE_DIRECTORY)
        return 0;

    return 1;
}

int Ini_PathIsAbsolute(const char *path)
{
    if (!path || !path[0])
        return 0;

    if (path[0] == '\\' && path[1] == '\\')
        return 1;

    if (((path[0] >= 'A' && path[0] <= 'Z') ||
         (path[0] >= 'a' && path[0] <= 'z')) &&
        path[1] == ':')
    {
        return 1;
    }

    return 0;
}

static void Ini_GetBaseName(
    const char *path,
    char *buffer,
    int bufferSize
)
{
    const char *base;
    int i;

    base = path ? path : "";

    for (i = 0; path && path[i]; i++)
    {
        if (path[i] == '\\' || path[i] == '/')
            base = path + i + 1;
    }

    Ini_CopyText(buffer, bufferSize, base);
}

static void Ini_NormalizeSlashName(
    const char *name,
    char *buffer,
    int bufferSize
)
{
    int i;

    Ini_CopyText(buffer, bufferSize, name);

    for (i = 0; buffer[i]; i++)
    {
        if (buffer[i] == '\\')
            buffer[i] = '/';
    }
}

int Ini_BuildExeRelativePath(
    const char *fileName,
    char *buffer,
    int bufferSize
)
{
    char exePath[MAX_PATH];
    int len;
    int i;
    int lastSlash;

    if (!fileName || !buffer || bufferSize <= 0)
        return 0;

    if (!GetModuleFileName(NULL, exePath, sizeof(exePath)))
        return 0;

    exePath[sizeof(exePath) - 1] = '\0';

    len = lstrlen(exePath);
    lastSlash = -1;

    for (i = 0; i < len; i++)
    {
        if (exePath[i] == '\\' || exePath[i] == '/')
            lastSlash = i;
    }

    if (lastSlash < 0)
        return 0;

    exePath[lastSlash + 1] = '\0';

    Ini_CopyText(buffer, bufferSize, exePath);
    Ini_AppendText(buffer, bufferSize, fileName);

    return 1;
}

static char *Ini_TextFromUtf16Le(const BYTE *data, DWORD size, DWORD start)
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

        if (ch <= 255)
            text[out++] = (char)ch;
        else
            text[out++] = '?';
    }

    text[out] = '\0';
    return text;
}

static char *Ini_TextFromUtf16Be(const BYTE *data, DWORD size, DWORD start)
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

        if (ch <= 255)
            text[out++] = (char)ch;
        else
            text[out++] = '?';
    }

    text[out] = '\0';
    return text;
}

static int Ini_LooksLikeUtf16Le(const BYTE *data, DWORD size)
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

static int Ini_LooksLikeUtf16Be(const BYTE *data, DWORD size)
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

static char *Ini_TextFromBytes(const BYTE *data, DWORD size)
{
    char *text;
    DWORD start;

    if (!data || size == 0)
        return NULL;

    if (size >= 2 && data[0] == 0xFF && data[1] == 0xFE)
        return Ini_TextFromUtf16Le(data, size, 2);

    if (size >= 2 && data[0] == 0xFE && data[1] == 0xFF)
        return Ini_TextFromUtf16Be(data, size, 2);

    if (Ini_LooksLikeUtf16Le(data, size))
        return Ini_TextFromUtf16Le(data, size, 0);

    if (Ini_LooksLikeUtf16Be(data, size))
        return Ini_TextFromUtf16Be(data, size, 0);

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

static int Ini_LoadFromBytes(
    const char *displayName,
    const BYTE *data,
    DWORD size,
    ButtonGridIniConfig *loaded
)
{
    IniSource source;
    char *text;
    int ok;

    if (!data || size == 0 || !loaded)
        return 0;

    text = Ini_TextFromBytes(data, size);

    if (!text)
        return 0;

    ZeroMemory(&source, sizeof(source));

    source.displayName = displayName;
    source.memoryText = text;

    ok = ButtonGridIni_LoadFromSource(&source, loaded);

    free(text);

    return ok;
}

static int Ini_LoadFileToBytes(
    const char *fileName,
    BYTE **data,
    DWORD *size
)
{
    HANDLE file;
    DWORD fileSize;
    DWORD readSize;
    BYTE *buffer;

    if (data)
        *data = NULL;

    if (size)
        *size = 0;

    if (!fileName || !data || !size)
        return 0;

    file = CreateFile(
        fileName,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (file == INVALID_HANDLE_VALUE)
        return 0;

    fileSize = GetFileSize(file, NULL);

    if (fileSize == INVALID_FILE_SIZE || fileSize == 0)
    {
        CloseHandle(file);
        return 0;
    }

    buffer = (BYTE *)malloc(fileSize);

    if (!buffer)
    {
        CloseHandle(file);
        return 0;
    }

    readSize = 0;

    if (!ReadFile(file, buffer, fileSize, &readSize, NULL) ||
        readSize != fileSize)
    {
        free(buffer);
        CloseHandle(file);
        return 0;
    }

    CloseHandle(file);

    *data = buffer;
    *size = fileSize;

    return 1;
}

int Ini_LoadFileToConfig(
    const char *fileName,
    ButtonGridIniConfig *loaded
)
{
    BYTE *data;
    DWORD size;
    int ok;

    if (!fileName || !loaded)
        return 0;

    if (!Ini_FileExists(fileName))
        return 0;

    data = NULL;
    size = 0;

    if (!Ini_LoadFileToBytes(fileName, &data, &size))
        return 0;

    ok = Ini_LoadFromBytes(fileName, data, size, loaded);

    free(data);

    return ok;
}

static int Ini_FindWin32ResourceBytesByName(
    const char *resourceName,
    const BYTE **data,
    DWORD *size
)
{
    HRSRC resourceInfo;
    HGLOBAL resourceData;
    const char *types[4];
    int i;

    if (data)
        *data = NULL;

    if (size)
        *size = 0;

    if (!resourceName || !resourceName[0])
        return 0;

    types[0] = "INI";
    types[1] = "TEXT";
    types[2] = "CONFIG";
    types[3] = (const char *)RT_RCDATA;

    resourceInfo = NULL;

    for (i = 0; i < 4; i++)
    {
        resourceInfo = FindResource(NULL, resourceName, types[i]);

        if (resourceInfo)
            break;
    }

    if (!resourceInfo)
        return 0;

    resourceData = LoadResource(NULL, resourceInfo);

    if (!resourceData)
        return 0;

    if (data)
        *data = (const BYTE *)LockResource(resourceData);

    if (size)
        *size = SizeofResource(NULL, resourceInfo);

    if (!data || !*data || !size || *size == 0)
        return 0;

    return 1;
}

static int Ini_FindWin32ResourceBytes(
    const char *fileName,
    const BYTE **data,
    DWORD *size
)
{
    char normalized[MAX_PATH];
    char baseName[MAX_PATH];

    if (Ini_FindWin32ResourceBytesByName(fileName, data, size))
        return 1;

    Ini_NormalizeSlashName(fileName, normalized, sizeof(normalized));

    if (!Ui_SameText(normalized, fileName))
    {
        if (Ini_FindWin32ResourceBytesByName(normalized, data, size))
            return 1;
    }

    Ini_GetBaseName(fileName, baseName, sizeof(baseName));

    if (baseName[0] && !Ui_SameText(baseName, fileName))
    {
        if (Ini_FindWin32ResourceBytesByName(baseName, data, size))
            return 1;
    }

    return 0;
}

int Ini_LoadWin32ResourceToConfig(
    const char *fileName,
    ButtonGridIniConfig *loaded
)
{
    char displayName[MAX_PATH + 32];
    const BYTE *resourceData;
    DWORD resourceSize;

    resourceData = NULL;
    resourceSize = 0;

    if (!Ini_FindWin32ResourceBytes(fileName, &resourceData, &resourceSize))
        return 0;

    Ini_CopyText(displayName, sizeof(displayName), "win32-resource:");
    Ini_AppendText(displayName, sizeof(displayName), fileName);

    return Ini_LoadFromBytes(
        displayName,
        resourceData,
        resourceSize,
        loaded
    );
}

int Ini_LoadEmbeddedResourceToConfig(
    const char *fileName,
    ButtonGridIniConfig *loaded
)
{
    char displayName[MAX_PATH + 32];
    const BYTE *resourceData;
    unsigned long embeddedSize;

    resourceData = NULL;
    embeddedSize = 0;

    if (!EmbeddedResources_Find("ini", fileName, &resourceData, &embeddedSize))
        return 0;

    Ini_CopyText(displayName, sizeof(displayName), "embedded:");
    Ini_AppendText(displayName, sizeof(displayName), fileName);

    return Ini_LoadFromBytes(
        displayName,
        resourceData,
        (DWORD)embeddedSize,
        loaded
    );
}

int Ini_LoadBuiltinResourceToConfig(
    const char *fileName,
    ButtonGridIniConfig *loaded
)
{
    char baseName[MAX_PATH];
    char displayName[MAX_PATH + 32];
    const BYTE *resourceData;
    DWORD resourceSize;

    Ini_GetBaseName(fileName, baseName, sizeof(baseName));

    if (!Ui_SameTextI(baseName, "grid_tester.ini"))
        return 0;

    resourceData = (const BYTE *)g_builtinGridTesterIni;
    resourceSize = (DWORD)lstrlen(g_builtinGridTesterIni);

    Ini_CopyText(displayName, sizeof(displayName), "builtin:");
    Ini_AppendText(displayName, sizeof(displayName), fileName);

    return Ini_LoadFromBytes(
        displayName,
        resourceData,
        resourceSize,
        loaded
    );
}