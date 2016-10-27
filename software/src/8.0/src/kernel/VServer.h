#ifndef VServer_Interface
#define VServer_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VServerApplication.h"

#include "Vca_VActivityRelay.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    /**
     * Rudimentary directory server application used to expose the default directory built by Vca from a @c sessions.cfg file, or one of its entries.
     */
    class VServer
	: public VServerApplication
	, private VActivityModel
	, private VActivityRelay
    {
	DECLARE_CONCRETE_RTTLITE (VServer, VServerApplication);

    //  Construction
    public:
	VServer (Context *pContext);

    //  Destruction
    private:
	~VServer ();

    //  Execution
    private:
	bool start_();
    };
}

#endif
