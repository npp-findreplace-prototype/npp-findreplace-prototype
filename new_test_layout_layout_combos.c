#include "new_test_layout_window_internal.h"

void NewTestLayout_ShowUtilityButtons(int show)
{
    NewTestLayoutActionButton_Show(g_ntl_copyToReplaceButton, show);
    NewTestLayoutActionButton_Show(g_ntl_swapFindReplaceButton, show);
    NewTestLayoutActionButton_Show(g_ntl_copyToFindButton, show);
}

static void NewTestLayout_LayoutUtilityButtons(
    int utilityX,
    int utilityTop,
    int utilitySquare,
    int utilityGap
)
{
    RECT r;

    NewTestLayout_SetRect(
        &r,
        utilityX,
        utilityTop,
        utilityX + utilitySquare,
        utilityTop + utilitySquare
    );

    NewTestLayoutActionButton_SetRect(g_ntl_copyToReplaceButton, &r);

    utilityTop += utilitySquare + utilityGap;

    NewTestLayout_SetRect(
        &r,
        utilityX,
        utilityTop,
        utilityX + utilitySquare,
        utilityTop + utilitySquare
    );

    NewTestLayoutActionButton_SetRect(g_ntl_swapFindReplaceButton, &r);

    utilityTop += utilitySquare + utilityGap;

    NewTestLayout_SetRect(
        &r,
        utilityX,
        utilityTop,
        utilityX + utilitySquare,
        utilityTop + utilitySquare
    );

    NewTestLayoutActionButton_SetRect(g_ntl_copyToFindButton, &r);
}

void NewTestLayout_LayoutCombosAndUtility(
    const NewTestLayoutVisibility *visibility,
    NewTestLayoutLayoutState *layout
)
{
    RECT r;

    int x;
    int y;
    int right;
    int comboRight;
    int utilityX;

    int showUtilityButtons;
    int utilityTop;
    int utilityBottom;
    int utilitySquare;
    int utilityGap;

    if (!visibility || !layout)
        return;

    x = NTL_MARGIN;
    y = NTL_MARGIN;
    right = layout->width - NTL_MARGIN;

    showUtilityButtons = visibility->showUtilityButtons;

    utilityGap = NTL_UTILITY_BUTTON_GAP;
    utilitySquare = 0;

    if (showUtilityButtons)
    {
        utilityTop = NTL_MARGIN;
        utilityBottom =
            NTL_MARGIN +
            NTL_FAUX_COMBO_HEIGHT +
            NTL_GAP +
            NTL_FAUX_COMBO_HEIGHT;

        utilitySquare =
            (utilityBottom - utilityTop - utilityGap * 2) / 3;

        if (utilitySquare < NTL_UTILITY_BUTTON_SQUARE_MIN)
            utilitySquare = NTL_UTILITY_BUTTON_SQUARE_MIN;

        if (utilitySquare > NTL_UTILITY_BUTTON_SQUARE_MAX)
            utilitySquare = NTL_UTILITY_BUTTON_SQUARE_MAX;

        utilityX = right - utilitySquare;
        comboRight = utilityX - NTL_GAP;

        if (comboRight < x + 200)
        {
            showUtilityButtons = 0;
            utilityX = right;
            comboRight = right;
        }
    }
    else
    {
        utilityX = right;
        comboRight = right;
    }

    NewTestLayout_SetRect(
        &r,
        x,
        y,
        comboRight,
        y + NTL_FAUX_COMBO_HEIGHT
    );

    NewTestLayoutFauxCombo_SetRect(g_ntl_findCombo, &r);
    NewTestLayoutFauxCombo_Show(g_ntl_findCombo, 1);

    if (visibility->showReplaceBox)
    {
        y += NTL_FAUX_COMBO_HEIGHT + NTL_GAP;

        NewTestLayout_SetRect(
            &r,
            x,
            y,
            comboRight,
            y + NTL_FAUX_COMBO_HEIGHT
        );

        NewTestLayoutFauxCombo_SetRect(g_ntl_replaceCombo, &r);
        NewTestLayoutFauxCombo_Show(g_ntl_replaceCombo, 1);
    }
    else
    {
        NewTestLayoutFauxCombo_Show(g_ntl_replaceCombo, 0);
    }

    if (showUtilityButtons)
    {
        NewTestLayout_LayoutUtilityButtons(
            utilityX,
            NTL_MARGIN,
            utilitySquare,
            utilityGap
        );
    }

    NewTestLayout_ShowUtilityButtons(showUtilityButtons);

    layout->showUtilityButtons = showUtilityButtons;
    layout->comboRight = comboRight;

    layout->groupY = NTL_MARGIN + NTL_FAUX_COMBO_HEIGHT;

    if (visibility->showReplaceBox)
        layout->groupY += NTL_GAP + NTL_FAUX_COMBO_HEIGHT;
}