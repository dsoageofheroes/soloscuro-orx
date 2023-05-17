#ifndef SOLOSCURO_RULES_H
#define SOLOSCURO_RULES_H

#include <stdint.h>

#include "soloscuro/combat.h"
#include "soloscuro/entity.h"

#include <gff/object.h>

extern int sol_dnd2e_randomize_stats_pc(sol_entity_t *pc);
extern int sol_dnd2e_set_starting_level(sol_entity_t *pc);
extern int sol_dnd2e_loop_creation_stats(sol_entity_t *pc);
extern int sol_dnd2e_set_exp(sol_entity_t *pc, const uint32_t amt);
extern int sol_dnd2e_award_exp(sol_entity_t *pc, const uint32_t amt);
extern int sol_dnd2e_character_is_valid(const sol_entity_t *pc);
extern int sol_dnd2e_psin_is_valid(ds_character_t *pc, sol_psin_t *psi);
extern int sol_dnd2e_roll_initiative(sol_entity_t *entity);
extern int sol_dnd2e_can_melee_again(sol_entity_t *source, const int attack_num, const int round);
extern sol_attack_t sol_dnd2e_melee_attack(sol_entity_t *source, sol_entity_t *target, const int round);
extern int sol_dnd2e_range_attack(sol_entity_t *source, sol_entity_t *target, const int round);
extern int sol_dnd2e_get_max_hp(sol_dude_t *dude);

// Leaving alone for now.
// TODO: refactor this if it makes sense too...
extern int16_t dnd2e_calc_ac(sol_entity_t *entity);
extern int16_t dnd2e_get_ac_pc(sol_entity_t *pc);
extern int16_t dnd2e_get_move(sol_entity_t *pc);
extern int16_t dnd2e_get_thac0(sol_entity_t *pc, int slot);
extern int16_t dnd2e_get_attack_num(const sol_entity_t *pc, const sol_item_t *item);
extern int16_t dnd2e_get_attack_die_pc(const sol_entity_t *pc, const sol_item_t *item);
extern int16_t dnd2e_get_attack_sides_pc(const sol_entity_t *pc, const sol_item_t *item);
extern int16_t dnd2e_get_attack_mod_pc(const sol_entity_t *pc, const sol_item_t *item);
extern int16_t dnd2e_dice_roll(const uint16_t num, const uint16_t sides);

//Combat:
#endif
