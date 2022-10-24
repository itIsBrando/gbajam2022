#ifndef HUD_H
#define HUD_H

#include "../../lib/defines.h"
#include "../unit/stats.h"
#include "../item/item.h"

typedef enum {
    FMT_LEFT_JUSTIFY,
    FMT_RIGHT_JUSTIFY,
    FMT_CENTER_JUSTIFY,
} format_t;

void bar_set_timeout(s16 frames);
void bar_hide();
void bar_show();
void bar_update();
void bar_string(char *str, format_t f);
void bar_num(u16 num, format_t f);
void bar_stat(Stats *s);
void bar_wait_for_key();

void hud_rect(uint x, uint y, uint w, uint h);
void hud_hp(uint tx, uint ty, s8 hp, s8 max_hp);
void hud_stat_menu(Stats *s, Inventory *inv);

#endif