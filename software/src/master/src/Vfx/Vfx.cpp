/*****  Vfx/Vfx.cpp  *****/

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

#include "Vfx.h"

/************************
 *****  Supporting  *****
 ************************/


/******************
 ******************
 *****  Junk  *****
 ******************
 ******************/

namespace Vfx {
    Vfx_API void Dummy () {
    }
}

/*********************
 *********************
 *****  DllMain  *****
 *********************
 *********************/

#if defined(_WIN32)

BOOL APIENTRY DllMain(HINSTANCE hModule, DWORD xCallReason, LPVOID lpReserved) {
    switch (xCallReason) {
    case DLL_PROCESS_ATTACH:
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
