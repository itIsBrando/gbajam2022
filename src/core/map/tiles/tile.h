#ifndef TILE_H
#define TILE_H

#include "../../unit/unit.h"

bool tile_wall_canpass(void *u);
void tile_wall_draw(uint tx, uint ty);

void tile_stair_interact(void *u, uint tx, uint ty);

void tile_draw_none(uint tx, uint ty);

#endif