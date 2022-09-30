#ifndef UNIT_H
#define UNIT_H

#include <stdio.h>
#include "../map/map.h"
#include "../../lib/obj.h"

#define UNIT_MAX_LVL 40

typedef enum {
    UNIT_HERO,
    UNIT_MAGE,

    UNIT_TYPES
} unit_type_t;


typedef enum {
    STAT_ATK_MELEE, // enemies direction in front or to the side
    STAT_ATK_MAGE, // 3 tiles in a straight line
    STAT_ATK_ARCH, // 2x2 diamond
} attack_type_t;


typedef struct {
    s8 max_hp;
    s8 hp;
    u8 atk;
    u8 def;
    u8 move_points;
    u8 level;
    attack_type_t attack_pattern;
} Stats;


typedef struct {
    uint tx, ty; // absolute tile position
    int dx, dy; // (x, y) pixel offsets
    u16 dead; // 0 is alive, otherwise this is frames until removal
    obj_t *obj;
    unit_type_t type;
    uint16_t tile;
    uint16_t anim_tile;
    direction_t dir;
    Stats stats;

    bool is_ai;
    bool is_moving;
    bool has_focus;

    void (*ondeinit)(void *); /** Unit pointer is parameter that is passed as void :'( */
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

void unit_init(Unit *u, unit_type_t type, void (*ondeinit)());
void unit_deinit(Unit *u);
void unit_draw(Unit *u);
void unit_update(Unit *u);
void unit_update_all();
bool unit_move(Unit *u, direction_t dir);
void unit_move_to(Unit *u, uint tx, uint ty);
void unit_hide(Unit *u);
void unit_show(Unit *u);

void unit_kill(Unit *u);
bool unit_is_dead(Unit *u);
bool unit_attack(Unit *atker, uint tx, uint ty);

int unit_px(uint tx);
int unit_py(uint ty);

bool unit_canpass(Unit *u, int dx, int dy);
Unit *unit_at(uint tx, uint ty); /** Gets the Unit at (tx, ty) or NULL */

uint unit_anim_frames();
void unit_inc_anim_frames();

void cur_draw();
void cur_init(Unit *u);
void cur_deinit();
void cur_move(direction_t dir);
bool cur_do_attack(Unit *plr);
bool atk_get(attack_type_t p, int dx, int dy);


void plr_init(Unit *p, WIN_REGULAR *win);
void plr_update();
void plr_move();
void plr_switch(Unit *u);
void plr_start_turn();
void plr_hud();
void ps_set(PlayerState);
PlayerState ps_get();

void mob_init(Unit *u, unit_type_t type, uint tx, uint ty);
bool mob_spawn();
void mob_update();
void mob_do_turns();

#endif