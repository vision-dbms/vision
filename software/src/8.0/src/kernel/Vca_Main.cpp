/**
 * @file
 * Vca_Main namespace implementation; provides definition of several global Vca_Main functions.
 */

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#include "Vca_Main.h"

/************************
 *****  Supporting  *****
 ************************/


/*********************
 *********************
 *****  DllMain  *****
 *********************
 *********************/

#if defined(_WIN32)

BOOL APIENTRY DllMain(HINSTANCE hModule, DWORD xCallReason, LPVOID lpReserved) {
    switch (xCallReason) {
    case DLL_PROCESS_ATTACH:
	DisableThreadLibraryCalls (hModule);
	break;
    case DLL_THREAD_ATTACH:
	break;
    case DLL_THREAD_DETACH:
	break;
    case DLL_PROCESS_DETACH:
	break;
    }
    return true;
}

#endif
