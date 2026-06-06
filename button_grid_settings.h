#ifndef BUTTON_GRID_SETTINGS_H
#define BUTTON_GRID_SETTINGS_H

#include <windows.h>

typedef struct ButtonGrid ButtonGrid;

BOOL ButtonGrid_SettingsRegisterClass(HINSTANCE hInstance);

void ButtonGrid_DrawGearIcon(ButtonGrid *grid, HDC hdc);

int ButtonGrid_HandleGearClick(ButtonGrid *grid, int x, int y);

void ButtonGrid_ShowSettingsPage(ButtonGrid *grid, int show);
void ButtonGrid_ToggleSettingsPage(ButtonGrid *grid);
void ButtonGrid_LayoutSettingsPage(ButtonGrid *grid);
void ButtonGrid_DestroySettingsPage(ButtonGrid *grid);

#endif