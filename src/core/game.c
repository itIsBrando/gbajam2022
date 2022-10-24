#include "GameState.h"
#include "team.h"
#include "unit/unit.h"
#include "map/map.h"
#include "map/mapgen.h"
#include "hud/hud.h"

#include <gba_systemcalls.h>
#include "../lib/keypad.h"
#include "../lib/obj.h"
#include "../lib/bg.h"
#include "../lib/window.h"
#include "../lib/random.h"
#include "../text.h"


static BG_REGULAR _bg_win, _bg;
static WIN_REGULAR win;

static Team _tm;


static void gme_init() {
    bg_affine_init(&_bg, 12, 0, 2);

    bg_init(&_bg_win, 30, 0, 1);
    bg_set_priority(&_bg_win, BG_PRIORITY_LOWEST);
    win.background = &_bg_win;

    map_init(&_bg);

    text_init(&_bg_win, 525);

    bar_string("A BUTTON TO START", FMT_CENTER_JUSTIFY);
    do {
        key_scan();
        rnd_seed(rnd_random());
        VBlankIntrWait();
    } while(key_pressed() != KEY_A);

    gen_generate();

    unit_type_t types[] = {UNIT_HERO, UNIT_MAGE, UNIT_ORGE};
    tm_init(&_tm, types, 3, &win);

    tm_set_starting_pos();
    tm_focus(&_tm, _tm.units);

    mob_spawn();
    mob_spawn();
    mob_spawn();
    mob_spawn();
}


static void gme_deinit() {
    // tm_deinit();
    map_deinit();
}


static void gme_update() {
    key_scan();

    map_update();

    unit_update_all();

    tm_update();
    bar_update();
    
    spr_copy_all();
}



const GameState gme_main_state = {
    .init=gme_init,
    .deinit=gme_deinit,
    .update=gme_update,
};
