#ifndef SOLOSCURO_STATE_H
#define SOLOSCURO_STATE_H

#include <orx.h>

#include <gff/manager.h>

#define MAX_WINDOWS (8)

typedef struct soloscuro_gui_window_entry_s {
    orxOBJECT *win;
    int32_t    win_id;
} soloscuro_gui_window_entry_t;

typedef struct soloscuro_gui_state_s {
    orxOBJECT *ebox;
    int32_t    win_pos;
    int32_t    last_selection;
    soloscuro_gui_window_entry_t win_stack[MAX_WINDOWS];
} soloscuro_gui_state_t;

typedef struct soloscuro_state_s {
    soloscuro_gui_state_t gui;
    gff_manager_t *man;
} soloscuro_state_t;

extern soloscuro_state_t* soloscuro_state_create();
extern void soloscuro_state_free(soloscuro_state_t *state);

#endif
