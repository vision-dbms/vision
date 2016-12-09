/*****  Vca_Registry_IUpdater Implementation  *****/

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

#define   Vca_Registry_IUpdater
#include "Vca_Registry_IUpdater.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/*************************************
 *************************************
 *****                           *****
 *****  Vca::Registry::IUpdater  *****
 *****                           *****
 *************************************
 *************************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::Registry::IUpdater)

namespace Vca {
    namespace Registry {
    // {E17CBB35-F874-4365-814E-8F30FD76EDB7}
	VINTERFACE_TYPEINFO_DEFINITION (
	    IUpdater,
	    0xe17cbb35, 0xf874, 0x4365, 0x81, 0x4e, 0x8f, 0x30, 0xfd, 0x76, 0xed, 0xb7
	);

    //  Member Definitions
	VINTERFACE_MEMBERINFO_DEFINITION (IUpdater, OnContact, 0);
    } // namespace Registry
} // namespace Vca
