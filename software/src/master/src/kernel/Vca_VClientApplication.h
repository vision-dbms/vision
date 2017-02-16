#ifndef Vca_VClientApplication_Interface
#define Vca_VClientApplication_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VApplication.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    /**
     * Client applications are intended to perform a number of activities and then exit.
     * Each activity performed by a client application is described by an Activity descriptor, which should be appropriately subclassed for the activities pertinent to your VClientApplication subclass. When an activity has completed (and Activity::onDone() has been called), VClientApplication::decrementActivityCount() is automatically called. When the activity count for a VClientApplication instance reaches zero, it will exit cleanly.
     *
     * @par Example Derivations
     * The following classes are great examples of VClientApplication instances that are used regularly.
     *  - vcatool (Vca::Tool)
     *  - vprompt (Vsa::VPromptApplication)
     */
    class ABSTRACT Vca_Main_API VClientApplication : public VApplication {
	DECLARE_ABSTRACT_RTTLITE (VClientApplication, VApplication);

    //  Aliases
    public:
	typedef Reference ClientApplicationReference;

    //  Construction
    protected:
        /** @copydoc Vca::VApplication::VApplication() */
	VClientApplication (Context *pContext);

    //  Destruction
    protected:
        /** @copydoc Vca::VApplication::~VApplication() */
	~VClientApplication ();
    };
}


#endif
