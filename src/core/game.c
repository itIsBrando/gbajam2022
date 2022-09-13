#include "GameState.h"
#include "team.h"
#include "unit/unit.h"
#include "map/map.h"
#include "map/mapgen.h"

#include "../lib/keypad.h"
#include "../lib/obj.h"
#include "../lib/bg.h"
#include "../lib/window.h"
#include "../text.h"


static BG_REGULAR _bg_win, _bg;
static WIN_REGULAR win;

static Team _tm;

static void gme_init() {
    bg_affine_init(&_bg, 12, 0, 2);

    // bg_fill(&_bg, 0, 0, 32, 32, 65);
    bg_init(&_bg_win, 30, 0, 1);
    win_init(&win, &_bg_win, 0);
    win_move(&win, 0, 160 - 8, 240, 8);

    map_init(&_bg);

    map_set(13, 5, TILE_WALL);

    text_init(&_bg_win, 525);

    gen_generate();

    unit_type_t types[] = {UNIT_HERO, UNIT_SKELETON, UNIT_SKELETON};
    tm_init(&_tm, types, 3, CONTROL_PLR, &win);

    tm_focus(&_tm, _tm.units);
}


static void gme_deinit() {
    // tm_deinit();
    map_deinit();
}


static void gme_update() {
    key_scan();
    uint key = key_pressed();

    map_update();
    tm_update(&_tm);
    
    spr_copy_all();
}



const GameState gme_main_state = {
    .init=gme_init,
    .deinit=gme_deinit,
    .update=gme_update,
};
