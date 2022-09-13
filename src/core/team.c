#include <assert.h>
#include "team.h"
#include "unit/unit.h"
#include "map/mapgen.h"

static Team *_tm;
static uint cur_unit = 0;

void tm_init(Team *t, unit_type_t types[], uint size, control_type_t cnt, WIN_REGULAR *win) {
    room_t *r = gen_get_starting_room();

    _tm = t;

    for(uint i = 0; i < size; i++) {
        unit_init(&t->units[i], *types++);
        unit_move_to(&t->units[i], r->x + 1, r->y + 1 + i);
    }

    if(cnt == CONTROL_PLR)
        plr_init(t->units, win);
    
    t->size = size;
    t->control = cnt;
}


void tm_update(Team *tm) {
    Unit *u = tm->units;

    plr_update();

    for(uint i = 0; i < tm->size; i++) {
        unit_update(u);
        u++;
    }

    if(ps_get() == PS_DONE) {
        cur_unit++;
        cur_unit %= tm->size;   D
        tm_focus(tm, &tm->units[cur_unit]);
    }
}



void tm_focus(Team *tm, Unit *u) {
    Unit *units = tm->units;

    for(uint i = 0; i < tm->size; i++) {
        units->has_focus = false;
    }

    u->has_focus = true;
    plr_switch(u);
    map_scroll_to(u->tx, u->ty, MAP_SCROLL_INSTANT);
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