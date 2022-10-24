#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_sprites.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "lib/defines.h"
#include "main.h"
#include "lib/obj.h"
#include "lib/bg.h"
#include "lib/window.h"
#include "lib/keypad.h"
#include "lib/random.h"
#include "lib/sound.h"
#include "lib/DMA.h"
#include "lib/mem.h"

#include "gfx/tiles16.h"
#include "gfx/character_idle.h"
#include "gfx/tileset.h"
#include "text.h"

#include "core/GameState.h"


extern const char FLASH_ID_TEXT[];
extern void foo(const char *); // @todo remove the need for this

int main(void) {
	REG_DISPCNT = 1; //  mode 1: bg0=reg bg1=reg bg2=aff bg3=none

	// copy sprite data and palette
	memcpy16((u16*)sprite_palette_mem, (u16*)character_idlePal, character_idlePalLen);
	memcpy16((u16*)sprite_palette_mem + 16, (u16*)character_idlePal, character_idlePalLen);
	memcpy16((u16*)sprite_palette_mem + 32, (u16*)character_idlePal, character_idlePalLen);
	memcpy16((u16*)background_palette_mem, (u16*)tilesetPal, tilesetPalLen);

	// copy sprite to tile 1 in tileblock 4
	bg_load_tiles(4, 1, character_idleTiles, character_idleTilesLen, false);

	// copy over tileset
	bg_load_tiles(0, 1, tilesetTiles, tilesetTilesLen, true); // load 8bpp tiles
	bg_load_tiles(1, 0, tiles16Tiles, tiles16TilesLen, false); // load 4bpp tiles

	// set first tile to be non-transparent black
	memset16((u16*)&tile_mem[0][0], 0, 64);
	memset16((u16*)&tile_mem[0][512], 0x0000, 32); // this tile is 4bpp
	background_palette_mem[0xC] = RGB15(0, 0, 0); // set window bg color to black
	background_palette_mem[0x0] = RGB15(0, 0, 0); // set window bg color to black
	background_palette_mem[4 + 16] = RGB15(31, 0, 0);
	background_palette_mem[4 + 32] = RGB15(0, 20, 31);

	spr_init(); // initialize sprites

	// enable interrupts
	irqInit();
	irqEnable(IRQ_VBLANK);

	foo(FLASH_ID_TEXT); // @todo remove this unnecessary function

	state_init(&gme_main_state, true);

	do {
		state_update();
	} while(true);

	memset16((u16*)oam_mem, 0, 128 << 2);
}
