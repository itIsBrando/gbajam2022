#include "../../lib/window.h"
#include "../../lib/keypad.h"
#include "../../text.h"
#include "../map/map.h"
#include "../map/path.h"
#include "../hud/hud.h"
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
        if(tm_is_leader(plr)) {
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
        if(!tm_is_leader(plr))
            ps_set(PS_FOLLOWING);
    } else if(key & KEY_LEFT) {
        ps_set(PS_ATK);
    } else if(key & KEY_START) {
        hud_stat_menu(&plr->stats, &plr->inv);
        ps_init_selecting();
    } else if(tm_is_leader(plr)) {
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


static void _attack_clean_up() {
    cur_deinit();
}


static void ps_init_attack() {
    cur_init(plr);
    plr_hud();
}


const char _errs[][23] = {
    [ATK_FAIL_EMPTY]="NOTHING TO ATTACK",
    [ATK_FAIL_FRIENDLY]="CANNOT ATTACK TEAMMATE",
    [ATK_FAIL_OUT_OF_RANGE]="NOT IN RANGE",
    [ATK_FAIL_SELF]="CANNOT ATTACK SELF",
};


static bool _do_atk() {
    atk_error_t e = cur_do_attack(plr);

    if(e != ATK_SUCCESS) {
        bar_string((char*)_errs[e], FMT_CENTER_JUSTIFY);
        bar_set_timeout(120);
        return false;
    }

    _attack_clean_up();
    ps_set(PS_DONE);
    
    return true;
}


static void ps_attack() {
    key = key_pressed_repeat_after(8);

    if(key & KEY_B) {
        if(steps)
            ps_set(PS_SELECTING);
        else
            ps_set(PS_DONE);
        _attack_clean_up();
        return;
    }

    if(key & KEY_RIGHT)
        cur_move(DIRECTION_RIGHT);
    else if(key & KEY_LEFT)
        cur_move(DIRECTION_LEFT);
    else if(key & KEY_UP)
        cur_move(DIRECTION_UP);
    else if(key & KEY_DOWN)
        cur_move(DIRECTION_DOWN);

    if(key & KEY_A)
        _do_atk(DIRECTION_NONE);

    cur_draw();
}


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
        if(tm_auto_follow())
            ps_set(PS_DONE);
        else
            ps_set(PS_ATK);
        return;
    }

    if((key & KEY_B) == 0 && tm_auto_follow() && tm_is_leader(plr)) {
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
    bg_fill(_win->background, 0, 0, 30, 18, 0);

    // draw HP bar
    hud_hp(0, 0, plr->stats.hp, plr->stats.max_hp);
    
    // draw lvl
    text_printf("%c%d", 0, 1, 546, plr->stats.level);

    // draw current floor
    text_printf("%c%d", 28, 0, 576, map_get_map()->level);

    // draw icon
    bg_write_tile(_win->background, 3, 1, ps_icons[state]);

    // draw leader icon
    if(tm_is_leader(plr))
        bg_write_tile(_win->background, 4, 1, 582);
}