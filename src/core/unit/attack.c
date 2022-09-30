#include "unit.h"

typedef u8 AttackPattern[7];

const AttackPattern _patterns[] = {
    [STAT_ATK_MELEE] = {
        0b0000000,
        0b0000000,
        0b0001000,
        0b0010100,
        0b0001000,
        0b0000000,
        0b0000000,
    },
    [STAT_ATK_MAGE] = {
        0b0000000,
        0b0001000,
        0b0011100,
        0b0110110,
        0b0011100,
        0b0001000,
        0b0000000,
    },
    [STAT_ATK_ARCH] = {
        0b0000000,
        0b0001000,
        0b0010100,
        0b0100010,
        0b0010100,
        0b0001000,
        0b0000000,
    },
};


/**
 * Gets the value of the attack pattern at (dx, dy)
 * @param dx In the range of [-3, 3]
 * @param dy In the range of [-3, 3]
 */
inline bool atk_get(attack_type_t p, int dx, int dy) {
    const uint x = dx + 3;
    const uint y = dy + 3;
    if(x > 7 || y > 7)
        return false;
    
    return (_patterns[p][y] >> x) & 1;
}


/**
 * @param dmg amount of HP to subtract from current health
 */
void unit_hurt(Unit *u, int dmg) {
    u->stats.hp -= dmg;

    if(u->stats.hp <= 0) {
        unit_kill(u);
    }

    if(u->stats.hp > u->stats.max_hp) {
        u->stats.hp = u->stats.max_hp;
    }
}


/**
 * @returns true if the attacker could attack at the coordinates
 */
bool unit_attack(Unit *atker, uint tx, uint ty) {
    Unit *u = unit_at(tx, ty);

    // if no unit, unit is dead or friendly fire, then return
    if(!u || unit_is_dead(u) || u->is_ai == atker->is_ai)
        return false;
    
    unit_hurt(u, atker->stats.atk);

    return true;
}