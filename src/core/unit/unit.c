#include "unit.h"

#include <stdlib.h>
#include "../../lib/bg.h"

static void internal_swap_tile(Unit *u);
static bool internal_off_screen(Unit *u);

static uint internal_anim_frames = 0;

const u16 internal_unit_tiles[] = {
    [UNIT_HERO]=1,
    [UNIT_SKELETON]=1,
};


void unit_init(Unit *u, unit_type_t type) {
    u->tile = u->anim_tile = internal_unit_tiles[type];

    u->obj = spr_alloc(16, 16, u->tile);
    u->is_moving = false;

    u->tx = u->ty = 1;
    u->dx = u->dy = 0;

    spr_set_size(u->obj, SPR_SIZE_16x16);
    unit_draw(u);
}


void unit_deinit(Unit *u) {
    spr_free(u->obj);
}


void unit_draw(Unit *u) {
    const uint tx = u->tx, ty = u->ty;
    spr_move(u->obj, (tx << 4) - map_get_px() + u->dx, (ty << 4) - map_get_py() + u->dy);
}


inline void unit_hide(Unit *u) {
    spr_hide(u->obj);
}


inline void unit_show(Unit *u) {
    spr_show(u->obj);
}


/**
 * Called every frame
 */
void unit_update(Unit *u) {
    if((internal_anim_frames & 0x7f) == 0) {
        internal_swap_tile(u); // animate
    }

    if(!u->is_moving) {
        unit_draw(u);
        return;
    }

    const int dx = dir_get_x(u->dir), dy = dir_get_y(u->dir);

    if(u->is_moving && u->has_focus && !map_moving()) {
        u->tx += dx;
        u->ty += dy;
        u->is_moving = false;
        unit_draw(u);
    }
    
    u->dx += dx;
    u->dy += dy;

    if(abs(u->dx) == 16 || abs(u->dy) == 16) {
        u->tx += dx;
        u->ty += dy;
        u->is_moving = false;
        u->dx = u->dy = 0;
    }

    unit_draw(u);
}


bool unit_move(Unit *u, direction_t dir) {
    if(!u->has_focus && internal_off_screen(u))
        unit_hide(u);
    else
        unit_show(u);

    if(u->is_moving || map_get_map()->is_moving)
        return false;

    if(!unit_canpass(u, dir_get_x(dir), dir_get_y(dir)))
        return false;

    u->dir = dir;

    if(u->has_focus) {
        map_scroll(dir, MAP_SCROLL_SLOW);
    }

    u->is_moving = true;
    return true;
}


/**
 * Sets the unit position to (tx, ty)
 */
void unit_move_to(Unit *u, uint tx, uint ty) {
    u->tx = tx;
    u->ty = ty;
    unit_draw(u);
}


/**
 * @returns true if the unit is out of the viewport
 */
static bool internal_off_screen(Unit *u) {
    const int map_tx = map_get_tx();
    const int map_ty = map_get_ty();
    const int tx = u->tx, ty = u->ty;
    return tx < map_tx || ty < map_ty || tx > map_tx + 15 || ty > map_ty + 10;
}

/**
 * @param dx position offset
 * @param dy position offset
 */
bool unit_canpass(Unit *u, int dx, int dy) {
    const TileData *d = map_get_data(map_get(u->tx + dx, u->ty + dy));

    if(d->canpass == NULL)
        return true;
    
    return d->canpass((void*)u);
}


inline bool internal_in_range(Unit *u) {
    uint mx = map_get_tx(), my = map_get_ty();

    return (u->tx >= mx && u->tx < mx + MAP_TILES_X) &&
     (u->ty >= my && u->ty < my + MAP_TILES_Y);
}


static void internal_swap_tile(Unit *u) {
    const uint16_t tile = u->tile;
    const uint16_t anim = u->anim_tile;
    u->tile = anim;
    u->anim_tile = tile;

    spr_set_tile(u->obj, u->tile);
}