#ifndef ITEM_H
#define ITEM_H

#include "../../lib/defines.h"

#define INV_MAX_SIZE 4

typedef enum {
    ITEM_HEAL_POTION,
    ITEM_DEF_POTION,
    ITEM_ATK_POTION,
} ItemId;

typedef struct {
    u8 id;
    u8 cnt;
} Item;


typedef struct {
    uint size;
    Item items[INV_MAX_SIZE];
} Inventory;

Item *itm_get(ItemId id);
void itm_draw(Item *itm, uint x, uint y);


void inv_init(Inventory *inv);
bool inv_add(Inventory *inv, Item *item);
bool inv_remove(Inventory *inv, ItemId id);

#endif