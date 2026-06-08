#include "button_grid_core_internal.h"

LRESULT CALLBACK ButtonGrid_WndProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam
)
{
    ButtonGrid *grid;

    grid = ButtonGrid_Get(hwnd);

    switch (msg)
    {
        case WM_CREATE:
        {
            return ButtonGrid_HandleCreate(hwnd, lParam);
        }

        case WM_SIZE:
        {
            if (grid)
            {
                ButtonGrid_DebugNoteSizeMessage(
                    grid,
                    LOWORD(lParam),
                    HIWORD(lParam)
                );

                ButtonGrid_RelayoutAndRedraw(grid, 0);
            }

            return 0;
        }

        case WM_WINDOWPOSCHANGED:
        {
            WINDOWPOS *wp;

            wp = (WINDOWPOS *)lParam;

            if (grid && wp && !(wp->flags & SWP_NOSIZE))
            {
                ButtonGrid_DebugNoteWindowPosChanged(
                    grid,
                    wp->cx,
                    wp->cy
                );
            }

            break;
        }

        case WM_DPICHANGED:
        {
            ButtonGrid_HandleDpiChanged(grid);
            return 0;
        }

        case WM_LBUTTONDOWN:
        {
            int x;
            int y;

            x = LOWORD(lParam);
            y = HIWORD(lParam);

            if (ButtonGrid_HandleGearClick(grid, x, y))
                return 0;

            break;
        }

        case WM_PAINT:
        {
            return ButtonGrid_HandlePaint(hwnd);
        }

        case WM_COMMAND:
        {
            int controlId;
            int notifyCode;
            int index;

            if (!grid)
                break;

            controlId = LOWORD(wParam);
            notifyCode = HIWORD(wParam);

            if (notifyCode == STN_CLICKED)
            {
                index = controlId - grid->idBase;
                ButtonGrid_ActivateButtonByIndex(grid, index);
                return 0;
            }

            break;
        }

        case WM_DRAWITEM:
        {
            return ButtonGrid_HandleDrawItem(grid, lParam);
        }

        case WM_ERASEBKGND:
        {
            return ButtonGrid_HandleEraseBackground(hwnd, wParam);
        }

        case WM_NCDESTROY:
        {
            if (grid)
                ButtonGrid_DebugFlush(grid, "destroy", 1);

            ButtonGrid_HandleDestroy(hwnd);
            break;
        }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}