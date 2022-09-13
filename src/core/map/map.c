#include "map.h"
#include "tiles/tile.h"
#include "../../lib/bg.h"

#include <stdlib.h>
#include <string.h>


Map map;

const TileData internal_tile_data[] = {
    [TILE_NONE] = {
        .tiles={66, 66, 66, 65},
        .solid=false,
    },
    [TILE_WALL] = {
        .tiles={1, 2, 33, 34},
        .solid=true,
        .canpass=tile_wall_canpass,
    },
    [TILE_VOID] = {
        .tiles={97, 97, 97, 97},
        .solid=false,
    },
    [TILE_DOOR] = {
        .tiles={71, 72, 103, 104},
        .solid=false,
        // .canpass=tile_door_canpass
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
    
    map_draw();
}


void map_deinit() {

}


void map_draw() {
    tile_t *d = map.data;

    for(uint y = 0; y < MAP_TILE_HEIGHT; y++) {
        for(uint x = 0; x < MAP_TILE_WIDTH; x++) {
            const TileData *tile_data = map_get_data(*d++);
            
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


void map_scroll(direction_t dir, scroll_speed_t spd) {
    if(map.is_moving)
        return;

    map.dx = dir_get_x(dir);
    map.dy = dir_get_y(dir);
    map.speed = spd;
    map.is_moving = true;
    map.to_px += (map.dx << 4);
    map.to_py += (map.dy << 4);
}


void map_scroll_to(uint tx, uint ty, scroll_speed_t spd) {
    if(map.is_moving)
        return;

    int px = ((int)tx - 6) << 4, py = ((int)ty - 4) << 4;

    map.dx = -dir_get_x(dir_to_x(px, map.px));
    map.dy = -dir_get_y(dir_to_y(py, map.py));
    
    map.speed = spd;
    map.is_moving = true;
    map.to_px = px, map.to_py = py;
}


/**
 * @param tx [0, 64)
 * @param ty [0, 64)
 */
void map_set(uint x, uint y, tile_t t) {
    *map_get_pointer(x, y) = t;
}

void map_redraw(uint tx, uint ty) {
    const TileData *d = map_get_data(map_get(tx, ty));

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
    return map.px >> 4;
}

int map_get_ty() {
    return map.py >> 4;
}

inline bool map_moving() {
    return map.is_moving;
}

Map *map_get_map() {
    return &map;
}

const inline TileData *map_get_data(tile_t tile) {
    return &internal_tile_data[tile];
}