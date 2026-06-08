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

/* resize / repaint diagnostics */
void ButtonGrid_DebugNoteSetRect(
    ButtonGrid *grid,
    int alreadyMatches,
    int sizeChanged,
    int didSetWindowPos,
    int width,
    int height
);

void ButtonGrid_DebugNoteSizeMessage(
    ButtonGrid *grid,
    int width,
    int height
);

void ButtonGrid_DebugNoteWindowPosChanged(
    ButtonGrid *grid,
    int width,
    int height
);

void ButtonGrid_DebugNoteRelayout(
    ButtonGrid *grid,
    int updateNow,
    int redrewButtons,
    int dpiRelayout
);

void ButtonGrid_DebugNoteGridRedraw(
    ButtonGrid *grid,
    UINT flags,
    int updateNow
);

void ButtonGrid_DebugNotePaint(
    ButtonGrid *grid
);

void ButtonGrid_DebugNoteDrawItem(
    ButtonGrid *grid
);

void ButtonGrid_DebugNoteErase(
    ButtonGrid *grid
);

void ButtonGrid_DebugFlush(
    ButtonGrid *grid,
    const char *reason,
    int force
);

#endif