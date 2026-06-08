#include "button_grid_ini_internal.h"

void Ini_ReadMemoryLine(
    const char **cursor,
    char *line,
    int lineSize
)
{
    int i;
    const char *p;

    if (!cursor || !*cursor || !line || lineSize <= 0)
        return;

    p = *cursor;
    i = 0;

    while (*p && *p != '\n')
    {
        if (i < lineSize - 1)
            line[i++] = *p;

        p++;
    }

    if (*p == '\n')
        p++;

    line[i] = '\0';

    if (i > 0 && line[i - 1] == '\r')
        line[i - 1] = '\0';

    *cursor = p;
}

void Ini_ReadString(
    const IniSource *source,
    const char *section,
    const char *key,
    const char *defaultValue,
    char *buffer,
    int bufferSize
)
{
    const char *cursor;
    char line[1024];
    char currentSection[128];
    char *equals;
    char *name;
    char *value;

    Ini_CopyText(buffer, bufferSize, defaultValue ? defaultValue : "");

    if (!source || !source->memoryText || !section || !key)
        return;

    cursor = source->memoryText;
    currentSection[0] = '\0';

    while (*cursor)
    {
        Ini_ReadMemoryLine(&cursor, line, sizeof(line));
        Ini_TrimInPlace(line);

        if (Ini_LineIsCommentOrEmpty(line))
            continue;

        if (line[0] == '[')
        {
            char *end;

            end = line + lstrlen(line) - 1;

            if (end > line && *end == ']')
            {
                *end = '\0';
                Ini_CopyText(currentSection, sizeof(currentSection), line + 1);
                Ini_TrimInPlace(currentSection);
            }

            continue;
        }

        if (lstrcmpi(currentSection, section) != 0)
            continue;

        equals = line;

        while (*equals && *equals != '=')
            equals++;

        if (*equals != '=')
            continue;

        *equals = '\0';

        name = line;
        value = equals + 1;

        Ini_TrimInPlace(name);
        Ini_TrimInPlace(value);

        if (lstrcmpi(name, key) == 0)
        {
            Ini_CopyText(buffer, bufferSize, value);
            return;
        }
    }
}