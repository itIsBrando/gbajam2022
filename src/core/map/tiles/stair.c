#include "tile.h"
#include "../../GameState.h"
#include "../../team.h"

void tile_stair_interact(void *u, uint tx, uint ty) {
    Unit *unit = (Unit *)u;

    if(unit == tm_leader()) {
        state_init(&sta_main_state, true);
    } else {
        // @todo communicate to the user that only the leader can use the stairs
    }
}