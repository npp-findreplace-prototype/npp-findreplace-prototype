#include "new_test_layout_controls_internal.h"

void NewTestLayoutTheme_GetDefault(NewTestLayoutTheme *theme)
{
    if (!theme)
        return;

    ZeroMemory(theme, sizeof(*theme));

    theme->windowBackColor = RGB(8, 8, 8);
    theme->panelBackColor = RGB(8, 8, 8);
    theme->panelBorderColor = RGB(70, 70, 70);

    theme->editBackColor = RGB(10, 10, 10);
    theme->editBorderColor = RGB(95, 95, 95);
    theme->editTextColor = RGB(230, 230, 230);
    theme->placeholderColor = RGB(135, 135, 145);
    theme->arrowColor = RGB(140, 140, 145);

    theme->buttonBackColor = RGB(18, 18, 18);
    theme->buttonBorderColor = RGB(80, 80, 80);
    theme->buttonTextColor = RGB(205, 205, 205);
    theme->countTextColor = RGB(0, 100, 210);

    theme->groupBackColor = RGB(8, 8, 8);
    theme->groupBorderColor = RGB(65, 65, 65);
    theme->groupTitleColor = RGB(175, 175, 175);

    theme->gearColor = RGB(170, 170, 170);
    theme->gearBackColor = RGB(22, 22, 22);
    theme->gearBorderColor = RGB(90, 90, 90);
}

void NewTestLayoutTheme_CreateDefaultFonts(NewTestLayoutTheme *theme)
{
    if (!theme)
        return;

    if (!theme->normalFont)
    {
        theme->normalFont = CreateFont(
            -18,
            0,
            0,
            0,
            FW_NORMAL,
            FALSE,
            FALSE,
            FALSE,
            ANSI_CHARSET,
            OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY,
            DEFAULT_PITCH | FF_DONTCARE,
            "Segoe UI"
        );
    }

    if (!theme->monoFont)
    {
        theme->monoFont = CreateFont(
            -18,
            0,
            0,
            0,
            FW_NORMAL,
            FALSE,
            FALSE,
            FALSE,
            ANSI_CHARSET,
            OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY,
            FIXED_PITCH | FF_MODERN,
            "Consolas"
        );
    }

    if (!theme->placeholderFont)
    {
        theme->placeholderFont = CreateFont(
            -28,
            0,
            0,
            0,
            FW_NORMAL,
            FALSE,
            FALSE,
            FALSE,
            ANSI_CHARSET,
            OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY,
            FIXED_PITCH | FF_MODERN,
            "Consolas"
        );
    }

    if (!theme->buttonFont)
    {
        theme->buttonFont = CreateFont(
            -17,
            0,
            0,
            0,
            FW_NORMAL,
            FALSE,
            FALSE,
            FALSE,
            ANSI_CHARSET,
            OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY,
            DEFAULT_PITCH | FF_DONTCARE,
            "Segoe UI"
        );
    }

    if (!theme->titleFont)
    {
        theme->titleFont = CreateFont(
            -17,
            0,
            0,
            0,
            FW_NORMAL,
            FALSE,
            FALSE,
            FALSE,
            ANSI_CHARSET,
            OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY,
            DEFAULT_PITCH | FF_DONTCARE,
            "Segoe UI"
        );
    }
}

void NewTestLayoutTheme_DeleteFonts(NewTestLayoutTheme *theme)
{
    if (!theme)
        return;

    if (theme->normalFont)
        DeleteObject(theme->normalFont);

    if (theme->monoFont)
        DeleteObject(theme->monoFont);

    if (theme->placeholderFont)
        DeleteObject(theme->placeholderFont);

    if (theme->buttonFont)
        DeleteObject(theme->buttonFont);

    if (theme->titleFont)
        DeleteObject(theme->titleFont);

    theme->normalFont = NULL;
    theme->monoFont = NULL;
    theme->placeholderFont = NULL;
    theme->buttonFont = NULL;
    theme->titleFont = NULL;
}