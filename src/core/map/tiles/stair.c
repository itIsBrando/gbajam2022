#include "tile.h"
#include "../../GameState.h"
#include "../../team.h"
#include "../../hud/hud.h"

void tile_stair_interact(void *u, uint tx, uint ty) {
    Unit *unit = (Unit *)u;

    if(tm_is_leader(unit)) {
        state_init(&sta_main_state, true);
    } else if(!unit->is_ai) {
        bar_string("ONLY LEADER CAN USE STAIRS", FMT_CENTER_JUSTIFY);
        bar_set_timeout(120);
    }
}