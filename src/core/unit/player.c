// #include "../../lib/obj.h"
#include <gba_systemcalls.h>
#include "../../lib/window.h"
#include "../../lib/keypad.h"
#include "../../text.h"
#include "../map/map.h"
#include "../map/path.h"
#include "unit.h"
#include "../team.h"

static Unit *plr;
static WIN_REGULAR *_win;
static uint key;
static uint steps;
static PlayerState state;


static void ps_selecting();
static void ps_init_selecting();
static void ps_move();
static void ps_init_move();
static void ps_attack();
static void ps_following();

const void (*ps_states[PS_SIZE])(void) = {
    [PS_ATK] = ps_attack,
    [PS_MOVE] = ps_move,
    [PS_SELECTING] = ps_selecting,
    [PS_FOLLOWING] = ps_following,
    [PS_DONE] = NULL,
};

const void (*ps_init[PS_SIZE])(void) = {
    [PS_MOVE]=ps_init_move,
    [PS_FOLLOWING]=ps_init_move,
    [PS_SELECTING]=ps_init_selecting,
};

const char ps_string[PS_SIZE][5] = {
    [PS_MOVE]="MOVE",
    [PS_ATK]="ATCK",
};

void plr_init(Unit *p, WIN_REGULAR *win) {
    _win = win;
    plr_switch(p);
}


/**
 * Called every frame
 */
void plr_update() {
    key = key_pressed();

    if(ps_states[state])
        ps_states[state]();
}

static void ps_init_move() {
    steps = 2;
    plr_hud();
}

static void ps_init_selecting() {
    u16 tiles[] = {
        516, 517, 518, 519
    };

    const char strings[4][5] = {
        "MOVE",
        "FLW",
        "WAIT",
        "ATK"
    };
    const u8 x[] = {2, 5, 2, 0};
    const u8 y[] = {0, 1, 3, 1};

    win_move(_win, 7 << 4, 6 << 4, 64, 32);
    bg_fill(_win->background, 0, 0, 8, 8, 0);
    bg_rect(_win->background, 3, 1, 2, 2, tiles);

    for(uint i = 0; i < 4; i++) {
        text_print((char*)strings[i], x[i], y[i]);
    }
}

static void ps_selecting() {
        /*
    flash unit

    IF UP
        draw normal hud
        draw action: MOVE
    IF RIGHT
        pathfind
        move towards team.unit[0]
    IF DOWN
        is_selecting = false
        tm_finished_action()
    IF LEFT
        change game state to attacking
    */

    if(key & KEY_UP) {
        ps_set(PS_MOVE);
    } else if(key & KEY_DOWN) {
        ps_set(PS_DONE);
    } else if(key & KEY_RIGHT) {
        if(tm_leader() != plr)
            ps_set(PS_FOLLOWING);
    }

    do {
        key_scan();
        VBlankIntrWait();
    } while(key_pressed());
}


static void ps_attack() {

}

static void ps_following() {
    if(!map_moving() && steps) {
        Unit *leader = tm_leader();

        u8 *m = pf_distance_map(leader->tx, leader->ty);
        unit_move(plr, pf_best_dir(m, plr->tx, plr->ty));
        steps--;
    } else if(!map_moving()) {
        ps_set(PS_DONE);
    }
}


static void ps_move() {
    if(!steps && !map_moving())
        ps_set(PS_DONE);

    if(key & KEY_RIGHT)
        steps -= unit_move(plr, DIRECTION_RIGHT);
    else if(key & KEY_LEFT)
        steps -= unit_move(plr, DIRECTION_LEFT);
    else if(key & KEY_UP)
        steps -= unit_move(plr, DIRECTION_UP);
    else if(key & KEY_DOWN)
        steps -= unit_move(plr, DIRECTION_DOWN);
}


void ps_set(PlayerState s) {
    state = s;
    // initialize state if needed
    if(ps_init[s])
        ps_init[s]();
}


PlayerState ps_get() {
    return state;
}


void plr_switch(Unit *u) {
    plr = u;
    ps_set(PS_SELECTING);
}


/**
 * Draws the player's hud
 */
void plr_hud() {
    win_move(_win, 0, 160 - 8, 240, 8);
    bg_fill(_win->background, 0, 0, 20, 1, 0);
    text_print((char*)ps_string[state], 0, 0);
}