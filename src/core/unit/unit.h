#ifndef UNIT_H
#define UNIT_H

#include <stdio.h>
#include "../map/map.h"
#include "../../lib/obj.h"

typedef enum {
    UNIT_HERO,
    UNIT_SKELETON,
} unit_type_t;

typedef struct {
    uint tx, ty; // absolute tile position
    int dx, dy; // (x, y) pixel offsets
    obj_t *obj;
    uint16_t tile;
    uint16_t anim_tile;
    direction_t dir;

    bool is_moving;
    bool has_focus; /** @todo camera can follow this Unit. Only one-per-team */
} Unit;


typedef enum {
    PS_SELECTING,
    PS_MOVE,
    PS_ATK,
    PS_MOVE_ALL,
    PS_FOLLOWING,
    PS_DONE,
    
    PS_SIZE,
} PlayerState;

void unit_init(Unit *u, unit_type_t type);
void unit_deinit(Unit *u);
void unit_draw(Unit *u);
void unit_update(Unit *u);
bool unit_move(Unit *u, direction_t dir);
void unit_move_to(Unit *u, uint tx, uint ty);
void unit_hide(Unit *u);
void unit_show(Unit *u);

int unit_px(uint tx);
int unit_py(uint ty);

bool unit_canpass(Unit *u, int dx, int dy);

void plr_init(Unit *p, WIN_REGULAR *win);
void plr_update();
void plr_move();
void plr_switch(Unit *u);
void plr_start_turn();
void plr_hud();
void ps_set(PlayerState);
PlayerState ps_get();

#endif