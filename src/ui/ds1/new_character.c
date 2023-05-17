/*
 * New Character Window
 *
 * This manages all the Character Creation for the first game.
 * Hopefully the correct rules are implemented.
 */
#include "soloscuro.h"
#include "soloscuro/alignment.h"
#include "soloscuro/entity.h"
#include "soloscuro/rules.h"
#include "soloscuro/race.h"
#include "soloscuro/window.h"
#include "gff/name.h"

// GUI state
static int32_t    win_id;
static orxOBJECT *win, *win_psionics, *win_spheres;
static orxOBJECT *class_buttons[8], *exit_button, *done_button;
static orxOBJECT *str_button, *dex_button, *con_button, *int_button, *wis_button, *cha_button;
static orxOBJECT *large_portrait, *sprite_portrait, *dice_button, *alignment, *hp;
static orxOBJECT *dice_flip;
static orxOBJECT *name_box;
static orxOBJECT *sphere_button[4], *psionic_button[4];

static orxOBJECT *portraits[14];
static int32_t    last_port = 0;

// Character State
static sol_entity_t pc;
static void init_pc(soloscuro_state_t *state);
static void update_ui(soloscuro_state_t *state);
static void randomize_pc(soloscuro_state_t *state);
static int has_class(const uint16_t class);
static int is_divine_spell_user();
static int has_selected_class(int class);

orxSTATUS orxFASTCALL anim_event_handler(const orxEVENT *event) {
    orxANIM_EVENT_PAYLOAD *payload;
     
    payload = (orxANIM_EVENT_PAYLOAD *)event->pstPayload;
     
    if (!strcmp(payload->zAnimName, "DiceFlip")) {
        randomize_pc(event->pContext);
    }
}

orxSTATUS orxFASTCALL render_event_handler(const orxEVENT *_pstEvent) {
    orxVECTOR size = { 10, 10, 0 };
    orxVECTOR win_pos, pos, scale, sphere_pos, psionic_pos;
    orxOBOX boxArea;
    orxVECTOR pivot = { 0, 0, 0 };
    orxRGBA colour;

    int is_divine = is_divine_spell_user();

    if (_pstEvent->eType != orxEVENT_TYPE_RENDER || _pstEvent->eID != orxRENDER_EVENT_STOP) {
        return orxSTATUS_SUCCESS;
    }
 
    colour.u8R = 0;
    colour.u8G = 0;
    colour.u8B = 0;
    colour.u8A = 255;

    orxObject_GetScale(win, &scale);
    orxObject_GetPosition(win, &win_pos);
    orxObject_GetPosition(win_spheres, &sphere_pos);
    orxObject_GetPosition(win_psionics, &psionic_pos);
    for (int i = 0; i < 8; i++ ) {
        orxObject_GetPosition(class_buttons[i], &pos);
        if (!has_selected_class(i)) { continue; }
        pos.fX += win_pos.fX + 28;
        pos.fY += win_pos.fY + 20 + i;
        pos.fX *= scale.fX;
        pos.fY *= scale.fY;

        orxOBox_2DSet(&boxArea, &pos, &pivot, &size, 0);
 
        orxDisplay_DrawOBox( &boxArea, colour, orxTRUE);
    }

    for (int i = 0; i < 4 && is_divine; i++) {
        switch (i) {
            case 0:if (!(  has_class(REAL_CLASS_AIR_DRUID)
                        || has_class(REAL_CLASS_AIR_CLERIC)
                        || has_class(REAL_CLASS_AIR_DRUID)
                        || has_class(REAL_CLASS_AIR_RANGER))) { continue;}
                       break;
            case 1:if (!(  has_class(REAL_CLASS_EARTH_DRUID)
                        || has_class(REAL_CLASS_EARTH_CLERIC)
                        || has_class(REAL_CLASS_EARTH_DRUID)
                        || has_class(REAL_CLASS_EARTH_RANGER))) { continue;}
                       break;
            case 2:if (!(  has_class(REAL_CLASS_FIRE_DRUID)
                        || has_class(REAL_CLASS_FIRE_CLERIC)
                        || has_class(REAL_CLASS_FIRE_DRUID)
                        || has_class(REAL_CLASS_FIRE_RANGER))) { continue;}
                       break;
            case 3:if (!(  has_class(REAL_CLASS_WATER_DRUID)
                        || has_class(REAL_CLASS_WATER_CLERIC)
                        || has_class(REAL_CLASS_WATER_DRUID)
                        || has_class(REAL_CLASS_WATER_RANGER))) { continue;}
                       break;
        }
        orxObject_GetPosition(sphere_button[i], &pos);

        pos.fX += sphere_pos.fX + 28;
        pos.fY += sphere_pos.fY + 28 + i;
        pos.fX *= scale.fX;
        pos.fY *= scale.fY;

        orxOBox_2DSet(&boxArea, &pos, &pivot, &size, 0);
 
        orxDisplay_DrawOBox( &boxArea, colour, orxTRUE);
    }
    for (int psi = 1; psi < 4; psi++) {
        int has = has_class(REAL_CLASS_PSIONICIST);
        for (int i = 0; i < 7; i++) {
            if (pc.psi.types[i] == psi) { has = 1; }
        }
        if (!has) { continue; }
        orxObject_GetPosition(psionic_button[psi - 1], &pos);

        pos.fX += psionic_pos.fX + 28;
        pos.fY += psionic_pos.fY + 28 + psi; 
        pos.fX *= scale.fX;
        pos.fY *= scale.fY;

        orxOBox_2DSet(&boxArea, &pos, &pivot, &size, 0);
 
        orxDisplay_DrawOBox( &boxArea, colour, orxTRUE);
    }

    return orxSTATUS_SUCCESS;
}

static int convert_to_actual_class(int step) {
    switch (step) {
        case 0: return REAL_CLASS_AIR_CLERIC;
        case 1: return REAL_CLASS_AIR_DRUID;
        case 2: return REAL_CLASS_FIGHTER;
        case 3: return REAL_CLASS_GLADIATOR;
        case 4: return REAL_CLASS_PRESERVER;
        case 5: return REAL_CLASS_PSIONICIST;
        case 6: return REAL_CLASS_AIR_RANGER;
        case 7: return REAL_CLASS_THIEF;
    }

    return REAL_CLASS_NONE;
}


static int fix_z_ordering(orxOBJECT *win) {
    orxVECTOR pos;

    for(orxOBJECT *child = orxOBJECT(orxObject_GetChild(win_spheres));
            child != orxNULL;
            child = orxOBJECT(orxObject_GetSibling(child))) {
        for(orxOBJECT *c2 = orxOBJECT(orxObject_GetChild(child));
                c2 != orxNULL;
                c2 = orxOBJECT(orxObject_GetSibling(c2))) {
            orxObject_GetPosition(c2, &pos);
            //printf("->%s\n", orxObject_GetName(c2));
            pos.fZ -= 0.1;
            //printf("->%f\n", pos.fZ);
            orxObject_SetPosition(c2, &pos);
        }
    }
}

static orxOBJECT* make_object(orxOBJECT *win, const char *name) {
    orxVECTOR pos;
    orxOBJECT *obj = orxObject_CreateFromConfig(name);

    orxObject_SetParent(obj, win);
    orxObject_GetPosition(obj, &pos);
    pos.fZ -= 0.01;
    orxObject_SetPosition(obj, &pos);

    return obj;
}

static int change_race_gender(soloscuro_state_t *state, int diff) {
    orxVECTOR lp_size, port_size, pos;

    orxObject_Enable(portraits[last_port], 0);
    last_port = ((last_port + diff + 14) % 14);
    orxObject_GetSize(large_portrait, &lp_size);
    orxObject_GetSize(portraits[last_port], &port_size);
    orxObject_GetPosition(portraits[last_port], &pos);
    pos.fX = (lp_size.fX - port_size.fX) / 2.0;
    pos.fY = (lp_size.fY - port_size.fY) / 2.0;
    orxObject_SetPosition(portraits[last_port], &pos);
    orxObject_Enable(portraits[last_port], 1);

    memset(&pc.psi, 0, sizeof(sol_psin_t));
    pc.psi.types[0] = 1;

    //TODO UPDATE RACE AND GENDER!
    pc.race = last_port / 2 + 1;
    pc.gender = last_port % 2 + 1;
    if (last_port == 13) {
        pc.race = RACE_THRIKREEN;
        pc.gender = GENDER_FEMALE;
    }

    pc.class[0].class = pc.class[1].class = pc.class[2].class = -1;
    pc.class[0].level = pc.class[1].level = pc.class[2].level = -1;

    randomize_pc(state);

    sol_dude_print(&pc);
    //pc.race = sel;
    return EXIT_SUCCESS;
}

extern int soloscuro_ds1_new_character_init(soloscuro_state_t *state) {
    orxVECTOR scale, pos, size;
    orxOBJECT *tmp;
    float     fz;

    soloscuro_get_top_window(state, &win, &win_id);

    orxObject_GetPosition(win, &pos);
    fz = pos.fZ;

    soloscuro_find_first_child(win, "Window3012", &win_psionics);
    soloscuro_find_first_child(win, "Window3013", &win_spheres);
    fix_z_ordering(win_psionics);
    fix_z_ordering(win_spheres);

    orxObject_EnableRecursive(win_psionics, 1);
    orxObject_EnableRecursive(win_spheres, 1);
    orxObject_GetScale(win_psionics, &scale);
    orxObject_GetPosition(win_psionics, &pos);
    scale.fX = scale.fY = scale.fZ = 1.0;
    pos.fX = 600;
    pos.fY = 80;
    //pos.fZ = fz - 0.01;
    orxObject_SetScale(win_psionics, &scale);
    orxObject_SetPosition(win_psionics, &pos);
    orxObject_GetScale(win_spheres, &scale);
    orxObject_GetPosition(win_spheres, &pos);
    scale.fX = scale.fY = scale.fZ = 1.0;
    pos.fX = 200;
    pos.fY = 80;
    //pos.fZ = fz - 0.03;
    orxObject_SetScale(win_spheres, &scale);
    orxObject_SetPosition(win_spheres, &pos);

    for (int i = 0; i < 4; i++) {
        soloscuro_get_button_object(win_spheres, 3013, i, &sphere_button[i]);
        soloscuro_get_button_object(win_psionics, 3013, i, &psionic_button[i]);
    }

    for (int i = 0; i < 8; i++) {
        soloscuro_get_button_object(win, win_id, 1 + i, &(class_buttons[i]));
    }
    soloscuro_get_button_object(win, win_id, 11, &exit_button);
    soloscuro_get_button_object(win, win_id, 12, &done_button);
    soloscuro_get_button_text_object(win, win_id, 14, &str_button);
    soloscuro_get_button_text_object(win, win_id, 15, &dex_button);
    soloscuro_get_button_text_object(win, win_id, 16, &con_button);
    soloscuro_get_button_text_object(win, win_id, 17, &int_button);
    soloscuro_get_button_text_object(win, win_id, 18, &wis_button);
    soloscuro_get_button_text_object(win, win_id, 19, &cha_button);
    soloscuro_get_button_object(win, win_id, 0, &large_portrait);
    soloscuro_get_button_object(win, win_id, 10, &sprite_portrait);
    soloscuro_get_button_object(win, win_id, 9, &dice_button);
    soloscuro_get_button_text_object(win, win_id, 13, &alignment);
    soloscuro_get_button_text_object(win, win_id, 20, &hp);
    soloscuro_get_ebox_object(win, win_id, 0, &name_box);

    portraits[0] = make_object(win, "PortraitMaleHuman");
    portraits[1] = make_object(win, "PortraitFemaleHuman");
    portraits[2] = make_object(win, "PortraitMaleDwarf");
    portraits[3] = make_object(win, "PortraitFemaleDwarf");
    portraits[4] = make_object(win, "PortraitMaleElf");
    portraits[5] = make_object(win, "PortraitFemaleElf");
    portraits[6] = make_object(win, "PortraitMaleHalfElf");
    portraits[7] = make_object(win, "PortraitFemaleHalfElf");
    portraits[8] = make_object(win, "PortraitMaleHalfGiant");
    portraits[9] = make_object(win, "PortraitFemaleHalfGiant");
    portraits[10] = make_object(win, "PortraitMaleHalfling");
    portraits[11] = make_object(win, "PortraitFemaleHalfling");
    portraits[12] = make_object(win, "PortraitMul");
    portraits[13] = make_object(win, "PortraitThrikeen");
    for (int i = 0; i < 14; i++) {
        orxObject_Enable(portraits[i], 0);
    }

    // Fix up the button0 text, so other texts are clickable.
    soloscuro_get_button_text_object(win, win_id, 0, &tmp);
    orxObject_GetSize(tmp, &size);
    orxObject_GetPosition(tmp, &pos);
    size.fY = 1;
    pos.fY = 0;
    orxObject_SetSize(tmp, &size);
    orxObject_SetPosition(tmp, &pos);

    dice_flip = make_object(win, "DiceObject");
    orxObject_GetPosition(dice_button, &pos);
    pos.fX += 10;
    pos.fY -= 4;
    orxObject_SetPosition(dice_flip, &pos);

    orxObject_SetTextString(str_button, "STR 12");
    orxObject_SetTextString(dex_button, "DEX 12");
    orxObject_SetTextString(con_button, "CON 12");
    orxObject_SetTextString(int_button, "INT  12");
    orxObject_SetTextString(wis_button, "WIS  12");
    orxObject_SetTextString(cha_button, "CHA 12");

    orxObject_GetPosition(alignment, &pos);
    orxObject_GetSize(alignment, &size);
    pos.fY -= 8;
    size.fY = 40;
    size.fX = 140;
    orxObject_SetPosition(alignment, &pos);
    orxObject_SetSize(alignment, &size);
    orxObject_SetTextString(alignment, "MALE HUMAN\nTRUE NEUTRAL\nFIGHTER\n3    EXP:(4000) 8000\nAC: 9 DAM: 1.5 X 1D1+8");

    orxObject_GetPosition(hp, &pos);
    orxObject_GetSize(hp, &size);
    pos.fY -= 3;
    size.fY = 20;
    size.fX = 140;
    orxObject_SetPosition(hp, &pos);
    orxObject_SetSize(hp, &size);
    orxObject_SetTextString(hp, "19/19\n27/27");
    //orxObject_GetPosition(str_button, &pos);
    //pos.fZ += .5;
    //orxObject_SetPosition(str_button, &pos);


    // Once we have all the gui items, load the player character.
    init_pc(state);
    change_race_gender(state, 0);
    orxEvent_AddHandlerWithContext(orxEVENT_TYPE_ANIM, anim_event_handler, state);
    orxEvent_AddHandler(orxEVENT_TYPE_RENDER, render_event_handler);
    return EXIT_SUCCESS;
}

static void set_class(soloscuro_state_t *state, int sel) {
    int next_class =
        (pc.class[0].class == -1) ? 0 :
        (pc.class[1].class == -1) ? 1 :
        (pc.class[2].class == -1) ? 2 :
        3;
    int idx = has_selected_class(sel);

    if (!strcmp(orxObject_GetCurrentAnim(class_buttons[sel]), "Disabled")) {
        return;
    }
    if (idx > 0) {
        for (int i = idx - 1; i < 2; i++) {
            pc.class[i] = pc.class[i + 1];
        }
        pc.class[next_class - 1].class = -1;
    } else {
        pc.class[next_class].class = convert_to_actual_class(sel);
    }
    update_ui(state);
}

static int change_alignment(soloscuro_state_t *state, int diff) {
    pc.alignment += diff;
    if (pc.alignment >= CHAOTIC_EVIL) {
        pc.alignment = (diff == 1) ? LAWFUL_GOOD : CHAOTIC_NEUTRAL;
    } else if (pc.alignment < LAWFUL_GOOD) {
        pc.alignment = CHAOTIC_NEUTRAL;
    } else if (pc.alignment == LAWFUL_EVIL) {
        pc.alignment = (diff == 1) ? NEUTRAL_GOOD : LAWFUL_NEUTRAL;
    } else if (pc.alignment == NEUTRAL_EVIL) {
        pc.alignment = (diff == 1) ? CHAOTIC_GOOD : TRUE_NEUTRAL;
    }
    update_ui(state);
}

static int change_hp(soloscuro_state_t *state, int diff) {
    int min_hp = 0, max_level = 0;

    for (int i = 0; i < 3; i++) {
        if (pc.class[i].class <= 0) { continue; }
        min_hp += pc.class[i].level;
        max_level = max_level > pc.class[i].level ? max_level : pc.class[i].level;
    }

    min_hp += max_level * sol_dnd2e_hp_mod(&pc.stats);

    pc.stats.hp += diff;

    if (pc.stats.hp > sol_dnd2e_get_max_hp(&pc)) {
        pc.stats.hp = min_hp;
    }

    if (pc.stats.hp < min_hp) {
        pc.stats.hp = sol_dnd2e_get_max_hp(&pc);
    }

    update_ui(state);
}

static int stat_check(uint8_t *stat, int slot) {
    if (*stat < sol_dnd2e_race_get_stats(pc.race, slot, 0)) {
        *stat = sol_dnd2e_race_get_stats(pc.race, slot, 1);
    }
    if (*stat > sol_dnd2e_race_get_stats(pc.race, slot, 1)) {
        *stat = sol_dnd2e_race_get_stats(pc.race, slot, 0);
    }
}

static int change_str(soloscuro_state_t *state, int diff) {
    pc.stats.str += diff;
    stat_check(&pc.stats.str, 0);
    update_ui(state);
}

static int change_dex(soloscuro_state_t *state, int diff) {
    pc.stats.dex += diff;
    stat_check(&pc.stats.dex, 1);
    update_ui(state);
}

static int change_con(soloscuro_state_t *state, int diff) {
    pc.stats.con += diff;
    stat_check(&pc.stats.con, 1);
    update_ui(state);
}

static int change_int(soloscuro_state_t *state, int diff) {
    pc.stats.intel += diff;
    stat_check(&pc.stats.intel, 1);
    update_ui(state);
}

static int change_wis(soloscuro_state_t *state, int diff) {
    pc.stats.wis += diff;
    stat_check(&pc.stats.wis, 1);
    update_ui(state);
}

static int change_cha(soloscuro_state_t *state, int diff) {
    pc.stats.cha += diff;
    stat_check(&pc.stats.cha, 1);
    update_ui(state);
}

extern int soloscuro_ds1_new_character_click(soloscuro_state_t *state, int button_num) {
    //printf("new-char button: %d\n", button_num);
    switch (button_num) {
        case 0: return change_race_gender(state,
                            (orxInput_HasNewStatus("RightClick") ? -1 : 1));
        case 9: 
                orxObject_SetCurrentAnim(dice_flip, "DiceFlip");
                orxObject_SetTargetAnim(dice_flip, "Rest");
                randomize_pc(state);
                break;
        case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8:
                set_class(state, button_num - 1);
                break;
        case 13: return change_alignment(state, orxInput_HasNewStatus("RightClick") ? -1 : 1);
        case 14: return change_str(state, orxInput_HasNewStatus("RightClick") ? -1 : 1);
        case 15: return change_dex(state, orxInput_HasNewStatus("RightClick") ? -1 : 1);
        case 16: return change_con(state, orxInput_HasNewStatus("RightClick") ? -1 : 1);
        case 17: return change_int(state, orxInput_HasNewStatus("RightClick") ? -1 : 1);
        case 18: return change_wis(state, orxInput_HasNewStatus("RightClick") ? -1 : 1);
        case 19: return change_cha(state, orxInput_HasNewStatus("RightClick") ? -1 : 1);
        case 20: return change_hp(state, orxInput_HasNewStatus("RightClick") ? -1 : 1);
    }
    return EXIT_SUCCESS;
}

static void set_sphere(int offset) {
    int idx;
    if ((idx = has_class(REAL_CLASS_AIR_CLERIC)) > 0) {
        pc.class[idx - 1].class = REAL_CLASS_AIR_CLERIC + offset;
    }
    if ((idx = has_class(REAL_CLASS_EARTH_CLERIC)) > 0) {
        pc.class[idx - 1].class = REAL_CLASS_AIR_CLERIC + offset;
    }
    if ((idx = has_class(REAL_CLASS_FIRE_CLERIC)) > 0) {
        pc.class[idx - 1].class = REAL_CLASS_AIR_CLERIC + offset;
    }
    if ((idx = has_class(REAL_CLASS_WATER_CLERIC)) > 0) {
        pc.class[idx - 1].class = REAL_CLASS_AIR_CLERIC + offset;
    }
    if ((idx = has_class(REAL_CLASS_AIR_DRUID)) > 0) {
        pc.class[idx - 1].class = REAL_CLASS_AIR_DRUID + offset;
    }
    if ((idx = has_class(REAL_CLASS_EARTH_DRUID)) > 0) {
        pc.class[idx - 1].class = REAL_CLASS_AIR_DRUID + offset;
    }
    if ((idx = has_class(REAL_CLASS_FIRE_DRUID)) > 0) {
        pc.class[idx - 1].class = REAL_CLASS_AIR_DRUID + offset;
    }
    if ((idx = has_class(REAL_CLASS_WATER_DRUID)) > 0) {
        pc.class[idx - 1].class = REAL_CLASS_AIR_DRUID + offset;
    }
    if ((idx = has_class(REAL_CLASS_AIR_RANGER)) > 0) {
        pc.class[idx - 1].class = REAL_CLASS_AIR_RANGER + offset;
    }
    if ((idx = has_class(REAL_CLASS_EARTH_RANGER)) > 0) {
        pc.class[idx - 1].class = REAL_CLASS_AIR_RANGER + offset;
    }
    if ((idx = has_class(REAL_CLASS_FIRE_RANGER)) > 0) {
        pc.class[idx - 1].class = REAL_CLASS_AIR_RANGER + offset;
    }
    if ((idx = has_class(REAL_CLASS_WATER_RANGER)) > 0) {
        pc.class[idx - 1].class = REAL_CLASS_AIR_RANGER + offset;
    }
}

extern int soloscuro_ds1_sphere_click(soloscuro_state_t *state, int button_num) {
    //printf("sphere button: %d\n", button_num);
    orxVECTOR pos;
    switch (button_num) {
        case 0: set_sphere(0); break;
        case 1: set_sphere(1); break;
        case 2: set_sphere(2); break;
        case 3: set_sphere(3); break;
        case 4:
            orxObject_GetPosition(win_spheres, &pos);
            pos.fX = 600;
            orxObject_SetPosition(win_spheres, &pos);
            orxObject_GetPosition(win_psionics, &pos);
            pos.fX = 200;
            orxObject_SetPosition(win_psionics, &pos);
            update_ui(state);
            break;
    }
}

static void set_psionic(int psi) {
    if (has_class(REAL_CLASS_PSIONICIST)) { return; }

    pc.psi.types[0] = psi;
}

extern int soloscuro_ds1_psionic_click(soloscuro_state_t *state, int button_num) {
    //printf("psionic button: %d\n", button_num);
    orxVECTOR pos;
    switch (button_num) {
        case 0: set_psionic(1); break;
        case 1: set_psionic(2); break;
        case 2: set_psionic(3); break;
        case 4:
            orxObject_GetPosition(win_spheres, &pos);
            pos.fX = 200;
            orxObject_SetPosition(win_spheres, &pos);
            orxObject_GetPosition(win_psionics, &pos);
            pos.fX = 600;
            orxObject_SetPosition(win_psionics, &pos);
            update_ui(state);
            break;
    }
}

static void set_button_text(orxOBJECT *obj, const char *name, const int32_t val) {
    char buf[128];

    snprintf(buf, 128, "%s %d", name, val);
    buf[127] = '\0';

    orxObject_SetTextString(obj, buf);
}

static void set_str(int val) { set_button_text(str_button, "STR", pc.stats.str); }
static void set_dex(int val) { set_button_text(dex_button, "DEX", pc.stats.dex); }
static void set_con(int val) { set_button_text(con_button, "CON", pc.stats.con); }
static void set_int(int val) { set_button_text(int_button, "INT", pc.stats.intel); }
static void set_wis(int val) { set_button_text(wis_button, "WIS", pc.stats.wis); }
static void set_cha(int val) { set_button_text(cha_button, "CHA", pc.stats.cha); }

static void update_ui(soloscuro_state_t *state) {
    orxObject_SetTextString(name_box, pc.name);
    int is_divine = 0;
    int next_class =
        (pc.class[0].class == -1) ? 0 :
        (pc.class[1].class == -1) ? 1 :
        (pc.class[2].class == -1) ? 2 :
        3;

    set_str(pc.stats.str);
    set_dex(pc.stats.dex);
    set_con(pc.stats.con);
    set_int(pc.stats.intel);
    set_wis(pc.stats.wis);
    set_cha(pc.stats.cha);

    if (next_class < 3) {
        for (int i = 0; i < 8; i++) {
            if (has_selected_class(i)) {
                orxObject_SetCurrentAnim(class_buttons[i], "Inactive");
                continue;
            }
            pc.class[next_class].class = convert_to_actual_class(i);
            //printf("%d\n", !sol_dnd2e_is_class_allowed(pc.race, pc.class));
            //orxObject_Enable(class_buttons[i], !sol_dnd2e_is_class_allowed(pc.race, pc.class));
            orxObject_SetCurrentAnim(class_buttons[i],
                !sol_dnd2e_is_class_allowed(pc.race, pc.class) ? "Inactive" : "Disabled");
        }
        pc.class[next_class].class = -1;
    } else {
        for (int i = 0; i < 8; i++) {
            orxObject_SetCurrentAnim(class_buttons[i],
                has_selected_class(i) ? "Inactive" : "Disabled");
        }
    }

    is_divine = is_divine_spell_user();

    for (int i = 0; i < 4; i++) {
        orxObject_SetCurrentAnim(sphere_button[i], is_divine ? "Inactive" : "Disabled");
    }

    for (int i = 0; i < 4; i++) {
        //soloscuro_get_button_object(win_spheres, 3013, i, &sphere_button[i]);
        //soloscuro_get_button_object(win_psionics, 3013, i, &psionic_button[i]);
    }

    int pos = 0;
    sol_item_t *items = pc.inv;
    char storage[1024];
#define BUF_MAX (1024)
    int attack_num = dnd2e_get_attack_num(&pc, items ? items + 3 : NULL); 
    int sides = dnd2e_get_attack_sides_pc(&pc, items ? items + 3 : NULL);
    int mod = dnd2e_get_attack_mod_pc(&pc, items ? items + 3 : NULL);

    pos += snprintf(storage + pos, BUF_MAX - pos, "%s %s\n",
            pc.gender == GENDER_MALE ? "MALE" : "FEMALE",
            (pc.race == RACE_HUMAN) ? "HUMAN"
            : (pc.race == RACE_ELF) ? "ELF"
            : (pc.race == RACE_MUL) ? "MUL"
            : (pc.race == RACE_DWARF) ? "DWARF"
            : (pc.race == RACE_HALFELF) ? "HALFELF"
            : (pc.race == RACE_HALFGIANT) ? "HALFGIANT"
            : "THRIKREEN");
    pos += snprintf(storage + pos, BUF_MAX - pos, "%s\n",
            (pc.alignment == LAWFUL_GOOD) ? "LAWFUL GOOD"
            : (pc.alignment == NEUTRAL_GOOD) ? "NEUTRAL GOOD"
            : (pc.alignment == CHAOTIC_GOOD) ? "CHAOTIC GOOD"
            : (pc.alignment == LAWFUL_NEUTRAL) ? "LAWFUL NEUTRAL"
            : (pc.alignment == TRUE_NEUTRAL) ? "TRUE NEUTRAL"
            : (pc.alignment == CHAOTIC_NEUTRAL) ? "CHAOTIC NEUTRAL"
            : (pc.alignment == LAWFUL_EVIL) ? "LAWFUL EVIL"
            : (pc.alignment == NEUTRAL_EVIL) ? "NEUTRAL EVIL"
            : "CHAOTIC EVIL");
    for (int i = 0; i < 3; i++) {
        if (pc.class[i].class > 0) {
            pos += snprintf(storage + pos, BUF_MAX - pos, "%s%d",
                    i == 0 ? "" : "/",
                    pc.class[i].level);
        } else {
            pos += snprintf(storage + pos, BUF_MAX - pos, "  ");
        }
    }
    uint32_t nlevel;
    sol_dnd2e_next_level_exp(pc.class[0].class, pc.class[0].level + 1, &nlevel);
    if (pc.class[0].class > 0) {
        pos += snprintf(storage + pos, BUF_MAX - pos, "EXP: %d (%d)",
                pc.class[0].current_xp,
                nlevel);
    }
    pos += snprintf(storage + pos, BUF_MAX - pos, "\n");

    pos += snprintf(storage + pos, BUF_MAX - pos, "AC %d ",
                    dnd2e_get_ac_pc(&pc));
    pos += snprintf(storage + pos, BUF_MAX - pos, "DAM: %d%s",
                   attack_num >> 1, attack_num & 0x01 ? ".5" : "");
    pos += snprintf(storage + pos, BUF_MAX - pos, "x1D%d", sides);
    pos += snprintf(storage + pos, BUF_MAX - pos, "+%d", mod);
    pos += snprintf(storage + pos, BUF_MAX - pos, "\n");

    //orxObject_SetTextString(alignment, "MALE HUMAN\nTRUE NEUTRAL\nFIGHTER\n3    EXP:(4000) 8000\nAC: 9 DAM: 1.5 X 1D1+8");
    orxObject_SetTextString(alignment, storage);

    pos = 0;
    pos += snprintf(storage + pos, BUF_MAX - pos, "%2d/%2d\n",
            pc.stats.hp,
            pc.stats.hp);
    pos += snprintf(storage + pos, BUF_MAX - pos, "%2d/%2d\n",
            pc.stats.high_psp,
            pc.stats.high_psp);
    orxObject_SetTextString(hp, storage);
}

static void randomize_pc(soloscuro_state_t *state) {
    gff_name_get_random(state->man, pc.name, 32, pc.race, pc.gender);

    sol_dnd2e_randomize_stats_pc(&pc);
    sol_dnd2e_loop_creation_stats(&pc); // in case something need adjustment
    sol_item_set_starting(state, &pc);

    update_ui(state);
}

// Logic for handling character
static void init_pc(soloscuro_state_t *state) {
    memset(&pc, 0x0, sizeof(sol_entity_t));
    pc.race = RACE_HUMAN;
    pc.gender = GENDER_MALE;
    pc.alignment = TRUE_NEUTRAL;
    pc.class[0].class = pc.class[1].class = pc.class[2].class = -1;
    pc.class[0].level = pc.class[1].level = pc.class[2].level = -1;
    //memset(&psi, 0x0, sizeof(psi));
    //memset(&spells, 0x0, sizeof(spells));
    //memset(&psionics, 0x0, sizeof(psionics));
    pc.allegiance = 1;

    randomize_pc(state);
}

static int has_class(const uint16_t class) {
    for (int i = 0; i < 3; i++) {
        if (pc.class[i].class == class) { return i + 1; }
    }
    return 0;
}
  
static int is_divine_spell_user() {
    return   has_class(REAL_CLASS_AIR_CLERIC)
          || has_class(REAL_CLASS_EARTH_CLERIC)
          || has_class(REAL_CLASS_FIRE_CLERIC)
          || has_class(REAL_CLASS_WATER_CLERIC)
          || has_class(REAL_CLASS_AIR_DRUID)
          || has_class(REAL_CLASS_EARTH_DRUID)
          || has_class(REAL_CLASS_FIRE_DRUID)
          || has_class(REAL_CLASS_WATER_DRUID)
          || has_class(REAL_CLASS_AIR_RANGER)
          || has_class(REAL_CLASS_EARTH_RANGER)
          || has_class(REAL_CLASS_FIRE_RANGER)
          || has_class(REAL_CLASS_WATER_RANGER);
}

static int has_selected_class(int class) {
    int idx;
    switch (class) {
        case 0:
            if ((idx = has_class(REAL_CLASS_AIR_CLERIC))) { return idx; }
            if ((idx = has_class(REAL_CLASS_EARTH_CLERIC))) { return idx; }
            if ((idx = has_class(REAL_CLASS_FIRE_CLERIC))) { return idx; }
            if ((idx = has_class(REAL_CLASS_WATER_CLERIC))) { return idx; }
            return 0;
        case 1:
            if ((idx = has_class(REAL_CLASS_AIR_DRUID))) { return idx; }
            if ((idx = has_class(REAL_CLASS_EARTH_DRUID))) { return idx; }
            if ((idx = has_class(REAL_CLASS_FIRE_DRUID))) { return idx; }
            if ((idx = has_class(REAL_CLASS_WATER_DRUID))) { return idx; }
            return 0;
        case 2: return has_class(REAL_CLASS_FIGHTER);
        case 3: return has_class(REAL_CLASS_GLADIATOR);
        case 4: return has_class(REAL_CLASS_PRESERVER);
        case 5: return has_class(REAL_CLASS_PSIONICIST);
        case 6:
            if ((idx = has_class(REAL_CLASS_AIR_RANGER))) { return idx; }
            if ((idx = has_class(REAL_CLASS_EARTH_RANGER))) { return idx; }
            if ((idx = has_class(REAL_CLASS_FIRE_RANGER))) { return idx; }
            if ((idx = has_class(REAL_CLASS_WATER_RANGER))) { return idx; }
            return 0;
        case 7: return has_class(REAL_CLASS_THIEF);
    }

    return 0;
}
