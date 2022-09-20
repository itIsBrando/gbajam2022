#include <assert.h>
#include "team.h"
#include "unit/unit.h"
#include "map/mapgen.h"

static Team *_tm;
static uint cur_unit = 0;

void internal_rm_all_focus();


void tm_init(Team *t, unit_type_t types[], uint size, WIN_REGULAR *win) {
    room_t *r = gen_get_starting_room();

    _tm = t;

    for(uint i = 0; i < size; i++) {
        unit_init(&t->units[i], *types++);
        unit_move_to(&t->units[i], r->x + 1, r->y + 1 + i);
    }

    plr_init(t->units, win);
    
    t->size = size;
    t->auto_follow = false;
}


void tm_update() {
    Unit *u = _tm->units;

    unit_inc_anim_frames();

    plr_update();

    for(uint i = 0; i < _tm->size; i++) {
        unit_update(u);
        u++;
    }

    mob_update();

    if(ps_get() == PS_DONE) {
        cur_unit++;
        cur_unit %= _tm->size;
        mob_do_turns();
        tm_start_turn(_tm, &_tm->units[cur_unit]);
    }
}


void tm_start_turn(Team *tm, Unit *u) {
    internal_rm_all_focus();
    plr_switch(u);

    if(!tm_auto_follow() || u == tm_leader())
        tm_focus(tm, u);
}


void internal_rm_all_focus() {
    Unit *units = _tm->units;
    
    for(uint i = 0; i < _tm->size; i++) {
        units->has_focus = false;
        units++;
    }
}


void tm_focus(Team *tm, Unit *u) {
    u->has_focus = true;
    map_set_scroll_speed(MAP_SCROLL_FAST);
    map_scroll_to(u->tx, u->ty);
}


/**
 * Adds a unit to a team if the team has available space
 */
bool tm_add(Team *tm, Unit *u) {
    if(tm->size >= TEAM_MAX_SIZE)
        return false;

    tm->units[tm->size++] = *u;
    return true;
}


Unit *tm_leader() {
    return &_tm->units[0];
}

Team *tm_get() {
    return _tm;
}


bool tm_auto_follow() {
    return _tm->auto_follow;
}


void tm_set_auto_follow(bool v) {
    _tm->auto_follow = v;
}