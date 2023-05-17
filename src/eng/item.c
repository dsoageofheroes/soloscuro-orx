#include <string.h>

#include <gff/gff.h>
#include <gff/item.h>
#include <gff/object.h>

#include "soloscuro/debug.h"
#include "soloscuro/item.h"
#include "soloscuro/entity.h"
#include "soloscuro/state.h"

extern int sol_inventory_create(sol_item_t **d) {
    if (!d) { return EXIT_FAILURE; }

    sol_item_t *ret = calloc(1, sizeof(sol_inventory_t));
    if (!ret) { return EXIT_FAILURE; }

    //for (int i = 0; i < 26; i++) {
        //ret[i].anim.spr = SPRITE_ERROR;
    //}

    *d = ret;
    return EXIT_SUCCESS;
}

/*
static int32_t get_bmp_id(sol_item_t *item) {
    disk_object_t dobj;
    if (!item) { return -1; }

    gff_chunk_header_t chunk = gff_find_chunk_header(OBJEX_GFF_INDEX, GFF_OJFF,
            item->ds_id < 0 ? -1 * item->ds_id : item->ds_id);
    if (chunk.length <= 0) { return -1;}

    gff_read_chunk(OBJEX_GFF_INDEX, &chunk, &dobj, sizeof(dobj));

    return dobj.bmp_id;
}
*/

extern int sol_item_convert_from_ds1(soloscuro_state_t *state, sol_item_t *item, const ds1_item_t *ds1_item) {
    ds_item1r_t ds1_item1r;

    if (!item || !ds1_item) { return EXIT_FAILURE; }
    
    if (gff_manager_get_item1r(state->man, ds1_item->item_index, &ds1_item1r) == EXIT_FAILURE) {
        error("Unable to find item1r for %d\n", ds1_item->item_index);
        return EXIT_FAILURE;
    }

    item->ds_id = ds1_item->id;
    gff_manager_get_name(state->man, ds1_item->name_idx, item->name);
    //strncpy(item->name, ssi_item_name(ds1_item->name_idx), ITEM_NAME_MAX - 1);

    if (ds1_item1r.flags & DS1_ARMOR_FLAG) {
        item->type = ITEM_ARMOR;
    } else if (ds1_item1r.weapon_type & DS1_WEAPON_MELEE) {
        item->type = ITEM_MELEE;
    } else if (ds1_item1r.weapon_type & DS1_WEAPON_MISSILE) {
        if (ds1_item1r.weapon_type & DS1_WEAPON_THROWN) {
            item->type = ITEM_MISSILE_THROWN;
        } else {
            item->type = ITEM_MISSILE_USE_AMMO;
        }
    } else {
        item->type = ITEM_CONSUMABLE;
    }

    item->quantity = ds1_item->quantity;
    item->value = ds1_item->value;
    item->charges = ds1_item->charges;
    item->legal_class = ds1_item1r.legal_class;
    item->placement = ds1_item1r.placement;
    item->weight = ds1_item1r.weight;
    item->material = ds1_item1r.material;
    item->ac = ds1_item1r.base_AC;
    item->attack.number = ds1_item1r.num_attacks;
    item->attack.num_dice = ds1_item1r.dice;
    item->attack.sides = ds1_item1r.sides;
    item->attack.bonus = ds1_item1r.mod;
    item->attack.range = ds1_item1r.range;
    item->attack.damage_type = ds1_item1r.damage_type;
    //item->anim.bmp_id = get_bmp_id(item);
    item->item_index = ds1_item->item_index;

    //return sol_load_item(item);
    debug("Do I need to create the sprite?\n");
    return EXIT_SUCCESS;
}

extern int sol_item_allowed_in_slot(sol_item_t *item, const int slot) {
    if (!item)                 { return EXIT_FAILURE; }
    if (slot < 0 || slot > 25) { return EXIT_FAILURE; }

    if (slot > 13) { return EXIT_SUCCESS; } // backpack is always okay!

    switch (item->placement) {
        case 1: // Chest
            return slot == SLOT_CHEST ? EXIT_SUCCESS : EXIT_FAILURE;
        case 2: // Waist
            return slot == SLOT_WAIST ? EXIT_SUCCESS : EXIT_FAILURE;
        case 3: // ARM
            return slot == SLOT_ARM ? EXIT_SUCCESS : EXIT_FAILURE;
        case 4: // FOOT
            return slot == SLOT_FOOT ? EXIT_SUCCESS : EXIT_FAILURE;
        case 5: // HAND
            return slot == SLOT_HAND0 || slot == SLOT_HAND1 ? EXIT_SUCCESS : EXIT_FAILURE;
        case 6: // HEAD
            return slot == SLOT_HEAD ? EXIT_SUCCESS : EXIT_FAILURE;
        case 7: // NECK
            return slot == SLOT_NECK ? EXIT_SUCCESS : EXIT_FAILURE;
        case 8: // CLOAK
            return slot == SLOT_CLOAK ? EXIT_SUCCESS : EXIT_FAILURE;
        case 9: // FINGER
            return slot == SLOT_FINGER0 || slot == SLOT_FINGER1 ? EXIT_SUCCESS : EXIT_FAILURE;
        case 10: // LEGS
            return slot == SLOT_LEGS ? EXIT_SUCCESS : EXIT_FAILURE;
        case 11: // AMMO
            return slot == SLOT_AMMO ? EXIT_SUCCESS : EXIT_FAILURE;
        case 12: // MISSILE
            return slot == SLOT_MISSILE ? EXIT_SUCCESS : EXIT_FAILURE;
    }

    return EXIT_FAILURE;
}

extern int sol_item_dup(sol_item_t *item, sol_item_t **d) {
    if (!item || !d) { return EXIT_FAILURE; }

    sol_item_t *ret = malloc(sizeof(sol_item_t));
    if (!ret) { return EXIT_FAILURE; }

    memcpy(ret, item, sizeof(sol_item_t));
    *d = ret;
    return EXIT_SUCCESS;
}

extern int sol_item_free_except_graphics(sol_item_t *item) {
    if (!item) { return EXIT_FAILURE; }

    free(item);
    return EXIT_SUCCESS;
}

static void free_item(sol_item_t *item) {
    if (!item) { return; }

    //if (item->anim.spr != SPRITE_ERROR) {
        //sol_status_check(sol_sprite_free(item->anim.spr), "Unable to free sprite.");
        //item->anim.spr = SPRITE_ERROR;
    //}
}

extern int sol_item_free(sol_item_t *item) {
    if (!item) { return EXIT_FAILURE; }

    free_item(item);
    free(item);
    return EXIT_SUCCESS;
}

extern int sol_item_free_inventory(sol_item_t *inv) {
    if (!inv) { return EXIT_FAILURE; }
    sol_item_t *items = (sol_item_t*)inv;

    for (int i = 0; i < 26; i++) {
        free_item(items + i);
    }

    //TODO: Free up the effects!
    return EXIT_SUCCESS;
}

// TODO: Implement!
extern int sol_item_get_wizard_level(sol_item_t *item, uint8_t *level) {
    if (!item) { return EXIT_FAILURE; }

    return EXIT_FAILURE;
}

extern int sol_item_get_priest_level(sol_item_t *item, uint8_t *level) {
    if (!item) { return EXIT_FAILURE; }

    return EXIT_FAILURE;
}

/*
extern int sol_item_icon(sol_item_t *item, animate_sprite_t **d) {
    if (!item || !d) { return EXIT_FAILURE; }

    gff_palette_t *pal = open_files[RESOURCE_GFF_INDEX].pals->palettes + 0;

    //if (!port_valid_sprite(&item->sprite)) {
    if (item->anim.spr == SPRITE_ERROR) {
        if (!item->anim.bmp_id) { return EXIT_FAILURE; }
        sol_sprite_load(&item->anim, pal, OBJEX_GFF_INDEX, GFF_BMP, item->anim.bmp_id, 1);
    }

    *d = &(item->anim);
    return EXIT_SUCCESS;
}
*/

extern int sol_give_ds1_item(soloscuro_state_t *state, sol_entity_t *pc, const int slot, const int item_index, const int id) {
    if (!pc) { return EXIT_FAILURE; }
    if (!pc->inv) { sol_inventory_create(&pc->inv); }
    ds1_item_t ds1_item;
    ds1_item.slot = SLOT_HAND0;
    ds1_item.item_index = item_index;
    gff_item_load(state->man, &ds1_item, id);
    return sol_item_convert_from_ds1(state, pc->inv + slot, &ds1_item);
}

extern int sol_item_set_starting(soloscuro_state_t *state, sol_dude_t *dude) {
    if (!dude) { return EXIT_FAILURE; }
    int has_gladiator = sol_entity_has_class(dude, REAL_CLASS_GLADIATOR) == EXIT_SUCCESS;
    int has_fighter = sol_entity_has_class(dude, REAL_CLASS_FIGHTER) == EXIT_SUCCESS;
    int has_ranger = sol_entity_has_class(dude, REAL_CLASS_AIR_RANGER) == EXIT_SUCCESS
        || sol_entity_has_class(dude, REAL_CLASS_EARTH_RANGER) == EXIT_SUCCESS
        || sol_entity_has_class(dude, REAL_CLASS_FIRE_RANGER) == EXIT_SUCCESS
        || sol_entity_has_class(dude, REAL_CLASS_WATER_RANGER) == EXIT_SUCCESS;
    int has_druid = sol_entity_has_class(dude, REAL_CLASS_AIR_DRUID) == EXIT_SUCCESS
        || sol_entity_has_class(dude, REAL_CLASS_EARTH_DRUID) == EXIT_SUCCESS
        || sol_entity_has_class(dude, REAL_CLASS_FIRE_DRUID) == EXIT_SUCCESS
        || sol_entity_has_class(dude, REAL_CLASS_WATER_DRUID) == EXIT_SUCCESS;
    int has_cleric = sol_entity_has_class(dude, REAL_CLASS_AIR_CLERIC) == EXIT_SUCCESS
        || sol_entity_has_class(dude, REAL_CLASS_EARTH_CLERIC) == EXIT_SUCCESS
        || sol_entity_has_class(dude, REAL_CLASS_FIRE_CLERIC) == EXIT_SUCCESS
        || sol_entity_has_class(dude, REAL_CLASS_WATER_CLERIC) == EXIT_SUCCESS;
    int has_psionicist = sol_entity_has_class(dude, REAL_CLASS_PSIONICIST) == EXIT_SUCCESS;
    int has_thief = sol_entity_has_class(dude, REAL_CLASS_THIEF) == EXIT_SUCCESS;
    int has_preserver = sol_entity_has_class(dude, REAL_CLASS_PRESERVER) == EXIT_SUCCESS;

    sol_item_free_inventory(dude->inv);
    if (dude->inv) {
        memset(dude->inv, 0x0, sizeof(sol_inventory_t));
        //for (int i = 0; i < 26; i++) {
            //dude->inv[i].anim.spr = SPRITE_ERROR;
        //}
    } else {
        sol_inventory_create(&dude->inv);
    }

    if (dude->race == RACE_THRIKREEN) {
        sol_give_ds1_item(state, dude, SLOT_MISSILE, 48, -1010);
        return EXIT_SUCCESS;
    }

    if (has_gladiator || has_fighter || has_ranger || has_thief) {
        sol_give_ds1_item(state, dude, SLOT_HAND0, 81, -1012); // bone sword
    } else if (has_druid || has_cleric || has_psionicist) {
        sol_give_ds1_item(state, dude, SLOT_HAND0, 18, -1185); // club
    } else if (has_preserver) {
        sol_give_ds1_item(state, dude, SLOT_HAND0, 3, -1019); // quaterstaff
    } else {
        return EXIT_FAILURE; // Dude has no class!
    }

    if (has_gladiator) {
        sol_give_ds1_item(state, dude, SLOT_HAND1, 18, -1185); // club
    } else if (has_fighter || has_ranger || has_cleric || has_thief) {
        sol_give_ds1_item(state, dude, SLOT_HAND1, 4, -1020); // leather shield
    }

    if (has_gladiator || has_fighter || has_ranger || has_cleric) {
        sol_give_ds1_item(state, dude, SLOT_ARM, 7, -1023); // leather arm armor
    }

    if (has_fighter) {
        sol_give_ds1_item(state, dude, SLOT_LEGS, 8, -1026); // leather leg armor
    }

    if (has_fighter || has_gladiator || has_ranger || has_cleric || has_thief) {
        sol_give_ds1_item(state, dude, SLOT_CHEST, 6, -1022); // leather chest armor
    }

    if (has_ranger || has_psionicist) {
        sol_give_ds1_item(state, dude, SLOT_MISSILE, 1, -1017); // Bow
        sol_give_ds1_item(state, dude, SLOT_AMMO, 62, -1070); // arrows
    } else if (has_druid || has_thief || has_preserver) {
        sol_give_ds1_item(state, dude, SLOT_MISSILE, 64, -1015); // sling
    }

    return EXIT_SUCCESS;
}

extern int sol_item_get(sol_inventory_t *inv, const int8_t slot, sol_item_t **d) {
    if (!inv || !d) { return EXIT_FAILURE; }

    sol_item_t *item = (sol_item_t*) inv;
    if (!inv || slot < 0 || slot >= SLOT_END) { return EXIT_FAILURE; }
    if ((item + slot)->name[0] == 0) { return EXIT_FAILURE; }

    *d = item + slot;
    return EXIT_SUCCESS;
}
