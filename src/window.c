#include "soloscuro.h"
#include "soloscuro/window.h"

extern int soloscuro_create_window(soloscuro_state_t *state, int win_id) {
    char config[128];
    orxOBJECT *win = orxNULL;
    orxVECTOR pos;
    float delta = -.9 / MAX_WINDOWS;

    if (state->gui.win_pos == MAX_WINDOWS) {
        goto too_many_windows;
    }

    snprintf(config, 127, "Window%d", win_id);
    config[127] = '\0'; // guard

    win = orxObject_CreateFromConfig(config);

    if (win == orxNULL) { goto create_error;}

    state->gui.win_stack[state->gui.win_pos].win = win;
    state->gui.win_stack[state->gui.win_pos].win_id = win_id;
    state->gui.win_pos++;

    orxObject_GetPosition(win, &pos);
    pos.fZ = delta * state->gui.win_pos;
    orxObject_SetPosition(win, &pos);

    // Now call any init function
    switch(win_id) {
        case DS1_WINDOW_MAINMENU:
        case DS1_WINDOW1:
        case DS1_WINDOW2:
        case DS1_WINDOW_NARRATE:
        case DS1_WINDOW4:
        case DS1_WINDOW_SAVE:
        case DS1_WINDOW_GEN_PSIONIC:
        case DS1_WINDOW_GEN_SPHERES:
        case DS1_WINDOW_INTERACT:
        case DS1_WINDOW10:
        case DS1_WINDOW11:
        case DS1_WINDOW_GAME_MENU:
        case DS1_WINDOW_MESSAGE:
        case DS1_WINDOW_INVENTORY:
        case DS1_WINDOW16:
        case DS1_WINDOW_INSPECT:
        case DS1_WINDOW_ALERT2:
        case DS1_WINDOW_INSPECT2:
        case DS1_WINDOW_VIEW_ITEM:
        case DS1_WINDOW_SPELL_INFO:
        case DS1_WINDOW_PREFERENCES:
        case DS1_WINDOW_SPELL_TRAIN:
        case DS1_WINDOW_PSI_TRAIN:
        case DS1_WINDOW_DUAL:
            break;
        case DS1_WINDOW_NEW_CHARACTER: soloscuro_ds1_new_character_init(state); break;
        case DS1_WINDOW_POPUP: soloscuro_ds1_popup_init(state); break;
        case DS1_WINDOW_VIEW_CHARACTER: soloscuro_ds1_view_character_init(state); break;
    }

    return EXIT_SUCCESS;
create_error:
too_many_windows:
    return EXIT_FAILURE;
}

//extern int soloscuro_ds1_view_character_init(soloscuro_state_t *state) {
