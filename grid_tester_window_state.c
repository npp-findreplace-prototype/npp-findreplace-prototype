#include "grid_tester_window_internal.h"

HINSTANCE g_gridTesterHInstance = NULL;
HWND g_gridTesterWindow = NULL;
HWND g_gridTesterButtonGrid = NULL;
int g_gridTesterSquareSize = BUTTON_GRID_DEFAULT_BUTTON_WIDTH;
GridTesterWindowClosedCallback g_gridTesterOnClosed = NULL;