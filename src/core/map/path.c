#include <string.h>
#include <stdlib.h>
#include "../../lib/defines.h"
#include "path.h"
#include "../../lib/bitfield.h"

static u8 pf_dist_map[MAP_TILE_WIDTH * MAP_TILE_HEIGHT];

#define dist_map_set(x, y, v) pf_dist_map[(x) + ((y) << MAP_BITS_WIDTH)] = v
#define dist_map_get(x, y) pf_dist_map[(x) + ((y) << MAP_BITS_WIDTH)]


u8 *pf_distance_map(uint fromX, uint fromY)
{
    uint cand_len = 1, cand_new_len, step = 2;
    Position *cand = malloc(30 * 30 * sizeof(Position));
    Position *cand_new = malloc(30 * 30 * sizeof(Position));

    memset(pf_dist_map, 0, LENGTH(pf_dist_map));
    cand->x = fromX, cand->y = fromY;
    dist_map_set(fromX, fromY, 1);

    do {
        cand_new_len = 0;
        for(uint i = 0; i < cand_len; i++) {
            for(direction_t dir = 0; dir < 4; dir++) {
                int dx = cand[i].x + dir_get_x(dir), dy = cand[i].y + dir_get_y(dir);

                if(map_inbounds(dx, dy) && !map_is_solid(dx, dy) && dist_map_get(dx, dy) == 0) {
                    Position *c = &cand_new[cand_new_len++];
                    c->x = dx;
                    c->y = dy;
                    dist_map_set(cand[i].x, cand[i].y, step);
                    if(cand_new_len >= 30 * 30) {
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

    free(cand);
    free(cand_new);
    return pf_dist_map;
}


direction_t pf_best_dir(u8 *dist_map, uint x, uint y) {
    uint i;
    uint smallest = 9999;
    direction_t best_dir = 0;

    for(i = 0; i < 4; i++) {
        int dx = x + dir_get_x(i), dy = y + dir_get_y(i);
        u8 v = dist_map[dx + (dy << MAP_BITS_WIDTH)];
        if(smallest >= v && v) {
            smallest = v;
            best_dir = i;
        }
    }

    return best_dir;
}