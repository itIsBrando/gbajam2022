#include "stats.h"
#include <assert.h>


typedef struct {
    u8 base_hp;
    u8 base_atk;
    u8 base_def;
    attack_type_t atktype;
} BaseStats;


const BaseStats unit_base_stats[] = {
    [UNIT_HERO] = {
        .atktype  = STAT_ATK_MELEE,
        .base_hp  = 35,
        .base_atk = 35,
        .base_def = 28,
    },
    [UNIT_MAGE] = {
        .atktype  = STAT_ATK_MAGE,
        .base_hp  = 14,
        .base_atk = 28,
        .base_def = 25,
    },
};


static_assert(LENGTH(unit_base_stats) == UNIT_TYPES, "`unit_base_stats` not filled with All unit types in stats.c");


void stat_fill(Unit *u, uint lvl) {
    const BaseStats *bs = &unit_base_stats[u->type];

    if(lvl > UNIT_MAX_LVL)
        lvl = UNIT_MAX_LVL;
    
    u->stats.attack_pattern = bs->atktype;
    u->stats.atk = stat_scale(lvl, bs->base_atk);
    u->stats.def = stat_scale(lvl, bs->base_def);
    u->stats.max_hp = stat_scale(lvl, bs->base_hp);
    u->stats.move_points = 1;
    u->stats.level = lvl;
}



/**
 * Calculates a unit's stat based on level
 * Based on the formula: base * 2 / 40 + 5
 * @param base_value value between 20-40. Higher base value, the stronger that stat will be once leveled up
 * @returns the adjusted stat between [1 - 2*base_val]
 */
uint stat_scale(uint lvl, u8 base_val) {
    return base_val * lvl / (UNIT_MAX_LVL >> 1) + 5;
}