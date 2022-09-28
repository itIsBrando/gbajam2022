#ifndef TEAM_H
#define TEAM_H

#include "unit/unit.h"
#include "map/map.h"
#include <stdio.h>

#define TEAM_MAX_SIZE 6


typedef struct Team {
    Unit units[TEAM_MAX_SIZE];
    u8 size;
    bool auto_follow; // true for other units to follow leader
} Team;


void tm_init(Team *t, unit_type_t types[], uint size, WIN_REGULAR *win);
void tm_update();

Team *tm_get();
bool tm_add(Team *tm, Unit *u);
void tm_focus(Team *tm, Unit *u);
void tm_start_turn(Team *tm, Unit *u);
void tm_set_starting_pos();

Unit *tm_leader();
bool tm_auto_follow();
void tm_set_auto_follow(bool v);

#endif