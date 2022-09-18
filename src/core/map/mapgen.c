// #include <stdlib.h>
#include "map.h"
#include "mapgen.h"
#include "../../text.h"
#include "../../lib/random.h"
#include <string.h>

// @todo remove
void internal_error(char *str, bool fatal);

static void internal_gen_room(room_t *rm);
static bool internal_place_room(room_t *r);
static bool internal_can_place_room(room_t *r, uint x, uint y);
static void internal_gen_stairs();

static u8 internal_get_flag(int x, int y);
static void internal_set_flag(int x, int y, u8 v);
static void internal_expand_flag(uint x, uint y, u8 v);


room_t gen_starting_room;

void gen_generate() {
    text_print("MAKING ROOMS", 0, 0);
    gen_gen_rooms();

    text_print("MAKING HALLS", 0, 0);
    wrm_do_worms();

    text_print("MAKING DOOR", 0, 0);
    gen_make_flags();

    text_print("FINISHING UP    ", 0, 0);

    internal_gen_stairs();

    map_draw();
}


inline bool sig_test(sig_t sig, sig_t test, sig_t mask) {
    return (sig & mask) == test;
}


sig_t sig_get(int tx, int ty) {
    tx--;
    ty--;

    sig_t s = 0;
    
    for(int y = 0; y < 3; y++) {
        tile_t *d = map_get_pointer(tx, ty + y);

        for(int x = 0; x < 3; x++) {
            if(x == 1 && y == 1) {
                d++;
                continue;
            }
            
            s <<= 1;
            if(!map_inbounds(tx + x, ty + y))
                s |= 1;
            else {
                s |= (*d == TILE_WALL);
            }

            d++;
        }

    }
    
    return s;
}


void gen_gen_rooms() {
    uint room_max = GEN_MAX_ROOMS, r = 0;
    uint fail_max = 2; // number of times we fail placing a room before we stop
    room_t rooms[room_max];

    text_uint(room_max, 17, 0);
    text_char('/', 15, 0);
    do {
        room_t *cur_room = &rooms[r];
        internal_gen_room(cur_room);
        text_uint(GEN_MAX_ROOMS + 1 - room_max, 14, 0);
        if(internal_place_room(cur_room))
            room_max--, r++;
        else
            fail_max--;

    } while(room_max && fail_max);

    gen_starting_room = rooms[rnd_random_bounded(0, r)];
}


/**
 * @returns gets the room that your team will start in
 */
room_t *gen_get_starting_room() {
    return &gen_starting_room;
}


static void internal_gen_room(room_t *rm) {
    do {
        rm->width = rnd_random_bounded(GEN_MIN_ROOM_WIDTH, GEN_MAX_ROOM_WIDTH + 1);
        rm->height = rnd_random_bounded(GEN_MIN_ROOM_WIDTH, GEN_MAX_ROOM_WIDTH + 1);
    } while(rm->height * rm->width > GEN_MAX_ROOM_AREA);

    rm->x = rm->y = 0;
}


inline bool map_inbounds(int tx, int ty) {
    return tx >= 0 && ty >= 0 && tx < MAP_TILE_WIDTH && ty < MAP_TILE_HEIGHT;
}


/**
 * @returns true if the room was placed on the map, false otherwise
 */
static bool internal_place_room(room_t *r) {
    Position candidates[MAP_SIZE];
    uint i = 0, x, y;
    
    for(y = 1; y < MAP_TILE_HEIGHT - r->height; y++) {
        for(x = 1; x < MAP_TILE_WIDTH - r->width; x++) {
            if(internal_can_place_room(r, x, y)) {
                candidates[i].x = x;
                candidates[i++].y = y;
            }
        }
    }

    if(i == 0)
        return false;

    Position *p = &candidates[(uint)rnd_random_bounded(0, i)];


    r->x = p->x;
    r->y = p->y;

    for(y = 0; y < r->height; y++) {
        for(x = 0; x < r->width; x++) {
            map_set(r->x + x, r->y + y, TILE_NONE);
        }
    }

    return true;
}


static bool internal_can_place_room(room_t *r, uint x, uint y) {
    for(int i = -1; i <= r->height + 1; i++) {
        tile_t *d = map_get_pointer(x - 1, y + i);
        for(int j = -1; j <= r->width + 1; j++) {
            if(!map_get_data(*d)->solid)
                return false;
            d++;
        }
    }

    return true;
}


static u8 flags[MAP_SIZE]; // size is 64x64
static u8 cur_flag;

static inline u8 internal_get_flag(int x, int y) {
    if(!map_inbounds(x, y))
        return 1;
    
    return flags[x + (y << MAP_BITS_WIDTH)];
}


static inline void internal_set_flag(int x, int y, u8 v) {
    if(map_inbounds(x, y))
        flags[x + (y << MAP_BITS_WIDTH)] = v;
}


void internal_expand_flag(uint x, uint y, u8 v) {
    uint cands_len = 1, new_cands_len;
    Position cands[30 * 30], new_cands[30 * 30];

    cands->x = x, cands->x = y;
    internal_set_flag(x, y, v);
    
    do { 
        new_cands_len = 0;
        for(uint j = 0; j < cands_len; j++) {
            for(direction_t dir = 0; dir < 4; dir++) {
                const int dx = cands[j].x + dir_get_x(dir), dy = cands[j].y + dir_get_y(dir);
                u8 f = internal_get_flag(dx, dy);

                if(f != v && !map_is_solid(dx, dy)) {
                    Position *c = &new_cands[new_cands_len++];
                    c->x = dx, c->y = dy;
                    internal_set_flag(dx, dy, v);
                }
            }
        }

        memcpy(cands, new_cands, new_cands_len * sizeof(Position));
        cands_len = new_cands_len;
    } while(cands_len);
}


/**
 * This function uses `flags` so `candidate` memory is NOT available
 */
void gen_make_flags() {
    uint x, y;
    memset(flags, 0, LENGTH(flags));
    cur_flag = 1;

    for(y = 0; y < MAP_TILE_HEIGHT; y++) {
        for(x = 0; x < MAP_TILE_WIDTH; x++) {
            if(!map_is_solid(x, y) && internal_get_flag(x, y) == 0) {
                internal_expand_flag(x, y, cur_flag++);
            }
        }
    }

    // now we will make doors n shiz
    uint doors = 0;
    door_t cands[600];

    for(y = 1; y < MAP_TILE_HEIGHT - 1; y++) {
        for(x = 1; x < MAP_TILE_WIDTH - 1; x++) {
            sig_t s = sig_get(x, y);
            u8 f1 = 0, f2 = 0, d = 0;
            bool found = false;

            if(!map_is_solid(x, y))
                continue;

            if(sig_test(s, SIGN_LEFT | SIGN_RIGHT, SIGN_CARDINAL)) { // door to connect up & down
                f1 = internal_get_flag(x, y - 1);
                f2 = internal_get_flag(x, y + 1);
                d = DIRECTION_UP;
                found = true;
            } else if(sig_test(s, SIGN_UP | SIGN_DOWN, SIGN_CARDINAL)) {
                f1 = internal_get_flag(x - 1, y);
                f2 = internal_get_flag(x + 1, y);
                d = DIRECTION_LEFT;
                found = true;
            }

            if(found && f1 != f2) {
                door_t *c = &cands[doors++];
                c->x = x;
                c->y = y;
                c->d = d;
                if(doors >= LENGTH(cands)) {
                    y = 100, x = 100;
                }
            }
        }
    }

    do {
        door_t *c = &cands[rnd_random_bounded(0, doors)];

        int dx = dir_get_x(c->d), dy = dir_get_y(c->d);
        u8 f1 = internal_get_flag(c->x + dx, c->y + dy);
        u8 f2 = internal_get_flag(c->x - dx, c->y - dy);
        if(f1 != f2) {
            map_set(c->x, c->y, TILE_NONE);
            internal_expand_flag(c->x, c->y, f1);
        }

        *c = cands[--doors];
    } while(doors);

}

void internal_gen_stairs() {
    uint tx;
    uint ty;

    do {
        tx = rnd_random() & (MAP_TILE_WIDTH - 1);
        ty = rnd_random() & (MAP_TILE_HEIGHT - 1);
    } while(sig_get(tx, ty) != SIGN_NONE);
    
    map_set(tx, ty, TILE_STAIR);
}



void internal_error(char *str, bool fatal) {
    text_print("                    ", 0, 0);
    text_print(str, 0, 0);
    while(fatal);
}