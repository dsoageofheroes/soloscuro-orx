#include "soloscuro.h"
#include "soloscuro/window.h"

static int start_game(soloscuro_state_t *state) {
    soloscuro_create_popup_message(state, "Start Game not implemented!");
    return EXIT_FAILURE;
}

static int create_character(soloscuro_state_t *state) {
    soloscuro_create_popup_message(state, "Create Chararacter not implemented!");
    return EXIT_FAILURE;
}

static int load_game(soloscuro_state_t *state) {
    soloscuro_create_popup_message(state, "Load game not implemented!");
    return EXIT_FAILURE;
}

static int exit_game(soloscuro_state_t *state) {
    //printf("DS1 Main Menu: exit game!\n");
    orxEvent_SendShort(orxEVENT_TYPE_SYSTEM, orxSYSTEM_EVENT_CLOSE);
    return EXIT_SUCCESS;
}

extern int soloscuro_ds1_mainmenu_key_down(soloscuro_state_t *state, char c) {
    switch (c) {
        case 's': return start_game(state);
        case 'c': return create_character(state);
        case 'l': return load_game(state);
        case 'e': return exit_game(state);
    }

    return EXIT_FAILURE;
}

extern int soloscuro_ds1_mainmenu_click(soloscuro_state_t *state, int button_num) {
    printf("DS1 MAIN MENU\n");
}
