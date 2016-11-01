/*****  Vca_Registry_IUpdate Implementation  *****/

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

#define   Vca_Registry_IUpdate
#include "Vca_Registry_IUpdate.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/************************************
 ************************************
 *****                          *****
 *****  Vca::Registry::IUpdate  *****
 *****                          *****
 ************************************
 ************************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::Registry::IUpdate)

namespace Vca {
    namespace Registry {
    // {9050FC8E-36A4-401b-B1DF-43A3F5A7812E}
	VINTERFACE_TYPEINFO_DEFINITION (
	    IUpdate,
	    0x9050fc8e, 0x36a4, 0x401b, 0xb1, 0xdf, 0x43, 0xa3, 0xf5, 0xa7, 0x81, 0x2e
	);

    //  Member Definitions
	VINTERFACE_MEMBERINFO_DEFINITION (IUpdate, AddObject, 0);
    } // namespace Registry
} // namespace Vca
