#include "unit.h"
#include "../hud/hud.h"
#include "../../text.h"

#include <stdlib.h>


static uint cx, cy; // unit's tx and ty position
static obj_t *cobj; // cursor's sprite pointer
static int dx, dy;
static obj_t *_atk_spr[49];


static bool cur_can_move(direction_t dir) {
    const int xx = dir_get_x(dir);
    const int yy = dir_get_y(dir);

    if(abs(dx + xx) > 3 || abs(dy + yy) > 3)
        return false;

    return true;
}


/**
 * Attacks for a player at the cursor's current position
 * @returns false if player cannot attack at that position
 */
atk_error_t cur_do_attack(Unit *plr) {
    if(!atk_get(plr->stats.attack_pattern, dx, dy))
        return ATK_FAIL_OUT_OF_RANGE;

    return unit_attack(plr, plr->tx + dx, plr->ty + dy);
}


void cur_draw() {
    const uint x = (cx - map_get_tx() + dx) << 4;
    const uint y = (cy - map_get_ty() + dy) << 4;

    // animate cursor
    if((unit_anim_frames() & 31) < 10) {
        spr_hide(cobj);
    } else
        spr_show(cobj);

    spr_move(cobj, x, y);
}


void cur_init(Unit *u) {
    uint i = 0;

    cobj = spr_alloc(0, 0, 37);
    spr_set_size(cobj, SPR_SIZE_16x16);
    cx = u->tx;
    cy = u->ty;
    dx = dy = 0;
    
    for(int y = -3; y < 3; y++) {
        for(int x = -3; x < 3; x++) {

            if(atk_get(u->stats.attack_pattern, x, y) && !map_is_solid(u->tx + x, u->ty + y)) {
                _atk_spr[i] = spr_alloc((u->tx - map_get_tx() + x) << 4, (u->ty - map_get_ty() + y) << 4, 33);
                spr_set_size(_atk_spr[i], SPR_SIZE_16x16);
                spr_set_priority(_atk_spr[i], SPR_PRIORITY_HIGH);
            } else {
                _atk_spr[i] = NULL;
            }

            i++;
        }
    }
}


void cur_deinit() {
    spr_free(cobj);

    for(uint i = 0; i < LENGTH(_atk_spr); i++) {
        if(_atk_spr[i])
            spr_free(_atk_spr[i]);
    }

    bar_hide();
}


void cur_move(direction_t dir) {
    const int xx = dir_get_x(dir);
    const int yy = dir_get_y(dir);
    if(!cur_can_move(dir))
        return;
    
    dx += xx;
    dy += yy;

    Unit *u = unit_at(cx + dx, cy + dy);

    if(!u) {
        bar_hide();
        return;
    }

    if(u->is_ai)
        text_set_pal(1);
    else
        text_set_pal(2);
    
    bar_stat(&u->stats);
    text_set_pal(0);
}