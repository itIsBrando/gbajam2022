#include "map.h"
#include "tiles/tile.h"
#include "../../lib/bg.h"

#include <stdlib.h>
#include <string.h>


Map map;

const TileData internal_tile_data[] = {
    [TILE_NONE] = {
        .tiles={67, 68, 99, 100},
        .solid=false,
        .ondraw=&tile_draw_none,
    },
    [TILE_WALL] = {
        .solid=true,
        .canpass=&tile_wall_canpass,
        .ondraw=&tile_wall_draw,
    },
    [TILE_VOID] = {
        .tiles={97, 97, 97, 97},
        .solid=false,
        .ondraw=NULL,
    },
    [TILE_DOOR] = {
        .tiles={69, 70, 101, 102},
        .solid=false,
        .ondraw=NULL,
        // .canpass=tile_door_canpass
    },
    [TILE_STAIR] = {
        .tiles={3, 4, 35, 36},
        .solid=false,
        .oninteract=&tile_stair_interact,
    }
};


/**
 * Initializes a blank map
 */
void map_init(BG_REGULAR *bg) {
    map.bg = bg;
    map.px = map.py = 0;
    map.to_px = map.to_py = -1;

    bg_set_size(bg, BG_SIZE_AFF_128x128);
    bg_set_priority(bg, BG_PRIORITY_HIGHEST);

    memset(map.data, TILE_WALL, sizeof(map.data) / sizeof(map.data[0]));
}


void map_deinit() {

}


void map_draw() {
    tile_t *d = map.data;

    for(uint y = 0; y < MAP_TILE_HEIGHT; y++) {
        for(uint x = 0; x < MAP_TILE_WIDTH; x++) {
            const TileData *tile_data = map_get_data(*d++);

            if(tile_data->ondraw)
                // tile_wall_draw(x, y);
                tile_data->ondraw(x, y);
            else
                bg_rect(map.bg, x << 1, y << 1, 2, 2, (u8*)tile_data->tiles);
        }
    }
}


void map_update() {
    if(!map.is_moving)
        return;
    
    map.px += map.dx << map.speed;
    map.py += map.dy << map.speed;

    if(map.px == map.to_px) {
        map.dx = 0;
    }

    if(map.py == map.to_py) {
        map.dy = 0;
    }
    
    if(map.dx == 0 && map.dy == 0)
        map.is_moving = false;
    
    bg_move(map.bg, map.px, map.py);
}


void map_scroll(direction_t dir) {
    if(map.is_moving)
        return;

    map.dx = dir_get_x(dir);
    map.dy = dir_get_y(dir);
    map.is_moving = true;
    map.to_px += (map.dx << 4);
    map.to_py += (map.dy << 4);
}


void map_scroll_to(uint tx, uint ty) {
    if(map.is_moving)
        return;

    int px = ((int)tx - 7) << 4, py = ((int)ty - 4) << 4;

    map.dx = -dir_get_x(dir_to_x(px, map.px));
    map.dy = -dir_get_y(dir_to_y(py, map.py));
    
    map.is_moving = true;
    map.to_px = px, map.to_py = py;
}


/**
 * @param tx [0, 64)
 * @param ty [0, 64)
 */
void map_set(uint x, uint y, tile_t t) {
    if(map_inbounds(x, y))
        *map_get_pointer(x, y) = t;
}


void map_redraw(uint tx, uint ty) {
    const TileData *d = map_get_data(map_get(tx, ty));

    if(d->ondraw)
        d->ondraw(tx, ty);
    else
        bg_rect(map.bg, tx << 1, ty << 1, 2, 2, (void*)d->tiles);
}


/**
 * @param tx [0, 64)
 * @param ty [0, 64)
 */
tile_t map_get(uint tx, uint ty) {

    if(tx > MAP_TILE_WIDTH || ty > MAP_TILE_HEIGHT)
        return TILE_WALL;
    
    const tile_t t = map.data[tx + (ty << MAP_BITS_WIDTH)];

    return t;
}


inline tile_t *map_get_pointer(uint tx, uint ty) {
    return &map.data[tx + (ty << MAP_BITS_WIDTH)];
}


inline bool map_is_solid(uint tx, uint ty) {
    return map_get_data(map_get(tx, ty))->solid;
}

inline int map_get_px() {
    return map.px;
}

inline int map_get_py() {
    return map.py;
}

int map_get_tx() {
    return map_get_px() >> 4;
}

int map_get_ty() {
    return map_get_py() >> 4;
}

inline bool map_moving() {
    return map.is_moving;
}

inline Map *map_get_map() {
    return &map;
}

inline void map_set_scroll_speed(scroll_speed_t s) {
    if(!map_moving())
        map.speed = s;
}


inline scroll_speed_t map_scroll_speed() {
    return map.speed;
}


const inline TileData *map_get_data(tile_t tile) {
    return &internal_tile_data[tile];
}