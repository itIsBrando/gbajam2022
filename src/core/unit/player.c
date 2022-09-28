#include "../../lib/window.h"
#include "../../lib/keypad.h"
#include "../../text.h"
#include "../map/map.h"
#include "../map/path.h"
#include "../team.h"
#include "unit.h"
#include "stats.h"

static Unit *plr;
static WIN_REGULAR *_win;
static uint key;
static uint steps;
static PlayerState state;
static uint flash_timer = 0;


static void ps_init_selecting();
static void ps_init_move();
static void ps_init_done();
static void ps_init_attack();
static void ps_selecting();
static void ps_move();
static void ps_attack();
static void ps_following();

static void plr_do_flash();

const void (*ps_states[PS_SIZE])(void) = {
    [PS_ATK] = ps_attack,
    [PS_MOVE] = ps_move,
    [PS_SELECTING] = ps_selecting,
    [PS_FOLLOWING] = ps_following,
    [PS_DONE] = NULL, // this is not guaranteed to get called.
};

const void (*ps_init[PS_SIZE])(void) = {
    [PS_ATK] = ps_init_attack,
    [PS_MOVE] = ps_init_move,
    [PS_FOLLOWING] = ps_init_move,
    [PS_SELECTING] = ps_init_selecting,
    [PS_DONE] = ps_init_done,
};

const u16 ps_icons[] = {
    [PS_MOVE]=580,
    [PS_FOLLOWING]=580,
    [PS_ATK]=579,
};


void plr_init(Unit *p, WIN_REGULAR *win) {
    _win = win;
    plr_switch(p);
}


void plr_update() {
    key = key_pressed();

    if(ps_states[state])
        ps_states[state]();
}


static void ps_init_move() {
    plr_hud();
    map_set_scroll_speed(MAP_SCROLL_MED);
}


static void ps_init_done() {
    unit_show(plr);
}


static void ps_init_selecting() {
    const uint SX = 14, SY = 12;
    // if we are in follow mode, then skip selections
    if(tm_auto_follow()) {
        if(tm_leader() == plr) {
            if(key & KEY_B) {
                ps_set(PS_MOVE);
                return;
            }
        } else {
            ps_set(PS_FOLLOWING);
            return;
        }
    }

    const u16 tiles[] = {
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

    plr_hud();

    bg_rect(_win->background, SX + 3, SY + 1, 2, 2, (void*)tiles);

    for(uint i = 0; i < 4; i++) {
        text_print((char*)strings[i], SX + x[i], SY + y[i]);
    }
}


static void plr_do_flash() {
    flash_timer++;
    flash_timer &= 15;

    if(flash_timer >= 10) {
        unit_hide(plr);
    } else {
        unit_show(plr);
    }
}


static void ps_selecting() {
    plr_do_flash();

    if(key & KEY_UP) {
        ps_set(PS_MOVE);
    } else if(key & KEY_DOWN) {
        ps_set(PS_DONE);
    } else if(key & KEY_RIGHT) {
        if(tm_leader() != plr)
            ps_set(PS_FOLLOWING);
    } else if(key & KEY_LEFT) {
        ps_set(PS_ATK);
    } else if(tm_leader() == plr) {
        // set auto mode if we are pressing B
        if(key & KEY_B) {
            tm_set_auto_follow(true);
            ps_set(PS_MOVE);
        } else {
            tm_set_auto_follow(false);
        }
    }

    key_wait_until_released(KEY_RIGHT | KEY_LEFT | KEY_UP | KEY_DOWN);
}

static obj_t *_atk_spr[4];

static void _attack_clean_up() {
    for(uint i = 0; i < 4; i++) {
        spr_free(_atk_spr[i]);
    }
}


static void ps_init_attack() {
    for(uint i = 0; i < 4; i++) {
        uint tx = plr->tx + dir_get_x(i), ty = plr->ty + dir_get_y(i);
        
        _atk_spr[i] = spr_alloc((tx - map_get_tx()) << 4, (ty - map_get_ty()) << 4, 33);
        if(map_is_solid(tx, ty)) {
            spr_hide(_atk_spr[i]);
        } else {
            spr_set_size(_atk_spr[i], SPR_SIZE_16x16);
            spr_set_gfx_mode(_atk_spr[i], SPR_GFX_ALPHA_BLENDING);
            spr_set_priority(_atk_spr[i], SPR_PRIORITY_HIGH);
        }
    }

    plr_hud();
}

static bool _do_atk(direction_t d) {
    Unit *u = unit_at(plr->tx + dir_get_x(d), plr->ty + dir_get_y(d));

    if(!u || unit_is_dead(u) || !u->is_ai)
        return false;
    
    unit_kill(u);

    key_wait_until_released(KEY_LEFT | KEY_RIGHT | KEY_UP | KEY_DOWN);

    _attack_clean_up();
    ps_set(PS_DONE);
    
    return true;
}


static void ps_attack() {
    if(key & KEY_B) {
        ps_set(PS_SELECTING);
        _attack_clean_up();
        return;
    }


    if(key & KEY_RIGHT)
        _do_atk(DIRECTION_RIGHT);
    else if(key & KEY_LEFT)
        _do_atk(DIRECTION_LEFT);
    else if(key & KEY_UP)
        _do_atk(DIRECTION_UP);
    else if(key & KEY_DOWN)
        _do_atk(DIRECTION_DOWN);
}


// @todo prevent follower from swapping with the leader
static void ps_following() {
    if(map_moving() || plr->is_moving || plr == tm_leader())
        return;
    
    if(steps) {
        Unit *leader = tm_leader();

        u8 *m = pf_distance_map(leader->tx, leader->ty);
        direction_t d = pf_best_dir(m, plr->tx, plr->ty);
        
        // ensures that this unit will not swap places with the leader
        if(d == DIRECTION_NONE || (leader->tx == plr->tx + dir_get_x(d) && leader->ty == plr->ty + dir_get_y(d))) {
            ps_set(PS_DONE);
        } else {
            map_set_scroll_speed(MAP_SCROLL_INSTANT);
            unit_move(plr, d);
        }
        steps--;
    } else {
        ps_set(PS_DONE);
    }
}


static void ps_move() {
    plr_do_flash();

    if(!steps && !map_moving()) {
        ps_set(PS_DONE);
        return;
    }

    if((key & KEY_B) == 0 && tm_auto_follow() && plr == tm_leader()) {
        tm_set_auto_follow(false);
        ps_set(PS_SELECTING);
        return;
    }


    if((key & KEY_B) && !tm_auto_follow())
        ps_set(PS_SELECTING);
    else if(key & KEY_RIGHT)
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
    flash_timer = 0;
    // initialize state if needed
    if(ps_init[s])
        ps_init[s]();
}


PlayerState ps_get() {
    return state;
}


void plr_switch(Unit *u) {
    plr = u;
    steps = u->stats.move_points;
    ps_set(PS_SELECTING);
}


/**
 * Draws the player's hud
 */
void plr_hud() {
    bg_fill(_win->background, 0, 0, 30, 20, 0);

    // draw HP bar
    u16 tiles[] = {
        544, 576, 577, 577, 578
    };
    bg_rect(_win->background, 0, 0, 5, 1, tiles);
    bg_write_tile(_win->background, 0, 1, ps_icons[state]);

    // draw leader icon
    if(tm_leader() == plr)
        bg_write_tile(_win->background, 1, 1, 582);
}