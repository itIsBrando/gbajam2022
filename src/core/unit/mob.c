#include "../map/map.h"

#include "../../lib/random.h"
#include "../../text.h"
#include "../map/path.h"
#include "../map/mapgen.h"
#include "unit.h"
#include "stats.h"
#include "../team.h"

static Unit _units[10];
static uint _unit_size = 0;

static void _do_turn(Unit *u);

void mob_init(Unit *u, unit_type_t type, uint tx, uint ty) {
    unit_init(u, type, NULL);
    u->tx = tx, u->ty = ty;
    u->is_ai = true;
    
    // get stats to satisfy floor level
    stat_fill(u, map_get_map()->level);
}


bool mob_spawn() {
    if(_unit_size == 10) {
        return false;
    }

    uint tx, ty;

    do {
        tx = rnd_random_bounded(0, MAP_TILE_WIDTH);
        ty = rnd_random_bounded(0, MAP_TILE_HEIGHT);
    } while(map_is_solid(tx, ty) || sig_get(tx, ty) != 0);

    mob_init(&_units[_unit_size++], UNIT_MAGE, tx, ty);
    return true;
}


void mob_update() {

}


void mob_do_turns() {
    for(uint i = 0; i < _unit_size; i++) {
        _do_turn(&_units[i]);
    }
}


static void _do_turn(Unit *u) {
    Team *tm = tm_get();

    const Unit *opp = tm->units;
    for(uint i = 0; i < tm->size; i++) {
        if(pf_has_line_of_sight(u->tx, u->ty, opp->tx, opp->ty)) {
            u8 *m = pf_distance_map(opp->tx, opp->ty);
            unit_move(u, pf_best_dir(m, u->tx, u->ty));
            break;
        }

        opp++;
    }
}