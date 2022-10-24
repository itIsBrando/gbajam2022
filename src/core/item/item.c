#include "item.h"
#include "../../text.h"

Item _itm;

Item *itm_get(ItemId id) {
    _itm.cnt = 1;
    _itm.id = id;
    return &_itm;
}

void inv_init(Inventory *inv) {
    inv->size = 0;
}


bool inv_add(Inventory *inv, Item *item) {
    if(inv->size >= INV_MAX_SIZE) {
        return false;
    }

    inv->items[inv->size++] = *item;

    return true;
}


bool inv_contains(Inventory *inv, ItemId id, uint cnt) {
    Item *itm = inv->items;
    for(uint i = 0; i < inv->size; i++) {
        if(itm->id == id && itm->cnt >= cnt) {
            return true;
        }

        itm++;
    }

    return false;
}


bool inv_remove(Inventory *inv, ItemId id) {
    Item *itm = inv->items;

    for(uint i = 0; i < inv->size; i++) {
        if(itm->id == id) {
            *itm = inv->items[--inv->size];
            return true;
        }
    }

    return false;
}


void itm_draw(Item *itm, uint x, uint y) {
    const u16 tiles[] = {
        578, 578, 578
    };

    const char *names[8] = {
        "HP POT",
        "ATK POT",
        "DEF POT"
    };

    text_printf("%c %s %u", x, y, tiles[itm->id], names[itm->id], itm->cnt);
}