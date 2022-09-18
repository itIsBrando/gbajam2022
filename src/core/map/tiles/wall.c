#include <string.h>
#include "tile.h"
#include "../mapgen.h"
#include "../map.h"

bool tile_wall_canpass(void *u) {
    return false;
}



const sig_t masks[] = {
    SIGN_CARDINAL, SIGN_CARDINAL, SIGN_CARDINAL, SIGN_CARDINAL,

    SIGN_CARDINAL, SIGN_CARDINAL, SIGN_CARDINAL, SIGN_CARDINAL,

    SIGN_CARDINAL, SIGN_CARDINAL, SIGN_CARDINAL, SIGN_CARDINAL,

    SIGN_CARDINAL, SIGN_CARDINAL, SIGN_CARDINAL, SIGN_CARDINAL,
};

const sig_t tests[] = {
    SIGN_LEFT | SIGN_RIGHT | SIGN_UP,
    SIGN_LEFT | SIGN_RIGHT | SIGN_DOWN,
    SIGN_LEFT | SIGN_RIGHT,
    SIGN_LEFT | SIGN_UP,

    SIGN_RIGHT | SIGN_UP,
    SIGN_LEFT | SIGN_DOWN,
    SIGN_RIGHT | SIGN_DOWN,
    SIGN_RIGHT | SIGN_UP | SIGN_DOWN,

    SIGN_LEFT | SIGN_UP | SIGN_DOWN,
    SIGN_LEFT,
    SIGN_RIGHT,
    SIGN_DOWN,

    SIGN_UP,
    SIGN_LEFT | SIGN_RIGHT,
    SIGN_UP | SIGN_DOWN,
    SIGN_NONE,
};

const u8 tiles[][4] = {
    {0, 0, 8, 8},
    {7, 7, 0, 0},
    {7, 7, 8, 8},
    {0, 43, 8, 42},

    {11, 0, 41, 8},
    {7, 10, 0, 43},
    {9, 7, 11, 0},
    {11, 0, 11, 0},

    {0, 43, 0, 43},
    {7, 10, 8, 42},
    {9, 7, 41, 8},
    {9, 10, 11, 43},

    {11, 43, 41, 42},
    {7, 7, 8, 8},
    {11, 43, 11, 43},
    {9, 10, 41, 42},
};


void tile_wall_draw(uint tx, uint ty) {
    sig_t sig = sig_get(tx, ty);
    
    if(sig == SIGN_ALL) {
        bg_fill(map_get_map()->bg, tx << 1, ty << 1, 2, 2, 0);
        return;
    }

    for(uint i = 0; i < LENGTH(tests); i++ ) {
        if(sig_test(sig, tests[i], masks[i])) {
            bg_rect(map_get_map()->bg, tx << 1, ty << 1, 2, 2, (void*)tiles[i]);
            return;
        }
    }

}


// move
void tile_draw_none(uint tx, uint ty) {
    sig_t s = sig_get(tx, ty);

    if(sig_test(s, SIGN_UP, SIGN_UP)) {
        tile_t tiles[] = {40, 40, 65, 66};
        bg_rect(map_get_map()->bg, tx << 1, ty << 1, 2, 2, tiles);
    } else {
        tile_t tiles[] = {67, 68, 99, 100};
        bg_rect(map_get_map()->bg, tx << 1, ty << 1, 2, 2, tiles);
    }
}