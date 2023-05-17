#ifndef DS1_WINDOW_H
#define DS1_WINDOW_H

#include <gff/gui.h>
#include <stdint.h>

#include "soloscuro/state.h"
#include "soloscuro/window.h"

enum ds1_windows_e {
    DS1_WINDOW_MAINMENU = 3000,
    DS1_WINDOW1 = 3001,
    DS1_WINDOW2 = 3004,
    DS1_WINDOW_NARRATE = 3007,
    DS1_WINDOW4 = 3008,
    DS1_WINDOW_SAVE = 3009,
    DS1_WINDOW_NEW_CHARACTER = 3011,
    DS1_WINDOW_GEN_PSIONIC = 3012,
    DS1_WINDOW_GEN_SPHERES = 3013,
    DS1_WINDOW_INTERACT = 3020,
    DS1_WINDOW10 = 3024, // SAVE?
    DS1_WINDOW11 = 3500,
    DS1_WINDOW_GAME_MENU = 10500,
    DS1_WINDOW_MESSAGE = 10501,
    DS1_WINDOW_VIEW_CHARACTER = 11500,
    DS1_WINDOW_INVENTORY = 13500,
    DS1_WINDOW16 = 13501, // Body for container selection?
    DS1_WINDOW_POPUP = 14000,
    DS1_WINDOW_INSPECT = 14001,
    DS1_WINDOW_ALERT2 = 14002,
    DS1_WINDOW_INSPECT2 = 15500,
    DS1_WINDOW_VIEW_ITEM = 15502,
    DS1_WINDOW_SPELL_INFO = 15503,
    DS1_WINDOW_PREFERENCES = 16500,
    DS1_WINDOW_SPELL_TRAIN = 17500,
    DS1_WINDOW_PSI_TRAIN = 17501,
    DS1_WINDOW_DUAL = 17502,
};

extern int32_t       soloscuro_window_background(int win_id);
extern int32_t       soloscuro_window_backgroundx(int win_id);
extern int32_t       soloscuro_window_backgroundy(int win_id);
extern gff_button_t* soloscuro_window_button_extra(int win_id, int index);

extern int soloscuro_ds1_mainmenu_click(soloscuro_state_t *state, int button_num);
extern int soloscuro_ds1_mainmenu_key_down(soloscuro_state_t *state, char c);

extern int soloscuro_create_window(soloscuro_state_t *state, int win_id);
extern int soloscuro_create_popup_message(soloscuro_state_t *state, const char *msg);

extern int soloscuro_ds1_view_character_init(soloscuro_state_t *state);
extern int soloscuro_ds1_view_character_select(soloscuro_state_t *state);
extern int soloscuro_ds1_view_character_click(soloscuro_state_t *state, int button_num);
extern int soloscuro_ds1_view_character_key_down(soloscuro_state_t *state, char c);
extern int soloscuro_ds1_view_character_return(soloscuro_state_t *state);

extern int soloscuro_ds1_new_character_init(soloscuro_state_t *state);
extern int soloscuro_ds1_new_character_click(soloscuro_state_t *state, int button_num);

extern int soloscuro_ds1_popup_init(soloscuro_state_t *state);
extern int soloscuro_ds1_popup_click(soloscuro_state_t *state, int button_num);
extern int soloscuro_ds1_popup_create(soloscuro_state_t *state, char *msg, char *b0, char *b1, char *b2);

extern int soloscuro_get_button_object(orxOBJECT *win, int32_t win_id, int32_t button_id, orxOBJECT **button);
extern int soloscuro_get_button_text_object(orxOBJECT *win, int32_t win_id, int32_t button_id, orxOBJECT **button);
extern int soloscuro_get_ebox_object(orxOBJECT *win, int32_t win_id, int32_t ebox_id, orxOBJECT **ebox);

extern int soloscuro_ds1_sphere_click(soloscuro_state_t *state, int button_num);
extern int soloscuro_ds1_psionic_click(soloscuro_state_t *state, int button_num);
#endif
