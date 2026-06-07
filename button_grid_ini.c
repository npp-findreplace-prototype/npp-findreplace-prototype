#include <windows.h>
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

static void Ini_CopyText(char *dest, int destSize, const char *src)
{
    if (!dest || destSize <= 0)
        return;

    if (!src)
        src = "";

    lstrcpyn(dest, src, destSize);
    dest[destSize - 1] = '\0';
}

static void Ini_AppendText(char *dest, int destSize, const char *src)
{
    int len;

    if (!dest || destSize <= 0 || !src)
        return;

    len = lstrlen(dest);

    if (len >= destSize - 1)
        return;

    lstrcpyn(dest + len, src, destSize - len);
    dest[destSize - 1] = '\0';
}

static void Ini_TrimInPlace(char *text)
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

static int Ini_LineIsCommentOrEmpty(const char *line)
{
    if (!line || !line[0])
        return 1;

    if (line[0] == ';' || line[0] == '#')
        return 1;

    return 0;
}

static void Ini_ReadMemoryLine(
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

static void Ini_ReadString(
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

static int Ini_ReadInt(
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

    if (lstrcmpi(text, "1") == 0)
        return 1;

    if (lstrcmpi(text, "true") == 0)
        return 1;

    if (lstrcmpi(text, "yes") == 0)
        return 1;

    if (lstrcmpi(text, "on") == 0)
        return 1;

    if (lstrcmpi(text, "enabled") == 0)
        return 1;

    if (lstrcmpi(text, "enable") == 0)
        return 1;

    return 0;
}

static int Ini_IsFalseText(const char *text)
{
    if (!text)
        return 0;

    if (lstrcmpi(text, "0") == 0)
        return 1;

    if (lstrcmpi(text, "false") == 0)
        return 1;

    if (lstrcmpi(text, "no") == 0)
        return 1;

    if (lstrcmpi(text, "off") == 0)
        return 1;

    if (lstrcmpi(text, "disabled") == 0)
        return 1;

    if (lstrcmpi(text, "disable") == 0)
        return 1;

    return 0;
}

static int Ini_ReadBool(
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
        if (r < 0)
            r = 0;

        if (r > 255)
            r = 255;

        if (g < 0)
            g = 0;

        if (g > 255)
            g = 255;

        if (b < 0)
            b = 0;

        if (b > 255)
            b = 255;

        *color = RGB(r, g, b);
        return 1;
    }

    return 0;
}

static COLORREF Ini_ReadColor(
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

static int Ini_ReadLayout(
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

    if (lstrcmpi(text, "horizontal") == 0)
        return BUTTON_GRID_LAYOUT_HORIZONTAL;

    if (lstrcmpi(text, "vertical") == 0)
        return BUTTON_GRID_LAYOUT_VERTICAL;

    return atoi(text);
}

static int Ini_ReadSizeMode(
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

    if (lstrcmpi(text, "default") == 0 ||
        lstrcmpi(text, "useDefault") == 0)
        return BUTTON_GRID_SIZE_USE_DEFAULT;

    if (lstrcmpi(text, "fixed") == 0)
        return BUTTON_GRID_SIZE_FIXED;

    if (lstrcmpi(text, "matchImageSize") == 0 ||
        lstrcmpi(text, "match_image_size") == 0 ||
        lstrcmpi(text, "imageSize") == 0)
        return BUTTON_GRID_SIZE_MATCH_IMAGE_SIZE;

    if (lstrcmpi(text, "aspectHorizontal") == 0 ||
        lstrcmpi(text, "aspect_horizontal") == 0 ||
        lstrcmpi(text, "horizontalAspect") == 0)
        return BUTTON_GRID_SIZE_MATCH_IMAGE_ASPECT_HORIZONTAL;

    if (lstrcmpi(text, "aspectVertical") == 0 ||
        lstrcmpi(text, "aspect_vertical") == 0 ||
        lstrcmpi(text, "verticalAspect") == 0)
        return BUTTON_GRID_SIZE_MATCH_IMAGE_ASPECT_VERTICAL;

    if (lstrcmpi(text, "aspectByLayout") == 0 ||
        lstrcmpi(text, "aspect_by_layout") == 0 ||
        lstrcmpi(text, "byLayout") == 0)
        return BUTTON_GRID_SIZE_MATCH_IMAGE_ASPECT_BY_LAYOUT;

    return atoi(text);
}

static int Ini_ReadButtonBackMode(
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

    if (lstrcmpi(text, "opaque") == 0)
        return BUTTON_GRID_BUTTON_BACK_OPAQUE;

    if (lstrcmpi(text, "transparent") == 0 ||
        lstrcmpi(text, "transparentSimulated") == 0 ||
        lstrcmpi(text, "simulatedTransparent") == 0)
        return BUTTON_GRID_BUTTON_BACK_TRANSPARENT;

    return atoi(text);
}

static int Ini_ReadBorderStyle(
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

    if (lstrcmpi(text, "none") == 0)
        return BUTTON_GRID_BORDER_STYLE_NONE;

    if (lstrcmpi(text, "simple") == 0)
        return BUTTON_GRID_BORDER_STYLE_SIMPLE;

    if (lstrcmpi(text, "etched") == 0)
        return BUTTON_GRID_BORDER_STYLE_ETCHED;

    if (lstrcmpi(text, "rounded") == 0)
        return BUTTON_GRID_BORDER_STYLE_ROUNDED;

    if (lstrcmpi(text, "etchedRounded") == 0 ||
        lstrcmpi(text, "etched_rounded") == 0)
        return BUTTON_GRID_BORDER_STYLE_ETCHED_ROUNDED;

    if (lstrcmpi(text, "container") == 0)
        return BUTTON_GRID_BORDER_STYLE_CONTAINER;

    if (lstrcmpi(text, "sunken") == 0)
        return BUTTON_GRID_BORDER_STYLE_SUNKEN;

    if (lstrcmpi(text, "raised") == 0)
        return BUTTON_GRID_BORDER_STYLE_RAISED;

    if (lstrcmpi(text, "double") == 0)
        return BUTTON_GRID_BORDER_STYLE_DOUBLE;

    return atoi(text);
}

static int Ini_ReadGearCorner(
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

    if (lstrcmpi(text, "topLeft") == 0 ||
        lstrcmpi(text, "top_left") == 0)
        return BUTTON_GRID_GEAR_CORNER_TOP_LEFT;

    if (lstrcmpi(text, "topRight") == 0 ||
        lstrcmpi(text, "top_right") == 0)
        return BUTTON_GRID_GEAR_CORNER_TOP_RIGHT;

    if (lstrcmpi(text, "bottomLeft") == 0 ||
        lstrcmpi(text, "bottom_left") == 0)
        return BUTTON_GRID_GEAR_CORNER_BOTTOM_LEFT;

    if (lstrcmpi(text, "bottomRight") == 0 ||
        lstrcmpi(text, "bottom_right") == 0)
        return BUTTON_GRID_GEAR_CORNER_BOTTOM_RIGHT;

    return atoi(text);
}

static int Ini_ReadContentAlignment(
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

    if (lstrcmpi(text, "topLeft") == 0 || lstrcmpi(text, "top_left") == 0)
        return BUTTON_GRID_ALIGN_TOP_LEFT;

    if (lstrcmpi(text, "top") == 0)
        return BUTTON_GRID_ALIGN_TOP;

    if (lstrcmpi(text, "topRight") == 0 || lstrcmpi(text, "top_right") == 0)
        return BUTTON_GRID_ALIGN_TOP_RIGHT;

    if (lstrcmpi(text, "left") == 0)
        return BUTTON_GRID_ALIGN_LEFT;

    if (lstrcmpi(text, "center") == 0)
        return BUTTON_GRID_ALIGN_CENTER;

    if (lstrcmpi(text, "right") == 0)
        return BUTTON_GRID_ALIGN_RIGHT;

    if (lstrcmpi(text, "bottomLeft") == 0 || lstrcmpi(text, "bottom_left") == 0)
        return BUTTON_GRID_ALIGN_BOTTOM_LEFT;

    if (lstrcmpi(text, "bottom") == 0)
        return BUTTON_GRID_ALIGN_BOTTOM;

    if (lstrcmpi(text, "bottomRight") == 0 || lstrcmpi(text, "bottom_right") == 0)
        return BUTTON_GRID_ALIGN_BOTTOM_RIGHT;

    if (lstrcmpi(text, "xy") == 0 || lstrcmpi(text, "x/y") == 0)
        return BUTTON_GRID_ALIGN_XY;

    if (lstrcmpi(text, "percent") == 0 || lstrcmpi(text, "percentage") == 0)
        return BUTTON_GRID_ALIGN_PERCENT;

    return atoi(text);
}

static int Ini_ReadBehavior(
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

    if (lstrcmpi(text, "toggle") == 0)
        return BUTTON_GRID_BUTTON_TOGGLE;

    if (lstrcmpi(text, "radio") == 0)
        return BUTTON_GRID_BUTTON_RADIO;

    if (lstrcmpi(text, "disabled") == 0)
        return BUTTON_GRID_BUTTON_DISABLED;

    return atoi(text);
}

static int Ini_ReadShowTextOverride(
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

    if (lstrcmpi(text, "default") == 0 ||
        lstrcmpi(text, "useDefault") == 0)
        return BUTTON_GRID_TEXT_USE_DEFAULT;

    if (lstrcmpi(text, "show") == 0 ||
        lstrcmpi(text, "on") == 0 ||
        lstrcmpi(text, "true") == 0)
        return BUTTON_GRID_TEXT_SHOW;

    if (lstrcmpi(text, "hide") == 0 ||
        lstrcmpi(text, "off") == 0 ||
        lstrcmpi(text, "false") == 0)
        return BUTTON_GRID_TEXT_HIDE;

    return atoi(text);
}

static void Ini_ReadGridSection(
    const IniSource *source,
    ButtonGridIniConfig *loaded
)
{
    ButtonGridConfig *config;
    const char *section;

    section = "Grid";
    config = &loaded->config;

    config->buttonCount = Ini_ReadInt(source, section, "buttonCount", config->buttonCount);

    config->buttonWidth = Ini_ReadInt(source, section, "buttonWidth", config->buttonWidth);
    config->buttonHeight = Ini_ReadInt(source, section, "buttonHeight", config->buttonHeight);
    config->horizontalSpacing = Ini_ReadInt(source, section, "horizontalSpacing", config->horizontalSpacing);
    config->verticalSpacing = Ini_ReadInt(source, section, "verticalSpacing", config->verticalSpacing);

    config->layout = Ini_ReadLayout(source, section, "layout", config->layout);
    config->sizeMode = Ini_ReadSizeMode(source, section, "sizeMode", config->sizeMode);

    config->showText = Ini_ReadBool(source, section, "showText", config->showText);
    config->hidePartialButtons = Ini_ReadBool(source, section, "hidePartialButtons", config->hidePartialButtons);
    config->resizeInLayoutSteps = Ini_ReadBool(source, section, "resizeInLayoutSteps", config->resizeInLayoutSteps);
    config->settingsWheelScrub = Ini_ReadBool(source, section, "settingsWheelScrub", config->settingsWheelScrub);

    config->dpiScaleEnabled = Ini_ReadBool(source, section, "dpiScaleEnabled", config->dpiScaleEnabled);

    config->contentAlignment = Ini_ReadContentAlignment(source, section, "contentAlignment", config->contentAlignment);
    config->contentAlignX = Ini_ReadInt(source, section, "contentAlignX", config->contentAlignX);
    config->contentAlignY = Ini_ReadInt(source, section, "contentAlignY", config->contentAlignY);
    config->contentAlignPercentX = Ini_ReadInt(source, section, "contentAlignPercentX", config->contentAlignPercentX);
    config->contentAlignPercentY = Ini_ReadInt(source, section, "contentAlignPercentY", config->contentAlignPercentY);

    Ini_ReadString(source, section, "themeName", loaded->themeName, loaded->themeName, sizeof(loaded->themeName));
    config->themeName = loaded->themeName;

    config->allowThemeSelection = Ini_ReadBool(source, section, "allowThemeSelection", config->allowThemeSelection);

    config->showBorder = Ini_ReadBool(source, section, "showBorder", config->showBorder);
    config->showBorderTitle = Ini_ReadBool(source, section, "showBorderTitle", config->showBorderTitle);

    Ini_ReadString(source, section, "borderTitle", loaded->borderTitle, loaded->borderTitle, sizeof(loaded->borderTitle));
    config->borderTitle = loaded->borderTitle;

    config->borderPadding = Ini_ReadInt(source, section, "borderPadding", config->borderPadding);
    config->borderTitleHeight = Ini_ReadInt(source, section, "borderTitleHeight", config->borderTitleHeight);
    config->borderTitlePadding = Ini_ReadInt(source, section, "borderTitlePadding", config->borderTitlePadding);
    config->borderTitleFontSize = Ini_ReadInt(source, section, "borderTitleFontSize", config->borderTitleFontSize);
    config->borderTitleTransparent = Ini_ReadBool(source, section, "borderTitleTransparent", config->borderTitleTransparent);
    config->borderTitleAutoBackColor = Ini_ReadBool(source, section, "borderTitleAutoBackColor", config->borderTitleAutoBackColor);

    config->borderStyle = Ini_ReadBorderStyle(source, section, "borderStyle", config->borderStyle);
    config->borderThickness = Ini_ReadInt(source, section, "borderThickness", config->borderThickness);
    config->borderCornerRadius = Ini_ReadInt(source, section, "borderCornerRadius", config->borderCornerRadius);

    config->borderColor = Ini_ReadColor(source, section, "borderColor", config->borderColor);
    config->borderLightColor = Ini_ReadColor(source, section, "borderLightColor", config->borderLightColor);
    config->borderShadowColor = Ini_ReadColor(source, section, "borderShadowColor", config->borderShadowColor);
    config->borderTitleColor = Ini_ReadColor(source, section, "borderTitleColor", config->borderTitleColor);
    config->borderTitleBackColor = Ini_ReadColor(source, section, "borderTitleBackColor", config->borderTitleBackColor);

    config->showGearIcon = Ini_ReadBool(source, section, "showGearIcon", config->showGearIcon);
    config->gearCorner = Ini_ReadGearCorner(source, section, "gearCorner", config->gearCorner);
    config->gearSize = Ini_ReadInt(source, section, "gearSize", config->gearSize);
    config->gearMargin = Ini_ReadInt(source, section, "gearMargin", config->gearMargin);

    config->gearColor = Ini_ReadColor(source, section, "gearColor", config->gearColor);
    config->gearBackColor = Ini_ReadColor(source, section, "gearBackColor", config->gearBackColor);
    config->gearBorderColor = Ini_ReadColor(source, section, "gearBorderColor", config->gearBorderColor);

    config->idBase = Ini_ReadInt(source, section, "idBase", config->idBase);
    config->firstIndex = Ini_ReadInt(source, section, "firstIndex", config->firstIndex);

    Ini_ReadString(source, section, "namePrefix", loaded->namePrefix, loaded->namePrefix, sizeof(loaded->namePrefix));
    Ini_ReadString(source, section, "actionPrefix", loaded->actionPrefix, loaded->actionPrefix, sizeof(loaded->actionPrefix));
    Ini_ReadString(source, section, "textFormat", loaded->textFormat, loaded->textFormat, sizeof(loaded->textFormat));
    Ini_ReadString(source, section, "clickIdentifierFormat", loaded->clickIdentifierFormat, loaded->clickIdentifierFormat, sizeof(loaded->clickIdentifierFormat));

    config->namePrefix = loaded->namePrefix;
    config->actionPrefix = loaded->actionPrefix;
    config->textFormat = loaded->textFormat;
    config->clickIdentifierFormat = loaded->clickIdentifierFormat;

    config->buttonBackMode = Ini_ReadButtonBackMode(source, section, "buttonBackMode", config->buttonBackMode);
    config->showButtonBorder = Ini_ReadBool(source, section, "showButtonBorder", config->showButtonBorder);
    config->buttonBorderThickness = Ini_ReadInt(source, section, "buttonBorderThickness", config->buttonBorderThickness);
    config->buttonBorderColor = Ini_ReadColor(source, section, "buttonBorderColor", config->buttonBorderColor);

    config->backColor = Ini_ReadColor(source, section, "backColor", config->backColor);
    config->foreColor = Ini_ReadColor(source, section, "foreColor", config->foreColor);

    config->usePictures = Ini_ReadBool(source, section, "usePictures", config->usePictures);
    config->toggleOnClick = Ini_ReadBool(source, section, "toggleOnClick", config->toggleOnClick);
    config->defaultState = Ini_ReadBool(source, section, "defaultState", config->defaultState);
    config->stretchPictures = Ini_ReadBool(source, section, "stretchPictures", config->stretchPictures);

    config->generatedOffPictureColor = Ini_ReadColor(source, section, "generatedOffPictureColor", config->generatedOffPictureColor);
    config->generatedOnPictureColor = Ini_ReadColor(source, section, "generatedOnPictureColor", config->generatedOnPictureColor);
    config->generatedErrorPictureColor = Ini_ReadColor(source, section, "generatedErrorPictureColor", config->generatedErrorPictureColor);
}

static void Ini_DefaultButtonName(char *buffer, int bufferSize, int index)
{
    wsprintf(buffer, "Button%d", index);
    buffer[bufferSize - 1] = '\0';
}

static void Ini_ReadButtonSection(
    const IniSource *source,
    ButtonGridIniConfig *loaded,
    int index
)
{
    char section[64];
    char defaultName[64];
    ButtonGridItemConfig *item;
    ButtonGridIniItemText *text;

    wsprintf(section, "Button%d", index + 1);
    section[sizeof(section) - 1] = '\0';

    item = &loaded->items[index];
    text = &loaded->itemTexts[index];

    ZeroMemory(item, sizeof(*item));
    ZeroMemory(text, sizeof(*text));

    Ini_DefaultButtonName(defaultName, sizeof(defaultName), index + 1);

    Ini_ReadString(source, section, "name", defaultName, text->name, sizeof(text->name));
    Ini_ReadString(source, section, "action", "", text->action, sizeof(text->action));
    Ini_ReadString(source, section, "text", text->name, text->text, sizeof(text->text));
    Ini_ReadString(source, section, "tooltip", text->name, text->tooltip, sizeof(text->tooltip));
    Ini_ReadString(source, section, "iconBaseName", text->name, text->iconBaseName, sizeof(text->iconBaseName));

    item->name = text->name[0] ? text->name : NULL;
    item->action = text->action[0] ? text->action : NULL;
    item->text = text->text[0] ? text->text : NULL;
    item->tooltip = text->tooltip[0] ? text->tooltip : NULL;
    item->iconBaseName = text->iconBaseName[0] ? text->iconBaseName : NULL;

    item->behavior = Ini_ReadBehavior(source, section, "behavior", BUTTON_GRID_BUTTON_TOGGLE);
    item->radioGroup = Ini_ReadInt(source, section, "radioGroup", 0);
    item->defaultState = Ini_ReadBool(source, section, "defaultState", loaded->config.defaultState);

    item->widthOverride = Ini_ReadInt(source, section, "widthOverride", 0);
    item->heightOverride = Ini_ReadInt(source, section, "heightOverride", 0);

    item->sizeModeOverride = Ini_ReadSizeMode(
        source,
        section,
        "sizeModeOverride",
        BUTTON_GRID_SIZE_USE_DEFAULT
    );

    item->showTextOverride = Ini_ReadShowTextOverride(
        source,
        section,
        "showTextOverride",
        BUTTON_GRID_TEXT_USE_DEFAULT
    );

    item->pictureOff = NULL;
    item->pictureOn = NULL;
    item->ownsPictureOff = 0;
    item->ownsPictureOn = 0;
    item->pictureOffLoadFailed = 0;
    item->pictureOnLoadFailed = 0;
}

static int ButtonGridIni_LoadFromSource(
    const IniSource *source,
    ButtonGridIniConfig *loaded
)
{
    int i;

    if (!source || !loaded || !source->memoryText)
        return 0;

    ZeroMemory(loaded, sizeof(*loaded));

    Ini_CopyText(loaded->fileName, sizeof(loaded->fileName), source->displayName);

    ButtonGrid_GetDefaultConfig(&loaded->config);

    Ini_CopyText(loaded->themeName, sizeof(loaded->themeName), BUTTON_GRID_DEFAULT_THEME_NAME);
    Ini_CopyText(loaded->borderTitle, sizeof(loaded->borderTitle), BUTTON_GRID_DEFAULT_BORDER_TITLE);

    Ini_CopyText(loaded->namePrefix, sizeof(loaded->namePrefix), BUTTON_GRID_DEFAULT_NAME_PREFIX);
    Ini_CopyText(loaded->actionPrefix, sizeof(loaded->actionPrefix), BUTTON_GRID_DEFAULT_ACTION_PREFIX);
    Ini_CopyText(loaded->textFormat, sizeof(loaded->textFormat), BUTTON_GRID_DEFAULT_TEXT_FORMAT);
    Ini_CopyText(loaded->clickIdentifierFormat, sizeof(loaded->clickIdentifierFormat), BUTTON_GRID_DEFAULT_CLICK_IDENTIFIER_FORMAT);

    loaded->config.themeName = loaded->themeName;
    loaded->config.borderTitle = loaded->borderTitle;
    loaded->config.namePrefix = loaded->namePrefix;
    loaded->config.actionPrefix = loaded->actionPrefix;
    loaded->config.textFormat = loaded->textFormat;
    loaded->config.clickIdentifierFormat = loaded->clickIdentifierFormat;

    Ini_ReadGridSection(source, loaded);

    if (loaded->config.buttonCount < 1)
        loaded->config.buttonCount = 1;

    loaded->itemCount = loaded->config.buttonCount;

    loaded->items = (ButtonGridItemConfig *)malloc(
        sizeof(ButtonGridItemConfig) * loaded->itemCount
    );

    loaded->itemTexts = (ButtonGridIniItemText *)malloc(
        sizeof(ButtonGridIniItemText) * loaded->itemCount
    );

    if (!loaded->items || !loaded->itemTexts)
    {
        ButtonGridIni_Free(loaded);
        return 0;
    }

    ZeroMemory(loaded->items, sizeof(ButtonGridItemConfig) * loaded->itemCount);
    ZeroMemory(loaded->itemTexts, sizeof(ButtonGridIniItemText) * loaded->itemCount);

    for (i = 0; i < loaded->itemCount; i++)
        Ini_ReadButtonSection(source, loaded, i);

    loaded->config.items = loaded->items;
    loaded->config.buttonCount = loaded->itemCount;

    ButtonGrid_NormalizeConfig(&loaded->config);

    return 1;
}

static int Ini_FileExists(const char *fileName)
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

static int Ini_PathIsAbsolute(const char *path)
{
    if (!path || !path[0])
        return 0;

    if (path[0] == '\\' && path[1] == '\\')
        return 1;

    if (((path[0] >= 'A' && path[0] <= 'Z') ||
         (path[0] >= 'a' && path[0] <= 'z')) &&
        path[1] == ':')
        return 1;

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

static int Ini_BuildExeRelativePath(
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

int ButtonGridIni_Load(
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

    if (lstrcmp(normalized, fileName) != 0)
    {
        if (Ini_FindWin32ResourceBytesByName(normalized, data, size))
            return 1;
    }

    Ini_GetBaseName(fileName, baseName, sizeof(baseName));

    if (baseName[0] && lstrcmp(baseName, fileName) != 0)
    {
        if (Ini_FindWin32ResourceBytesByName(baseName, data, size))
            return 1;
    }

    return 0;
}

static int Ini_FindBuiltinResourceBytes(
    const char *fileName,
    const BYTE **data,
    DWORD *size
)
{
    char baseName[MAX_PATH];

    if (data)
        *data = NULL;

    if (size)
        *size = 0;

    Ini_GetBaseName(fileName, baseName, sizeof(baseName));

    if (lstrcmpi(baseName, "grid_tester.ini") == 0)
    {
        if (data)
            *data = (const BYTE *)g_builtinGridTesterIni;

        if (size)
            *size = (DWORD)lstrlen(g_builtinGridTesterIni);

        return 1;
    }

    return 0;
}

int ButtonGridIni_LoadFromFileOrResource(
    const char *fileName,
    ButtonGridIniConfig *loaded
)
{
    char exeRelativePath[MAX_PATH];
    char displayName[MAX_PATH + 32];
    const BYTE *resourceData;
    DWORD resourceSize;
    unsigned long embeddedSize;

    if (!fileName || !loaded)
        return 0;

    if (Ini_FileExists(fileName))
        return ButtonGridIni_Load(fileName, loaded);

    if (!Ini_PathIsAbsolute(fileName))
    {
        if (Ini_BuildExeRelativePath(
                fileName,
                exeRelativePath,
                sizeof(exeRelativePath)
            ))
        {
            if (Ini_FileExists(exeRelativePath))
                return ButtonGridIni_Load(exeRelativePath, loaded);
        }
    }

    resourceData = NULL;
    resourceSize = 0;

    if (Ini_FindWin32ResourceBytes(fileName, &resourceData, &resourceSize))
    {
        Ini_CopyText(displayName, sizeof(displayName), "win32-resource:");
        Ini_AppendText(displayName, sizeof(displayName), fileName);

        return Ini_LoadFromBytes(
            displayName,
            resourceData,
            resourceSize,
            loaded
        );
    }

    resourceData = NULL;
    embeddedSize = 0;

    if (EmbeddedResources_Find("ini", fileName, &resourceData, &embeddedSize))
    {
        Ini_CopyText(displayName, sizeof(displayName), "embedded:");
        Ini_AppendText(displayName, sizeof(displayName), fileName);

        return Ini_LoadFromBytes(
            displayName,
            resourceData,
            (DWORD)embeddedSize,
            loaded
        );
    }

    resourceData = NULL;
    resourceSize = 0;

    if (Ini_FindBuiltinResourceBytes(fileName, &resourceData, &resourceSize))
    {
        Ini_CopyText(displayName, sizeof(displayName), "builtin:");
        Ini_AppendText(displayName, sizeof(displayName), fileName);

        return Ini_LoadFromBytes(
            displayName,
            resourceData,
            resourceSize,
            loaded
        );
    }

    return 0;
}

void ButtonGridIni_Free(ButtonGridIniConfig *loaded)
{
    if (!loaded)
        return;

    if (loaded->items)
        free(loaded->items);

    if (loaded->itemTexts)
        free(loaded->itemTexts);

    ZeroMemory(loaded, sizeof(*loaded));
}