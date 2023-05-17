#include "soloscuro/window.h"

static gff_button_t sun = {
    .icon_id               = 20028,
    .frame.initbounds.xmin = 40,
    .frame.initbounds.ymin = 20,
    .frame.height          = 0,
};

static gff_button_t view_char = {
    .icon_id               = 20079,
    .frame.initbounds.xmin = 55,
    .frame.initbounds.ymin = 0,
    .frame.height          = 0,
};

static gff_button_t effects = {
    .icon_id               = 20075,
    .frame.initbounds.xmin = 75,
    .frame.initbounds.ymin = 0,
    .frame.height          = 0,
};

static gff_button_t use = {
    .icon_id               = 20080,
    .frame.initbounds.xmin = 75,
    .frame.initbounds.ymin = 0,
    .frame.height          = 0,
};

extern int32_t soloscuro_window_background(int32_t win_id) {
    switch (win_id) {
        case DS1_WINDOW_MAINMENU: return 20029;
        case DS1_WINDOW_NEW_CHARACTER: return 13001;
        case DS1_WINDOW_GEN_PSIONIC: return 20087;
        case DS1_WINDOW_GEN_SPHERES: return 20087;
        case DS1_WINDOW_GAME_MENU: return 10000;
        case DS1_WINDOW_MESSAGE: return 10001;
        case DS1_WINDOW_VIEW_CHARACTER: return 11000;
        case DS1_WINDOW_INVENTORY: return 13001;
        case DS1_WINDOW_ALERT2: return 20087;
        case DS1_WINDOW_INSPECT2: return 20086;
        case DS1_WINDOW_VIEW_ITEM: return 20087;
        case DS1_WINDOW_SPELL_INFO: return 20085;
        case DS1_WINDOW_PREFERENCES: return 10000;
        case DS1_WINDOW_SPELL_TRAIN: return 17000;
        case DS1_WINDOW_PSI_TRAIN: return 17001;
        case DS1_WINDOW_DUAL: return 17002;
    }

    return win_id;
}

extern int32_t soloscuro_window_backgroundx(int32_t win_id) {
    switch (win_id) {
        case DS1_WINDOW_MAINMENU: return 0;
        case DS1_WINDOW_NEW_CHARACTER: return 0;
    }

    return 0;
}

extern int32_t soloscuro_window_backgroundy(int32_t win_id) {
    switch (win_id) {
        case DS1_WINDOW_MAINMENU: return 42;
        case DS1_WINDOW_NEW_CHARACTER: return 0;
    }

    return 0;
}

extern gff_button_t* soloscuro_window_button_extra(int win_id, int index) {
    switch(win_id) {
        case DS1_WINDOW_MAINMENU:
            return (index == 0) ? &sun : NULL;
        case DS1_WINDOW_VIEW_CHARACTER: return  (index == 0) ? &view_char : NULL;
    }

    return NULL;
}
