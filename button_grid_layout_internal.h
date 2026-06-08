#ifndef BUTTON_GRID_LAYOUT_INTERNAL_H
#define BUTTON_GRID_LAYOUT_INTERNAL_H

#include "button_grid_internal.h"

#include <stdlib.h>

int ButtonGrid_LayoutHasVisibleBorder(
    ButtonGrid *grid
);

int ButtonGrid_LayoutShouldShowBorderTitle(
    ButtonGrid *grid
);

void ButtonGrid_GetScaledBorderMetrics(
    ButtonGrid *grid,
    int *padding,
    int *titleHeight
);

void ButtonGrid_GetContentRect(
    ButtonGrid *grid,
    RECT *rc
);

void ButtonGrid_GetOuterDecorationSize(
    ButtonGrid *grid,
    int *extraWidth,
    int *extraHeight
);

void ButtonGrid_CalculateHorizontalPositions(
    ButtonGrid *grid,
    RECT *content,
    GridPosition *positions,
    RECT *used
);

void ButtonGrid_CalculateVerticalPositions(
    ButtonGrid *grid,
    RECT *content,
    GridPosition *positions,
    RECT *used
);

void ButtonGrid_GetAlignmentOffset(
    ButtonGrid *grid,
    RECT *content,
    RECT *used,
    int *offsetX,
    int *offsetY
);

void ButtonGrid_ApplyPositions(
    ButtonGrid *grid,
    RECT *content,
    GridPosition *positions,
    RECT *used
);

#endif