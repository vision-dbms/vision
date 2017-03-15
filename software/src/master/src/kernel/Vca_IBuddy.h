#ifndef Vca_IBuddy_Interface
#define Vca_IBuddy_Interface

#ifndef Vca_Main_IBuddy
#define Vca_Main_IBuddy extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vca_Main.h"

/************************
 *****  Components  *****
 ************************/

#include "IVUnknown.h"

/*************************
 *****  Definitions  *****
 *************************/

/**
 * @class Vca::IBuddy
 *
 * Used by Vca::VServerApplication to spawn buddy processes in order to handle high request loads.
 * @todo Document buddy system for VServerApplication (probably in a page).
 */

VINTERFACE_TEMPLATE_INSTANTIATIONS_IN_API(Vca_Main,Vca,IBuddy)

DECLARE_VcaINTERFACE (IRequest)

#include "Vca_IPauseResume.h"
#include "Vca_IStop.h"

namespace Vca {
    /** Convenience definition for receivers of IRequest instances. */
    typedef IVReceiver<IRequest*> IRequestReceiver;

    VINTERFACE_ROLE (IBuddy, IVUnknown)
	VINTERFACE_ROLE_3 (ControlThese, IRequestReceiver*, IPauseResume*, IStop*)
    VINTERFACE_ROLE_END

    VcaMainINTERFACE (IBuddy, IVUnknown)
        /**
         * Used to provide a buddy process with control interfaces to other buddies.
         *
         * @param p2 an IPauseResume instance for a particular buddy that should be controlled by this buddy.
         * @param p3 an IStop instance for a particular buddy that should be controlled by this buddy.
         */
	VINTERFACE_METHOD_3 (ControlThese, IRequestReceiver*, IPauseResume*, IStop*)
    VINTERFACE_END
}

#endif
