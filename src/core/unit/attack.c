#include "unit.h"
#include "../hud/hud.h"
#include "../../text.h"
#include "../../lib/random.h"

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
    // apply defense stat
    if(dmg < 0) {
        dmg -= u->stats.def - 1;
        if(dmg > 0)
            dmg = 0;
    }

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
atk_error_t unit_attack(Unit *atker, uint tx, uint ty) {
    Unit *u = unit_at(tx, ty);

    // if no unit or unit is dead
    if(!u || unit_is_dead(u))
        return ATK_FAIL_EMPTY;

    // if friendly fire
    if(u->is_ai == atker->is_ai)
        return ATK_FAIL_FRIENDLY;
    
    unit_hurt(u, atker->stats.atk);

    // gain EXP
    if(unit_is_dead(u)) {
        uint exp = u->stats.level * 3 + 5 + (rnd_random() & 3); // @todo move animation into a function
        bar_show();
        text_printf("%s GAINED %dEXP", 1, 18, unit_name(atker->type), exp); // @todo add slow text write
        bar_wait_for_key();

        // if level up
        if(stat_gain_exp(&atker->stats, exp)) {
            text_printf("%s GREW TO LEVEL %d.", 1, 18, unit_name(atker->type), atker->stats.level);
            bar_wait_for_key();
        }
    }

    return ATK_SUCCESS;
}