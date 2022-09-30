#ifndef MAP_H
#define MAP_H

#include "../../lib/bg.h"
#include "../GameState.h"
#include <stdint.h>



#define MAP_BITS_WIDTH  5 // 32 x 32
#define MAP_BITS_HEIGHT 5 // 32 x 32
#define MAP_TILE_WIDTH  (1 << MAP_BITS_WIDTH)
#define MAP_TILE_HEIGHT (1 << MAP_BITS_HEIGHT)
#define MAP_SIZE        (MAP_TILE_WIDTH * MAP_TILE_HEIGHT)
#define MAP_TILES_X    20
#define MAP_TILES_Y    14

typedef u8 tile_t;

enum {
    TILE_NONE,
    TILE_WALL,
    TILE_VOID,
    TILE_DOOR,
    TILE_STAIR,
};


typedef struct {
    u16 x, y;
} Position;



typedef enum {
    MAP_SCROLL_SLOW    = 0, // 16 frames per tile
    MAP_SCROLL_MED     = 1, // 8 frames per tile
    MAP_SCROLL_FAST    = 2, // 4 frames per tile
    MAP_SCROLL_INSTANT = 4, // 2 frame per tile
} scroll_speed_t;


typedef struct {
    const u8 tiles[4]; // 16x16px tile
    bool solid;        // fallback for when 'canpass' is not used
    bool (*canpass)(void *);
    void (*oninteract)(void *, uint, uint);
    void (*ondraw)(uint, uint);
} TileData;


typedef struct {
    tile_t data[MAP_TILE_WIDTH * MAP_TILE_HEIGHT];
    BG_REGULAR *bg;
    int px, py; // pixel scroll offsets
    int to_px, to_py; // set to move to a specific pixel coordinate
    s16 dx, dy;
    u16 level;

    bool is_moving;
    scroll_speed_t speed;
} Map;


tile_t map_get(uint tx, uint ty);
void map_set(uint tx, uint ty, tile_t t);
const TileData *map_get_data(tile_t tile);
tile_t *map_get_pointer(uint tx, uint ty);


void map_init(BG_REGULAR *bg);
void map_deinit();
void map_draw();
void map_redraw(uint tx, uint ty);
void map_update();

void map_scroll(direction_t dir);
void map_scroll_to(uint tx, uint ty);

Map *map_get_map();
int map_get_px();
int map_get_py();
int map_get_tx();
int map_get_ty();
bool map_moving();
void map_set_scroll_speed(scroll_speed_t s);
scroll_speed_t map_scroll_speed();
bool map_is_solid(uint tx, uint ty);
bool map_inbounds(int tx, int ty);

#endif