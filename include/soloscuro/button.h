#ifndef SOLOSCURO_BUTTON_H
#define SOLOSCURO_BUTTON_H

#include <orx.h>

#include "soloscuro/state.h"

extern int soloscuro_ds1button_handle(soloscuro_state_t *state, orxVECTOR *mouse_pos);
extern int soloscuro_ds1key_handle(soloscuro_state_t *state);

extern char* soloscuro_ds1button_font(int32_t win_id, int32_t button_id);
extern char* soloscuro_ds1ebox_font(int32_t win_id, int32_t button_id);

#endif
