#include "new_test_layout_settings_internal.h"
#include "embedded_resources.h"

#define NTL_SETTINGS_INI_NAME "new_test_layout.ini"
#define NTL_SETTINGS_INI_SECTION "NewTestLayout"

typedef struct SettingsIniFieldDef
{
    const char *name;
    size_t offset;
    int isColor;
} SettingsIniFieldDef;

static const SettingsIniFieldDef g_settingsIniFields[] =
{
    { "overlayWidth", offsetof(NewTestLayoutSettingsConfig, overlayWidth), 0 },
    { "overlayMargin", offsetof(NewTestLayoutSettingsConfig, overlayMargin), 0 },
    { "rowHeight", offsetof(NewTestLayoutSettingsConfig, rowHeight), 0 },
    { "gap", offsetof(NewTestLayoutSettingsConfig, gap), 0 },
    { "singleRowModeGridHeight", offsetof(NewTestLayoutSettingsConfig, singleRowModeGridHeight), 0 },

    { "replaceBoxMinHeightPercent", offsetof(NewTestLayoutSettingsConfig, replaceBoxMinHeightPercent), 0 },
    { "utilityButtonsMinWidthPercent", offsetof(NewTestLayoutSettingsConfig, utilityButtonsMinWidthPercent), 0 },
    { "modeGridMinHeightPercent", offsetof(NewTestLayoutSettingsConfig, modeGridMinHeightPercent), 0 },
    { "findDocumentGridMinHeightPercent", offsetof(NewTestLayoutSettingsConfig, findDocumentGridMinHeightPercent), 0 },
    { "replaceDocumentGridMinHeightPercent", offsetof(NewTestLayoutSettingsConfig, replaceDocumentGridMinHeightPercent), 0 },
    { "replaceSelectionGridMinHeightPercent", offsetof(NewTestLayoutSettingsConfig, replaceSelectionGridMinHeightPercent), 0 },
    { "leftModePanelMinWidthPercent", offsetof(NewTestLayoutSettingsConfig, leftModePanelMinWidthPercent), 0 },
    { "leftModePanelMinGroupWidthPercent", offsetof(NewTestLayoutSettingsConfig, leftModePanelMinGroupWidthPercent), 0 },

    { "autoLayoutEnabled", offsetof(NewTestLayoutSettingsConfig, autoLayoutEnabled), 0 },
    { "showReplaceBox", offsetof(NewTestLayoutSettingsConfig, showReplaceBox), 0 },
    { "showUtilityButtons", offsetof(NewTestLayoutSettingsConfig, showUtilityButtons), 0 },
    { "showModeGrid", offsetof(NewTestLayoutSettingsConfig, showModeGrid), 0 },
    { "showFindDocumentGrid", offsetof(NewTestLayoutSettingsConfig, showFindDocumentGrid), 0 },
    { "showReplaceDocumentGrid", offsetof(NewTestLayoutSettingsConfig, showReplaceDocumentGrid), 0 },
    { "showReplaceSelectionGrid", offsetof(NewTestLayoutSettingsConfig, showReplaceSelectionGrid), 0 },

    { "showCounts", offsetof(NewTestLayoutSettingsConfig, showCounts), 0 },
    { "showZeroCounts", offsetof(NewTestLayoutSettingsConfig, showZeroCounts), 0 },
    { "countInParentheses", offsetof(NewTestLayoutSettingsConfig, countInParentheses), 0 },

    { "enableLeftModePanel", offsetof(NewTestLayoutSettingsConfig, enableLeftModePanel), 0 },
    { "enableGrowingBorder", offsetof(NewTestLayoutSettingsConfig, enableGrowingBorder), 0 },
    { "enableRecentFindDropdown", offsetof(NewTestLayoutSettingsConfig, enableRecentFindDropdown), 0 },
    { "enableRecentReplaceDropdown", offsetof(NewTestLayoutSettingsConfig, enableRecentReplaceDropdown), 0 },

    { "fauxComboPlaceholderLarge", offsetof(NewTestLayoutSettingsConfig, fauxComboPlaceholderLarge), 0 },
    { "actionButtonCountColorEnabled", offsetof(NewTestLayoutSettingsConfig, actionButtonCountColorEnabled), 0 },

    { "overlayBackColor", offsetof(NewTestLayoutSettingsConfig, overlayBackColor), 1 },
    { "overlayBorderColor", offsetof(NewTestLayoutSettingsConfig, overlayBorderColor), 1 },
    { "overlayTitleColor", offsetof(NewTestLayoutSettingsConfig, overlayTitleColor), 1 },
    { "labelColor", offsetof(NewTestLayoutSettingsConfig, labelColor), 1 },
    { "textColor", offsetof(NewTestLayoutSettingsConfig, textColor), 1 },
    { "mutedTextColor", offsetof(NewTestLayoutSettingsConfig, mutedTextColor), 1 },
    { "accentColor", offsetof(NewTestLayoutSettingsConfig, accentColor), 1 },
    { "buttonBackColor", offsetof(NewTestLayoutSettingsConfig, buttonBackColor), 1 },
    { "buttonBorderColor", offsetof(NewTestLayoutSettingsConfig, buttonBorderColor), 1 },
    { "buttonTextColor", offsetof(NewTestLayoutSettingsConfig, buttonTextColor), 1 },
    { "countTextColor", offsetof(NewTestLayoutSettingsConfig, countTextColor), 1 }
};

static char *SettingsIni_TrimLeft(
    char *text
)
{
    if (!text)
        return text;

    while (*text == ' ' || *text == '\t' || *text == '\r' || *text == '\n')
        text++;

    return text;
}

static void SettingsIni_TrimRight(
    char *text
)
{
    int len;

    if (!text)
        return;

    len = lstrlen(text);

    while (len > 0)
    {
        if (text[len - 1] != ' ' &&
            text[len - 1] != '\t' &&
            text[len - 1] != '\r' &&
            text[len - 1] != '\n')
        {
            break;
        }

        text[len - 1] = '\0';
        len--;
    }
}

static void SettingsIni_SetField(
    NewTestLayoutSettingsConfig *config,
    const char *key,
    const char *valueText
)
{
    int i;
    long value;
    BYTE *base;

    if (!config || !key || !valueText)
        return;

    value = strtol(valueText, NULL, 0);
    base = (BYTE *)config;

    for (i = 0; i < (int)(sizeof(g_settingsIniFields) / sizeof(g_settingsIniFields[0])); i++)
    {
        if (lstrcmpi(key, g_settingsIniFields[i].name) == 0)
        {
            if (g_settingsIniFields[i].isColor)
                *((COLORREF *)(base + g_settingsIniFields[i].offset)) = (COLORREF)value;
            else
                *((int *)(base + g_settingsIniFields[i].offset)) = (int)value;

            return;
        }
    }
}

static void SettingsIni_ParseBuffer(
    NewTestLayoutSettingsConfig *config,
    const char *buffer
)
{
    char line[512];
    char section[128];
    const char *p;
    char *out;
    char *trimmed;
    char *equals;
    int inSection;
    int len;

    if (!config || !buffer)
        return;

    p = buffer;
    inSection = 0;

    while (*p)
    {
        out = line;
        len = 0;

        while (*p && *p != '\n' && len < (int)sizeof(line) - 1)
        {
            *out++ = *p++;
            len++;
        }

        while (*p && *p != '\n')
            p++;

        if (*p == '\n')
            p++;

        *out = '\0';

        trimmed = SettingsIni_TrimLeft(line);
        SettingsIni_TrimRight(trimmed);

        if (!trimmed[0])
            continue;

        if (trimmed[0] == ';' || trimmed[0] == '#')
            continue;

        if (trimmed[0] == '[')
        {
            lstrcpyn(section, trimmed + 1, sizeof(section));
            section[sizeof(section) - 1] = '\0';

            out = section;

            while (*out && *out != ']')
                out++;

            *out = '\0';
            SettingsIni_TrimRight(section);

            inSection = lstrcmpi(section, NTL_SETTINGS_INI_SECTION) == 0;
            continue;
        }

        if (!inSection)
            continue;

        equals = trimmed;

        while (*equals && *equals != '=')
            equals++;

        if (*equals != '=')
            continue;

        *equals = '\0';
        equals++;

        SettingsIni_TrimRight(trimmed);
        equals = SettingsIni_TrimLeft(equals);
        SettingsIni_TrimRight(equals);

        SettingsIni_SetField(config, trimmed, equals);
    }
}

static int SettingsIni_GetExternalPath(
    char *path,
    int pathSize
)
{
    DWORD len;
    char *lastSlash;
    char *p;
    int dirLen;

    if (!path || pathSize <= 0)
        return 0;

    len = GetModuleFileName(NULL, path, pathSize);

    if (len == 0 || len >= (DWORD)pathSize)
        return 0;

    path[pathSize - 1] = '\0';

    lastSlash = NULL;
    p = path;

    while (*p)
    {
        if (*p == '\\' || *p == '/')
            lastSlash = p;

        p++;
    }

    if (lastSlash)
        *(lastSlash + 1) = '\0';
    else
        path[0] = '\0';

    dirLen = lstrlen(path);

    if (dirLen + lstrlen(NTL_SETTINGS_INI_NAME) + 1 > pathSize)
        return 0;

    lstrcpyn(path + dirLen, NTL_SETTINGS_INI_NAME, pathSize - dirLen);
    path[pathSize - 1] = '\0';

    return 1;
}

static int SettingsIni_LoadFile(
    NewTestLayoutSettingsConfig *config,
    const char *path
)
{
    HANDLE file;
    DWORD size;
    DWORD readSize;
    char *buffer;
    int ok;

    if (!config || !path || !path[0])
        return 0;

    file = CreateFile(
        path,
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (file == INVALID_HANDLE_VALUE)
        return 0;

    size = GetFileSize(file, NULL);

    if (size == INVALID_FILE_SIZE || size > 1024 * 1024)
    {
        CloseHandle(file);
        return 0;
    }

    buffer = (char *)malloc(size + 1);

    if (!buffer)
    {
        CloseHandle(file);
        return 0;
    }

    readSize = 0;
    ok = ReadFile(file, buffer, size, &readSize, NULL);

    CloseHandle(file);

    if (!ok)
    {
        free(buffer);
        return 0;
    }

    buffer[readSize] = '\0';

    SettingsIni_ParseBuffer(config, buffer);

    free(buffer);

    return 1;
}

static int SettingsIni_LoadEmbedded(
    NewTestLayoutSettingsConfig *config
)
{
    const unsigned char *data;
    unsigned long size;
    char *buffer;

    if (!config)
        return 0;

    data = NULL;
    size = 0;

    if (!EmbeddedResources_Find("ini", NTL_SETTINGS_INI_NAME, &data, &size))
        return 0;

    if (!data || size == 0)
        return 0;

    buffer = (char *)malloc(size + 1);

    if (!buffer)
        return 0;

    CopyMemory(buffer, data, size);
    buffer[size] = '\0';

    SettingsIni_ParseBuffer(config, buffer);

    free(buffer);

    return 1;
}

void NewTestLayoutSettings_LoadDefaultConfig(
    NewTestLayoutSettingsConfig *config
)
{
    if (!config)
        return;

    NewTestLayoutSettings_GetDefaultConfig(config);
    SettingsIni_LoadEmbedded(config);
}

void NewTestLayoutSettings_LoadConfig(
    NewTestLayoutSettingsConfig *config
)
{
    char path[MAX_PATH];

    if (!config)
        return;

    NewTestLayoutSettings_LoadDefaultConfig(config);

    if (SettingsIni_GetExternalPath(path, sizeof(path)))
        SettingsIni_LoadFile(config, path);
}

static int SettingsIni_WriteText(
    HANDLE file,
    const char *text
)
{
    DWORD written;
    DWORD len;

    if (!file || file == INVALID_HANDLE_VALUE || !text)
        return 0;

    len = lstrlen(text);
    written = 0;

    if (!WriteFile(file, text, len, &written, NULL))
        return 0;

    return written == len;
}

static int SettingsIni_WriteLine(
    HANDLE file,
    const char *text
)
{
    if (!SettingsIni_WriteText(file, text))
        return 0;

    return SettingsIni_WriteText(file, "\r\n");
}

static int SettingsIni_WriteInt(
    HANDLE file,
    const char *name,
    int value
)
{
    char line[256];

    wsprintf(line, "%s=%d", name, value);
    return SettingsIni_WriteLine(file, line);
}

static int SettingsIni_WriteColor(
    HANDLE file,
    const char *name,
    COLORREF value
)
{
    char line[256];

    wsprintf(line, "%s=%lu", name, (unsigned long)value);
    return SettingsIni_WriteLine(file, line);
}

static int SettingsIni_WriteField(
    HANDLE file,
    const NewTestLayoutSettingsConfig *config,
    const SettingsIniFieldDef *field
)
{
    const BYTE *base;

    if (!file || file == INVALID_HANDLE_VALUE || !config || !field)
        return 0;

    base = (const BYTE *)config;

    if (field->isColor)
    {
        return SettingsIni_WriteColor(
            file,
            field->name,
            *((const COLORREF *)(base + field->offset))
        );
    }

    return SettingsIni_WriteInt(
        file,
        field->name,
        *((const int *)(base + field->offset))
    );
}

int NewTestLayoutSettings_SaveConfig(
    const NewTestLayoutSettingsConfig *config
)
{
    char path[MAX_PATH];
    HANDLE file;
    int i;
    int ok;

    if (!config)
        return 0;

    if (!SettingsIni_GetExternalPath(path, sizeof(path)))
        return 0;

    file = CreateFile(
        path,
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (file == INVALID_HANDLE_VALUE)
        return 0;

    ok = 1;

    ok = ok && SettingsIni_WriteLine(file, "[NewTestLayout]");

    for (i = 0; i < (int)(sizeof(g_settingsIniFields) / sizeof(g_settingsIniFields[0])); i++)
    {
        if (i == 5 ||
            i == 13 ||
            i == 20 ||
            i == 23 ||
            i == 27 ||
            i == 29)
        {
            ok = ok && SettingsIni_WriteLine(file, "");
        }

        ok = ok && SettingsIni_WriteField(file, config, &g_settingsIniFields[i]);
    }

    CloseHandle(file);

    return ok;
}