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

#include "Vca_VClassInfoHolder.h"

#include "Vca_VServerApplication.h"

/***********************
 ***********************
 *****             *****
 *****  ClassInfo  *****
 *****             *****
 ***********************
 ***********************/

/*************************************
 *************************************
 *****  Vca::VServerApplication  *****
 *************************************
 *************************************/

namespace Vca {
    VClassInfoHolder &VServerApplication::ClassInfo () {
	static VClassInfoHolder_<ThisClass> iClassInfoHolder;
	if (iClassInfoHolder.isntComplete ()) {
	    iClassInfoHolder
		.addBase (BaseClass::ClassInfo ())

		.addProperty ("ListenerCount"    , &ThisClass::listenerCount)
		.addProperty ("ListenerName"     , &ThisClass::listenerName)
		.addProperty ("OfferCount"       , &ThisClass::activeOfferCountTrackable)
		.addProperty ("PassiveOfferCount", &ThisClass::passiveOfferCount)
		.addProperty ("RegistrationCount", &ThisClass::registrationCount)

		.markCompleted ();
	}
	return iClassInfoHolder;
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
