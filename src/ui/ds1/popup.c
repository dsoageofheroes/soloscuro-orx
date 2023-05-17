#include "soloscuro.h"
#include "soloscuro/window.h"

static orxOBJECT *win;
static int32_t    id;
static orxOBJECT *text;
static orxOBJECT *selection[3];
static orxOBJECT *game_return;

extern int soloscuro_ds1_popup_init(soloscuro_state_t *state) {
    orxVECTOR pos;

    soloscuro_get_top_window(state, &win, &id);
    soloscuro_get_button_text_object(win, id, 1, &selection[0]);
    soloscuro_get_button_text_object(win, id, 2, &selection[1]);
    soloscuro_get_button_text_object(win, id, 3, &selection[2]);
    soloscuro_get_button_object(win, id, 4, &game_return);
    soloscuro_get_button_text_object(win, id, 0, &text);

    orxObject_SetTextString(text, "");
}

static int select_button(soloscuro_state_t *state, int sel_num) {
    state->gui.last_selection = sel_num;
    return soloscuro_destroy_top_window(state);
}

extern int soloscuro_ds1_popup_create(soloscuro_state_t *state, char *msg, char *b0, char *b1, char *b2) {
    soloscuro_create_window(state, DS1_WINDOW_POPUP);
    orxObject_SetTextString(text, msg);
    orxObject_SetTextString(selection[0], b0);
    orxObject_SetTextString(selection[1], b1);
    orxObject_SetTextString(selection[2], b2);
}

extern int soloscuro_ds1_popup_click(soloscuro_state_t *state, int button_num) {
    switch (button_num) {
        case 1: return select_button(state, 0);
        case 2: return select_button(state, 1);
        case 3: return select_button(state, 2);
        case 4: return select_button(state, -1);
    }
}
