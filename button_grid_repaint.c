#include "button_grid_core_internal.h"
#include "debug_window.h"

#define BUTTON_GRID_DEBUG_LOG_INTERVAL_MS 1000

static int ButtonGrid_DebugHasActivity(ButtonGrid *grid)
{
    if (!grid)
        return 0;

    return
        grid->debugSetRectCalls ||
        grid->debugSetRectSameCalls ||
        grid->debugSetRectSizeChangedCalls ||
        grid->debugSetWindowPosCalls ||
        grid->debugSizeMessages ||
        grid->debugWindowPosChangedMessages ||
        grid->debugRelayoutCalls ||
        grid->debugDpiRelayoutCalls ||
        grid->debugGridRedrawCalls ||
        grid->debugGridRedrawUpdateNowCalls ||
        grid->debugGridRedrawAllChildrenCalls ||
        grid->debugPaintCalls ||
        grid->debugDrawItemCalls ||
        grid->debugEraseCalls ||
        grid->debugButtonRedrawEstimated;
}

static void ButtonGrid_DebugReset(ButtonGrid *grid)
{
    if (!grid)
        return;

    grid->debugSetRectCalls = 0;
    grid->debugSetRectSameCalls = 0;
    grid->debugSetRectSizeChangedCalls = 0;
    grid->debugSetWindowPosCalls = 0;

    grid->debugSizeMessages = 0;
    grid->debugWindowPosChangedMessages = 0;

    grid->debugRelayoutCalls = 0;
    grid->debugDpiRelayoutCalls = 0;

    grid->debugGridRedrawCalls = 0;
    grid->debugGridRedrawUpdateNowCalls = 0;
    grid->debugGridRedrawAllChildrenCalls = 0;

    grid->debugPaintCalls = 0;
    grid->debugDrawItemCalls = 0;
    grid->debugEraseCalls = 0;

    grid->debugButtonRedrawEstimated = 0;
}

void ButtonGrid_DebugFlush(
    ButtonGrid *grid,
    const char *reason,
    int force
)
{
    DWORD now;
    DWORD elapsed;

    if (!grid)
        return;

    now = GetTickCount();

    if (!grid->debugLastLogTick)
        grid->debugLastLogTick = now;

    elapsed = now - grid->debugLastLogTick;

    if (!force && elapsed < BUTTON_GRID_DEBUG_LOG_INTERVAL_MS)
        return;

    if (!ButtonGrid_DebugHasActivity(grid))
    {
        grid->debugLastLogTick = now;
        return;
    }

    Debug_Log(
        "ButtonGrid",
        "ResizeDiag",
        "reason=%s hwnd=0x%08lX size=%dx%d setRect=%d same=%d setPos=%d sizeChanged=%d WM_SIZE=%d WM_WINDOWPOSCHANGED=%d relayout=%d dpiRelayout=%d redraw=%d redrawNow=%d redrawChildren=%d paint=%d drawItem=%d erase=%d estButtonRedraw=%d",
        reason ? reason : "",
        (DWORD)(ULONG_PTR)grid->hwnd,
        grid->debugLastWidth,
        grid->debugLastHeight,
        grid->debugSetRectCalls,
        grid->debugSetRectSameCalls,
        grid->debugSetWindowPosCalls,
        grid->debugSetRectSizeChangedCalls,
        grid->debugSizeMessages,
        grid->debugWindowPosChangedMessages,
        grid->debugRelayoutCalls,
        grid->debugDpiRelayoutCalls,
        grid->debugGridRedrawCalls,
        grid->debugGridRedrawUpdateNowCalls,
        grid->debugGridRedrawAllChildrenCalls,
        grid->debugPaintCalls,
        grid->debugDrawItemCalls,
        grid->debugEraseCalls,
        grid->debugButtonRedrawEstimated
    );

    ButtonGrid_DebugReset(grid);
    grid->debugLastLogTick = now;
}

void ButtonGrid_DebugNoteSetRect(
    ButtonGrid *grid,
    int alreadyMatches,
    int sizeChanged,
    int didSetWindowPos,
    int width,
    int height
)
{
    if (!grid)
        return;

    grid->debugSetRectCalls++;

    if (alreadyMatches)
        grid->debugSetRectSameCalls++;

    if (sizeChanged)
        grid->debugSetRectSizeChangedCalls++;

    if (didSetWindowPos)
        grid->debugSetWindowPosCalls++;

    grid->debugLastWidth = width;
    grid->debugLastHeight = height;

    ButtonGrid_DebugFlush(grid, "setrect", 0);
}

void ButtonGrid_DebugNoteSizeMessage(
    ButtonGrid *grid,
    int width,
    int height
)
{
    if (!grid)
        return;

    grid->debugSizeMessages++;
    grid->debugLastWidth = width;
    grid->debugLastHeight = height;

    ButtonGrid_DebugFlush(grid, "wm_size", 0);
}

void ButtonGrid_DebugNoteWindowPosChanged(
    ButtonGrid *grid,
    int width,
    int height
)
{
    if (!grid)
        return;

    grid->debugWindowPosChangedMessages++;
    grid->debugLastWidth = width;
    grid->debugLastHeight = height;

    ButtonGrid_DebugFlush(grid, "wm_windowposchanged", 0);
}

void ButtonGrid_DebugNoteRelayout(
    ButtonGrid *grid,
    int updateNow,
    int redrewButtons,
    int dpiRelayout
)
{
    if (!grid)
        return;

    grid->debugRelayoutCalls++;

    if (dpiRelayout)
        grid->debugDpiRelayoutCalls++;

    if (redrewButtons && grid->buttonCount > 0)
        grid->debugButtonRedrawEstimated += grid->buttonCount;

    (void)updateNow;

    ButtonGrid_DebugFlush(grid, "relayout", 0);
}

void ButtonGrid_DebugNoteGridRedraw(
    ButtonGrid *grid,
    UINT flags,
    int updateNow
)
{
    if (!grid)
        return;

    grid->debugGridRedrawCalls++;

    if (updateNow)
        grid->debugGridRedrawUpdateNowCalls++;

    if (flags & RDW_ALLCHILDREN)
        grid->debugGridRedrawAllChildrenCalls++;

    ButtonGrid_DebugFlush(grid, "redraw", 0);
}

void ButtonGrid_DebugNotePaint(ButtonGrid *grid)
{
    if (!grid)
        return;

    grid->debugPaintCalls++;
    ButtonGrid_DebugFlush(grid, "paint", 0);
}

void ButtonGrid_DebugNoteDrawItem(ButtonGrid *grid)
{
    if (!grid)
        return;

    grid->debugDrawItemCalls++;
    ButtonGrid_DebugFlush(grid, "drawitem", 0);
}

void ButtonGrid_DebugNoteErase(ButtonGrid *grid)
{
    if (!grid)
        return;

    grid->debugEraseCalls++;
    ButtonGrid_DebugFlush(grid, "erase", 0);
}

void ButtonGrid_RedrawGridWindow(ButtonGrid *grid, int updateNow)
{
    UINT flags;

    if (!grid || !grid->hwnd)
        return;

    flags = RDW_INVALIDATE | RDW_NOERASE | RDW_ALLCHILDREN;

    if (updateNow)
        flags |= RDW_UPDATENOW;

    ButtonGrid_DebugNoteGridRedraw(grid, flags, updateNow);

    RedrawWindow(grid->hwnd, NULL, NULL, flags);
}

void ButtonGrid_RelayoutAndRedraw(ButtonGrid *grid, int updateNow)
{
    if (!grid)
        return;

    ButtonGrid_DebugNoteRelayout(grid, updateNow, 1, 0);

    ButtonGrid_UpdateDpi(grid);
    ButtonGrid_UpdateAllButtonSizes(grid);
    ButtonGrid_Layout(grid);
    ButtonGrid_LayoutSettingsPage(grid);
    ButtonGrid_RedrawAllButtons(grid);
    ButtonGrid_RedrawGridWindow(grid, updateNow);
}

void ButtonGrid_HandleDpiChanged(ButtonGrid *grid)
{
    if (!grid)
        return;

    ButtonGrid_DebugNoteRelayout(grid, 1, 1, 1);

    ButtonGrid_UpdateDpi(grid);
    ButtonGrid_UpdateAllButtonSizes(grid);
    ButtonGrid_Layout(grid);
    ButtonGrid_LayoutSettingsPage(grid);
    ButtonGrid_RedrawAllButtons(grid);
    ButtonGrid_RedrawGridWindow(grid, 1);
}