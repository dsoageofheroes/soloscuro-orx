#ifndef SOLOSCURO_H
#define SOLOSCURO_H

#include <gff/manager.h>
#include "soloscuro/state.h"

extern gff_manager_t* soloscuro_get_gff_manager();
extern int soloscuro_register_gff_type(soloscuro_state_t *state);
extern int soloscuro_generate_window(soloscuro_state_t *state, int window_id);
extern int soloscuro_find_first_child_with(orxOBJECT *obj, const char *str, orxOBJECT **out);
extern int soloscuro_find_first_child(orxOBJECT *object, const char *str, orxOBJECT **out);
extern int soloscuro_get_top_window(soloscuro_state_t *state, orxOBJECT **out, int32_t *id);
extern int soloscuro_destroy_top_window(soloscuro_state_t *state);

#endif
