#ifndef MAPGEN_H
#define MAPGEN_H

#include <gba_types.h>
#include <gba_base.h>
#include "../../lib/defines.h"
#include "map.h"


#define GEN_MAX_ROOMS       9
#define GEN_MAX_WORMS       15

#define GEN_MAX_ROOM_WIDTH 8
#define GEN_MIN_ROOM_WIDTH 4
#define GEN_MAX_ROOM_AREA   (GEN_MAX_ROOM_WIDTH * GEN_MAX_ROOM_WIDTH)

typedef struct {
    u8 x, y;
    u8 width, height;
} room_t;



typedef struct {
    u8 x, y;
    u8 d; // direction
} door_t;


typedef struct {
    direction_t dir;
    u16 x, y;
    uint fails;
} Worm;


typedef enum {
    SIGN_UP_LEFT    = BIT(7),
    SIGN_UP         = BIT(6),
    SIGN_UP_RIGHT   = BIT(5),
    SIGN_LEFT       = BIT(4),
    SIGN_RIGHT      = BIT(3),
    SIGN_DOWN_LEFT  = BIT(2),
    SIGN_DOWN       = BIT(1),
    SIGN_DOWN_RIGHT = BIT(0),
    SIGN_ALL        = 0xFF,
    SIGN_NONE       = 0,
    SIGN_CARDINAL   = SIGN_UP | SIGN_LEFT | SIGN_RIGHT | SIGN_DOWN,
} sig_t;


void gen_generate();
void gen_gen_rooms();
room_t *gen_get_starting_room();
void gen_make_flags();

bool wrm_can_carve(uint tx, uint ty, direction_t dir);
bool wrm_random_dir(Worm *w);
void wrm_carve(Worm *w);
void wrm_create(uint tx, uint ty, direction_t dir);
void wrm_do_worms();


Position *gen_get_cand(uint i);
void gen_set_cand(uint i, uint x, uint y);

sig_t sig_get(int tx, int ty);
bool sig_test(sig_t, sig_t, sig_t);


#endif