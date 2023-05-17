/**
 * @file soloscuro.c
 * @date 28-Apr-2023
 */

#include "orx.h"
#include "soloscuro.h"
#include "soloscuro/font.h"
#include "soloscuro/gui.h"
#include "soloscuro/state.h"
#include "soloscuro/window.h"

soloscuro_state_t *state = NULL;

#ifdef __orxMSVC__

/* Requesting high performance dedicated GPU on hybrid laptops */
__declspec(dllexport) unsigned long NvOptimusEnablement        = 1;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

#endif // __orxMSVC__

/** Update function, it has been registered to be called every tick of the core clock
 */
void orxFASTCALL Update(const orxCLOCK_INFO *_pstClockInfo, void *context)
{
    // Should quit?
    if(orxInput_IsActive("Quit"))
    {
        // Send close event
        orxEvent_SendShort(orxEVENT_TYPE_SYSTEM, orxSYSTEM_EVENT_CLOSE);
    }
    soloscuro_ds1key_handle((soloscuro_state_t*) context);
}

void orxFASTCALL mouse_update(const orxCLOCK_INFO *clock_info, void *context) {
    orxVECTOR vPos;
    orxOBJECT *obj = orxNULL;
    static orxOBJECT *last_obj = orxNULL;

    if(orxRender_GetWorldPosition(orxMouse_GetPosition(&vPos), orxNULL, &vPos) == orxNULL) {
        return;
    }

    soloscuro_ds1button_handle((soloscuro_state_t*)context, &vPos);
}


/*
orxSTATUS orxFASTCALL object_handler (const orxEVENT *event) {
    soloscuro_state_t *state = event->pContext;
    printf("object_handler: %d\n", event->eID);
    switch (event->eID) {
        case orxOBJECT_EVENT_DELETE:
            printf("DELETE: %d\n", event->hSender);
            break;
    }
    return orxTRUE;
}
*/


/** Init function, it is called when all orx's modules have been initialized
 */
orxSTATUS orxFASTCALL Init()
{
    // Display a small hint in console
    //orxLOG("\n* This template project creates a simple scene"
    //"\n* You can play with the config parameters in ../data/config/soloscuro.ini"
    //"\n* After changing them, relaunch the executable to see the changes.");
    state = soloscuro_state_create();
    //orxEvent_AddHandlerWithContext( orxEVENT_TYPE_OBJECT, object_handler, state );

    if (soloscuro_register_gff_type(state)) {
        printf("GFF resource system registered.\n");
    }
    orxViewport_CreateFromConfig("MainViewport");
    soloscuro_generate_window(state, DS1_WINDOW_MESSAGE);

    /*
    soloscuro_generate_window(state, DS1_WINDOW_MAINMENU);
    soloscuro_create_window(state, DS1_WINDOW_MAINMENU);
    state->gui.win = win;
    state->gui.win_id = DS1_WINDOW_MAINMENU;
    */
    //soloscuro_generate_window(state, DS1_WINDOW_NEW_CHARACTER);
    //soloscuro_create_window(DS1_WINDOW_NEW_CHARACTER, &win);
    //soloscuro_generate_window(state, DS1_WINDOW_VIEW_CHARACTER);
    soloscuro_generate_window(state, DS1_WINDOW_POPUP);
    soloscuro_generate_window(state, DS1_WINDOW_GEN_PSIONIC);
    soloscuro_generate_window(state, DS1_WINDOW_GEN_SPHERES);
    soloscuro_generate_window(state, DS1_WINDOW_NEW_CHARACTER);
    //soloscuro_generate_window(state, DS1_WINDOW2);
    //soloscuro_create_window(state, DS1_WINDOW_GEN_SPHERES);
    soloscuro_create_window(state, DS1_WINDOW_NEW_CHARACTER);
    //soloscuro_create_window(state, DS1_WINDOW_GEN_PSIONIC);
    //soloscuro_create_window(state, DS1_WINDOW_GEN_PSIONIC);
    //soloscuro_create_window(state, DS1_WINDOW_VIEW_CHARACTER);
    /*
    soloscuro_ds1_view_character_select(state);
    //state->gui.win = win;
    //state->gui.win_id = DS1_WINDOW_VIEW_CHARACTER;
    for(orxOBJECT *child = orxOBJECT(orxObject_GetChild(win));
            child != orxNULL;
            child = orxOBJECT(orxObject_GetSibling(child))) {
        printf("view_char: %s\n", orxObject_GetName(child));
        if (!strcmp(orxObject_GetName(child), "Window11500Button5Object")) {
    for(orxOBJECT *child2 = orxOBJECT(orxObject_GetChild(child));
            child2 != orxNULL;
            child2 = orxOBJECT(orxObject_GetSibling(child2))) {
        printf("child2: %s\n", orxObject_GetName(child2));
            orxObject_SetTextString(child2, "TEST!");
    }
            printf("FOUND!\n");
        }
    }
    */

    //orxConfig_PushSection("Window11500Button5Text");
    //orxConfig_PushSection("Window11500Button5TextText");
    //soloscuro_generate_window(state, DS1_WINDOW11, &win);
    // Create the viewport
    //win = orxObject_CreateFromConfig("Window3000");
    //orxObject_SetParent(win, state->gui.window_stack);

    // Create the scene
    //orxOBJECT *mo = orxObject_CreateFromConfig("MessageObject");
    //printf("->'%s'\n", orxObject_GetTextString(mo));
    //orxObject_SetTextString(mo, "Hello!");

    //orxObject_CreateFromConfig("HeroObject");
    //orxObject_CreateFromConfig("FirstObject");
    //orxObject_CreateFromConfig("Window3020");
    //orxObject_CreateFromConfig("FirstButton");

    // Register the Update function to the core clock
    orxClock_Register(orxClock_Get(orxCLOCK_KZ_CORE), Update, state, orxMODULE_ID_MAIN, orxCLOCK_PRIORITY_NORMAL);
    orxClock_Register(orxClock_Get(orxCLOCK_KZ_CORE), mouse_update, state, orxMODULE_ID_MAIN, orxCLOCK_PRIORITY_NORMAL);

    // Done!
    return orxSTATUS_SUCCESS;
}

/** Run function, it should not contain any game logic
 */
orxSTATUS orxFASTCALL Run()
{
    // Return orxSTATUS_FAILURE to instruct orx to quit
    return orxSTATUS_SUCCESS;
}

/** Exit function, it is called before exiting from orx
 */
void orxFASTCALL Exit()
{
    // Let orx clean all our mess automatically. :)
    soloscuro_state_free(state);
}

/** Bootstrap function, it is called before config is initialized, allowing for early resource storage definitions
 */
orxSTATUS orxFASTCALL Bootstrap()
{
    // Add config storage to find the initial config file
    //orxResource_AddStorage(orxCONFIG_KZ_RESOURCE_GROUP, "../data/config", orxFALSE);
    orxResource_AddStorage(orxCONFIG_KZ_RESOURCE_GROUP, "./", orxFALSE);

    // Return orxSTATUS_FAILURE to prevent orx from loading the default config file
    return orxSTATUS_SUCCESS;
}

/** Main function
 */
int main(int argc, char **argv)
{
    // Set the bootstrap function to provide at least one resource storage before loading any config files
    orxConfig_SetBootstrap(Bootstrap);
    //orxConfig_Load("soloscuro.ini");

    // Execute our game
    orx_Execute(argc, argv, Init, Run, Exit);

    // Done!
    return EXIT_SUCCESS;
}

extern int soloscuro_find_first_child(orxOBJECT *object, const char *str, orxOBJECT **out) {
    for(orxOBJECT *obj = orxOBJECT(orxObject_GetChild(object));
                   obj != orxNULL;
                   obj = orxOBJECT(orxObject_GetSibling(obj))) {

        if (!strcmp(orxObject_GetName(obj), str)) {
            *out = obj;
            goto found;
        }
    }

    return EXIT_FAILURE;
found:
    return EXIT_SUCCESS;
}

extern int soloscuro_find_first_child_with(orxOBJECT *object, const char *str, orxOBJECT **out) {
    for(orxOBJECT *obj = orxOBJECT(orxObject_GetChild(object));
                   obj != orxNULL;
                   obj = orxOBJECT(orxObject_GetSibling(obj))) {

        if (strstr(orxObject_GetName(obj), str)) {
            *out = obj;
            goto found;
        }
    }

    return EXIT_FAILURE;
found:
    return EXIT_SUCCESS;
}

extern int soloscuro_get_top_window(soloscuro_state_t *state, orxOBJECT **out, int32_t *id) {
    *out = state->gui.win_stack[state->gui.win_pos - 1].win;
    *id = state->gui.win_stack[state->gui.win_pos - 1].win_id;
}

extern int soloscuro_destroy_top_window(soloscuro_state_t *state) {
    if (state->gui.win_pos <= 0) {
        goto overflow;
    }

    state->gui.win_pos--;

    orxObject_Delete(state->gui.win_stack[state->gui.win_pos].win);
    state->gui.win_stack[state->gui.win_pos].win = NULL;

    if (state->gui.win_pos <= 0) {
        goto done;
    }

    switch(state->gui.win_stack[state->gui.win_pos - 1].win_id) {
        case DS1_WINDOW_VIEW_CHARACTER: return soloscuro_ds1_view_character_return(state);
    }

done:
    return EXIT_SUCCESS;
overflow:
    return EXIT_FAILURE;
}

extern int soloscuro_get_button_object(orxOBJECT *win, int32_t win_id, int32_t button_id, orxOBJECT **button) {
    char name[128];
    orxOBJECT *group;

    snprintf(name, 128, "Window%dButton%dObject", win_id, button_id);
    soloscuro_find_first_child(win, name, &group);
    snprintf(name, 128, "Window%dButton%d", win_id, button_id);
    soloscuro_find_first_child(group, name, button);
}

extern int soloscuro_get_button_text_object(orxOBJECT *win, int32_t win_id, int32_t button_id, orxOBJECT **button) {
    char name[128];
    orxOBJECT *group;

    snprintf(name, 128, "Window%dButton%dObject", win_id, button_id);
    soloscuro_find_first_child(win, name, &group);
    snprintf(name, 128, "Window%dButton%dText", win_id, button_id);
    soloscuro_find_first_child(group, name, button);
}

extern int soloscuro_get_ebox_object(orxOBJECT *win, int32_t win_id, int32_t ebox_id, orxOBJECT **ebox) {
    char name[128];
    orxOBJECT *group;

    snprintf(name, 128, "Window%dBox%d", win_id, ebox_id);
    soloscuro_find_first_child(win, name, &group);
    snprintf(name, 128, "Window%dBox%dText", win_id, ebox_id);
    soloscuro_find_first_child(group, name, ebox);
}

