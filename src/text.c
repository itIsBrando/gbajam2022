#include "text.h"
#include <gba_systemcalls.h>
#include <string.h>

static BG_REGULAR *target_background;
static u16 tile_offset;
static u16 pal_mask = 0;


void text_set_pal(u8 pal_num)
{
    pal_mask = (pal_num & 0xF) << 0xC;
}

BG_REGULAR *text_get_bg() {
    return target_background;
}

inline void text_write_tile(u16 tile, const u16 x, const u16 y)
{
    bg_write_tile(target_background, x, y, tile | pal_mask);
}


inline void text_char(char character, const u16 x, const u16 y)
{
    u16 c = character == ' ' ? 0 : (u16)character + tile_offset - '+';
    text_write_tile(c, x, y);
}


void text_print(char *string, u16 x, u16 y)
{
    while(*string)
        text_char(*string++, x++, y);

}


static uint _get_digits(uint num) {
    u16 n = num, digits = 1;
    
    while(n > 9) {
        n /= 10;
        digits++;
    }

    return digits;
}


void text_printf(char *str, u16 x, u16 y, ...) {
    va_list vl;
    uint i = 0;
    va_start(vl, y);

    while(str[i]) {
        const char c = str[i++];

        if(c == '%') {
            switch(str[i++]) {
                case 'c':       // char (actually 16-bit cuz tiles be like dat)
                    const u16 c = (u16)va_arg(vl, int);
                    text_write_tile(c, x++, y);
                    break;
                case 'd':       // signed ints
                case 'i':       // signed ints
                    const s16 s = (s16)va_arg(vl, int);
                    x += _get_digits(s) + (s < 0);
                    text_int(s, x - 1, y);
                    break;
                case 'u': // unsigned ints
                    const uint num = (uint)va_arg(vl, int);
                    x  += _get_digits(s);
                    text_uint(num, x - 1, y);
                    break;
                case 's':       // strings
                    char *str = (char*)va_arg(vl, int);
                    text_print(str, x, y);
                    x += strlen(str);
                    break;
                case 'h':       // @todo
                case 'o':       // @todo
                default:
                    break;
            }
        } else {
            text_char(c, x++, y);
        }
    }

    va_end(vl);
}


void text_uint(u16 num, uint x, uint y)
{
    char buffer[6];
    u16 i = 4;
    do {
        buffer[i--] = (num % 10) + '0';
        num /= 10;
    } while(num);

    buffer[5] = 0;
    text_print(buffer + 1 + i, x - (4 - (i + 1)), y);
}


void text_int(s16 num, u16 x, u16 y)
{
    if(num < 0)
        text_char('-', x++, y);

    text_uint(num < 0 ? -num : num, x, y);
}


void text_init(BG_REGULAR *bg, u16 startTile)
{
    target_background = bg;
    tile_offset = startTile;
}


void text_error(char *string)
{
    bg_fill(target_background, 0, 0, 240/8, 1, 0);
    text_print(string, 0, 0);

    for (uint i = 0; i < 68; i++)
        VBlankIntrWait();
    
}