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
    stat_fill(&u->stats, type, map_get_map()->level);
    u->stats.hp = u->stats.max_hp;
}


bool mob_spawn() {
    if(_unit_size == 10) {
        return false;
    }

    uint tx, ty;

    do {
        tx = rnd_random_bounded(0, MAP_TILE_WIDTH);
        ty = rnd_random_bounded(0, MAP_TILE_HEIGHT);
    } while(map_is_solid(tx, ty) || sig_get(tx, ty) != 0 || unit_at(tx, ty));

    mob_init(&_units[_unit_size++], UNIT_MAGE + (rnd_random() & 1), tx, ty);
    return true;
}


void mob_update() {

}


void mob_do_turns() {
    for(uint i = 0; i < _unit_size; i++) {
        _do_turn(&_units[i]);
    }
}


/**
 * The higher the heuristic, the worse choice `other` is for attacking
 * Depends on:
 *  - level
 *  - current HP
 *  - linear distance to unit
 */
static int _get_attack_heuristic(Unit *mob, Unit *other) {
    return other->stats.hp + other->stats.level + pf_dist_unit(mob, other);
}


Unit *mob_get_best_candidate(Unit *mob) {
    Team *tm = tm_get();
    Unit *best_u = NULL;
    int best_heuristic = 999;

    for(uint i = 0; i < tm->size; i++) {
        int h = _get_attack_heuristic(mob, &tm->units[i]);

        if(h < best_heuristic) {
            best_heuristic = h;
            best_u = &tm->units[i];
        }
    }

    return best_u;
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


void mob_remove_all() {
    for(uint i = 0; i < _unit_size; i++) {
        unit_deinit(&_units[i]);
    }

    _unit_size = 0;
}