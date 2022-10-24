#ifndef STAT_H
#define STAT_H

#include "../../lib/defines.h"

typedef enum {
    STAT_ATK_MELEE, // enemies direction in front or to the side
    STAT_ATK_MAGE, // 3 tiles in a straight line
    STAT_ATK_ARCH, // 2x2 diamond
} attack_type_t;

typedef enum unit_type_t unit_type_t;

typedef struct {
    s8 max_hp;
    s8 hp;
    u8 atk;
    u8 def;
    u8 move_points;
    u8 level;
    u16 exp;
    attack_type_t attack_pattern;
    u8 type; // unit_type_t
} Stats;

void stat_fill(Stats *stats, unit_type_t type, uint lvl);
uint stat_scale(uint lvl, u8 base_val);
bool stat_gain_exp(Stats *s, u16 exp);
u16 stat_required_exp(uint lvl);
bool stat_can_lvl_up(Stats *s, u16 exp);

#endif