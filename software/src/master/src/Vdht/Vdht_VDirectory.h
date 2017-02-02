#ifndef Vdht_VDirectory_Interface
#define Vdht_VDirectory_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vdht_VHT.h"
#include "Vca_Registry_VRegistry.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vdht {
    /**
     * Distributed Registry
     */
    class Vdht_API VDirectory : public VHT, public Vca::Registry::VRegistry {
	DECLARE_CONCRETE_RTTLITE (VDirectory, VHT);

    //  Construction
    public:
        /** Constructor initializes roles. */
	VDirectory ();

    //  Destruction
    protected:
        /** Empty destructor restricts scope. */
	~VDirectory ();
    };
}


#endif
