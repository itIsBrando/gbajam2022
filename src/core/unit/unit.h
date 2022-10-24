#ifndef UNIT_H
#define UNIT_H

#include <stdio.h>
#include "../map/map.h"
#include "../../lib/obj.h"
#include "../item/item.h"
#include "stats.h"

#define UNIT_MAX_LVL 40

typedef enum unit_type_t {
    UNIT_HERO,
    UNIT_MAGE,
    UNIT_ORGE,
    
    UNIT_TYPES
} unit_type_t;


typedef enum {
    ATK_FAIL_EMPTY,
    ATK_FAIL_FRIENDLY,
    ATK_FAIL_OUT_OF_RANGE,
    ATK_FAIL_SELF,
    ATK_SUCCESS,
} atk_error_t;



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
    Inventory inv;

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
const char *unit_name(unit_type_t type);
void unit_deinit(Unit *u);
void unit_draw(Unit *u);
void unit_update(Unit *u);
void unit_update_all();
void unit_hide_all();
bool unit_move(Unit *u, direction_t dir);
void unit_move_to(Unit *u, uint tx, uint ty);
void unit_hide(Unit *u);
void unit_show(Unit *u);
void unit_use_item(Unit *u, Item *itm);
u16 unit_get_tile(unit_type_t type);

void unit_heal(Unit *u, uint hp);
void unit_kill(Unit *u);
bool unit_is_dead(Unit *u);
atk_error_t unit_attack(Unit *atker, uint tx, uint ty);

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
atk_error_t cur_do_attack(Unit *plr);
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
void mob_remove_all();
void mob_update();
void mob_do_turns();

#endif