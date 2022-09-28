#include "stats.h"


const Stats _stats[] = {
    [UNIT_HERO]={
        .atkrng=STAT_ATK_MELEE,
        .move_points=1,
        .hp=3,
    },
    [UNIT_SKELETON]={
        .atkrng=STAT_ATK_ARCH,
        .move_points=1,
        .hp=2
    },
};


inline const Stats *stat_get(Unit *u) {
    return &_stats[u->type];
}