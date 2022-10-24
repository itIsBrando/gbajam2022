#include "stats.h"
 #include "unit.h"
 
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
        .base_hp  = 48,
        .base_atk = 33,
        .base_def = 22,
    },
    [UNIT_MAGE] = {
        .atktype  = STAT_ATK_MAGE,
        .base_hp  = 45,
        .base_atk = 26,
        .base_def = 20,
    },
    [UNIT_ORGE] = {
        .atktype = STAT_ATK_MELEE,
        .base_hp = 45,
        .base_atk = 20,
        .base_def = 25,
    }
};


static_assert(LENGTH(unit_base_stats) == UNIT_TYPES, "`unit_base_stats` not filled with All unit types in stats.c");


void stat_fill(Stats *stats, unit_type_t type, uint lvl) {
    const BaseStats *bs = &unit_base_stats[type];

    if(lvl > UNIT_MAX_LVL)
        lvl = UNIT_MAX_LVL;
    
    stats->type = type;
    stats->attack_pattern = bs->atktype;
    stats->atk = stat_scale(lvl, bs->base_atk);
    stats->def = stat_scale(lvl, bs->base_def);
    stats->max_hp = stat_scale(lvl, bs->base_hp);
    stats->move_points = 1;
    stats->level = lvl;
}



/**
 * Calculates a unit's stat based on level
 * Based on the formula: base * 2 / 40 + 5
 * @param base_value value between 20-40. Higher base value, the stronger that stat will be once leveled up
 * @returns the adjusted stat between [1 - 2*base_val]
 */
uint stat_scale(uint lvl, u8 base_val) {
    return base_val * (lvl + 3) / (UNIT_MAX_LVL >> 1) + 2;
}


/**
 * @todo add different exp curves for different units
 * @returns the required EXP for `lvl`
 */
u16 stat_required_exp(uint lvl) {
    lvl++;
    return lvl * lvl * lvl / 2 + 3;
}

inline bool stat_can_lvl_up(Stats *s, u16 exp) {
    return exp >= stat_required_exp(s->level + 1);
}


bool stat_gain_exp(Stats *s, u16 exp) {
    s->exp += exp;

    if(!stat_can_lvl_up(s, s->exp))
        return false;
    
    do {
        stat_fill(s, s->type, s->level + 1);
    } while(stat_can_lvl_up(s, s->exp));
    
    return true;
}