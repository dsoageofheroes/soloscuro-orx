#include "soloscuro/gui.h"

#include <ctype.h>
#include <stdint.h>

#include "soloscuro/state.h"
#include "soloscuro/window.h"

static orxOBJECT *ebox = orxNULL;
static int button_activate(soloscuro_state_t *state, orxOBJECT *button);
static int key_activate(soloscuro_state_t *state, char key, int down);
static int key_down();

static orxOBJECT* find_object_in_list(orxOBJECT *parent, orxVECTOR *mouse_pos, orxVECTOR scale, orxVECTOR wp) {
    orxVECTOR pos, size;

    for(orxOBJECT *child = orxOBJECT(orxObject_GetChild(parent));
            child != orxNULL;
            child = orxOBJECT(orxObject_GetSibling(child))) {
        orxObject_GetPosition(child, &pos);
        orxObject_GetSize(child, &size);
        /*
        printf("%s: (%f, %f) -> (%f, %f), m:(%f, %f)\n", orxObject_GetName(child), wp.fX + pos.fX, wp.fY + pos.fY,
                wp.fX + pos.fX + size.fX, wp.fY + pos.fY + size.fY, mouse_pos->fX / scale.fX, mouse_pos->fY / scale.fY);
                */
        if ((wp.fX + pos.fX) <= mouse_pos->fX / scale.fX
                && (wp.fY + pos.fY) <= mouse_pos->fY / scale.fY
                && (wp.fX + pos.fX + size.fX) >= mouse_pos->fX / scale.fX
                && (wp.fY + pos.fY + size.fY) >= mouse_pos->fY / scale.fY
                ) {
            const char *name = orxObject_GetName(child);
            if (isdigit(name[strlen(name)-1]) || name[strlen(name) - 1] == 't') {
                //printf("%s: (%f, %f) -> (%f, %f), m:(%f, %f)\n", orxObject_GetName(child), wp.fX + pos.fX, wp.fY + pos.fY,
                        //wp.fX + pos.fX + size.fX, wp.fY + pos.fY + size.fY, mouse_pos->fX / scale.fX, mouse_pos->fY / scale.fY);
                //printf("name = '%s'\n", name);
                //orxObject_Enable(child, 1);
                //pos.fZ -= 0.1;
                return child;
            }
        }
    }
    return orxNULL;
}

static orxOBJECT* get_button_on_window(orxOBJECT *win, orxVECTOR *mouse_pos, orxVECTOR scale) {
    orxOBJECT *tmp;
    orxVECTOR  wp;
    int32_t    sub_win, c;

    orxObject_GetPosition(win, &wp);
    //printf("(%s):wp.fX = %f\n", orxObject_GetName(win), wp.fX);

    for(orxOBJECT *child = orxOBJECT(orxObject_GetChild(win));
            child != orxNULL;
            child = orxOBJECT(orxObject_GetSibling(child))) {
        if (sscanf(orxObject_GetName(child), "Window%d%c", &sub_win, &c) == 1) {
            tmp = get_button_on_window(child, mouse_pos, scale);
            if (tmp) { return tmp; }
        }
        tmp = find_object_in_list(child, mouse_pos, scale, wp);
        if (tmp) { return tmp; }
    }

    return orxNULL;
}

static orxOBJECT* get_button(soloscuro_state_t *state, orxVECTOR *mouse_pos) {
    orxOBJECT *win = state->gui.win_stack[state->gui.win_pos - 1].win;
    orxVECTOR scale;

    if (!win) { return orxNULL; }

    orxObject_GetScale(win, &scale);

    // Hopefully I can find a better way...
    return get_button_on_window(win, mouse_pos, scale);
    /*
    for(orxOBJECT *child = orxOBJECT(orxObject_GetChild(win));
            child != orxNULL;
            child = orxOBJECT(orxObject_GetSibling(child))) {
        if (sscanf(orxObject_GetName(child), "Window%d%c", &sub_win, &c) == 1) {
            printf("FOUND: win_num = %d\n", sub_win);
        }
        orxOBJECT *tmp = find_object_in_list(child, mouse_pos, scale);
        if (tmp) { return tmp; }
    }
    */
    return orxNULL;
}

extern int soloscuro_ds1button_handle(soloscuro_state_t *state, orxVECTOR *mouse_pos) {
    //orxOBJECT *obj = orxObject_Pick(mouse_pos, orxString_GetID("DS1Button"));
    orxOBJECT *obj = get_button(state, mouse_pos);
    orxOBJECT *tmp = orxNULL;
    static orxOBJECT *last_obj = orxNULL;
    static orxOBJECT *last_pressed = orxNULL;

    if (obj != last_obj) {
        if (last_obj != orxNULL) {
            //orxObject_SetCurrentAnim(last_obj, "Inactive");
            if (strcmp(orxObject_GetCurrentAnim(last_obj), "Disabled")) {
                orxObject_SetTargetAnim(last_obj, "Inactive");
            }
        }

        last_obj = obj;
    }

    if (last_pressed != orxNULL &&
            (orxInput_HasNewStatus("LeftClick")
             ||  orxInput_HasNewStatus("RightClick"))) {
        tmp = last_pressed;
        last_pressed = orxNULL;
        last_obj = last_pressed = orxNULL;
        return button_activate(state, tmp);
    }

    if (!obj) {
        goto no_object;
    }

    //orxObject_SetCurrentAnim(obj, "Active");
    if (strcmp(orxObject_GetCurrentAnim(obj), "Disabled")) {
        orxObject_SetTargetAnim(obj, "Active");
    }

    if (orxInput_IsActive("LeftClick") == orxTRUE
            || orxInput_IsActive("RightClick") == orxTRUE) {
        //orxObject_SetCurrentAnim(obj, "Pressed");
        if (strcmp(orxObject_GetCurrentAnim(obj), "Disabled")) {
            orxObject_SetTargetAnim(obj, "Pressed");
        }
        last_pressed = obj;
    }

no_object:
    return EXIT_SUCCESS;
}

typedef struct key_entry_s {
    const char *str;
    const char  key;
} key_entry_t;

static key_entry_t keys[] = {
    { "0", '0' },
    { "1", '1' },
    { "2", '2' },
    { "3", '3' },
    { "4", '4' },
    { "5", '5' },
    { "6", '6' },
    { "7", '7' },
    { "8", '8' },
    { "9", '9' },
    { "A", 'a' },
    { "B", 'b' },
    { "C", 'c' },
    { "D", 'd' },
    { "E", 'e' },
    { "F", 'f' },
    { "G", 'g' },
    { "H", 'h' },
    { "I", 'i' },
    { "J", 'j' },
    { "K", 'k' },
    { "L", 'l' },
    { "M", 'm' },
    { "N", 'n' },
    { "O", 'o' },
    { "P", 'p' },
    { "Q", 'q' },
    { "R", 'r' },
    { "S", 's' },
    { "T", 't' },
    { "U", 'u' },
    { "V", 'v' },
    { "W", 'w' },
    { "X", 'x' },
    { "Y", 'y' },
    { "Z", 'z' },
    { "SPACE", ' ' },
    { "QUOTE", '\'' },
    { "COMMA", ',' },
    { "DASH", '-' },
    { "PERIOD", '.' },
    { "SLASH", '/' },
    { "BACKSPACE", '\b' },
    { NULL, '\0' },
};

static char get_char_pressed() {
    key_entry_t *key = keys;
    char c = '\0';;

    while (key->str) {
        if (orxInput_IsActive(key->str) == orxTRUE && orxInput_HasNewStatus(key->str) == orxTRUE) {
            c = key->key;
            if (orxInput_IsActive("LSHIFT") || orxInput_IsActive("RSHIFT")) {
                if (c >= 'a' && c <= 'z') {
                    c = 'A' + (c - 'a');
                } else if (c == '\'') {
                    c = '"';
                }
            }

            return c;
        }
        key++;
    }

    return c;
}

static void ebox_update(orxOBJECT *ebox, char c) {
    key_entry_t *key = keys;
    static char msg[128];
    static int msg_pos = 0;
    //if (orxInput_IsActive("0") && orxInput_HasNewStatus("0") == orxTRUE) {

    if (c == '\0') {
        goto no_char;
    }

    // We have a char to add to the ebox.
    //printf("%s, ebox = %p\n", msg, ebox);
    orxGRAPHIC *g = orxObject_GetWorkingGraphic(ebox);
    orxVECTOR v;
    orxGraphic_GetSize(g, &v);

    if (v.fX / 6 < msg_pos) {
        return;
    }

    if (c == '\b') {
        if (msg_pos > 0) {
            msg_pos--;
            msg[msg_pos] = '\0';
        }
    } else {
        if (msg_pos < 128 - 1) {
            msg[msg_pos++] = c;
            msg[msg_pos] = '\0';
        }
    }

    orxObject_SetTextString(ebox, msg);

no_char:
    return;
}

static int key_down() {
    char c;

    if ((c = get_char_pressed()) == '\0') {
        goto no_char;
    }

no_char:
    return EXIT_FAILURE;
}

static int find_ebox(soloscuro_state_t *state) {
    const char *name = NULL;
    int len = 0;
    orxOBJECT *cobj = orxObject_GetNext(orxNULL, orxString_GetID("DS1EBox"));
    while (cobj) {
        name = orxObject_GetName(cobj);
        len = strlen(name) - 1;
        if (len > 4
                && name[len-3] == 'T'
                && name[len-2] == 'e'
                && name[len-1] == 'x') {
            goto success;
        }
        cobj = orxObject_GetNext(cobj, orxString_GetID("DS1EBox"));
    }

    return EXIT_FAILURE;
success:
    printf("%s\n", orxObject_GetName(cobj));
    state->gui.ebox = ebox = cobj;
    return EXIT_SUCCESS;
}

extern int soloscuro_ds1key_handle(soloscuro_state_t *state) {
    char c = get_char_pressed();
    //orxSTRUCTURE_GetCount(orxString_GetID("DS1Box"));
    //printf("%p\n", state);
    if (state->gui.ebox == orxNULL) {
        //find_ebox(state);
    }

    if (state->gui.ebox) {
        ebox_update(state->gui.ebox, c);
        goto ebox_handled;
    }

    return key_activate(state, c, 1);

ebox_handled:
    return EXIT_SUCCESS;

}

static int key_activate(soloscuro_state_t *state, char key, int down) {
    if (key == '\0') { goto no_char; }

    switch(state->gui.win_stack[state->gui.win_pos - 1].win_id) {
        case DS1_WINDOW_MAINMENU: if (down) { return soloscuro_ds1_mainmenu_key_down(state, key);}
        case DS1_WINDOW1:
        case DS1_WINDOW2:
        case DS1_WINDOW_NARRATE:
        case DS1_WINDOW4:
        case DS1_WINDOW_SAVE:
        case DS1_WINDOW_NEW_CHARACTER:
        case DS1_WINDOW_GEN_PSIONIC:
        case DS1_WINDOW_GEN_SPHERES:
        case DS1_WINDOW_INTERACT:
        case DS1_WINDOW10:
        case DS1_WINDOW11:
        case DS1_WINDOW_GAME_MENU:
        case DS1_WINDOW_MESSAGE:
        case DS1_WINDOW_VIEW_CHARACTER: if (down) { return soloscuro_ds1_view_character_key_down(state, key);}
        case DS1_WINDOW_INVENTORY:
        case DS1_WINDOW16:
        case DS1_WINDOW_POPUP:
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
    }

no_char:
    return EXIT_FAILURE;
}

static int button_activate(soloscuro_state_t *state, orxOBJECT *button) {
    int window_num, button_num;

    sscanf(orxObject_GetName(button), "Window%dButton%d", &window_num, &button_num);

    switch(window_num) {
        case DS1_WINDOW_MAINMENU: return soloscuro_ds1_mainmenu_click(state, button_num);
        case DS1_WINDOW_VIEW_CHARACTER: return soloscuro_ds1_view_character_click(state, button_num);
        case DS1_WINDOW_POPUP: return soloscuro_ds1_popup_click(state, button_num);
        case DS1_WINDOW_NEW_CHARACTER: return soloscuro_ds1_new_character_click(state, button_num);
        case DS1_WINDOW_GEN_SPHERES: return soloscuro_ds1_sphere_click(state, button_num);
        case DS1_WINDOW_GEN_PSIONIC: return soloscuro_ds1_psionic_click(state, button_num);
        case DS1_WINDOW1:
        case DS1_WINDOW2:
        case DS1_WINDOW_NARRATE:
        case DS1_WINDOW4:
        case DS1_WINDOW_SAVE:
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
    }

    printf("UNKNOWN CLICK on '%s': %d %d\n", orxObject_GetName(button), window_num, button_num);

    return EXIT_FAILURE;
}

extern char* soloscuro_ds1button_font(int32_t win_id, int32_t button_id) {
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
        case DS1_WINDOW_VIEW_CHARACTER:
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
        case DS1_WINDOW_NEW_CHARACTER:
            switch(button_id) {
                case 13: case 14: case 15: case 16: case 17: case 18: case 19: case 20:
                    return "DS1GreyLight";
            }
            break;
        case DS1_WINDOW_POPUP:
            switch (button_id) {
                case 0: return "DS1Grey";
                case 1: case 2: case 3: return "DS1BlackLight";
            }
            break;
    }
    return "DS1Yellow";
}

extern char* soloscuro_ds1ebox_font(int32_t win_id, int32_t button_id) {
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
        case DS1_WINDOW_VIEW_CHARACTER:
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
        case DS1_WINDOW_POPUP:
            break;
        case DS1_WINDOW_NEW_CHARACTER:
            return "DS1GreyLight";
    }
    return "DS1Yellow";
}
