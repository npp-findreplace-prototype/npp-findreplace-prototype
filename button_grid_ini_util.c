#include "button_grid_ini_internal.h"

void Ini_CopyText(char *dest, int destSize, const char *src)
{
    Ui_CopyText(dest, destSize, src);
}

void Ini_AppendText(char *dest, int destSize, const char *src)
{
    Ui_AppendText(dest, destSize, src);
}

void Ini_TrimInPlace(char *text)
{
    char *start;
    char *end;
    int len;

    if (!text)
        return;

    start = text;

    while (*start == ' ' || *start == '\t' || *start == '\r' || *start == '\n')
        start++;

    if (start != text)
        MoveMemory(text, start, lstrlen(start) + 1);

    len = lstrlen(text);

    while (len > 0)
    {
        end = text + len - 1;

        if (*end != ' ' && *end != '\t' && *end != '\r' && *end != '\n')
            break;

        *end = '\0';
        len--;
    }
}

int Ini_LineIsCommentOrEmpty(const char *line)
{
    if (!line || !line[0])
        return 1;

    if (line[0] == ';' || line[0] == '#')
        return 1;

    return 0;
}

int Ini_ReadInt(
    const IniSource *source,
    const char *section,
    const char *key,
    int defaultValue
)
{
    char text[128];

    Ini_ReadString(source, section, key, "", text, sizeof(text));

    if (!text[0])
        return defaultValue;

    return (int)strtol(text, NULL, 0);
}

static int Ini_IsTrueText(const char *text)
{
    if (!text)
        return 0;

    if (Ui_SameTextI(text, "1"))
        return 1;

    if (Ui_SameTextI(text, "true"))
        return 1;

    if (Ui_SameTextI(text, "yes"))
        return 1;

    if (Ui_SameTextI(text, "on"))
        return 1;

    if (Ui_SameTextI(text, "enabled"))
        return 1;

    if (Ui_SameTextI(text, "enable"))
        return 1;

    return 0;
}

static int Ini_IsFalseText(const char *text)
{
    if (!text)
        return 0;

    if (Ui_SameTextI(text, "0"))
        return 1;

    if (Ui_SameTextI(text, "false"))
        return 1;

    if (Ui_SameTextI(text, "no"))
        return 1;

    if (Ui_SameTextI(text, "off"))
        return 1;

    if (Ui_SameTextI(text, "disabled"))
        return 1;

    if (Ui_SameTextI(text, "disable"))
        return 1;

    return 0;
}

int Ini_ReadBool(
    const IniSource *source,
    const char *section,
    const char *key,
    int defaultValue
)
{
    char text[128];

    Ini_ReadString(source, section, key, "", text, sizeof(text));

    if (!text[0])
        return defaultValue ? 1 : 0;

    if (Ini_IsTrueText(text))
        return 1;

    if (Ini_IsFalseText(text))
        return 0;

    return atoi(text) ? 1 : 0;
}

static int Ini_HexDigitValue(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';

    if (c >= 'a' && c <= 'f')
        return 10 + c - 'a';

    if (c >= 'A' && c <= 'F')
        return 10 + c - 'A';

    return -1;
}

static int Ini_ParseHexColor(const char *text, COLORREF *color)
{
    int r1;
    int r2;
    int g1;
    int g2;
    int b1;
    int b2;

    if (!text || text[0] != '#')
        return 0;

    if (lstrlen(text) < 7)
        return 0;

    r1 = Ini_HexDigitValue(text[1]);
    r2 = Ini_HexDigitValue(text[2]);
    g1 = Ini_HexDigitValue(text[3]);
    g2 = Ini_HexDigitValue(text[4]);
    b1 = Ini_HexDigitValue(text[5]);
    b2 = Ini_HexDigitValue(text[6]);

    if (r1 < 0 || r2 < 0 || g1 < 0 || g2 < 0 || b1 < 0 || b2 < 0)
        return 0;

    *color = RGB(r1 * 16 + r2, g1 * 16 + g2, b1 * 16 + b2);
    return 1;
}

static int Ini_ParseRgbColor(const char *text, COLORREF *color)
{
    int r;
    int g;
    int b;

    if (!text)
        return 0;

    if (sscanf(text, "RGB(%d,%d,%d)", &r, &g, &b) == 3 ||
        sscanf(text, "rgb(%d,%d,%d)", &r, &g, &b) == 3 ||
        sscanf(text, "%d,%d,%d", &r, &g, &b) == 3)
    {
        r = Ui_ClampInt(r, 0, 255);
        g = Ui_ClampInt(g, 0, 255);
        b = Ui_ClampInt(b, 0, 255);

        *color = RGB(r, g, b);
        return 1;
    }

    return 0;
}

COLORREF Ini_ReadColor(
    const IniSource *source,
    const char *section,
    const char *key,
    COLORREF defaultValue
)
{
    char text[128];
    COLORREF color;

    Ini_ReadString(source, section, key, "", text, sizeof(text));

    if (!text[0])
        return defaultValue;

    if (Ini_ParseHexColor(text, &color))
        return color;

    if (Ini_ParseRgbColor(text, &color))
        return color;

    return (COLORREF)strtoul(text, NULL, 0);
}