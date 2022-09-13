#include "mapgen.h"
#include "map.h"
#include "../../lib/random.h"
#include "../../text.h"


/**
 * @param tx current tile x
 * @param ty current tile y
 * @param dir direction to carve
 */
bool wrm_can_carve(uint tx, uint ty, direction_t dir) {
    tx += dir_get_x(dir);
    ty += dir_get_y(dir);
    
    if(tx == 0 || ty == 0 || tx >= MAP_TILE_WIDTH - 1 || ty >=  MAP_TILE_HEIGHT - 1)
        return false;

    // sig for carving right
    const sig_t test[] = {
        [DIRECTION_LEFT]  = SIGN_UP | SIGN_DOWN | SIGN_LEFT,
        [DIRECTION_RIGHT] = SIGN_UP | SIGN_DOWN | SIGN_RIGHT,
        [DIRECTION_UP]    = SIGN_UP | SIGN_LEFT | SIGN_RIGHT,
        [DIRECTION_DOWN]  = SIGN_DOWN | SIGN_LEFT | SIGN_RIGHT,
    };

    return sig_test(sig_get(tx, ty), test[dir], test[dir]);
}


/**
 * @param tx starting x coordinate
 * @param ty starting y coordinate
 * @param dir starting direction
 */
void wrm_create(uint tx, uint ty, direction_t dir) {
    Worm w = {.x = tx, .y = ty, .fails = 8, .dir = dir};

    wrm_carve(&w);

    do {
        if(wrm_can_carve(w.x, w.y, w.dir)) {
            wrm_carve(&w);
            // 1 in 16 chance to switch directions
            if((rnd_random() & 15) == 1)
                wrm_random_dir(&w);
        } else {
            w.fails--;
            if(!wrm_random_dir(&w))
                w.fails = 0;
        }

    } while(w.fails);
}


/**
 * Sets the worm's direction randomly
 */
bool wrm_random_dir(Worm *w) {
    uint i = 0;
    direction_t cands[4];

    for(direction_t dir = 0; dir < 4; dir++) {
        if(wrm_can_carve(w->x, w->y, dir)) {
            cands[i++] = dir;
        }
    }

    if(i == 0) 
        return false;

    w->dir = cands[rnd_random_bounded(0, i)];

    return true;
}


void wrm_carve(Worm *w) {
    map_set(w->x, w->y, TILE_NONE);
    w->x += dir_get_x(w->dir);
    w->y += dir_get_y(w->dir);
}




void wrm_do_worms() {
    uint cands = 0;
    uint max_worms = GEN_MAX_WORMS;

    for(uint y = 1; y < MAP_TILE_HEIGHT - 1; y++) {
        for(uint x = 1; x < MAP_TILE_WIDTH - 1; x++) {
            if(sig_get(x, y) == 255)
                gen_set_cand(cands++, x, y);
        }
    }

    if(cands < max_worms)
        max_worms = cands;

    while(max_worms) {
        Position *p;
        do {
            p = gen_get_cand(rnd_random_bounded(0, cands));
        } while(sig_get(p->x, p->y) != 255);
        
        wrm_create(p->x, p->y, rnd_random() & 3);
        max_worms--;
    }
}