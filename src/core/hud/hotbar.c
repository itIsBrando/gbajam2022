#include "hud.h"
#include "../../lib/bg.h"
#include "../../lib/keypad.h"
#include "../../text.h"

#include <gba_systemcalls.h>
#include <string.h>

static s16 _shown = 0;

static void bar_border() {
    hud_rect(0, 17, 30, 3);
    _shown = -1;
}


void bar_set_timeout(s16 frames) {
    _shown = frames;
}


static void bar_clear() {
    bg_fill(text_get_bg(), 0, 17, 30, 20, 0);
}


void bar_hide() {
    bar_clear();
    _shown = 0;
}


inline void bar_show() {
    bar_border();
}


void bar_update() {
    if(_shown == -1)
        return;
    
    if(_shown) {
        _shown--;
    } else {
        bar_hide();
    }
}


static uint _get_x(uint len, format_t f) {
    switch (f) {
        case FMT_CENTER_JUSTIFY:
            return 15 - len / 2;
        case FMT_RIGHT_JUSTIFY:
            return 29 - len;
        case FMT_LEFT_JUSTIFY:
        default:
            return 1;
    }
}


void bar_string(char *str, format_t f) {
    const uint x = _get_x(strlen(str), f);

    bar_border();

    text_print(str, x, 18);
}


void bar_num(u16 num, format_t f) {
    const uint x = _get_x(3, f);

    bar_border();
    text_uint(num, x, 18);
}


void bar_stat(Stats *s) {
    bar_border();
    hud_hp(1, 18, s->hp, s->max_hp);
    text_printf("%c%d %c%d ", 10, 18, 579, s->atk, 581, s->def);
}


void bar_wait_for_key() {
    uint blnk = 0;

    do {
        key_scan();
        VBlankIntrWait();
    } while(key_pressed());

    do {
        key_scan();
        // @todo call `unit_update_all` this may require us to turn this into a GameState
        if((blnk++ & 0x3f) < 0x2f) {
            text_write_tile(524, 28, 18);
        } else {
            text_write_tile(0, 28, 18);
        }

        VBlankIntrWait();
    } while(key_pressed() != KEY_A);
}