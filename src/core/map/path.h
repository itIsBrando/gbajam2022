#ifndef PATH_H
#define PATH_H

#include "map.h"

/**
 * Gets the distance between two points
 */
uint pf_dist(int x1, int y1, int x2, int y2);

/**
 * @returns true if (x1, y1) has a clear line of sight to (x2, y2)
 */
bool pf_has_line_of_sight(int x1, int y1, int x2, int y2);

u8 *pf_distance_map(uint fromX, uint fromY);
direction_t pf_best_dir(u8 *dist_map, uint x, uint y);

#endif