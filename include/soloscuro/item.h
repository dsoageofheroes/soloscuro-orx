#ifndef SOLOSCURO_ITEM_H
#define SOLOSCURO_ITEM_H

#include <gff/item.h>
#include <stdint.h>

#define ITEM_NAME_MAX (32)

typedef enum sol_item_type_e {
    ITEM_CONSUMABLE,
    ITEM_MELEE,
    ITEM_MISSILE_THROWN,
    ITEM_MISSILE_USE_AMMO,
    ITEM_ARMOR
} sol_item_type_t;

typedef struct sol_effect_node_s {
    uint32_t psi;
    uint32_t spell;
    uint32_t other;
    uint32_t action; // IE: on attack, on equip, on use
    struct effect_node_s *next;
} sol_effect_node_t;

typedef struct sol_item_attack_s {
    uint8_t  number;
    uint8_t  num_dice;
    uint8_t  sides;
    uint8_t  bonus;
    uint8_t  range;
    uint16_t damage_type;
} sol_item_attack_t;

typedef struct sol_item_s { 
    int16_t            ds_id;
    char               name[ITEM_NAME_MAX];
    sol_item_type_t    type;
    uint16_t           quantity;
    uint16_t           value;
    uint16_t           charges;
    uint16_t           legal_class;
    int16_t            item_index;
    uint8_t            placement; // where on the user
    uint8_t            weight;
    uint8_t            material;
    int8_t             ac;
    sol_item_attack_t  attack;
    sol_effect_node_t *effect;
} sol_item_t;

typedef struct sol_inventory_s {
    sol_item_t arm;
    sol_item_t ammo;
    sol_item_t missile;
    sol_item_t hand0;
    sol_item_t finger0;
    sol_item_t waist;
    sol_item_t legs;
    sol_item_t head;
    sol_item_t neck;
    sol_item_t chest;
    sol_item_t hand1;
    sol_item_t finger1;
    sol_item_t cloak;
    sol_item_t foot;
    sol_item_t bp[12];
} sol_inventory_t;

#define ITEM_SLOT_MAX (sizeof(sol_inventory_t) / sizeof(sol_item_t))

extern int sol_item_get_wizard_level(sol_item_t *item, uint8_t *level);
extern int sol_item_get_priest_level(sol_item_t *item, uint8_t *level);
extern int sol_item_free(sol_item_t *item);
extern int sol_item_free_except_graphics(sol_item_t *item);
extern int sol_item_free_inventory(sol_item_t *item);
extern int sol_item_allowed_in_slot(sol_item_t *item, const int slot);
extern int sol_inventory_create(sol_item_t **d);
extern int sol_item_dup(sol_item_t *item, sol_item_t **d);
extern int sol_item_get(sol_inventory_t *inv, const int8_t slot, sol_item_t **d);
//extern int sol_item_icon(sol_item_t *item, animate_sprite_t **);

#endif
