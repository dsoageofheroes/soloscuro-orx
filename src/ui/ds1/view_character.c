#include "soloscuro.h"
#include "soloscuro/window.h"

static orxOBJECT *win;
static int32_t    id;
static orxOBJECT *character, *inv, *spells, *psionics;
static orxOBJECT *pb, *pbl;
static orxOBJECT *game_menu, *game_return;
static orxOBJECT *players[4], *ai[4], *leader[4];
static orxOBJECT *character_ebox;
static int32_t    last_click = -1;

static int character_click(soloscuro_state_t *state) {
    soloscuro_create_popup_message(state, "char not implemented");
    return EXIT_FAILURE;
}

static int inventory_click(soloscuro_state_t *state) {
    soloscuro_create_popup_message(state, "inventory not implemented");
    return EXIT_FAILURE;
}

static int spells_click(soloscuro_state_t *state) {
    soloscuro_create_popup_message(state, "spells not implemented");
    return EXIT_FAILURE;
}

static int psionics_click(soloscuro_state_t *state) {
    soloscuro_create_popup_message(state, "psionics not implemented");
    return EXIT_FAILURE;
}

// mage/cleric/psionic
static int powers_click(soloscuro_state_t *state) {
    //soloscuro_create_popup_message(state, "powers not implemented");
    return EXIT_FAILURE;
}

static int power_level_click(soloscuro_state_t *state) {
    //soloscuro_create_popup_message(state, "power level not implemented");
    return EXIT_FAILURE;
}

static int game_menu_click(soloscuro_state_t *state) {
    soloscuro_create_popup_message(state, "game menu not implemented");
    return EXIT_FAILURE;
}

static int game_return_click(soloscuro_state_t *state) {
    //soloscuro_create_popup_message(state, "game return not implemented");
    //return EXIT_FAILURE;
    return soloscuro_destroy_top_window(state);
}

extern int soloscuro_ds1_view_character_init(soloscuro_state_t *state) {
    soloscuro_get_top_window(state, &win, &id);

    printf("View Character Init: %p, %d\n", win, id);

    // grab all the buttons
    soloscuro_get_button_object(win, id, 1, &character);
    soloscuro_get_button_object(win, id, 2, &inv);
    soloscuro_get_button_object(win, id, 3, &spells);
    soloscuro_get_button_object(win, id, 4, &psionics);
    soloscuro_get_button_object(win, id, 5, &pb);
    soloscuro_get_button_object(win, id, 6, &pbl);
    soloscuro_get_button_object(win, id, 7, &game_menu);
    soloscuro_get_button_object(win, id, 8, &game_return);
    for (int i = 0; i < 4; i++) {
        soloscuro_get_button_object(win, id, 9 + i, &players[i]);
        soloscuro_get_button_object(win, id, 13 + i, &leader[i]);
        soloscuro_get_button_object(win, id, 17 + i, &ai[i]);
    }
    soloscuro_get_ebox_object(win, id, 0, &character_ebox);

    last_click = -1;

    return soloscuro_ds1_view_character_select(state);
}

extern int soloscuro_ds1_view_character_select(soloscuro_state_t *state) {
    //state->gui.ebox = character_ebox;
    state->gui.ebox = NULL;

    for (int i = 0; i < 4; i++) {
        orxObject_SetCurrentAnim(players[i], "Disabled");
        orxObject_SetCurrentAnim(leader[i], "Disabled");
        orxObject_SetCurrentAnim(ai[i], "Disabled");
    }

    // Right now disable power buttons
    orxObject_Enable(pb, 0);
    orxObject_Enable(pbl, 0);

    orxObject_SetTextString(character_ebox, "");
}

static int player_click(soloscuro_state_t *state, int button_num) {
    last_click = 1;// player
    if (orxInput_HasNewStatus("RightClick") == orxTRUE) {
        return soloscuro_ds1_popup_create(state, "INACTIVE CHARACTER", "NEW", "ADD", "CANCEL");
    }
}

extern int soloscuro_ds1_view_character_click(soloscuro_state_t *state, int button_num) {
    printf("button_num = %d\n", button_num);
    switch(button_num) {
        case 1: return character_click(state);
        case 2: return inventory_click(state);
        case 3: return spells_click(state);
        case 4: return psionics_click(state);
        case 5: return powers_click(state);
        case 6: return power_level_click(state);
        case 7: return game_menu_click(state);
        case 8: return game_return_click(state);
        case 9: case 10: case 11: case 12: return player_click(state, 12 - button_num);

    }
}

static int player_click_return(soloscuro_state_t *state) {
    switch (state->gui.last_selection) {
        case -1: case 2: goto finish;
        case 0:
            return soloscuro_create_popup_message(state, "New Char Not implemented");
            break;
        case 1:
            return soloscuro_create_popup_message(state, "Add Char Not implemented");
            break;
    }

finish:
    return EXIT_SUCCESS;
}

extern int soloscuro_ds1_view_character_return(soloscuro_state_t *state) {
    switch (last_click) {
        case 1: return player_click_return(state);
    }

    return EXIT_SUCCESS;
}

extern int soloscuro_ds1_view_character_key_down(soloscuro_state_t *state, char c) {
}
