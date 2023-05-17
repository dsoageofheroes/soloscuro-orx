#include "soloscuro.h"
#include "soloscuro/window.h"

extern int soloscuro_create_popup_message(soloscuro_state_t *state, const char *msg) {
    orxOBJECT *message = NULL;
    orxOBJECT *text = NULL;
    orxVECTOR  pos;
    size_t     len = strlen(msg), cpos = 0;
    char buf[64];

    memset(buf, ' ', 32);
    buf[30] = '\0';
    cpos = (30 - len);

    if (len >= 30) { cpos = 0; }

    strncpy(buf + cpos, msg, len);

    pos.fX = 150;
    pos.fY = 150;
    pos.fZ = -.9;
    soloscuro_create_window(state, DS1_WINDOW_MESSAGE);
    message = state->gui.win_stack[state->gui.win_pos - 1].win;
    orxObject_SetPosition(message, &pos);

    soloscuro_find_first_child_with(message, "Object", &text);

    for(orxOBJECT *obj2 = orxOBJECT(orxObject_GetChild(text));
        obj2 != orxNULL;
        obj2 = orxOBJECT(orxObject_GetSibling(obj2))) {
        //orxObject_SetTextString(obj2, msg);
        orxObject_SetTextString(obj2, buf);
    }

    orxObject_AddTimeLineTrack(message, "MessageWindowTrack");
    orxObject_EnableTimeLine(message, orxTRUE);

    // Popup mesage is a special window that gets destroyed,
    // so lets remove it from the stack.
    state->gui.win_pos--;
}
