#ifndef TEAM_H
#define TEAM_H

#include "unit/unit.h"
#include "map/map.h"
#include <stdio.h>

#define TEAM_MAX_SIZE 6

typedef enum {
    CONTROL_PLR,
    CONTROL_AI,
} control_type_t;

typedef struct Team {
    Unit units[TEAM_MAX_SIZE];
    u8 size;

    control_type_t control;
} Team;


void tm_init(Team *t, unit_type_t types[], uint size, control_type_t cnt, WIN_REGULAR *win);
void tm_update(Team *tm);

bool tm_add(Team *tm, Unit *u);
void tm_focus(Team *tm, Unit *u);

Unit *tm_leader();

#endif