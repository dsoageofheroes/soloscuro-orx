/**
 * @file soloscuro.c
 * @date 28-Apr-2023
 */

#include "orx.h"
#include "soloscuro.h"

#ifdef __orxMSVC__

/* Requesting high performance dedicated GPU on hybrid laptops */
__declspec(dllexport) unsigned long NvOptimusEnablement        = 1;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

#endif // __orxMSVC__

/** Update function, it has been registered to be called every tick of the core clock
 */
void orxFASTCALL Update(const orxCLOCK_INFO *_pstClockInfo, void *_pContext)
{
    // Should quit?
    if(orxInput_IsActive("Quit"))
    {
        // Send close event
        orxEvent_SendShort(orxEVENT_TYPE_SYSTEM, orxSYSTEM_EVENT_CLOSE);
    }
}

/** Init function, it is called when all orx's modules have been initialized
 */
orxSTATUS orxFASTCALL Init()
{
    // Display a small hint in console
    //orxLOG("\n* This template project creates a simple scene"
    //"\n* You can play with the config parameters in ../data/config/soloscuro.ini"
    //"\n* After changing them, relaunch the executable to see the changes.");

    if (soloscuro_register_gff_type()) {
        printf("GFF resource system registered.\n");
    }
    // Create the viewport
    orxViewport_CreateFromConfig("MainViewport");

    // Create the scene
    orxObject_CreateFromConfig("Scene");
    orxObject_CreateFromConfig("HeroObject");
    orxObject_CreateFromConfig("FirstObject");

    // Register the Update function to the core clock
    orxClock_Register(orxClock_Get(orxCLOCK_KZ_CORE), Update, orxNULL, orxMODULE_ID_MAIN, orxCLOCK_PRIORITY_NORMAL);

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
