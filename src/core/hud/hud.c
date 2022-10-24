#include "hud.h"
#include "../../text.h"
#include "../unit/unit.h"
#include "../../lib/keypad.h"
#include <gba_systemcalls.h>


void hud_hp(uint tx, uint ty, s8 hp, s8 max_hp) {
    text_printf("%c%d/%d", tx, ty, 544, hp, max_hp);
}


void hud_rect(uint x, uint y, uint w, uint h) {
    const BG_REGULAR *bg = text_get_bg();
    w--, h--;

    bg_fill(bg, x, y, w, h, 0); // clear
    bg_fill(bg, x, y, w, 1, 584); // top
    bg_fill(bg, x, y, 1, h, 586); // left
    bg_fill(bg, x, y + h, w, 1, 589); // bottom
    bg_fill(bg, x + w, y, 1, h, 586); // right

    // four corners
    bg_write_tile(bg, x, y, 583);
    bg_write_tile(bg, x, y + h, 587);
    bg_write_tile(bg, x + w, y, 585);
    bg_write_tile(bg, x + w, y + h, 588);
}


static const uint x = 4, y = 4;

static void _draw_inv(Inventory *inv) {
    // draw inventory
    hud_rect(x + 12, y + 4, 13, 10);
    text_print("INVENTORY", x + 14, y + 4);

    for(uint i = 0; i < inv->size; i++) {
        itm_draw(&inv->items[i], x + 14, y + 5 + i);
    }

}


void hud_stat_menu(Stats *s, Inventory *inv) {

    bg_fill(text_get_bg(), 0, 0, 30, 20, 0);

    hud_rect(x - 1, y - 1, 28 - x, 18 - y);

    // text_set_pal(2);
    text_printf("%s, LVL:%d", x, y, unit_name(s->type), s->level);
    // text_set_pal(0);
    hud_hp(x, y + 1, s->hp, s->max_hp);
    text_printf("%c%d ATK", x + 1, y + 2, 579, s->atk);
    text_printf("%c%d DEF", x + 1, y + 3, 581, s->def);
    text_printf("EXP:%d", x + 1, y + 8, s->exp);
    text_printf("%d TO NEXT", x + 2, y + 9, stat_required_exp(s->level + 1) - s->exp);

    _draw_inv(inv);
    // cursor

    uint tile = unit_get_tile(s->type);
    uint key;
    obj_t *obj = spr_alloc(24 << 3, 4 << 3, tile);
    bool cur_shwn = true;
    spr_set_size(obj, SPR_SIZE_16x16);

    bg_hide(2);
    unit_hide_all();
    
    do {
        key_scan();
        key = key_pressed();
        unit_inc_anim_frames();

        if(key & KEY_A) {
            // unit_use_item(unit, help);
        }

        if(unit_anim_frames() == 0) { 
            spr_set_tile(obj, spr_get_tile(obj) == tile ? tile + 16 : tile);

            cur_shwn = !cur_shwn;
        }

        text_write_tile(cur_shwn ? 523 : 0, x + 13, y + 5);
        spr_copy_all();
        VBlankIntrWait();
    } while(key != KEY_B);

    spr_free(obj);
    bg_show(2);
}