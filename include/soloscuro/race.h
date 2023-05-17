#ifndef SOLOSCURO_RACE_H
#define SOLOSCURO_RACE_H

#include "entity.h"

extern int sol_dnd2e_race_apply_mods(sol_entity_t *pc);
extern int sol_dnd2e_race_apply_initial_stats(sol_entity_t *dude);
extern const int8_t sol_dnd2e_race_get_stats(int race, int stat, int max);

#endif
