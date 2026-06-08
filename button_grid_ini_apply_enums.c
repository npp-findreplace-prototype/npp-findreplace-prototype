#include "button_grid_ini_internal.h"

static int Ini_TextIsAny2(
    const char *text,
    const char *a,
    const char *b
)
{
    return Ui_SameTextI(text, a) || Ui_SameTextI(text, b);
}

static int Ini_TextIsAny3(
    const char *text,
    const char *a,
    const char *b,
    const char *c
)
{
    return
        Ui_SameTextI(text, a) ||
        Ui_SameTextI(text, b) ||
        Ui_SameTextI(text, c);
}

int Ini_ReadLayout(
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

    if (Ui_SameTextI(text, "horizontal"))
        return BUTTON_GRID_LAYOUT_HORIZONTAL;

    if (Ui_SameTextI(text, "vertical"))
        return BUTTON_GRID_LAYOUT_VERTICAL;

    return atoi(text);
}

int Ini_ReadSizeMode(
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

    if (Ini_TextIsAny2(text, "default", "useDefault"))
        return BUTTON_GRID_SIZE_USE_DEFAULT;

    if (Ui_SameTextI(text, "fixed"))
        return BUTTON_GRID_SIZE_FIXED;

    if (Ini_TextIsAny3(text, "matchImageSize", "match_image_size", "imageSize"))
        return BUTTON_GRID_SIZE_MATCH_IMAGE_SIZE;

    if (Ini_TextIsAny3(text, "aspectHorizontal", "aspect_horizontal", "horizontalAspect"))
        return BUTTON_GRID_SIZE_MATCH_IMAGE_ASPECT_HORIZONTAL;

    if (Ini_TextIsAny3(text, "aspectVertical", "aspect_vertical", "verticalAspect"))
        return BUTTON_GRID_SIZE_MATCH_IMAGE_ASPECT_VERTICAL;

    if (Ini_TextIsAny3(text, "aspectByLayout", "aspect_by_layout", "byLayout"))
        return BUTTON_GRID_SIZE_MATCH_IMAGE_ASPECT_BY_LAYOUT;

    return atoi(text);
}

int Ini_ReadButtonBackMode(
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

    if (Ui_SameTextI(text, "opaque"))
        return BUTTON_GRID_BUTTON_BACK_OPAQUE;

    if (Ini_TextIsAny3(text, "transparent", "transparentSimulated", "simulatedTransparent"))
        return BUTTON_GRID_BUTTON_BACK_TRANSPARENT;

    return atoi(text);
}

int Ini_ReadBorderStyle(
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

    if (Ui_SameTextI(text, "none"))
        return BUTTON_GRID_BORDER_STYLE_NONE;

    if (Ui_SameTextI(text, "simple"))
        return BUTTON_GRID_BORDER_STYLE_SIMPLE;

    if (Ui_SameTextI(text, "etched"))
        return BUTTON_GRID_BORDER_STYLE_ETCHED;

    if (Ui_SameTextI(text, "rounded"))
        return BUTTON_GRID_BORDER_STYLE_ROUNDED;

    if (Ini_TextIsAny2(text, "etchedRounded", "etched_rounded"))
        return BUTTON_GRID_BORDER_STYLE_ETCHED_ROUNDED;

    if (Ui_SameTextI(text, "container"))
        return BUTTON_GRID_BORDER_STYLE_CONTAINER;

    if (Ui_SameTextI(text, "sunken"))
        return BUTTON_GRID_BORDER_STYLE_SUNKEN;

    if (Ui_SameTextI(text, "raised"))
        return BUTTON_GRID_BORDER_STYLE_RAISED;

    if (Ui_SameTextI(text, "double"))
        return BUTTON_GRID_BORDER_STYLE_DOUBLE;

    return atoi(text);
}

int Ini_ReadGearCorner(
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

    if (Ini_TextIsAny2(text, "topLeft", "top_left"))
        return BUTTON_GRID_GEAR_CORNER_TOP_LEFT;

    if (Ini_TextIsAny2(text, "topRight", "top_right"))
        return BUTTON_GRID_GEAR_CORNER_TOP_RIGHT;

    if (Ini_TextIsAny2(text, "bottomLeft", "bottom_left"))
        return BUTTON_GRID_GEAR_CORNER_BOTTOM_LEFT;

    if (Ini_TextIsAny2(text, "bottomRight", "bottom_right"))
        return BUTTON_GRID_GEAR_CORNER_BOTTOM_RIGHT;

    return atoi(text);
}

int Ini_ReadContentAlignment(
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

    if (Ini_TextIsAny2(text, "topLeft", "top_left"))
        return BUTTON_GRID_ALIGN_TOP_LEFT;

    if (Ui_SameTextI(text, "top"))
        return BUTTON_GRID_ALIGN_TOP;

    if (Ini_TextIsAny2(text, "topRight", "top_right"))
        return BUTTON_GRID_ALIGN_TOP_RIGHT;

    if (Ui_SameTextI(text, "left"))
        return BUTTON_GRID_ALIGN_LEFT;

    if (Ui_SameTextI(text, "center"))
        return BUTTON_GRID_ALIGN_CENTER;

    if (Ui_SameTextI(text, "right"))
        return BUTTON_GRID_ALIGN_RIGHT;

    if (Ini_TextIsAny2(text, "bottomLeft", "bottom_left"))
        return BUTTON_GRID_ALIGN_BOTTOM_LEFT;

    if (Ui_SameTextI(text, "bottom"))
        return BUTTON_GRID_ALIGN_BOTTOM;

    if (Ini_TextIsAny2(text, "bottomRight", "bottom_right"))
        return BUTTON_GRID_ALIGN_BOTTOM_RIGHT;

    if (Ini_TextIsAny2(text, "xy", "x/y"))
        return BUTTON_GRID_ALIGN_XY;

    if (Ini_TextIsAny2(text, "percent", "percentage"))
        return BUTTON_GRID_ALIGN_PERCENT;

    return atoi(text);
}

int Ini_ReadBehavior(
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

    if (Ui_SameTextI(text, "toggle"))
        return BUTTON_GRID_BUTTON_TOGGLE;

    if (Ui_SameTextI(text, "radio"))
        return BUTTON_GRID_BUTTON_RADIO;

    if (Ui_SameTextI(text, "disabled"))
        return BUTTON_GRID_BUTTON_DISABLED;

    return atoi(text);
}

int Ini_ReadShowTextOverride(
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

    if (Ini_TextIsAny2(text, "default", "useDefault"))
        return BUTTON_GRID_TEXT_USE_DEFAULT;

    if (Ui_SameTextI(text, "show") ||
        Ui_SameTextI(text, "on") ||
        Ui_SameTextI(text, "true"))
    {
        return BUTTON_GRID_TEXT_SHOW;
    }

    if (Ui_SameTextI(text, "hide") ||
        Ui_SameTextI(text, "off") ||
        Ui_SameTextI(text, "false"))
    {
        return BUTTON_GRID_TEXT_HIDE;
    }

    return atoi(text);
}