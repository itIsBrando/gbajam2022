#include <string.h>
#include <gba_systemcalls.h>

#include "GameState.h"

static GameState internal_cur_state;

void state_init(const GameState *s, bool initialize) {
    if(s->init && initialize)
        s->init();

    memcpy(&internal_cur_state, s, sizeof(internal_cur_state));
}


inline void state_update() {

    if(internal_cur_state.update)
        internal_cur_state.update();

    VBlankIntrWait();
}