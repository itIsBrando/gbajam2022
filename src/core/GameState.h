#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "map/map.h"

typedef struct {
    void (*init)(void); /** Called when installed */
    void (*deinit)(void); /** Called when this state is no longer in use */
    void (*update)(void); /** Called at 60fps */
} GameState;


void state_init(const GameState *s, bool initialize);
void state_update();


extern const GameState gme_main_state;
extern const GameState sta_main_state;

#endif