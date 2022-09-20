#include "unit.h"

#include <stdlib.h>
#include "../../lib/bg.h"

static void internal_swap_tile(Unit *u);
static bool internal_off_screen(Unit *u);

uint internal_anim_frames = 0;

const u16 internal_unit_tiles[] = {
    [UNIT_HERO]=1,
    [UNIT_SKELETON]=5,
};


static uint _units_size = 0;
static Unit *_all_units[30];


void unit_init(Unit *u, unit_type_t type) {
    u->tile = internal_unit_tiles[type];
    u->anim_tile = u->tile + 16;
    u->obj = spr_alloc(16, 16, u->tile);
    u->is_moving = false;

    u->tx = u->ty = 1;
    u->dx = u->dy = 0;

    _all_units[_units_size++] = u;

    spr_set_size(u->obj, SPR_SIZE_16x16);
    spr_set_priority(u->obj, SPR_PRIORITY_LOW);
    unit_draw(u);
}


void unit_deinit(Unit *u) {
    spr_free(u->obj);
    // @todo REMOVE UNIT FROM `_all_units`
}


void unit_draw(Unit *u) {
    if(internal_off_screen(u)) {
        unit_hide(u);
    } else {
        unit_show(u);
        spr_move(u->obj, (u->tx << 4) - map_get_px() + u->dx, (u->ty << 4) - map_get_py() + u->dy);
    }
}


inline void unit_hide(Unit *u) {
    spr_hide(u->obj);
}


inline void unit_show(Unit *u) {
    spr_show(u->obj);
}


inline uint unit_anim_frames() {
    return internal_anim_frames;
}

inline void unit_inc_anim_frames() {
    internal_anim_frames++;
    internal_anim_frames &= 0xF;
}


/**
 * Called every frame
 */
void unit_update(Unit *u) {
    if((unit_anim_frames()) == 0) {
        internal_swap_tile(u); // animate
    }

    if(!u->is_moving) {
        unit_draw(u);
        return;
    }

    const int dx = dir_get_x(u->dir), dy = dir_get_y(u->dir);

    if(u->is_moving && u->has_focus) {
        if(map_moving())
            return;
        
        u->tx += dx;
        u->ty += dy;
        u->dx = u->dy = 0;
        u->is_moving = false;
        unit_draw(u);
        return;
    }
    
    u->dx += dx << map_get_map()->speed;
    u->dy += dy << map_get_map()->speed;

    if(abs(u->dx) >= 16 || abs(u->dy) >= 16) {
        u->tx += dx;
        u->ty += dy;
        u->is_moving = false;
        u->dx = u->dy = 0;
    }

    unit_draw(u);
}


bool unit_move(Unit *u, direction_t dir) {
    Unit *other_unit;

    if(u->is_moving || map_moving())
        return false;

    other_unit = unit_at(u->tx + dir_get_x(dir), u->ty + dir_get_y(dir));

    if(other_unit != NULL && other_unit->is_ai == u->is_ai) {
        unit_move_to(other_unit, u->tx, u->ty); // swap positions
    } else if(!unit_canpass(u, dir_get_x(dir), dir_get_y(dir))) {
        return false;
    }

    u->dir = dir;

    if(u->has_focus) {
        map_scroll(dir);
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
static inline bool internal_off_screen(Unit *u) {
    const int map_tx = map_get_tx();
    const int map_ty = map_get_ty();
    const int tx = u->tx, ty = u->ty;
    return tx < map_tx || ty < map_ty || tx > map_tx + 15 || ty > map_ty + 10;
}


Unit *unit_at(uint tx, uint ty) {
    Unit **units = _all_units;

    for(uint i = 0; i < _units_size; i++) {
        Unit *u = *units;

        if(u->tx == tx && u->ty == ty) {
            return u;
        }

        units++;
    }

    return NULL;
}


/**
 * @param dx position offset
 * @param dy position offset
 */
bool unit_canpass(Unit *u, int dx, int dy) {
    dx += u->tx, dy += u->ty;
    const TileData *d = map_get_data(map_get(dx, dy));

    if(d->oninteract)
        d->oninteract(u, dx, dy);
    
    if(d->canpass && !d->canpass(u))
        return false;

    return unit_at(dx, dy) == NULL;
}


static void internal_swap_tile(Unit *u) {
    const uint16_t tile = u->tile;
    const uint16_t anim = u->anim_tile;
    u->tile = anim;
    u->anim_tile = tile;

    spr_set_tile(u->obj, u->tile);
}