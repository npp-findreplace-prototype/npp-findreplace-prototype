#ifndef BUTTON_GRID_CORE_INTERNAL_H
#define BUTTON_GRID_CORE_INTERNAL_H

#include "win_compat.h"
#include "ui_common.h"

#include "button_grid_internal.h"
#include "button_grid_settings.h"

#define BUTTON_GRID_BUTTON_PROP_GRID "ButtonGridButtonGrid"
#define BUTTON_GRID_BUTTON_PROP_OLDPROC "ButtonGridButtonOldProc"

LRESULT CALLBACK ButtonGrid_WndProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam
);

LRESULT ButtonGrid_HandleCreate(
    HWND hwnd,
    LPARAM lParam
);

void ButtonGrid_HandleDestroy(
    HWND hwnd
);

void ButtonGrid_HandleDpiChanged(
    ButtonGrid *grid
);

void ButtonGrid_RedrawGridWindow(
    ButtonGrid *grid,
    int updateNow
);

void ButtonGrid_RelayoutAndRedraw(
    ButtonGrid *grid,
    int updateNow
);

void ButtonGrid_PrepareButtonsForKeyboard(
    ButtonGrid *grid
);

#endif