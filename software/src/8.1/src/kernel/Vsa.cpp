/*****  Vsa (Vision Services Architecture) Globals *****/

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

#include "Vsa.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vsa_VsaDirectoryBuilder.h"

/******************************
 ******************************
 *****  Vsa Design Notes  *****
 ******************************
 ******************************/

/*----------------------------------------------------------------------------*

 *----------------------------------------------------------------------------*/


/****************************
 ****************************
 *****  Initialization  *****
 ****************************
 ****************************/

namespace Vsa {
    class Registrar {
    public:
	Registrar () {
	    Vca::VDirectoryBuilder::Reference pBuilder (new VsaDirectoryBuilder ());
	    Vca::RegisterDirectoryBuilder (pBuilder);
	}
	~Registrar () {
	}
    };
    static Registrar g_iRegistrar;
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
