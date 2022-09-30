#ifndef STAT_H
#define STAT_H

 #include "unit.h"

void stat_fill(Unit *u, uint lvl);
uint stat_scale(uint lvl, u8 base_val);

#endif