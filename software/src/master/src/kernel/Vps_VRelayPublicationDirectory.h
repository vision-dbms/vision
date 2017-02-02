#ifndef Vps_VRelayPublicationDirectory_Interface
#define Vps_VRelayPublicationDirectory_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vps.h"

/************************
 *****  Components  *****
 ************************/

#include "Vca_VDirectory.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_VGoferInterface.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    namespace Vps {
        class RelayPublicationRegistrar;
	class Vps_API VRelayPublicationDirectory : public VDirectory {
	    DECLARE_CONCRETE_RTT (VRelayPublicationDirectory, VDirectory);

	//  Construction
	public:
	    VRelayPublicationDirectory (VString const &rSource, RelayPublicationRegistrar* = 0);

	//  Destruction
	private:
	    ~VRelayPublicationDirectory ();

	//  Maintenance
	private:
	    virtual bool locate (Entry::Reference &rpEntry, VString const &rName);

	//  State
	private:
	    VGoferInterface<IDirectory>::Reference const m_pSourceGofer;
            RelayPublicationRegistrar* m_pRegistrar;
	};
    }
}


#endif
