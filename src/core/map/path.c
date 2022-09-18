#include <string.h>
#include <stdlib.h>
#include "../../lib/defines.h"
#include "../../lib/bitfield.h"
#include "../unit/unit.h"
#include "path.h"
#include "mapgen.h"

static u8 pf_dist_map[MAP_TILE_WIDTH * MAP_TILE_HEIGHT];

#define dist_map_set(x, y, v) pf_dist_map[(x) + ((y) << MAP_BITS_WIDTH)] = v
#define dist_map_get(x, y) pf_dist_map[(x) + ((y) << MAP_BITS_WIDTH)]


u8 *pf_distance_map(uint fromX, uint fromY)
{
    uint cand_len = 1, cand_new_len, step = 2;
    Position cand[GEN_MAX_ROOM_AREA];
    Position cand_new[GEN_MAX_ROOM_AREA];

    memset(pf_dist_map, 0, LENGTH(pf_dist_map));
    cand->x = fromX, cand->y = fromY;
    dist_map_set(fromX, fromY, 1);

    do {
        cand_new_len = 0;
        for(uint i = 0; i < cand_len; i++) {
            for(direction_t dir = 0; dir < 4; dir++) {
                int dx = cand[i].x + dir_get_x(dir), dy = cand[i].y + dir_get_y(dir);

                if(map_inbounds(dx, dy) && dist_map_get(dx, dy) == 0 && !map_is_solid(dx, dy)) {
                    Position *c = &cand_new[cand_new_len++];
                    c->x = dx;
                    c->y = dy;
                    dist_map_set(c->x, c->y, step);
                    if(cand_new_len >= GEN_MAX_ROOM_AREA) {
                        i = 1000;
                        break;
                    }
                }
            }
        }

        memcpy(cand, cand_new, cand_new_len * sizeof(Position));
        cand_len = cand_new_len;
        step++;
    } while(cand_len && step < 30);

    return pf_dist_map;
}


// @todo add candidates to choose a random direction
direction_t pf_best_dir(u8 *dist_map, uint x, uint y) {
    uint i;
    uint smallest = dist_map_get(x, y);
    direction_t best_dir = DIRECTION_NONE;

    for(i = 0; i < 4; i++) {
        int dx = x + dir_get_x(i), dy = y + dir_get_y(i);
        u8 v = dist_map_get(dx, dy);

        if(smallest >= v && v && unit_at(dx, dy) == NULL) {
            smallest = v;
            best_dir = i;
        }
    }

    return best_dir;
}