#ifndef PATH_H
#define PATH_H

#include "map.h"

u8 *pf_distance_map(uint fromX, uint fromY);
direction_t pf_best_dir(u8 *dist_map, uint x, uint y);

#endif