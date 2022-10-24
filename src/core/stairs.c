#include "GameState.h"
#include "team.h"
#include "unit/unit.h"
#include "map/map.h"
#include "map/mapgen.h"

#include "../lib/keypad.h"

static Team *_tm;

static void sta_init();
static void sta_update();
static void sta_done();


static void sta_init() {
    _tm = tm_get();

    map_get_map()->level++;

    // @todo add animation here por favors

}


static void sta_update() {
    key_scan();

    map_update();
    unit_update_all();

    // if unit is on stairs, then hide
    Unit *u =tm_leader();
    if(!u->is_moving && !map_moving()) {
        sta_done();
    }

    // if not, then move towards stairs

    spr_copy_all();
}


static void sta_done() {
    unit_hide_all();

    mob_remove_all();
    gen_generate();
    tm_set_starting_pos();
    mob_spawn(); mob_spawn(); mob_spawn(); mob_spawn(); // @todo mob spawning
    state_init(&gme_main_state, false);
}


const GameState sta_main_state = {
    .init=sta_init,
    .update=sta_update
};